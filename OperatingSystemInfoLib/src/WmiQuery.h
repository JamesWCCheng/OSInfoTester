#pragma once

#include <array>
#include <string>
#include <vector>

#include <Objbase.h>
#include <Wbemidl.h>
#include <comutil.h>
#include <comdef.h>
#include <wrl/client.h>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Wbemuuid.lib")

namespace detail
{
template <class BSTRType> struct BSTRTrait;

template <> struct BSTRTrait<wchar_t>
{
    using StdType = std::wstring;
};

template <> struct BSTRTrait<char>
{
    using StdType = std::string;
};

// If we need to support more different types of accessing variant member,
// we should extend the specialization template.
template <VARENUM ExpectedVariantType> struct VariantTrait;

template <> struct VariantTrait<VARENUM::VT_BSTR>
{
    using Type = BSTRTrait<std::remove_pointer<BSTR>::type>::StdType;
    static constexpr auto Value = &VARIANT::bstrVal;
};

template <> struct VariantTrait<VARENUM::VT_I4>
{
    using Type = INT;
    static constexpr auto Value = &VARIANT::intVal;
};

template <> struct VariantTrait<VARENUM::VT_BOOL>
{
    using Type = VARIANT_BOOL;
    static constexpr auto Value = &VARIANT::boolVal;
};

template <> struct VariantTrait<static_cast<VARENUM>(VARENUM::VT_ARRAY | VARENUM::VT_BSTR)>
{
    using Type = std::vector<BSTRTrait<std::remove_pointer<BSTR>::type>::StdType>;
    static constexpr auto Value = &VARIANT::parray;
};
} // namespace detail

// This function will assign a zero initialized value ValueType{}
// once the expected variant type is not matched.
template <VARENUM ExpectedVariantType>
bool GetValueFromVariant(typename detail::VariantTrait<ExpectedVariantType>::Type& outValue, const VARIANT& variant)
{
    if (variant.vt == ExpectedVariantType)
    {
        outValue = variant.*detail::VariantTrait<ExpectedVariantType>::Value;
        return true;
    }
    return false;
}

template <>
bool GetValueFromVariant<static_cast<VARENUM>(VARENUM::VT_ARRAY | VARENUM::VT_BSTR)>(typename detail::VariantTrait<static_cast<VARENUM>(VARENUM::VT_ARRAY | VARENUM::VT_BSTR)>::Type& outValue, const VARIANT& variant)
{
    if (variant.vt != static_cast<VARENUM>(VARENUM::VT_ARRAY | VARENUM::VT_BSTR))
    {
        return false;
    }

    long lower = 0, upper = 0;
    SAFEARRAY *pSafeArray = variant.parray;
    SafeArrayGetLBound(pSafeArray, 1, &lower);
    SafeArrayGetUBound(pSafeArray, 1, &upper);

    for (auto i = lower; i <= upper; i++)
    {
        BSTR bstrVal = nullptr;
        if (SUCCEEDED(SafeArrayGetElement(pSafeArray, &i, &bstrVal)))
        {
            outValue.emplace_back(bstrVal);
        }
    }
    return true;
}

template <typename T> class ComPtr : public Microsoft::WRL::ComPtr<T>
{
public:
    ~ComPtr() noexcept
    {
    }

    ComPtr() noexcept : Microsoft::WRL::ComPtr<T>()
    {
    }

    ComPtr(const ComPtr<T>& other) noexcept : Microsoft::WRL::ComPtr<T>(other)
    {
    }

    ComPtr(ComPtr<T>&& other) noexcept : Microsoft::WRL::ComPtr<T>(std::forward<ComPtr<T>>(other))
    {
    }

    ComPtr& operator=(const ComPtr<T>& other) noexcept
    {
        Microsoft::WRL::ComPtr<T>::operator=(other);
        return *this;
    }

    ComPtr& operator=(ComPtr<T>&& other) noexcept
    {
        Microsoft::WRL::ComPtr<T>::operator=(std::forward<ComPtr<T>>(other));
        return *this;
    }

    ComPtr(decltype(nullptr) other) noexcept : Microsoft::WRL::ComPtr<T>(other)
    {
    }

    template <class U> ComPtr(U* other) noexcept : Microsoft::WRL::ComPtr<T>(other)
    {
    }

    ComPtr& operator=(decltype(nullptr)) noexcept
    {
        Microsoft::WRL::ComPtr<T>::operator=(nullptr);
        return *this;
    }

    template <typename U> ComPtr& operator=(U* other) noexcept
    {
        Microsoft::WRL::ComPtr<T>::operator=(other);
        return *this;
    }

    ComPtr& operator=(T* other) noexcept
    {
        Microsoft::WRL::ComPtr<T>::operator=(other);
        return *this;
    }

    operator T*() const noexcept
    {
        return Microsoft::WRL::ComPtr<T>::ptr_;
    }

    explicit operator bool() const noexcept
    {
        return (Microsoft::WRL::ComPtr<T>::ptr_ != nullptr);
    }
};

class ComCtx
{
public:
    ComCtx(const ComCtx&) = delete;
    ComCtx(ComCtx&&) = delete;
    ComCtx& operator=(const ComCtx&) = delete;
    ComCtx& operator=(ComCtx&&) = delete;

public:
    virtual ~ComCtx() noexcept
    {
        CoUninitialize();
    }

    ComCtx() noexcept
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                             nullptr, EOAC_NONE, nullptr);
    }
};

class WmiQuery final
{
public:
    bool ConnectServer(BSTR networkResource) noexcept
    {
        if (SUCCEEDED(
                CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_locator)) &&
            SUCCEEDED(m_locator->ConnectServer(networkResource, NULL, NULL, 0, NULL, 0, 0, &m_services)) &&
            SUCCEEDED(CoSetProxyBlanket(m_services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
                                        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
        {
            return true;
        }
        return true;
    }

    bool ExecQuery(BSTR query, IEnumWbemClassObject** objects) noexcept
    {
        if (m_services == nullptr)
        {
            return false;
        }

        long flags = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;
        if (SUCCEEDED(m_services->ExecQuery(_bstr_t(L"WQL"), query, flags, NULL, objects)))
        {
            return true;
        }
        return false;
    }

private:
    ComCtx m_env{};
    ComPtr<IWbemLocator> m_locator{};
    ComPtr<IWbemServices> m_services{};
};

class WmiClassAccessor
{
public:
    virtual ~WmiClassAccessor() = default;
    WmiClassAccessor(const WmiClassAccessor&) = delete;
    WmiClassAccessor(WmiClassAccessor&&) = delete;
    WmiClassAccessor& operator=(const WmiClassAccessor&) = delete;
    WmiClassAccessor& operator=(WmiClassAccessor&&) = delete;
    WmiClassAccessor() noexcept {};

protected:
    template <VARENUM ExpectedVariantType>
    bool StoreWmiPropertyValue(ComPtr<IWbemClassObject> object, wchar_t* propertyName,
                               typename detail::VariantTrait<ExpectedVariantType>::Type& outValue) const
    {
        _variant_t data;
        HRESULT hr = object->Get(propertyName, 0, &data, nullptr, nullptr);
        if (FAILED(hr))
        {
            return false;
        }

        return GetValueFromVariant<ExpectedVariantType>(outValue, data);
    }

    WmiQuery m_wmi;
};

struct OSInfo
{
    std::wstring Caption;
    std::wstring OSArchitecture;
    std::wstring MUILanguage;
    std::wstring Locale;
    std::wstring Version;
    int32_t OSLanguage;
    int32_t ServicePackMajorVersion;
    int32_t ServicePackMinorVersion;
};

class WmiCimv2 final : private WmiClassAccessor
{
public:
    ~WmiCimv2() noexcept = default;
    WmiCimv2(const WmiCimv2&) = delete;
    WmiCimv2(WmiCimv2&&) = delete;
    WmiCimv2& operator=(const WmiCimv2&) = delete;
    WmiCimv2& operator=(WmiCimv2&&) = delete;

public:
    WmiCimv2() noexcept
    {
        m_wmi.ConnectServer(_bstr_t(L"root\\cimv2"));
    }

    OSInfo GetOSInfo()
    {
        ComPtr<IEnumWbemClassObject> objects;
        m_wmi.ExecQuery(_bstr_t(L"SELECT * FROM Win32_OperatingSystem"), &objects);
        if (!objects)
        {
            return {};
        }
        OSInfo result;
        auto getOSInfoFromObject = [this](ComPtr<IWbemClassObject> object) {
            OSInfo info{};
            // When meeting failure case inside StoreWmiPropertyValue,
            // we still get the remaining fields from object and print error log and return the info back to caller.
            assert(StoreWmiPropertyValue<VARENUM::VT_BSTR>(object, _bstr_t(L"Caption"), info.Caption));
            assert(StoreWmiPropertyValue<VARENUM::VT_BSTR>(object, _bstr_t(L"OSArchitecture"), info.OSArchitecture));
            std::vector<std::wstring> muilangs;
            assert(StoreWmiPropertyValue<static_cast<VARENUM>(VARENUM::VT_ARRAY | VARENUM::VT_BSTR)>(object, _bstr_t(L"MUILanguages"), muilangs));
            // currently, we only collect first element by design.
            if (!muilangs.empty())
            {
                info.MUILanguage = muilangs.front();
            }
            assert(StoreWmiPropertyValue<VARENUM::VT_BSTR>(object, _bstr_t(L"Locale"), info.Locale));
            assert(StoreWmiPropertyValue<VARENUM::VT_BSTR>(object, _bstr_t(L"Version"), info.Version));
            assert(StoreWmiPropertyValue<VARENUM::VT_I4>(object, _bstr_t(L"OSLanguage"), info.OSLanguage));
            assert(StoreWmiPropertyValue<VARENUM::VT_I4>(object, _bstr_t(L"ServicePackMajorVersion"), info.ServicePackMajorVersion));
            assert(StoreWmiPropertyValue<VARENUM::VT_I4>(object, _bstr_t(L"ServicePackMinorVersion"), info.ServicePackMinorVersion));

            return info;
        };
        while (true)
        {
            ComPtr<IWbemClassObject> object;
            ULONG returned = 0;
            objects->Next(WBEM_INFINITE, 1, &object, &returned);
            if (returned == 0)
            {
                break;
            }
            result = getOSInfoFromObject(std::move(object));
        }
        return result;
    }
};

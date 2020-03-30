#include "pch.h"
#include "OperatingSystemInfoProvider.h"
#include "IOperatingSystemInfoFetcher.h"

#include <array>
#include <cassert>

OperatingSystemInfoProvider::OperatingSystemInfoProvider(std::unique_ptr<IOperatingSystemInfoFetcher> fetcher) noexcept
    : m_Fetcher(std::move(fetcher))
{
    assert(m_Fetcher != nullptr);
}

OperatingSystemInfoProvider::~OperatingSystemInfoProvider() = default;

OperatingSystemInfo OperatingSystemInfoProvider::GetInformation()
{
    // Fill all the field with NotApplicableDefaultValue when it is not provided by fetcher or the value is empty.
    auto defaultValueFiller = [](OperatingSystemInfo& info) {
        constexpr char NotApplicableDefaultValue[] = "N/A";
        constexpr auto NumberOfFields = 21;
        std::array<std::optional<std::string> OperatingSystemInfo::*, NumberOfFields> fields =
        { &OperatingSystemInfo::BuildBranch,
          &OperatingSystemInfo::Caption,
          &OperatingSystemInfo::Codename,
          &OperatingSystemInfo::CSDBuildNumber,
          &OperatingSystemInfo::CSDVersion,
          &OperatingSystemInfo::CurrentBuildNumber,
          &OperatingSystemInfo::CurrentMajorVersionNumber,
          &OperatingSystemInfo::CurrentMinorVersionNumber,
          &OperatingSystemInfo::CurrentVersion,
          &OperatingSystemInfo::EditionID,
          &OperatingSystemInfo::Locale,
          &OperatingSystemInfo::MarketName,
          &OperatingSystemInfo::MUILanguage,
          &OperatingSystemInfo::OSArchitecture,
          &OperatingSystemInfo::OSLanguage,
          &OperatingSystemInfo::OSVersion,
          &OperatingSystemInfo::ReleaseId,
          &OperatingSystemInfo::ServicePackMajorVersion,
          &OperatingSystemInfo::ServicePackMinorVersion,
          &OperatingSystemInfo::UBR,
          &OperatingSystemInfo::Version };

        for (auto f : fields)
        {
            if (!(info.*f) || (*(info.*f)).empty())
            {
                info.*f = NotApplicableDefaultValue;
            }
        }
    };

    try
    {
        auto info = m_Fetcher->GetInformation();
        defaultValueFiller(info);
        return info;
    }
    catch (const std::exception&)
    {
        // TODO: Should put error information when logger is published.
        return {};
    }
    catch (...)
    {
        return {};
    }
}
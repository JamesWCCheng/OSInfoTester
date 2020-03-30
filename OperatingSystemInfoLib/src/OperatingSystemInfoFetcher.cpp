#include "pch.h"
#include "OperatingSystemInfoFetcher.h"
#include "WindowsReg.h"
#include "WmiQuery.h"

#include <algorithm>
#include <array>
#include <sstream>
#include <string>

namespace detail
{
struct WindowsEdition
{
    char Key[64];
    char BuildNumber[64];
    char Version[64];
    char Codename[128];
    char MarketName[128];
};

static constexpr std::array<WindowsEdition, 10> s_Windows10Editions{{
    // Windows 10 version history: https://en.wikipedia.org/wiki/Windows_10_version_history
    // Format: {Abbreviation, Build, Version, Codename, MarketingName}
    {"th1", "10240", "1507", "TH1 (Threshold 1)", "RTM"},
    {"th2", "10586", "1511", "TH2 (Threshold 2)", "November Update"},
    {"rs1", "14393", "1607", "RS1 (RedStone 1)", "Anniversary Update"},
    {"rs2", "15063", "1703", "RS2 (RedStone 2)", "Creators Update"},
    {"rs3", "16299", "1709", "RS3 (RedStone 3)", "Fall Creators Update"},
    {"rs4", "17134", "1803", "RS4 (RedStone 4)", "April 2018 Update"},
    {"rs5", "17763", "1809", "RS5 (RedStone 5)", "October 2018 Update"},
    {"19h1", "18362", "1903", "19H1", "May 2019 Update"},
    {"19h2", "18363", "1909", "19H2", "November 2019 Update"},
    {"20h1", "19041", "2004", "20H1", "May 2020 Update"}
    // Comment out the further Windows 10 version
    //{"20H2", "xxxxx", "xxxx", "Manganese", "Fall 2020 Update"},
    //{"21H1", "xxxxx", "xxxx", "TBD", "Spring 2021 Update"},
    //{"21H2", "xxxxx", "xxxx", "TBD", "Fall 2021 Update"},
}};

std::string ToUtf8String(const std::wstring& wideCharStr)
{
    if (wideCharStr.empty())
    {
        return "";
    }
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideCharStr.data(), -1, nullptr, 0, nullptr, nullptr);
    std::string strToUtf8(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideCharStr.data(), -1, &strToUtf8[0], sizeNeeded, nullptr, nullptr);
    return strToUtf8;
}

// 1st: Codename / MarketName
std::pair<std::string, std::string> GetCodeNameAndMarketName(const std::string currentOSBuildNumber)
{
    auto itr = std::find_if(
        std::begin(s_Windows10Editions), std::end(s_Windows10Editions),
        [&currentOSBuildNumber](const WindowsEdition& edition) { return edition.BuildNumber == currentOSBuildNumber; });
    return itr != std::end(s_Windows10Editions) ? std::make_pair(itr->Codename, itr->MarketName) : std::make_pair("", "");
}
} // namespace detail

// When we meet error, we continue to fill the information as possible as we can to the return object.
OperatingSystemInfo OperatingSystemInfoFetcher::GetInformation()
{
    // Get OS info by WMI Win32_OperatingSystem.
    // Caption / OSArchitecture/ MUILanguage / OSLanguage / Locale / Version /
    // ServicePackMajorVersion /ServicePackMinorVersion
    WmiCimv2 wmi;
    OSInfo osInfo = wmi.GetOSInfo();

    OperatingSystemInfo result;
    result.Caption = detail::ToUtf8String(osInfo.Caption);
    result.OSArchitecture = detail::ToUtf8String(osInfo.OSArchitecture);
    result.MUILanguage = detail::ToUtf8String(osInfo.MUILanguage);
    result.OSLanguage = std::to_string(osInfo.OSLanguage);
    result.Locale = detail::ToUtf8String(osInfo.Locale);
    result.Version= detail::ToUtf8String(osInfo.Version);
    result.ServicePackMajorVersion = std::to_string(osInfo.ServicePackMajorVersion);
    result.ServicePackMinorVersion = std::to_string(osInfo.ServicePackMinorVersion);
    result.OSVersion = *result.Caption + "[" + *result.OSArchitecture + "OS][System Locale " + *result.MUILanguage + "]";

    WindowsReg reg;
    reg.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");

    // [HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // EditionID="Professional"
    std::string editionId;
    auto rv = reg.ReadStringValue("EditionID", editionId);
    result.EditionID = rv ? std::optional<std::string>{std::move(editionId)} : std::nullopt;

    // BuildBranch = "rs2_release"
    // (Windows 10 only, Codename information)
    std::string buildBranch;
    rv = reg.ReadStringValue("BuildBranch", buildBranch);
    result.BuildBranch = rv ? std::optional<std::string>{std::move(buildBranch)} : std::nullopt;

    // ReleaseId="1703"
    // (Windows 10 only)
    std::string releaseId;
    rv = reg.ReadStringValue("ReleaseId", releaseId);
    result.ReleaseId = rv ? std::optional<std::string>{std::move(releaseId)} : std::nullopt;

    // CurrentMajorVersionNumber=dword:0xa(10)
    // (Windows 10 only)
    uint32_t currentMajorVersionNumber = 0;
    rv = reg.ReadIntValue(L"CurrentMajorVersionNumber", currentMajorVersionNumber);
    result.CurrentMajorVersionNumber =
        rv ? std::optional<std::string>{std::to_string(currentMajorVersionNumber)} : std::nullopt;

    // CurrentMinorVersionNumber=dword:0x0(0)
    // (Windows 10 only)
    uint32_t currentMinorVersionNumber = 0;
    rv = reg.ReadIntValue(L"CurrentMinorVersionNumber", currentMinorVersionNumber);
    result.CurrentMinorVersionNumber =
        rv ? std::optional<std::string>{std::to_string(currentMinorVersionNumber)} : std::nullopt;

    // CurrentVersion="6.3"
    std::string currentVersion;
    rv = reg.ReadStringValue("CurrentVersion", currentVersion);
    result.CurrentVersion = rv ? std::optional<std::string>{std::move(currentVersion)} : std::nullopt;

    // CurrentBuildNumber="15063"
    std::string currentBuildNumber;
    rv = reg.ReadStringValue("CurrentBuildNumber", currentBuildNumber);
    result.CurrentBuildNumber = rv ? std::optional<std::string>{std::move(currentBuildNumber)} : std::nullopt;

    // UBR = dword:0x2a2(674)
    // Update Build Revision
    uint32_t UBR = 0;
    rv = reg.ReadIntValue(L"UBR", UBR);
    result.UBR = rv ? std::optional<std::string>{std::to_string(UBR)} : std::nullopt;

    // CSDVersion="Service Pack 1"
    // Service Pack name
    // (Windows 7 only, Service Pack information)
    std::string CSDVersion;
    rv = reg.ReadStringValue("CSDVersion", CSDVersion);
    result.CSDVersion = rv ? std::optional<std::string>{std::move(CSDVersion)} : std::nullopt;

    // CSDBuildNumber="1130"
    // Service Pack build number
    // (Windows 7 only, Service Pack information)
    std::string CSDBuildNumber;
    rv = reg.ReadStringValue("CSDBuildNumber", CSDBuildNumber);
    result.CSDBuildNumber = rv ? std::optional<std::string>{std::move(CSDBuildNumber)} : std::nullopt;

    if (result.CurrentMajorVersionNumber == "10" && result.CurrentBuildNumber) // Support Windows 10 only
    {
        auto [codeName, marketNAme] = detail::GetCodeNameAndMarketName(*result.CurrentBuildNumber);
        result.Codename = !codeName.empty() ? std::optional<std::string>{std::move(codeName)} : std::nullopt;
        result.MarketName = !marketNAme.empty() ? std::optional<std::string>{std::move(marketNAme)} : std::nullopt;
    }
    return result;
}
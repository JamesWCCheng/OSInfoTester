#pragma once

#include <optional>
#include <string>

struct OperatingSystemInfo
{
    // DDV makes this OS version string for hardware information summary data
    // ex. "Microsoft Windows 10 Pro[64-bit OS][System Locale en-US]"
    std::optional<std::string> OSVersion;

    // Win32_OperatingSystem::Caption
    // (string) "Microsoft Windows 10 Pro"
    // (string) "Microsoft Windows 8.1 Pro"
    // (string) "Microsoft Windows 7 Ultimate"
    std::optional<std::string> Caption;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // EditionID="Professional"
    std::optional<std::string> EditionID;

    // Win32_OperatingSystem::OSArchitecture
    // (string) "64-bit"
    std::optional<std::string> OSArchitecture;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // BuildBranch="rs2_release"
    // (Windows 10 only, optional summary)
    std::optional<std::string> BuildBranch;

    // DDV makes this mapping
    // ex. "RS2 (RedStone 2)"
    // (Windows 10 only)
    std::optional<std::string> Codename;

    // DDV makes this mapping
    // ex. "Creators Update"
    // (Windows 10 only)
    std::optional<std::string> MarketName;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // ReleaseId="1703"
    // (Windows 10 only)
    std::optional<std::string> ReleaseId;

    // Win32_OperatingSystem::Version
    // (string) "10.0.15063"
    std::optional<std::string> Version;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CurrentMajorVersionNumber=dword:10
    // (Windows 10 only)
    std::optional<std::string> CurrentMajorVersionNumber;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CurrentMinorVersionNumber=dword:0
    // (Windows 10 only)
    std::optional<std::string> CurrentMinorVersionNumber;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CurrentVersion="6.3"
    std::optional<std::string> CurrentVersion;

    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CurrentBuildNumber="15063"
    std::optional<std::string> CurrentBuildNumber;

    // Update Build Revision
    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // UBR=dword:0x2a2(674)
    std::optional<std::string> UBR;

    // Default system language. Get 1st string in MUILanguage(s) only.
    // Win32_OperatingSystem::MUILanguages[]
    // (string) "en-US"
    std::optional<std::string> MUILanguage;

    // Win32_OperatingSystem::OSLanguage
    // (uint32) 0409
    std::optional<std::string> OSLanguage;

    // Win32_OperatingSystem::Locale
    // (string) "1033"
    std::optional<std::string> Locale;

    // The name of the most recently installed service pack for the OS.
    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CSDVersion="Service Pack 1"
    // (Windows 7 only)
    std::optional<std::string> CSDVersion;

    // The build number of the most recently installed service pack for the OS.
    // [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion]
    // CSDBuildNumber="1130"
    // (Windows 7 only)
    std::optional<std::string> CSDBuildNumber;

    // Service Pack major version
    // Win32_OperatingSystem::ServicePackMajorVersion
    // (uint16) 1
    std::optional<std::string> ServicePackMajorVersion;

    // Service Pack minor version
    // Win32_OperatingSystem::ServicePackMinorVersion
    // (uint16) 0
    std::optional<std::string> ServicePackMinorVersion;
};

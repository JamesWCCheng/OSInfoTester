#include <OperatingSystemInfoFetcher.h>
#include <OperatingSystemInfoProvider.h>
#include <WindowsReg.h>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

ostream& operator<<(ostream& out, const OperatingSystemInfo& info)
{
    if (info.OSVersion)
    {
        out << "OSVersion =  " << *info.OSVersion << endl;
    }
    if (info.Caption)
    {
        out << "Caption = " << *info.Caption << endl;
    }
    if (info.EditionID)
    {
        out << "EditionID = " << *info.EditionID << endl;
    }
    if (info.OSArchitecture)
    {
        out << "OSArchitecture = " << *info.OSArchitecture << endl;
    }
    if (info.BuildBranch)
    {
        out << "BuildBranch = " << *info.BuildBranch << endl;
    }
    if (info.Codename)
    {
        out << "Codename = " << *info.Codename << endl;
    }
    if (info.MarketName)
    {
        out << "MarketName = " << *info.MarketName << endl;
    }
    if (info.ReleaseId)
    {
        out << "ReleaseId = " << *info.ReleaseId << endl;
    }
    if (info.Version)
    {
        out << "Version = " << *info.Version << endl;
    }
    if (info.CurrentMajorVersionNumber)
    {
        out << "CurrentMajorVersionNumber = " << *info.CurrentMajorVersionNumber << endl;
    }
    if (info.CurrentMinorVersionNumber)
    {
        out << "CurrentMinorVersionNumber = " << *info.CurrentMinorVersionNumber << endl;
    }
    if (info.CurrentVersion)
    {
        out << "CurrentVersion = " << *info.CurrentVersion << endl;
    }
    if (info.CurrentBuildNumber)
    {
        out << "CurrentBuildNumber = " << *info.CurrentBuildNumber << endl;
    }
    if (info.UBR)
    {
        out << "UBR = " << *info.UBR << endl;
    }
    if (info.MUILanguage)
    {
        out << "MUILanguage = " << *info.MUILanguage << endl;
    }
    if (info.OSLanguage)
    {
        out << "OSLanguage = " << *info.OSLanguage << endl;
    }
    if (info.Locale)
    {
        out << "Locale = " << *info.Locale << endl;
    }
    if (info.CSDVersion)
    {
        out << "CSDVersion = " << *info.CSDVersion << endl;
    }
    if (info.CSDBuildNumber)
    {
        out << "CSDBuildNumber = " << *info.CSDBuildNumber << endl;
    }
    if (info.ServicePackMajorVersion)
    {
        out << "ServicePackMajorVersion = " << *info.ServicePackMajorVersion << endl;
    }
    if (info.ServicePackMinorVersion)
    {
        out << "ServicePackMinorVersion = " << *info.ServicePackMinorVersion << endl;
    }
    return out;
}

int main()
{
    SetConsoleOutputCP( CP_UTF8 );
    setvbuf( stdout, nullptr, _IONBF, 0 );
    // Get OS Information
    OperatingSystemInfoProvider provider(std::make_unique<OperatingSystemInfoFetcher>());
    auto osInfo = provider.GetInformation();

    cout << osInfo << endl;

    system("PAUSE");
    return 0;
}

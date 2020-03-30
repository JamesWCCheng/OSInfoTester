#pragma once

#include <memory>
#include <vector>
#include "OperatingSystemInfo.h"

class IOperatingSystemInfoFetcher;

class OperatingSystemInfoProvider final
{
public:
    OperatingSystemInfoProvider(std::unique_ptr<IOperatingSystemInfoFetcher> fetcher) noexcept;
    ~OperatingSystemInfoProvider();
    OperatingSystemInfoProvider(OperatingSystemInfoProvider&&) = delete;
    OperatingSystemInfoProvider(const OperatingSystemInfoProvider&) = delete;
    OperatingSystemInfoProvider& operator=(const OperatingSystemInfoProvider&) = delete;
    OperatingSystemInfoProvider& operator=(OperatingSystemInfoProvider&&) = delete;

    OperatingSystemInfo GetInformation();

private:
    std::unique_ptr<IOperatingSystemInfoFetcher> m_Fetcher;
};
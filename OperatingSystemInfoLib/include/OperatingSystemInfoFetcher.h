#pragma once

#include "IOperatingSystemInfoFetcher.h"
#include "OperatingSystemInfo.h"
#include <vector>

class OperatingSystemInfoFetcher final : public IOperatingSystemInfoFetcher
{
public:
    OperatingSystemInfoFetcher() noexcept = default;
    ~OperatingSystemInfoFetcher() override = default;
    OperatingSystemInfoFetcher(OperatingSystemInfoFetcher&&) = delete;
    OperatingSystemInfoFetcher(const OperatingSystemInfoFetcher&) = delete;
    OperatingSystemInfoFetcher& operator=(const OperatingSystemInfoFetcher&) = delete;
    OperatingSystemInfoFetcher& operator=(OperatingSystemInfoFetcher&&) = delete;

    OperatingSystemInfo GetInformation() override;
};

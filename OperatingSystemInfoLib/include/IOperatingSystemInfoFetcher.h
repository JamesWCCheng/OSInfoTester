#pragma once

#include <vector>
#include "OperatingSystemInfo.h"

class IOperatingSystemInfoFetcher
{
public:
    virtual ~IOperatingSystemInfoFetcher() = default;
    virtual OperatingSystemInfo GetInformation() = 0;
};
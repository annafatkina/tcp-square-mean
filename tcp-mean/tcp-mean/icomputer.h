#pragma once

#include "logger.h"
#include "numberdumper.h"
#include <cstdint>
#include <memory>

class IComputer {
    // This class is an interface for the server main computational function.

  public:
    // Return the result of computation.
    virtual double updateAndCompute(uint32_t value) = 0;

    // Destroy this object.
    virtual ~IComputer() = default;
};

// Return shared ptr to a new mean computer object for the range from 0 to the
// specified 'max'.
std::shared_ptr<IComputer>
createMeanComputer(std::shared_ptr<NumberDumper> dumper,
                   std::shared_ptr<Logger> logger, uint32_t max);

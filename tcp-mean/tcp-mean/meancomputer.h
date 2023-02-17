#pragma once

#include "icomputer.h"
#include "logger.h"
#include "numberdumper.h"
#include <shared_mutex>
#include <vector>

class MeanComputer : public IComputer {
    // This class provides a mechanism for computation of mean of squares of
    // values received by server in a given range.

    mutable std::shared_mutex     numbersMutex_;
    double                        mean_;
    uint32_t                      numOfNumbers_;
    double                        sumOfNumbers_;
    uint32_t                      max_;
    std::vector<bool>             numbers_;
    std::shared_ptr<NumberDumper> dumper_;
    std::shared_ptr<Logger>       logger_;

    // Add the specified 'value' to a list of received numbers and recompute the
    // mean of squares of received values.
    void addAndRecompute(uint32_t value);

    // Chechk if the specified 'value' already taken into account, recompute the
    // mean if not.
    void addValue(uint32_t value);

    // Get current mean value.
    double getMean();

  public:
    // NOT IMPLEMENTED
    MeanComputer(const MeanComputer &) = delete;
    MeanComputer(MeanComputer &&)      = delete;

    // Create 'MeanComputer' object with the optionally specified 'max' range
    // value.
    MeanComputer(std::shared_ptr<NumberDumper> dumper, std::shared_ptr<Logger>,
                 uint32_t                      max = 1023);

    // Update the mean with the specified 'value' if needed and return the
    // current mean value.
    double updateAndCompute(uint32_t value) override;
};

#pragma once

#include "icomputer.h"
#include <shared_mutex>
#include <vector>

class MeanComputer : public IComputer {
    // This class provides a mechanism for computation of mean of squares of
    // values received by server in a given range.

    mutable std::shared_mutex numbersMutex_;
    volatile double           mean_;
    volatile uint32_t         numOfNumbers_;
    volatile uint32_t         sumOfNumbers_;
    uint32_t                  max_;
    std::vector<bool>         numbers_;

    // Add the specified 'value' to a list of received numbers and recompute the
    // mean of squares of received values.
    void addAndRecompute(uint32_t value);

    // Chechk if the specified 'value' already taken into account, recompute the
    // mean if not.
    void addValue(uint32_t value);

    // Get current mean value.
    uint32_t getMean();

  public:
    // Create 'MeanComputer' object with the optionally specified 'max' range
    // value.
    MeanComputer(uint32_t max = 1023);

    // Update the mean with the specified 'value' if needed and return the
    // current mean value.
    double updateAndCompute(uint32_t value) override;
};

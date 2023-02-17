#include "meancomputer.h"
#include "icomputer.h"
#include "logger.h"
#include <atomic>
#include <iostream>

void MeanComputer::addAndRecompute(uint32_t value) {
    std::unique_lock<std::shared_mutex> lock(numbersMutex_);
    if (numbers_[value]) {
        // In case of a race this can be already update with another thread by
        // the moment we lock it with unique lock, so let's check it again under
        // the unique lock and do nothing if this happend.
        return;
    }

    numbers_[value] = true;
    dumper_->push(value);

    numOfNumbers_++;
    sumOfNumbers_ += value * value;
    mean_ = sumOfNumbers_ / (double)numOfNumbers_;
}

void MeanComputer::addValue(uint32_t value) {
    std::shared_lock<std::shared_mutex> lock(numbersMutex_);
    if (value >= numbers_.size() || value < 0) {
        logger_->printWarning("Server received number ", value,
                              " which is greater than maximum defined value ",
                              numbers_.size(), ". Ignore this value. ");
        return;
    }

    if (!numbers_[value]) {
        lock.unlock();
        addAndRecompute(value);
    }
}

double MeanComputer::getMean() {
    std::shared_lock<std::shared_mutex> lock(numbersMutex_);
    return mean_;
}

MeanComputer::MeanComputer(std::shared_ptr<NumberDumper> dumper,
                           std::shared_ptr<Logger> logger, uint32_t max)
    : mean_(0)
    , max_(max)
    , numOfNumbers_(0)
    , sumOfNumbers_(0)
    , numbers_(max_ + 1, false)
    , dumper_(dumper)
    , logger_(logger) {
}

double MeanComputer::updateAndCompute(uint32_t value) {
    try {
        addValue(value);
        return getMean();
    } catch (const std::exception &e) {
        logger_->printError("Error while update the mean value: ", e.what());
    }
}

std::shared_ptr<IComputer>
createMeanComputer(std::shared_ptr<NumberDumper> dumper, std::shared_ptr<Logger> logger, uint32_t max = 1023) {
    return std::make_shared<MeanComputer>(dumper, logger, max);
}

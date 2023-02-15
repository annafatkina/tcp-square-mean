#include "meancomputer.h"
#include "icomputer.h"
#include <atomic>
#include <iostream>

void MeanComputer::addAndRecompute(uint32_t value) {
	//std::unique_lock<std::shared_mutex> lock(numbersMutex_);

	if (numbers_[value]) {
		// In case of a race this can be already update with another thread by the 
		// moment we lock it with unique lock, so let's check it again under the
		// unique lock and do nothing if this happend.
		return;
	}

	numbers_[value] = true;
	numOfNumbers_++;
	sumOfNumbers_ += value * value;
	mean_ = (double)sumOfNumbers_ / (double)numOfNumbers_;

}

void MeanComputer::addValue(uint32_t value) {
	
	std::shared_lock<std::shared_mutex> lock(numbersMutex_);
	if (!numbers_[value]) {
		lock.release();
		addAndRecompute(value);
	}
}

uint32_t MeanComputer::getMean() {
	std::shared_lock<std::shared_mutex> lock(numbersMutex_);
	return mean_;
}

MeanComputer::MeanComputer(uint32_t max)
	: mean_(0)
	, max_(max)
	, numOfNumbers_(0)
	, sumOfNumbers_(0)
	, numbers_(max_ + 1, false) {
}


double MeanComputer::updateAndCompute(uint32_t value) {
	try {

	addValue(value);
	return getMean();
	}
	catch (const std::exception& e) {
		std::cout << "cought ex: " << e.what();
	}
}

std::shared_ptr<IComputer> createMeanComputer(uint32_t max = 1023) {
	return std::make_shared<MeanComputer>(max);
}

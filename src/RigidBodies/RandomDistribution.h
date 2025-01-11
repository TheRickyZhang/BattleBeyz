/////////////////////////////////////////////////////////////////////////////
// RandomDistribution.h -- Beyblade include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <random>

#include "Units.h"
using namespace Units;

/**
 * Represents a lognormal ditribution for use in calculating recoil strength. Scales both linear and angular impulse.
 * @param mean, stddev: the mean and standard deviation of the distribution. Scaled so still applies to lognormal distribution.
 */
class RandomDistribution {
public:
    RandomDistribution(Scalar mean = Scalar(1.0f), Scalar stddev = Scalar(0.1f))
        : mean(mean), stddev(stddev)
    {
        setDistribution(mean, stddev);
    }

    /**
    * Samples the distribution. Only provides the "randomness" of the impact: actual recoil will depend on many other factors
    */
    Scalar sample() {
        return Scalar(distribution(rng));
    }

    Scalar getMean()   const { return mean; }
    Scalar getStdDev() const { return stddev; }

    void setMean(Scalar m) {
        mean = m;
        setDistribution(mean, stddev);
    }

    void setStdDev(Scalar s) {
        stddev = s;
        setDistribution(mean, stddev);
    }

private:
    // Declare static inline to save memory
    static inline std::random_device rd;
    static inline std::mt19937 rng{ rd() };

    void setDistribution(Scalar mean, Scalar stddev) {
        if (mean.value() <= 0) {
            throw std::invalid_argument("Mean must be positive.");
        }
        if (stddev.value() <= 0) {
            throw std::invalid_argument("Standard deviation must be positive.");
        }
        float mu = std::log(mean.value() * mean.value() / std::sqrt(stddev.value() * stddev.value() + mean.value() * mean.value()));
        float sigma = std::sqrt(std::log(1 + (stddev.value() * stddev.value()) / (mean.value() * mean.value())));
        distribution = std::lognormal_distribution<float>(mu, sigma);
    }

    Scalar mean;
    Scalar stddev;
    std::lognormal_distribution<float> distribution;
};
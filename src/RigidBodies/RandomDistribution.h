/////////////////////////////////////////////////////////////////////////////
// RandomDistribution.h -- Beyblade include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RandomDistribution.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <random>

/**
 * Represents a lognormal ditribution for use in calculating recoil strength. Scales both linear and angular impulse.
 * @param mean, stddev: the mean and standard deviation of the distribution. Scaled so still applies to lognormal distribution.
 */
class RandomDistribution {
public:
    RandomDistribution(float mean = 1.0f, float stddev = 0.1f)
        : mean(mean),
        stddev(stddev),
        rng(std::random_device{}())
    {
        setDistribution(mean, stddev);
    }

    /**
    * Samples the distribution. Only provides the "randomness" of the impact: actual recoil will depend on many other factors
    */
    float sample() {
        return distribution(rng);
    }

    float getMean()   const { return mean; }
    float getStdDev() const { return stddev; }

    void setMean(float m) {
        mean = m;
        setDistribution(mean, stddev);
    }

    void setStdDev(float s) {
        stddev = s;
        setDistribution(mean, stddev);
    }

private:
    void setDistribution(float mean, float stddev) {
        if (mean <= 0) {
            throw std::invalid_argument("Mean must be positive.");
        }
        if (stddev <= 0) {
            throw std::invalid_argument("Standard deviation must be positive.");
        }
        float mu = std::log(mean * mean / std::sqrt(stddev * stddev + mean * mean));
        float sigma = std::sqrt(std::log(1 + (stddev * stddev) / (mean * mean)));
        distribution = std::lognormal_distribution<float>(mu, sigma);
    }

    float mean;
    float stddev;
    std::mt19937 rng;
    std::lognormal_distribution<float> distribution;
};
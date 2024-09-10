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
    RandomDistribution(double mean, double stddev)
        : rng(std::random_device{}())
    {
        double mu = std::log(mean * mean / std::sqrt(stddev * stddev + mean * mean));
        double sigma = std::sqrt(std::log(1 + (stddev * stddev) / (mean * mean)));
        distribution = std::lognormal_distribution<double>(mu, sigma);
    }

    /**
    * Samples the distribution. Only provides the "randomness" of the impact: actual recoil will depend on many other factors
    */
    double sample() {
        return distribution(rng);
    }

private:
    std::mt19937 rng;
    std::lognormal_distribution<double> distribution;
};
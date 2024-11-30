////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RandomDistribution.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

/**
 * Contains physical properties of the top section of a Beyblade, important for collisions.
 * 
 * @param recoilDistribution: the distribution of recoil forces
 * @param coefficientOfRestitution: how much energy is conserverd in a collision
 * RotationalDragCoefficient: dragCoefficient*averageRadiusExtending, should be between 0.001 and 0.015
 */
struct Layer {
    // TODO: Alter constructor to take in the mean and stddev
    Layer(double radius, double height,
        RandomDistribution* recoilDistribution,
        double coefficientOfRestitution,
        double rotationalDragCoefficient,
        double mass,
        glm::vec3 velocity,
        glm::vec3 acceleration,
        double momentOfInertia) :
        radius(radius),
        height(height),
        recoilDistribution(recoilDistribution),
        coefficientOfRestitution(coefficientOfRestitution),
        rotationalDragCoefficient(rotationalDragCoefficient),
        mass(mass),
        momentOfInertia(momentOfInertia) {}

    Layer() : 
        radius(0.025),
        height(0.01),
        coefficientOfRestitution(0.8),
        rotationalDragCoefficient(0.7*0.005),
        mass(0.022),
        momentOfInertia(0.5 * 0.022 * 0.025 * 0.025)
{
        recoilDistributionMean = 1.0;
        recoileDistributionStdDev = 0.1;
        recoilDistribution = new RandomDistribution(recoilDistributionMean, recoileDistributionStdDev);
}

    double radius;
    double height;
    double mass;
    double momentOfInertia;
    double recoilDistributionMean;
    double recoileDistributionStdDev;
    double rotationalDragCoefficient;
    RandomDistribution* recoilDistribution;
    double coefficientOfRestitution;
};

/**
 * Contains physical properties of the middle section of a Beyblade, not too important except for contributing weight.
 */
struct Disc {
    Disc(double radius, double height, double mass, double momentOfInertia, double rotationalDragCoefficient)
        : radius(radius), height(height), mass(mass), momentOfInertia(momentOfInertia), rotationalDragCoefficient(rotationalDragCoefficient) {}
    Disc()
        : radius(0.018),
        height(0.01),
        mass(0.027),
        momentOfInertia(0.7 * 0.027 * 0.018 * 0.018),
        rotationalDragCoefficient(0.1*0.005) {}
    double radius;
    double height;
    double mass;
    double momentOfInertia;
    double rotationalDragCoefficient;
};

/**
 *Contains physical properties of the bottom section of a Beyblade, important for friction and movement.
 */
struct Driver {
    Driver(double radius, double height, double mass, double momentOfInertia, double rotationalDragCoefficient, double coefficientOfFriction)
        : radius(radius), height(height), mass(mass), momentOfInertia(momentOfInertia),
        rotationalDragCoefficient(rotationalDragCoefficient), coefficientOfFriction(coefficientOfFriction) {}
    Driver()
        : radius(0.002),
        height(0.015),
        mass(0.005),
        momentOfInertia(0.5 * 0.005 * 0.002 * 0.0015),
        rotationalDragCoefficient(0.1*0.005),
        coefficientOfFriction(0.22)
{}
    double radius;
    double height;
    double mass;
    double momentOfInertia;
    double rotationalDragCoefficient;
    double coefficientOfFriction;
};
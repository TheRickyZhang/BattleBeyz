////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RandomDistribution.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Units.h"
using namespace Units;

/**
 * Contains physical properties of the top section of a Beyblade, important for collisions.
 * 
 * @param coefficientOfRestitution: how much energy is conserverd in a collision
 * RotationalDragCoefficient: dragCoefficient*averageRadiusExtending, should be between 0.001 and 0.015
 */
struct Layer {
    Layer(M radius, M height,
        Scalar recoilMean, Scalar recoilStddev,
        Scalar coefficientOfRestitution,
        Scalar rotationalDragCoefficient,
        Kg mass,
        KgM2 momentOfInertia) :
        radius(radius),
        height(height),
        recoilDistribution(RandomDistribution(recoilMean, recoilStddev)),
        coefficientOfRestitution(coefficientOfRestitution),
        rotationalDragCoefficient(rotationalDragCoefficient),
        mass(mass),
        momentOfInertia(momentOfInertia)
    {}

    Layer() :
        radius(0.025_m),
        height(0.01_m),
        coefficientOfRestitution(0.8f),
        rotationalDragCoefficient(0.7f * 0.005f),
        mass(0.022_kg),
        momentOfInertia(0.5f * 0.022_kg * 0.025_m * 0.025_m),
        recoilDistribution(RandomDistribution())
    {}

    M radius;
    M height;
    Kg mass;
    KgM2 momentOfInertia;
    Scalar rotationalDragCoefficient;
    RandomDistribution recoilDistribution; // NOTE: This is 5000 bytes so avoid copying layer directly, using unique_ptr<Layer> will be ideal
    Scalar coefficientOfRestitution;
};


/**
 * Contains physical properties of the middle section of a Beyblade, not too important except for contributing weight.
 */
struct Disc {
    Disc(M radius, M height, Kg mass, KgM2 momentOfInertia, Scalar rotationalDragCoefficient) :
        radius(radius), height(height), mass(mass), momentOfInertia(momentOfInertia), rotationalDragCoefficient(rotationalDragCoefficient)
    {}

    Disc() :
        radius(0.018_m),
        height(0.01_m),
        mass(0.027_kg),
        momentOfInertia(0.7f * 0.027_kg * 0.018_m * 0.018_m),
        rotationalDragCoefficient(0.1f * 0.005f)
    {}

    M radius;
    M height;
    Kg mass;
    KgM2 momentOfInertia;
    Scalar rotationalDragCoefficient;
};


/**
 *Contains physical properties of the bottom section of a Beyblade, important for friction and movement.
 */
struct Driver {
    Driver(M radius, M height, Kg mass, KgM2 momentOfInertia, Scalar rotationalDragCoefficient, Scalar coefficientOfFriction) :
        radius(radius),
        height(height),
        mass(mass),
        momentOfInertia(momentOfInertia),
        rotationalDragCoefficient(rotationalDragCoefficient),
        coefficientOfFriction(coefficientOfFriction)
    {}

    Driver() :
        radius(0.002_m),
        height(0.015_m),
        mass(0.005_kg),
        momentOfInertia(0.5f * 0.005_kg * 0.002_m * 0.0015_m),
        rotationalDragCoefficient(0.1f * 0.005f),
        coefficientOfFriction(0.22f)
    {}

    M radius;
    M height;
    Kg mass;
    KgM2 momentOfInertia;
    Scalar rotationalDragCoefficient;
    Scalar coefficientOfFriction;
};
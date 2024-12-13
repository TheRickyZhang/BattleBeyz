////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RandomDistribution.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <memory>

/**
 * Contains physical properties of the top section of a Beyblade, important for collisions.
 * 
 * @param coefficientOfRestitution: how much energy is conserverd in a collision
 * RotationalDragCoefficient: dragCoefficient*averageRadiusExtending, should be between 0.001 and 0.015
 */
struct Layer {
    Layer(float radius, float height,
        float recoilMean, float recoilStddev,
        float coefficientOfRestitution,
        float rotationalDragCoefficient,
        float mass,
        glm::vec3 velocity,
        glm::vec3 acceleration,
        float momentOfInertia) :
        radius(radius),
        height(height),
        recoilDistribution(RandomDistribution(recoilMean, recoilStddev)),
        coefficientOfRestitution(coefficientOfRestitution),
        rotationalDragCoefficient(rotationalDragCoefficient),
        mass(mass),
        momentOfInertia(momentOfInertia) {}

    Layer() :
        radius(0.025),
        height(0.01),
        coefficientOfRestitution(0.8),
        rotationalDragCoefficient(0.7 * 0.005),
        mass(0.022),
        momentOfInertia(0.5 * 0.022 * 0.025 * 0.025),
        recoilDistribution(RandomDistribution()) {}

    float radius;
    float height;
    float mass;
    float momentOfInertia;
    float rotationalDragCoefficient;
    RandomDistribution recoilDistribution; // NOTE: This is 5000 bytes so avoid copying layer directly, using unique_ptr<Layer> will be ideal
    float coefficientOfRestitution;
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
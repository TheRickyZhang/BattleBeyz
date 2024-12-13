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
        radius(0.025f),
        height(0.01f),
        coefficientOfRestitution(0.8f),
        rotationalDragCoefficient(0.7f * 0.005f),
        mass(0.022f),
        momentOfInertia(0.5f * 0.022f * 0.025f * 0.025f),
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
    Disc(float radius, float height, float mass, float momentOfInertia, float rotationalDragCoefficient)
        : radius(radius), height(height), mass(mass), momentOfInertia(momentOfInertia), rotationalDragCoefficient(rotationalDragCoefficient) {}
    Disc()
        : radius(0.018f),
        height(0.01f),
        mass(0.027f),
        momentOfInertia(0.7f * 0.027f * 0.018f * 0.018f),
        rotationalDragCoefficient(0.1f*0.005f) {}
    float radius;
    float height;
    float mass;
    float momentOfInertia;
    float rotationalDragCoefficient;
};

/**
 *Contains physical properties of the bottom section of a Beyblade, important for friction and movement.
 */
struct Driver {
    Driver(float radius, float height, float mass, float momentOfInertia, float rotationalDragCoefficient, float coefficientOfFriction)
        : radius(radius), height(height), mass(mass), momentOfInertia(momentOfInertia),
        rotationalDragCoefficient(rotationalDragCoefficient), coefficientOfFriction(coefficientOfFriction) {}
    Driver()
        : radius(0.002f),
        height(0.015f),
        mass(0.005f),
        momentOfInertia(0.5f * 0.005f * 0.002f * 0.0015f),
        rotationalDragCoefficient(0.1f*0.005f),
        coefficientOfFriction(0.22f)
{}
    float radius;
    float height;
    float mass;
    float momentOfInertia;
    float rotationalDragCoefficient;
    float coefficientOfFriction;
};
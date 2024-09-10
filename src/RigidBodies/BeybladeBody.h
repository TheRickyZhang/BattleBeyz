////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BeybladeParts.h"
#include "../BoundingBox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <optional>
#include "../Utils.h"

/**
 * BeybladeBody. Contains all of the physical properties of a beyblade.
 * 
 * Can be initialized with default values or with Layer, Disc, and Driver objects.	See typical SI values in units.txt
 */
class BeybladeBody {
public:
	BeybladeBody::BeybladeBody(Layer layer, Disc disc, Driver driver);
	BeybladeBody::BeybladeBody();

	// Simple getters
	glm::vec3 BeybladeBody::getCenter() const { return baseCenter; }
	glm::vec3 BeybladeBody::getVelocity() const { return velocity; }
	glm::vec3 BeybladeBody::getAngularVelocity() const { return angularVelocity; }
	double BeybladeBody::getLayerHeight() const { return layerHeight; }
	double BeybladeBody::getMass() const { return mass; }
	double BeybladeBody::getMomentOfInertia() const { return momentOfInertia; }
	double BeybladeBody::getDriverCOF() const { return coefficientOfFriction; }
	double BeybladeBody::getDriverRadius() const { return driverRadius; }
	double BeybladeBody::getLayerCOR() const { return coefficientOfRestitution; }
	double BeybladeBody::getLayerRadius() const { return layerRadius; }
	double BeybladeBody::getDiscRadius() const { return discRadius; }
	double BeybladeBody::getDiscHeight() const { return discHeight; }
	double BeybladeBody::getDriverHeight() const { return driverHeight; }

	double BeybladeBody::getLinearDragTerm() const { return linearDragTerm; }
	double BeybladeBody::getAngularDragTerm() const { return angularDragTerm; }

	// TODO: Need to distinguish between the top and bottom of the driver, or driverRadiusTop and driverRadiusBottom
	double BeybladeBody::getDriverTopRadius() const { return 0.012; }
	// TODO: Add linearDragCoefficient (low priority, currently assumed to be constant 0.9)

	// Specialized getters
	double BeybladeBody::getAngularVelocityMagnitude() const { return glm::length(angularVelocity); }
	bool isSpinningClockwise() const { return angularVelocity.y < 0; }
	glm::vec3 BeybladeBody::getNormal() const;
	glm::vec3 BeybladeBody::getBottomPosition() const;

	// Setters
	void BeybladeBody::setInitialLaunch(glm::vec3 initialCenter, glm::vec3 initialVelocity, glm::vec3 initialAngularVelocity);

	// Adjustors
	void BeybladeBody::addCenterY(double addY) { baseCenter.y += static_cast<float>(addY); }
	void BeybladeBody::addCenterXZ(double addX, double addZ) { baseCenter.x += static_cast<float>(addX); baseCenter.z += static_cast<float>(addZ); }
	void BeybladeBody::setCenterY(double addY) { baseCenter.y = static_cast<float>(addY); }
	void BeybladeBody::setVelocity(glm::vec3 newVelocity) { velocity = newVelocity; }
	void BeybladeBody::setVelocityY(double newY) { velocity.y = static_cast<float>(newY); }

	// Used in collision calculations
	double sampleRecoil();
	static std::optional<double> distanceOverlap(BeybladeBody* a, BeybladeBody* b);

	// Accumulators
	void accumulateVelocity(glm::vec3 addedVelocity);
	void accumulateAngularVelocity(glm::vec3 addedAngularVelocity);
	void accumulateAcceleration(glm::vec3 addedAcceleration);
	void accumulateAngularAcceleration(glm::vec3 addedAngularAcceleration);

	void accumulateImpulseMagnitude(double magnitude);
	void accumulateAngularImpulseMagnitude(double magnitude);

	// Updators: these are the ones that significantly change the values of the body!
	void applyAccumulatedChanges(double deltaTime);
	void update(double deltaTime);

	std::vector<BoundingBox*> boundingBoxes{};
private:
	// Global Position
	glm::vec3 baseCenter{};

	// Measurements
	double layerRadius;
	double layerHeight;
	double discRadius;
	double discHeight;
	double driverRadius;
	double driverHeight;

	// Contact Properties
	RandomDistribution* recoilDistribution;
	double coefficientOfRestitution;
	double coefficientOfFriction;

	// Linear Physics
	double mass;
	glm::vec3 velocity{};
	glm::vec3 acceleration{};

	// Rotational Physics
	double momentOfInertia;
	glm::vec3 angularVelocity{ 0.0, 1.0, 0.0 };
	glm::vec3 angularAcceleration{};
	double linearDragTerm; // Sum of Cd*A for parts 
	double angularDragTerm; // Sum of Cd*A*r^2 for parts

	// Accumulated delta velocity to be applied at cycle end (for instantaneous collisions, does not depend on deltaTime)
	glm::vec3 accumulatedVelocity {};
	glm::vec3 accumulatedAngularVelocity {};

	// Accumulated delta Acceleration to be applied at cycle end (for forces like friction and inclined plane that do depend on time)
	glm::vec3 accumulatedAcceleration {};
	glm::vec3 accumulatedAngularAcceleration{};
};
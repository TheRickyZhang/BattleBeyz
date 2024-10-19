////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <optional>

#include "BeybladeParts.h"
#include "BoundingBox.h"
#include "MeshObjects/BeybladeMesh.h"  // NEWMESH Added this, changed constructor definitions.
#include "Utils.h"

/**
 * BeybladeBody. Contains all of the physical properties of a beyblade.
 * 
 * Can be initialized with default values or with Layer, Disc, and Driver objects.	See typical SI values in units.txt
 */
class BeybladeBody {
public:
	// Two options for constructing: Do entirely by parts OR by mesh + stats

	BeybladeBody::BeybladeBody();
	BeybladeBody::BeybladeBody(Layer layer, Disc disc, Driver driver);

	// NEWMESH: Would it be possible to initialize the body entirely based off of the mesh + a few additional custom inputs? 
	// Instead of layer, disc, driver, etc. it would be only the additional variables needed
	BeybladeBody::BeybladeBody(BeybladeMesh* mesh, Layer layer, Disc disc, Driver driver);

	// Simple getters.
	// RZ:  Theoretically good style, but there are too many!  Just make the variables public!
	// ALSO, these class qualifiers are overkill.

	double BeybladeBody::getAngularDragTerm() const { return angularDragTerm; }
	glm::vec3 BeybladeBody::getAngularVelocity() const { return angularVelocity; }
	glm::vec3 BeybladeBody::getCenter() const { return baseCenter; }
	double BeybladeBody::getDiscHeight() const { return disc.height; }
	double BeybladeBody::getDiscMass() const { return disc.mass;  };
	double BeybladeBody::getDiscMomentOfInertia() const { return disc.momentOfInertia; }
	double BeybladeBody::getDiscRadius() const { return disc.radius; }
	double BeybladeBody::getDriverCOF() const { return driver.coefficientOfFriction; }
	double BeybladeBody::getDriverHeight() const { return driver.height; }
	double BeybladeBody::getDriverMass() const { return driver.mass; };
	double BeybladeBody::getDriverMomentOfInertia() const { return driver.momentOfInertia; }
	double BeybladeBody::getDriverRadius() const { return driver.radius; }
	double BeybladeBody::getLayerCOR() const { return layer.coefficientOfRestitution; }
	double BeybladeBody::getLayerHeight() const { return driver.height; }
	double BeybladeBody::getLayerMass() const { return layer.mass; };
	double BeybladeBody::getLayerMomentOfInertia() const { return layer.momentOfInertia; }
	double BeybladeBody::getLayerRadius() const { return layer.radius; }
	double BeybladeBody::getLayerRecoilDistributionMean() const { return layer.recoilDistributionMean; }
	double BeybladeBody::getLayerRecoilDistributionStdDev() const { return layer.recoileDistributionStdDev; }
	double BeybladeBody::getLinearDragTerm() const { return linearDragTerm; }
	double BeybladeBody::getMass() const { return mass; } // Total mass
	double BeybladeBody::getMomentOfInertia() const { return momentOfInertia; }
	glm::vec3 BeybladeBody::getVelocity() const { return velocity; }


	// TODO: Need to distinguish between the top and bottom of the driver, or driverRadiusTop and driverRadiusBottom
	double BeybladeBody::getDriverTopRadius() const { return 0.012; }
	// TODO: Add linearDragCoefficient (low priority, currently assumed to be constant 0.9)

	// Specialized getters
	double BeybladeBody::getAngularVelocityMagnitude() const { return glm::length(angularVelocity); }
	bool isSpinningClockwise() const { return angularVelocity.y < 0; }
	glm::vec3 BeybladeBody::getNormal() const;
	glm::vec3 BeybladeBody::getBottomPosition() const;

	// Setters  // NEWUI adds several members.
	void BeybladeBody::setInitialLaunch(glm::vec3 initialCenter, glm::vec3 initialVelocity, glm::vec3 initialAngularVelocity);
	void BeybladeBody::setDiscMass(double _mass) { disc.mass = _mass; }
	void BeybladeBody::setDiscMomentOfInertia(double _moi) { disc.momentOfInertia = _moi; }
	void BeybladeBody::setDriverCOF(double _cof) { driver.coefficientOfFriction = _cof;  }
	void BeybladeBody::setDriverMass(double _mass) { driver.mass = _mass; }
	void BeybladeBody::setDriverMomentOfInertia(double _moi) { driver.momentOfInertia = _moi; }
	void BeybladeBody::setLayerCoefficientOfRestitution(double _cor) { layer.coefficientOfRestitution = _cor; }
	void BeybladeBody::setLayerMass(double _mass) { layer.mass = _mass; }
	void BeybladeBody::setLayerMomentOfInertia(double _moi) { layer.momentOfInertia = _moi; }
	void BeybladeBody::setLayerRecoilDistribution(double mean, double stddev) {
		layer.recoilDistributionMean = mean;
		layer.recoileDistributionStdDev = stddev;
		delete layer.recoilDistribution;
		layer.recoilDistribution = new RandomDistribution(mean, stddev);
	}
	void BeybladeBody::setMass(double _mass) { mass = _mass; }  // Total mass
	void BeybladeBody::setMomentOfInertia(double _totalMOI) { momentOfInertia = _totalMOI; }

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

	// Parts - Access individual variables through these!
	Disc disc;
	Driver driver;
	Layer layer;

	// Linear Physics
	double mass;
	glm::vec3 velocity{};
	glm::vec3 acceleration{};

	// Rotational Physics
	double momentOfInertia;  // This is the total for all parts
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
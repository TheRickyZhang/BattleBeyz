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

	BeybladeBody();
	BeybladeBody(Layer layer, Disc disc, Driver driver);

	// NEWMESH: Would it be possible to initialize the body entirely based off of the mesh + a few additional custom inputs? 
	// Instead of layer, disc, driver, etc. it would be only the additional variables needed
	BeybladeBody(BeybladeMesh* mesh, Layer layer, Disc disc, Driver driver);

	// Simple getters.
	// RZ:  Theoretically good style, but there are too many!  Just make the variables public!

	double getAngularDragTerm() const { return angularDragTerm; }
	glm::vec3 getAngularVelocity() const { return angularVelocity; }
	glm::vec3 getCenter() const { return baseCenter; }
	double getDiscHeight() const { return disc.height; }
	double getDiscMass() const { return disc.mass;  };
	double getDiscMomentOfInertia() const { return disc.momentOfInertia; }
	double getDiscRadius() const { return disc.radius; }
	double getDriverCOF() const { return driver.coefficientOfFriction; }
	double getDriverHeight() const { return driver.height; }
	double getDriverMass() const { return driver.mass; };
	double getDriverMomentOfInertia() const { return driver.momentOfInertia; }
	double getDriverRadius() const { return driver.radius; }
	double getLayerCOR() const { return layer.coefficientOfRestitution; }
	double getLayerHeight() const { return driver.height; }
	double getLayerMass() const { return layer.mass; };
	double getLayerMomentOfInertia() const { return layer.momentOfInertia; }
	double getLayerRadius() const { return layer.radius; }
	double getLayerRecoilDistributionMean() const { return layer.recoilDistributionMean; }
	double getLayerRecoilDistributionStdDev() const { return layer.recoileDistributionStdDev; }
	double getLinearDragTerm() const { return linearDragTerm; }
	double getMass() const { return mass; } // Total mass
	double getMomentOfInertia() const { return momentOfInertia; }
	glm::vec3 getVelocity() const { return velocity; }


	// TODO: Need to distinguish between the top and bottom of the driver, or driverRadiusTop and driverRadiusBottom
	double getDriverTopRadius() const { return 0.012; }
	// TODO: Add linearDragCoefficient (low priority, currently assumed to be constant 0.9)

	// Specialized getters
	double getAngularVelocityMagnitude() const { return glm::length(angularVelocity); }
	bool isSpinningClockwise() const { return angularVelocity.y < 0; }
	glm::vec3 getNormal() const;
	glm::vec3 getBottomPosition() const;
	BoundingBox getBoundingBox() const;

	// Setters  // NEWUI adds several members.
	void setInitialLaunch(glm::vec3 initialCenter, glm::vec3 initialVelocity, glm::vec3 initialAngularVelocity);
	void setDiscMass(double _mass) { disc.mass = _mass; }
	void setDiscMomentOfInertia(double _moi) { disc.momentOfInertia = _moi; }
	void setDriverCOF(double _cof) { driver.coefficientOfFriction = _cof;  }
	void setDriverMass(double _mass) { driver.mass = _mass; }
	void setDriverMomentOfInertia(double _moi) { driver.momentOfInertia = _moi; }
	void setLayerCoefficientOfRestitution(double _cor) { layer.coefficientOfRestitution = _cor; }
	void setLayerMass(double _mass) { layer.mass = _mass; }
	void setLayerMomentOfInertia(double _moi) { layer.momentOfInertia = _moi; }
	void setLayerRecoilDistribution(double mean, double stddev) {
		layer.recoilDistributionMean = mean;
		layer.recoileDistributionStdDev = stddev;
		delete layer.recoilDistribution;
		layer.recoilDistribution = new RandomDistribution(mean, stddev);
	}
	void setMass(double _mass) { mass = _mass; }  // Total mass
	void setMomentOfInertia(double _totalMOI) { momentOfInertia = _totalMOI; }

	// Adjustors
	void addCenterY(double addY) { baseCenter.y += static_cast<float>(addY); }
	void addCenterXZ(double addX, double addZ) { baseCenter.x += static_cast<float>(addX); baseCenter.z += static_cast<float>(addZ); }
	void setCenterY(double addY) { baseCenter.y = static_cast<float>(addY); }
	void setVelocity(glm::vec3 newVelocity) { velocity = newVelocity; }
	void setVelocityY(double newY) { velocity.y = static_cast<float>(newY); }

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
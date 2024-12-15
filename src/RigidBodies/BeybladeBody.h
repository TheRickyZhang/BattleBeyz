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

#include "UnitsSystem.h"
using namespace Units;

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

	// Getters per part
	Meter getLayerHeight() const { return driver.height; }
	Kilogram getLayerMass() const { return layer.mass; }
	KgM2 getLayerMomentOfInertia() const { return layer.momentOfInertia; }
	Meter getLayerRadius() const { return layer.radius; }
	RandomDistribution getLayerRecoilDistribution() const { return layer.recoilDistribution; }
	Scalar getLayerCOR() const { return layer.coefficientOfRestitution; }

	void setLayerCoefficientOfRestitution(Scalar _cor) { layer.coefficientOfRestitution = _cor; }
	void setLayerMass(Kilogram _mass) { layer.mass = _mass; }
	void setLayerMomentOfInertia(KgM2 _moi) { layer.momentOfInertia = _moi; }
	void setLayerRecoilDistribution(Scalar mean, Scalar stddev) {
		layer.recoilDistribution.setMean(mean);
		layer.recoilDistribution.setStdDev(stddev);
	}

	Meter getDiscHeight() const { return disc.height; }
	Kilogram getDiscMass() const { return disc.mass; }
	KgM2 getDiscMomentOfInertia() const { return disc.momentOfInertia; }
	Meter getDiscRadius() const { return disc.radius; }

	Scalar getDriverCOF() const { return driver.coefficientOfFriction; }
	Meter getDriverHeight() const { return driver.height; }
	Kilogram getDriverMass() const { return driver.mass; }
	KgM2 getDriverMomentOfInertia() const { return driver.momentOfInertia; }
	Meter getDriverRadius() const { return driver.radius; }

	void setDiscMass(Kilogram _mass) { disc.mass = _mass; }
	void setDiscMomentOfInertia(KgM2 _moi) { disc.momentOfInertia = _moi; }
	void setDriverCOF(Scalar _cof) { driver.coefficientOfFriction = _cof; }
	void setDriverMass(Kilogram _mass) { driver.mass = _mass; }
	void setDriverMomentOfInertia(KgM2 _moi) { driver.momentOfInertia = _moi; }

	// Getters entire object
	glm::vec3 getCenter() const { return baseCenter; }
	glm::vec3 getVelocity() const { return velocity; }
	glm::vec3 getAngularVelocity() const { return angularVelocity; }

	Kilogram getMass() const { return mass; } // Total mass
	KgM2 getMomentOfInertia() const { return momentOfInertia; }
	M2 getLinearDragTerm() const { return linearDragTerm; }
	M2 getAngularDragTerm() const { return angularDragTerm; }


	// TODO: Need to distinguish between the top and bottom of the driver, or driverRadiusTop and driverRadiusBottom
	float getDriverTopRadius() const { return 0.012f; }
	// TODO: Add linearDragCoefficient (low priority, currently assumed to be constant 0.9)

	// Specialized getters
	float getAngularVelocityMagnitude() const { return glm::length(angularVelocity); }
	bool isSpinningClockwise() const { return angularVelocity.y < 0; }
	glm::vec3 getNormal() const;
	glm::vec3 getBottomPosition() const;
	BoundingBox getBoundingBox() const;

	// Setters  // NEWUI adds several members.
	void resetPhysics(); // 2024-11-18
	void setInitialLaunch(glm::vec3 initialCenter, glm::vec3 initialVelocity, glm::vec3 initialAngularVelocity);

	void setMass(Kilogram _mass) { mass = _mass; }  // Total mass
	void setMomentOfInertia(KgM2 _totalMOI) { momentOfInertia = _totalMOI; }
	void updateFromParts() {
		setMass(layer.mass + disc.mass + driver.mass);
		setMomentOfInertia(layer.momentOfInertia + disc.momentOfInertia + driver.momentOfInertia);
	}

	// Adjustors
	void addCenterY(float addY) { baseCenter.y += static_cast<float>(addY); }
	void addCenterXZ(float addX, float addZ) { baseCenter.x += static_cast<float>(addX); baseCenter.z += static_cast<float>(addZ); }
	void setCenterY(float addY) { baseCenter.y = static_cast<float>(addY); }
	void setVelocity(glm::vec3 newVelocity) { velocity = newVelocity; }
	void setVelocityY(float newY) { velocity.y = static_cast<float>(newY); }

	// Used in collision calculations
	float sampleRecoil();
	static std::optional<float> distanceOverlap(BeybladeBody* a, BeybladeBody* b);

	// Accumulators
	void accumulateVelocity(glm::vec3 addedVelocity);
	void accumulateAngularVelocity(glm::vec3 addedAngularVelocity);
	void accumulateAcceleration(glm::vec3 addedAcceleration);
	void accumulateAngularAcceleration(glm::vec3 addedAngularAcceleration);

	void accumulateImpulseMagnitude(float magnitude);
	void accumulateAngularImpulseMagnitude(float magnitude);

	// Updators: these are the ones that significantly change the values of the body!
	void applyAccumulatedChanges(float deltaTime);
	void update(float deltaTime);
	std::vector<BoundingBox*> boundingBoxes{};

	bool getModified() {  // 2024-12-03 for UI interactions
		return modified;
	}
	void setModified(bool _modified = true) {  // 2024-12-03 for UI interaction
		modified = _modified;
	}

private:
	// Parts - Access individual variables through these!
	Disc disc;
	Driver driver;
	Layer layer;

	// Global Position
	Vector3Quantity<Meter> baseCenter {};
	Vector3Quantity<Meter> _initialBaseCenter{};  // 2024-11-18 Saved for use by restart

	// Linear Physics
	Kilogram mass;
	Vector3Quantity<M_S> velocity {};
	Vector3Quantity<M_S> _initialVelocity{};  // 2024-11-18 Saved for use by restart

	Vector3Quantity<M_S2> acceleration {};

	// Rotational Physics
	KgM2 momentOfInertia;  // This is the total for all parts
	Vector3Quantity<R_S> angularVelocity{ 0.0f, 1.0f, 0.0f };
	Vector3Quantity<R_S> _initialAngularVelocity{};  // 2024-11-18 Saved for use by restart

	Vector3Quantity<R_S2> angularAcceleration {};
	M2 linearDragTerm; // Sum of Cd*A for parts 
	M2 angularDragTerm; // Sum of Cd*A*r^2 for parts

	// Accumulated delta velocity to be applied at cycle end (for instantaneous collisions, does not depend on deltaTime)
	glm::vec3 accumulatedVelocity {};
	glm::vec3 accumulatedAngularVelocity {};

	// Accumulated delta Acceleration to be applied at cycle end (for forces like friction and inclined plane that do depend on time)
	glm::vec3 accumulatedAcceleration {};
	glm::vec3 accumulatedAngularAcceleration{};

	bool modified;							// 2024-12-03 Settings modified in customzation screen.
};
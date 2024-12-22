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
#include "BeybladeMesh.h"  // NEWMESH Added this, changed constructor definitions.
#include "Utils.h"

#include "Units.h"
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

	// Simple getters.
	// TODO: Make Layer (5000 bytes), Disc, Driver public unique_ptrs, thus reducing too many getters/setters
	// 
	// RZ:  Theoretically good style, but there are too many!  Just make the variables public!

	// Getters per part
	M getLayerHeight() const { return driver.height; }
	Kg getLayerMass() const { return layer.mass; }
	KgM2 getLayerMomentOfInertia() const { return layer.momentOfInertia; }
	M getLayerRadius() const { return layer.radius; }
	RandomDistribution getLayerRecoilDistribution() const { return layer.recoilDistribution; }
	Scalar getLayerCOR() const { return layer.coefficientOfRestitution; }

	void setLayerCoefficientOfRestitution(Scalar _cor) { layer.coefficientOfRestitution = _cor; }
	void setLayerMass(Kg _mass) { layer.mass = _mass; }
	void setLayerMomentOfInertia(KgM2 _moi) { layer.momentOfInertia = _moi; }
	void setLayerRecoilDistribution(Scalar mean, Scalar stddev) {
		layer.recoilDistribution.setMean(mean);
		layer.recoilDistribution.setStdDev(stddev);
	}

	M getDiscHeight() const { return disc.height; }
	Kg getDiscMass() const { return disc.mass; }
	KgM2 getDiscMomentOfInertia() const { return disc.momentOfInertia; }
	M getDiscRadius() const { return disc.radius; }

	Scalar getDriverCOF() const { return driver.coefficientOfFriction; }
	M getDriverHeight() const { return driver.height; }
	Kg getDriverMass() const { return driver.mass; }
	KgM2 getDriverMomentOfInertia() const { return driver.momentOfInertia; }
	M getDriverRadius() const { return driver.radius; }

	void setDiscMass(Kg _mass) { disc.mass = _mass; }
	void setDiscMomentOfInertia(KgM2 _moi) { disc.momentOfInertia = _moi; }
	void setDriverCOF(Scalar _cof) { driver.coefficientOfFriction = _cof; }
	void setDriverMass(Kg _mass) { driver.mass = _mass; }
	void setDriverMomentOfInertia(KgM2 _moi) { driver.momentOfInertia = _moi; }

	// Getters entire object
	Vec3_M getCenter() const { return baseCenter; }
	Vec3_M_S getVelocity() const { return velocity; }
	Vec3_R_S getAngularVelocity() const { return angularVelocity; }

	Kg getMass() const { return mass; } // Total mass
	KgM2 getMomentOfInertia() const { return momentOfInertia; }
	M2 getLinearDragTerm() const { return linearDragTerm; }
	M5 getAngularDragTerm() const { return angularDragTerm; }


	// TODO: Need to distinguish between the top and bottom of the driver, or driverRadiusTop and driverRadiusBottom
	float getDriverTopRadius() const { return 0.012f; }
	// TODO: Add linearDragCoefficient (low priority, currently assumed to be constant 0.9)

	// Specialized getters
	//float getAngularVelocityMagnitude() const { return glm::length(angularVelocity); }
	bool isSpinningClockwise() const { return angularVelocity.y() < 0; }
	Vec3_Scalar getNormal() const;
	Vec3_M getBottomPosition() const;
	BoundingBox getBoundingBox() const;

	// Setters  // NEWUI adds several members.
	void resetPhysics(); // 2024-11-18
	void setInitialLaunch(Vec3_M initialCenter, Vec3_M_S initialVelocity, Vec3_R_S initialAngularVelocity);

	void setMass(Kg _mass) { mass = _mass; }  // Total mass
	void setMomentOfInertia(KgM2 _totalMOI) { momentOfInertia = _totalMOI; }
	void updateFromParts() {
		setMass(layer.mass + disc.mass + driver.mass);
		setMomentOfInertia(layer.momentOfInertia + disc.momentOfInertia + driver.momentOfInertia);
	}

	// Adjustors
	void addCenterY(M y) { baseCenter.addY(y); }
	void addCenterXZ(M x, M z) {
		baseCenter.addX(x);
		baseCenter.addZ(z);
	}
	void setCenterY(M y) { baseCenter.setY(y); }
	void setVelocity(Vec3_M_S newVelocity) { velocity = newVelocity; }
	void setVelocityY(M_S newY) { velocity.setY(newY); }

	// Used in collision calculations
	Scalar sampleRecoil();
	static std::optional<M> distanceOverlap(BeybladeBody* a, BeybladeBody* b);

	// Accumulators
	void accumulateVelocity(Vec3_M_S addedVelocity);
	void accumulateAngularVelocity(Vec3_R_S addedAngularVelocity);
	void accumulateAcceleration(Vec3_M_S2 addedAcceleration);
	void accumulateAngularAcceleration(Vec3_R_S2 addedAngularAcceleration);

	void accumulateImpulseMagnitude(KgM_S magnitude);
	void accumulateAngularImpulseMagnitude(KgM2_S magnitude);

	// Updators: these are the ones that significantly change the values of the body!
	void applyAccumulatedChanges(float deltaTime);
	void update(float deltaTime);
	std::vector<BoundingBox*> boundingBoxes{};

	bool getModified() const {  // 2024-12-03 for UI interactions
		return modified;
	}
	void setModified(bool _modified = true) {  // 2024-12-03 for UI interaction
		modified = _modified;
	}

	// Parts - Access individual variables through these!
	Disc disc;
	Driver driver;
	Layer layer;

	float prevCollision = 0.0f;
private:

	// Global Position
	Vec3_M baseCenter {};
	Vec3_M _initialBaseCenter{};  // 2024-11-18 Saved for use by restart

	// Linear Physics
	Kg mass;
	Vec3_M_S velocity {};
	Vec3_M_S _initialVelocity{};  // 2024-11-18 Saved for use by restart

	Vec3_M_S2 acceleration {};

	// Rotational Physics
	KgM2 momentOfInertia;  // This is the total for all parts
	Vec3_R_S angularVelocity{ 0.0f, 1.0f, 0.0f };
	Vec3_R_S _initialAngularVelocity{};  // 2024-11-18 Saved for use by restart

	Vec3_R_S2 angularAcceleration {};
	M2 linearDragTerm; // Sum of Cd*A for parts 
	M5 angularDragTerm; // Sum of Cd*A*r^3 for parts

	// Accumulated delta velocity to be applied at cycle end (for instantaneous collisions, does not depend on deltaTime)
	Vec3_M_S accumulatedVelocity {};
	Vec3_R_S accumulatedAngularVelocity {};

	// Accumulated delta Acceleration to be applied at cycle end (for forces like friction and inclined plane that do depend on time)
	Vec3_M_S2 accumulatedAcceleration {};
	Vec3_R_S2 accumulatedAngularAcceleration{};

	bool modified;							// 2024-12-03 Settings modified in customzation screen.
};
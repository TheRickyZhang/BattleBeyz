////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "BoundingBox.h"

#include "Units.h"
using namespace Units;

class BoundingBox;
/**
 * StadiumBody. Contains all of the physical properties of a Stadium.
 * 
 * center is where the stadium is located in the world, so local coordinates must be shifted correspondingly
 * 
 * Ordinary value ranges (SI Units):
 * - radius: 0.15 to 1.5 
 * - curvature: 0.0 (flat) to 1.0 (approximately 45 degrees), independent of radius
 * - scaledCurvature: curvature to be used in calculations, inversely proportional to radius
 * - coefficientOfFriction: 0.0 to 0.5
 */
class StadiumBody {
public:
	StadiumBody::StadiumBody(glm::vec3 center, float radius, float curvature, float coefficientOfFriction) :
		center(Vec3_M(center)), radius(M(radius)), curvature(Scalar(curvature)), scaledCurvature(__M(curvature / radius)),
		coefficientOfFriction(coefficientOfFriction) {}

    // Default parameters: 1.2 meters radius is quite large
	StadiumBody::StadiumBody() :
        center(Vec3_M(0.0f, 0.0f, 0.0f)), radius(1.2_m), curvature(0.10__), scaledCurvature(0.10__/1.2_m), coefficientOfFriction(0.35__) {}

	bool isInside(M x, M z) const;
	const M getRadius() const { return radius; }
	const M getYLocal(M r) const;
	const M getY(M x, M z) const;
	const Vec3_Scalar getNormal(M x, M z) const;

	Vec3_M getCenter() const { return center; }
	Scalar getCOF() const { return coefficientOfFriction; }

	std::vector<BoundingBox*> boundingBoxes{};

	// Dimensions
	Vec3_M center{};
	M radius;
	Scalar curvature;
	__M scaledCurvature;

	// Contact Properties
	Scalar coefficientOfFriction;
private:
};
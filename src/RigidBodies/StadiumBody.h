////////////////////////////////////////////////////////////////////////////////
// BeybladeBody.h -- Beyblade Physics include -- rz -- 2024-08-15
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BeybladeParts.h"
#include "BoundingBox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

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
	StadiumBody::StadiumBody(glm::vec3 center, double radius, double curvature, double coefficientOfFriction) :
		center(center), radius(radius), curvature(curvature), scaledCurvature(curvature / radius),
		coefficientOfFriction(coefficientOfFriction) {}
	StadiumBody::StadiumBody() :
        center(glm::vec3(0.0)), radius(0.4), curvature(0.05), scaledCurvature(0.05/0.4), coefficientOfFriction(0.1) {}

	bool isInside(double x, double z) const;
	const double getRadius() const { return radius; }
	const float getYLocal(double r) const;
	const double getY(double x, double z) const;
	const glm::vec3 getNormal(double x, double z) const;

	glm::vec3 getCenter() const { return center; }
	double getCOF() const { return coefficientOfFriction; }

	std::vector<BoundingBox*> boundingBoxes{};

private:
	// Dimensions
	glm::vec3 center{};
	double radius;
	double curvature;
	double scaledCurvature;

	// Contact Properties
	double coefficientOfFriction;
};
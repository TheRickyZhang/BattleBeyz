#include "StadiumBody.h"

/**
* Checks whether a point is in the x-z plane of the stadium.
*/
bool StadiumBody::isInside(double x, double z) const {
    double scaledX = x - center.x;
    double scaledZ = z - center.z;
    return scaledX * scaledX + scaledZ * scaledZ <= radius * radius;
}

// Returns the y-coordinate of the stadium at a given r in LOCAL space, where the stadium bottom (vertex) is at 0.0
const float StadiumBody::getYLocal(double r) const
{
    return static_cast<float>(scaledCurvature * r * r);
}

/**
* Returns the y-coordinate of the stadium at a given x and z.
*/
const double StadiumBody::getY(double x, double z) const {
    double scaledX = x - center.x;
    double scaledZ = z - center.z;
    double scaledY = scaledCurvature * (scaledX * scaledX + scaledZ * scaledZ);
    return scaledY + center.y;
}

/**
* Returns the unit normal of the stadium at a given x and z.
*/
const glm::vec3 StadiumBody::getNormal(double x, double z) const {
    double scaledX = x - center.x;
    double scaledZ = z - center.z;
    glm::vec3 normal = glm::normalize(glm::vec3(-2 * scaledCurvature * scaledX, 1.0, -2 * scaledCurvature * scaledZ));
    return normal;
}


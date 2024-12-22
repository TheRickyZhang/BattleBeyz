#include "StadiumBody.h"

/**
* Checks whether a point is in the x-z plane of the stadium.
*/
bool StadiumBody::isInside(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    return scaledX * scaledX + scaledZ * scaledZ < radius * radius;
}

// Returns the y-coordinate of the stadium at a given r in LOCAL space, where the stadium bottom (vertex) is at 0.0
const M StadiumBody::getYLocal(M r) const
{
    return scaledCurvature * r * r;
}

/**
* Returns the y-coordinate of the stadium at a given x and z.
*/
const M StadiumBody::getY(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    M scaledY = scaledCurvature * (scaledX * scaledX + scaledZ * scaledZ);
    return scaledY + center.yTyped();
}

/**
* Returns the unit normal of the stadium at a given x and z.
*/
const Vec3_Scalar StadiumBody::getNormal(M x, M z) const {
    M scaledX = x - center.xTyped();
    M scaledZ = z - center.zTyped();
    Vec3_Scalar normal = normalize(Vec3_Scalar(
                        (-2.0__ * scaledCurvature * scaledX).value(),
                        1.0f,
                        (-2.0__ * scaledCurvature * scaledZ).value()));
    return normal;
}


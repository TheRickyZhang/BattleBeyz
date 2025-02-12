#include "CameraBehavior.h"
#include "Stadium.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class PanningBehavior : public CameraBehavior {
    PanningBehavior(Stadium* stadium, float radius, float angularVelocity) :
        stadium(stadium), radius(radius), angularVelocity(angularVelocity) {
    }
    void update(Camera& camera, float deltaTime) override;
private:
    Stadium* stadium;
    float radius; // Meters 
    float angularVelocity; // Radians / second
    float currentAngle; // Degrees 
};
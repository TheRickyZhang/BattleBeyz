#include "CameraBehavior.h"
#include "BeybladeBody.h"

class AttachedBehavior : public CameraBehavior {
    AttachedBehavior(BeybladeBody* beyblade, float heightAbove) : beyblade(beyblade), heightAbove(heightAbove) {}
    void update(Camera& camera, float deltaTime);

private:
    BeybladeBody* beyblade;
    float heightAbove;
};
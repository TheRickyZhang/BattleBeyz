////////////////////////////////////////////////////////////////////////////////
// test_physics_world.cpp -- The fixed-timestep clock driving PhysicsWorld
////////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>

#include "PhysicsWorld.h"

// These exercise advance() with an empty world: the point is the clock, not the
// forces. Beyblade construction uploads a mesh, so it needs a GL context and cannot
// take part here; the force and collision maths is covered against BeybladeBody.

TEST_CASE("a frame shorter than one step runs nothing and banks the remainder") {
    PhysicsWorld world;

    CHECK(world.advance(PhysicsWorld::FIXED_DT * 0.5f) == 0);
    CHECK(world.getInterpolationAlpha() == doctest::Approx(0.5f).epsilon(0.01));
}

TEST_CASE("banked time carries into the next frame") {
    PhysicsWorld world;

    REQUIRE(world.advance(PhysicsWorld::FIXED_DT * 0.5f) == 0);
    // The two half-steps together are worth one whole step
    CHECK(world.advance(PhysicsWorld::FIXED_DT * 0.5f) == 1);
    CHECK(world.getInterpolationAlpha() == doctest::Approx(0.0f).epsilon(0.01));
}

TEST_CASE("a frame of n steps runs exactly n times") {
    PhysicsWorld world;

    CHECK(world.advance(PhysicsWorld::FIXED_DT * 10.0f) == 10);
}

TEST_CASE("a long stall is clamped instead of spiralling") {
    PhysicsWorld world;

    // Five seconds of stall would be 1200 steps if it were taken literally
    int steps = world.advance(5.0f);

    CHECK(steps <= PhysicsWorld::MAX_STEPS_PER_FRAME);
    // MAX_FRAME_DELTA alone caps this at 0.25 s, which is exactly the step ceiling
    CHECK(steps == PhysicsWorld::MAX_STEPS_PER_FRAME);
    // Debt is dropped rather than carried, so the next frame starts clean
    CHECK(world.getInterpolationAlpha() == doctest::Approx(0.0f));
}

TEST_CASE("interpolation alpha stays inside one step") {
    PhysicsWorld world;

    for (float frame : {0.001f, 0.004f, 0.017f, 0.033f, 0.1f}) {
        world.advance(frame);
        CHECK(world.getInterpolationAlpha() >= 0.0f);
        CHECK(world.getInterpolationAlpha() < 1.0f);
    }
}

TEST_CASE("stepping is independent of how the frame time is chopped up") {
    PhysicsWorld coarse;
    PhysicsWorld fine;

    int coarseSteps = coarse.advance(PhysicsWorld::FIXED_DT * 8.0f);

    int fineSteps = 0;
    for (int i = 0; i < 8; ++i) {
        fineSteps += fine.advance(PhysicsWorld::FIXED_DT);
    }

    CHECK(coarseSteps == fineSteps);
}

TEST_CASE("resetPhysics clears the accumulated clock") {
    PhysicsWorld world;

    world.advance(PhysicsWorld::FIXED_DT * 0.75f);
    REQUIRE(world.getInterpolationAlpha() > 0.0f);

    world.resetPhysics();

    CHECK(world.getInterpolationAlpha() == doctest::Approx(1.0f));
    // Nothing is banked, so a half step still runs nothing
    CHECK(world.advance(PhysicsWorld::FIXED_DT * 0.5f) == 0);
}

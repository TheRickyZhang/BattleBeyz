////////////////////////////////////////////////////////////////////////////////
// test_beyblade_body.cpp -- Collision detection and integration on BeybladeBody
////////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>
#include <stdexcept>

#include "BeybladeBody.h"

using namespace Units;

namespace {
    // Default parts: layer radius 0.025 m / height 0.01 m, disc height 0.01 m,
    // driver height 0.015 m. Two default bodies therefore have a radii sum of 0.05 m.
    BeybladeBody makeBody(glm::vec3 center, glm::vec3 spin = glm::vec3(0.0f, -500.0f, 0.0f)) {
        BeybladeBody body;
        body.setInitialLaunch(Vec3_M(center), Vec3_M_S(0.0f), Vec3_R_S(spin));
        return body;
    }
}

TEST_CASE("distanceOverlap reports contact only when the layers actually meet") {
    SUBCASE("horizontally clear of each other") {
        BeybladeBody a = makeBody({0.0f, 1.0f, 0.0f});
        BeybladeBody b = makeBody({0.06f, 1.0f, 0.0f});

        CHECK_FALSE(BeybladeBody::distanceOverlap(&a, &b).has_value());
    }

    SUBCASE("horizontally overlapping") {
        BeybladeBody a = makeBody({0.0f, 1.0f, 0.0f});
        BeybladeBody b = makeBody({0.04f, 1.0f, 0.0f});

        std::optional<M> contact = BeybladeBody::distanceOverlap(&a, &b);
        REQUIRE(contact.has_value());
        // sqrt(radiiSum^2 - centreDistance^2) = sqrt(0.05^2 - 0.04^2)
        CHECK(contact->value() == doctest::Approx(0.03f));
    }

    SUBCASE("stacked far enough apart vertically to miss") {
        BeybladeBody lower = makeBody({0.0f, 1.0f, 0.0f});
        BeybladeBody upper = makeBody({0.0f, 1.02f, 0.0f});

        CHECK_FALSE(BeybladeBody::distanceOverlap(&lower, &upper).has_value());
    }

    SUBCASE("stacked within one layer height still counts") {
        BeybladeBody lower = makeBody({0.0f, 1.0f, 0.0f});
        BeybladeBody upper = makeBody({0.0f, 1.005f, 0.0f});

        std::optional<M> contact = BeybladeBody::distanceOverlap(&lower, &upper);
        REQUIRE(contact.has_value());
        CHECK(contact->value() == doctest::Approx(0.05f));
    }

    SUBCASE("argument order does not change the answer") {
        BeybladeBody a = makeBody({0.0f, 1.0f, 0.0f});
        BeybladeBody b = makeBody({0.04f, 1.0f, 0.0f});

        std::optional<M> ab = BeybladeBody::distanceOverlap(&a, &b);
        std::optional<M> ba = BeybladeBody::distanceOverlap(&b, &a);
        REQUIRE(ab.has_value());
        REQUIRE(ba.has_value());
        CHECK(ab->value() == doctest::Approx(ba->value()));
    }
}

TEST_CASE("distanceOverlap rejects null bodies") {
    BeybladeBody a = makeBody({0.0f, 1.0f, 0.0f});
    CHECK_THROWS_AS(BeybladeBody::distanceOverlap(&a, nullptr), std::invalid_argument);
}

TEST_CASE("update integrates position and records the previous step") {
    BeybladeBody body;
    body.setInitialLaunch(Vec3_M(0.0f, 1.0f, 0.0f),
                          Vec3_M_S(2.0f, 0.0f, 0.0f),
                          Vec3_R_S(0.0f, -500.0f, 0.0f));

    body.update(0.5f);

    CHECK(body.getCenter().x() == doctest::Approx(1.0f));
    // Interpolation reaches back to where the body was before this step
    CHECK(body.getInterpolatedCenter(0.0f).x == doctest::Approx(0.0f));
    CHECK(body.getInterpolatedCenter(1.0f).x == doctest::Approx(1.0f));
    CHECK(body.getInterpolatedCenter(0.5f).x == doctest::Approx(0.5f));
}

TEST_CASE("accumulated changes apply once and then clear") {
    BeybladeBody body;
    body.setInitialLaunch(Vec3_M(0.0f, 1.0f, 0.0f),
                          Vec3_M_S(0.0f),
                          Vec3_R_S(0.0f, -500.0f, 0.0f));

    body.accumulateVelocity(Vec3_M_S(1.0f, 0.0f, 0.0f));
    body.accumulateAcceleration(Vec3_M_S2(0.0f, -10.0f, 0.0f));
    body.applyAccumulatedChanges(0.1f);

    CHECK(body.getVelocity().x() == doctest::Approx(1.0f));
    CHECK(body.getVelocity().y() == doctest::Approx(-1.0f));

    // A second application with nothing accumulated must be a no-op
    body.applyAccumulatedChanges(0.1f);
    CHECK(body.getVelocity().x() == doctest::Approx(1.0f));
    CHECK(body.getVelocity().y() == doctest::Approx(-1.0f));
}

TEST_CASE("spin direction and orientation follow the angular velocity") {
    BeybladeBody clockwise = makeBody({0.0f, 1.0f, 0.0f}, {0.0f, -500.0f, 0.0f});
    BeybladeBody counter = makeBody({0.0f, 1.0f, 0.0f}, {0.0f, 500.0f, 0.0f});

    CHECK(clockwise.isSpinningClockwise());
    CHECK_FALSE(counter.isSpinningClockwise());

    // The normal always points up out of the top, whichever way it spins
    CHECK(clockwise.getNormal().y() == doctest::Approx(1.0f));
    CHECK(counter.getNormal().y() == doctest::Approx(1.0f));
}

TEST_CASE("the contact point sits a disc plus driver below the centre") {
    BeybladeBody body = makeBody({0.0f, 1.0f, 0.0f});

    Vec3_M bottom = body.getBottomPosition();
    CHECK(bottom.x() == doctest::Approx(0.0f));
    CHECK(bottom.y() == doctest::Approx(1.0f - (0.01f + 0.015f)));
}

TEST_CASE("total mass and inertia are the sum of the parts") {
    BeybladeBody body;

    // Default layer 0.022 kg, disc 0.027 kg, driver 0.005 kg
    CHECK(body.getMass().value() == doctest::Approx(0.054f));
    CHECK(body.getMomentOfInertia().value() > 0.0f);
}

TEST_CASE("resetPhysics parks the body and clears its motion") {
    BeybladeBody body;
    body.setInitialLaunch(Vec3_M(1.0f, 2.0f, 3.0f),
                          Vec3_M_S(5.0f, 0.0f, 0.0f),
                          Vec3_R_S(0.0f, -500.0f, 0.0f));

    body.resetPhysics(Vec3_M(0.0f, 1.0f, 0.0f));

    CHECK(body.getCenter().y() == doctest::Approx(1.0f));
    CHECK(body.getVelocity().lengthTyped().value() == doctest::Approx(0.0f));
    CHECK(body.getAngularVelocity().lengthTyped().value() == doctest::Approx(0.0f));
    // previousCenter moves with it, so the first frame after a reset does not lerp
    CHECK(body.getInterpolatedCenter(0.0f).y == doctest::Approx(1.0f));
}

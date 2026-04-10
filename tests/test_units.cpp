////////////////////////////////////////////////////////////////////////////////
// test_units.cpp -- Dimensional arithmetic in Units.h
////////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>

#include "Units.h"

using namespace Units;

TEST_CASE("length literals normalise to metres") {
    CHECK(M(2.0_m).value() == doctest::Approx(2.0f));
    CHECK(M(500.0_cm).value() == doctest::Approx(5.0f));
    CHECK(M(1.0_km).value() == doctest::Approx(1000.0f));

    M total = 2.0_m + 500.0_cm + 1.0_km;
    CHECK(total.value() == doctest::Approx(1007.0f));
}

TEST_CASE("time and mass literals normalise to SI") {
    CHECK(S(1500.0_ms).value() == doctest::Approx(1.5f));
    CHECK(S(3.0_s).value() == doctest::Approx(3.0f));
    CHECK(Kg(1.5_kg).value() == doctest::Approx(1.5f));
    CHECK(Kg(250.0_g).value() == doctest::Approx(0.25f));
}

TEST_CASE("dividing a length by a time yields a velocity") {
    M distance = 1007.0_m;
    S time = 3.0_s;

    M_S velocity = distance / time;
    CHECK(velocity.value() == doctest::Approx(1007.0f / 3.0f));

    M_S2 acceleration = velocity / time;
    CHECK(acceleration.value() == doctest::Approx(1007.0f / 9.0f));
}

TEST_CASE("angular quantities divide down the same way") {
    R_S angularVelocity = 10.0_rad_s;
    S time = 4.0_s;

    R_S2 angularAcceleration = angularVelocity / time;
    CHECK(angularAcceleration.value() == doctest::Approx(2.5f));
}

TEST_CASE("mass times acceleration yields a force") {
    KgM_S2 force = 1.5_kg * 2.0_m_s2;
    CHECK(force.value() == doctest::Approx(3.0f));
}

TEST_CASE("vector length keeps or drops units depending on the accessor") {
    Vec3_R_S spin(0.0f, -300.0f, 400.0f);

    // lengthTyped() is the one physics comparisons must use; length() erases rad/s
    CHECK(spin.lengthTyped().value() == doctest::Approx(500.0f));
    CHECK(spin.length().value() == doctest::Approx(500.0f));

    // The typed form is comparable against a threshold of the same dimension
    CHECK(spin.lengthTyped() > 30.0_rad_s);
    CHECK_FALSE(spin.lengthTyped() < 30.0_rad_s);
}

TEST_CASE("vector arithmetic tracks dimensions") {
    Vec3_M_S velocity(3.0f, 0.0f, 4.0f);
    S time = 2.0_s;

    Vec3_M displacement = velocity * time;
    CHECK(displacement.x() == doctest::Approx(6.0f));
    CHECK(displacement.z() == doctest::Approx(8.0f));
    CHECK(displacement.lengthTyped().value() == doctest::Approx(10.0f));
}

TEST_CASE("normalize strips units and preserves direction") {
    Vec3_M_S velocity(0.0f, 0.0f, -7.0f);
    Vec3_Scalar unit = normalize(velocity);

    CHECK(unit.z() == doctest::Approx(-1.0f));
    CHECK(unit.length().value() == doctest::Approx(1.0f));
}

TEST_CASE("dot and cross combine dimensions") {
    Vec3_M a(1.0f, 2.0f, 3.0f);
    Vec3_M b(4.0f, -5.0f, 6.0f);

    // dot of two lengths is an area
    M2 dotted = dot(a, b);
    CHECK(dotted.value() == doctest::Approx(1.0f * 4.0f + 2.0f * -5.0f + 3.0f * 6.0f));

    Vec3_M2 crossed = cross(a, b);
    CHECK(crossed.x() == doctest::Approx(2.0f * 6.0f - 3.0f * -5.0f));
    CHECK(crossed.y() == doctest::Approx(3.0f * 4.0f - 1.0f * 6.0f));
    CHECK(crossed.z() == doctest::Approx(1.0f * -5.0f - 2.0f * 4.0f));
}

TEST_CASE("pow and root move through the dimension exponents") {
    Vec3_M edge(2.0f, 3.0f, 4.0f);

    Vec3_M2 squared = pow<2>(edge);
    CHECK(squared.x() == doctest::Approx(4.0f));
    CHECK(squared.z() == doctest::Approx(16.0f));

    Vec3_M back = root<2>(squared);
    CHECK(back.x() == doctest::Approx(2.0f));
    CHECK(back.z() == doctest::Approx(4.0f));
}

TEST_CASE("adjustors mutate a single component") {
    Vec3_M position(1.0f, 2.0f, 3.0f);

    position.addY(0.5_m);
    CHECK(position.y() == doctest::Approx(2.5f));

    position.setX(-1.0_m);
    CHECK(position.x() == doctest::Approx(-1.0f));

    position.reset();
    CHECK(position.lengthTyped().value() == doctest::Approx(0.0f));
}

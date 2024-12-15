// UnitsTest.cpp
#include <iostream>
#include "Units.h"
#include "Quantity.h"
#include "Dimensions.h"
#include "Literals.h"

using namespace Units;

static void UnitsTest() {
    Meter distance1 = 2.0_m;
    Meter distance2 = 500.0_cm;
    Meter distance3 = 1.0_km;

    std::cout << "distance1: " << distance1 << " meters" << std::endl;
    std::cout << "distance2: " << distance2 << " meters" << std::endl;
    std::cout << "distance3: " << distance3 << " meters" << std::endl;

    // Adding distances
    Meter total_distance = distance1 + distance2 + distance3;
    std::cout << "Total Distance: " << total_distance << " meters" << std::endl;

    // Define mass and time
    Kilogram mass = 1.5_kg;
    Second time = 3.0_s;
    Second time_ms = 1500.0_ms; // 1.5 seconds

    std::cout << "Mass: " << mass << " kilograms" << std::endl;
    std::cout << "Time: " << time << " seconds" << std::endl;
    std::cout << "Time_ms: " << time_ms << " seconds" << std::endl;

    // Calculating velocity
    M_S velocity = total_distance / time;
    std::cout << "Velocity: " << velocity << " meters/second" << std::endl;

    // Calculating acceleration
    M_S2 acceleration = velocity / time;
    std::cout << "Acceleration: " << acceleration << " meters/second^2" << std::endl;

    // Angular quantities
    Radian angle_rad = 3.14159_rad; // pi radians

    std::cout << "Angle_rad: " << angle_rad << " radians" << std::endl;

    // Angular velocity
    R_S angular_velocity_rad_s = 10.0_rad_s;
    std::cout << "Angular Velocity: " << angular_velocity_rad_s << " radians/second" << std::endl;

    // Calculating angular acceleration
    R_S2 angular_acceleration = angular_velocity_rad_s / time;
    std::cout << "Angular Acceleration: " << angular_acceleration << " radians/second^2" << std::endl;

    // Calculating force: F = m * a
    KgM_S2 force = mass * acceleration;
    std::cout << "Force: " << force << " KgM_S2s" << std::endl;

    // Calculating torque: Torque = Force * Length
    Meter lever_arm = 2.0_m;
    KgM2_S2 torque = force * lever_arm;
    std::cout << "Torque: " << torque << " KgM_S2-meters" << std::endl;

    // Intentional Error: Uncommenting the following line should cause a compile-time error
     //auto invalid = distance1 + mass;
}
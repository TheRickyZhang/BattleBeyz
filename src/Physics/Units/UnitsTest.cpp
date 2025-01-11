// UnitsTest.cpp
#include <iostream>
#include "Units.h"

using namespace Units;

static void UnitsTest() {
    M distance1 = 2.0_m;
    M distance2 = 500.0_cm;
    M distance3 = 1.0_km;

    std::cout << "distance1: " << distance1 << " Ms" << std::endl;
    std::cout << "distance2: " << distance2 << " Ms" << std::endl;
    std::cout << "distance3: " << distance3 << " Ms" << std::endl;

    // Adding distances
    M total_distance = distance1 + distance2 + distance3;
    std::cout << "Total Distance: " << total_distance << " meters" << std::endl;

    // Define mass and time
    Kg mass = 1.5_kg;
    S time = 3.0_s;
    S time_ms = 1500.0_ms; // 1.5 seconds

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
    R angle_rad = 3.14159_rad; // pi radians

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
    M lever_arm = 2.0_m;

    // Intentional: Uncommenting = compile-time error
     //auto invalid = distance1 + mass;
}
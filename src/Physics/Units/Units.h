#pragma once
#include <glm/vec3.hpp>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <cmath>


namespace Units {
    /**
     * Template parameters represent the exponents of the base dimensions:
     * Length (L), Mass (M), Time (T), Angle (A).
     */
    template<int L, int M, int T, int A>
    struct Dimension {
        static constexpr int length = L;
        static constexpr int mass = M;
        static constexpr int time = T;
        static constexpr int angle = A;

        // Compare two dimensions for equality
        template<typename Other>
        struct is_same : std::bool_constant<(length == Other::length) && (mass == Other::mass) && (time == Other::time) && (angle == Other::angle)> {};
    };


    /**
     * @brief Class representing a physical quantity with associated dimensions.
     *
     * @tparam D Dimension type.
     * @tparam T Underlying numerical type (default: float).
     */
    template<typename D, typename T = float>
    class Quantity {
    public:
        // Constructors
        constexpr Quantity();
        constexpr explicit Quantity(T value);

        // Explicit conversion constructor for different dimensions
        template<typename OtherD>
        explicit constexpr Quantity(const Quantity<OtherD, T>& other);

        // Accessor
        constexpr T value() const;

        // Unary operators
        constexpr Quantity operator+() const;
        constexpr Quantity operator-() const;

        // Compound assignment operators
        constexpr Quantity& operator+=(const Quantity& other);
        constexpr Quantity& operator-=(const Quantity& other);
        constexpr Quantity& operator*=(T scalar);
        constexpr Quantity& operator/=(T scalar);

    private:
        T value_;
    };


    /**
     * @brief Shorthands for referring to Quantity Types
     *
     * Syntax: X2_Y -> Units X^2 / Units Y
     *
     * M = Meters (Length)
     * Kg = Kgs (Mass)
     * S = Seconds (Time)
     * R = Radians (Angle)
     *
     * When combining multiple variables, use order: R, Kg, M, S
     */

     // **Base Dimensions**
    using Dimensionless = Dimension<0, 0, 0, 0>;       // No dimension (e.g., coefficients)
    using DimLength =     Dimension<1, 0, 0, 0>;       // L (Length)
    using DimMass =       Dimension<0, 1, 0, 0>;       // M (Mass)
    using DimTime =       Dimension<0, 0, 1, 0>;       // T (Time)
    using DimAngle =      Dimension<0, 0, 0, 1>;       // A (Angle, e.g., radians)

    // **Derived Dimensions**
    using DimKgM =        Dimension<1, 1, 0, 0>;       // kg·m
    using DimKgM2 =       Dimension<2, 1, 0, 0>;       // kg·m^2 (Inertia)
    using DimKgM_S =      Dimension<1, 1, -1, 0>;      // kg·m/s (Momentum)
    using DimKgM_S2 =     Dimension<1, 1, -2, 0>;      // kg·m/s^2 (Force)
    using DimKgM2_S =     Dimension<2, 1, -1, 0>;      // kg·m^2/s (Angular Momentum)
    using DimKgM2_S2 =    Dimension<2, 1, -2, 0>;      // kg·m^2/s^2 (Torque, Energy)
    using DimKg_M =       Dimension<-1, 1, 0, 0>;      // kg/m (Linear mass density)
    using DimKg_M3 =      Dimension<-3, 1, 0, 0>;      // kg/m³ (Density)
    using DimM2_S2 =      Dimension<2, 0, -2, 0>;      // m^2/s^2

    using DimM =          DimLength;                    // Reusing DimLength for Length
    using DimM2 =         Dimension<2, 0, 0, 0>;       // m^2 (Area)
    using DimM3 =         Dimension<3, 0, 0, 0>;       // m^3 (Volume)
    using DimM4 =         Dimension<4, 0, 0, 0>;       // m^4
    using DimM5 =         Dimension<5, 0, 0, 0>;       // m^5
    using DimM_S =        Dimension<1, 0, -1, 0>;      // m/s (Velocity)
    using DimM_S2 =       Dimension<1, 0, -2, 0>;      // m/s^2 (Acceleration)

    using DimS2 =         Dimension<0, 0, 2, 0>;       //

    using DimR_S =        Dimension<0, 0, -1, 1>;      // rad/s (Angular velocity)
    using DimR_S2 =       Dimension<0, 0, -2, 1>;      // rad/s^2 (Angular acceleration)
    using DimRKgM2_S2 =   Dimension<2, 1, -2, 1>;

    /* Quantity Definitions */
    // Scalar
    using Scalar = Quantity<Dimensionless, float>;
    using Sc = Scalar;

    // Mass and Related
    using Kg = Quantity<DimMass, float>;                    // Mass
    using KgM = Quantity<DimKgM, float>;                    // kg·m
    using KgM2 = Quantity<DimKgM2, float>;
    using KgM3 = Quantity<DimM3, float>;
    using KgM_S = Quantity<DimKgM_S, float>;
    using KgM_S2 = Quantity<DimKgM_S2, float>;
    using KgM2_S = Quantity<DimKgM2_S, float>;
    using Kg_M = Quantity<DimKg_M, float>;
    using Kg_M3 = Quantity<DimKg_M3, float>;                // kg/m^3

    // Motion
    using M = Quantity<DimM, float>;                        // Length
    using M2 = Quantity<DimM2, float>;                      // Area
    using M3 = Quantity<DimM3, float>;                      // Volume
    using M4 = Quantity<DimM4, float>;
    using M5 = Quantity<DimM5, float>;
    using M_S = Quantity<DimM_S, float>;                    // Velocity
    using M_S2 = Quantity<DimM_S2, float>;                  // Acceleration
    using M2_S2 = Quantity<DimM2_S2, float>;
    using __M = Quantity<Dimension<-1, 0, 0, 0>, float>;

    // Time
    using S = Quantity<DimTime, float>;                    // Second
    using S2 = Quantity<DimS2, float>;

    // Angular
    using R = Quantity<DimAngle, float>;                   // Angle (radian)
    using R_S = Quantity<DimR_S, float>;                   // Angular velocity
    using R_S2 = Quantity<DimR_S2, float>;                 // Angular acceleration
    using RKgM2_S2 = Quantity<DimRKgM2_S2, float>;
    using __R = Quantity<Dimension<0, 0, 0, -1>, float>;

    // Shorter Aliases
    using Force = Quantity<DimKgM_S2, float>;              // Force 
    using Torque = Quantity<DimKgM2_S2, float>;            // Torque
    using Energy = Quantity<DimKgM2_S2, float>;            // Energy
    using Momentum = Quantity<DimKgM_S, float>;            // Momentum
    using AngularMomentum = Quantity<DimKgM2_S, float>;    // Angular momentum 
    using Density = Quantity<DimKg_M3, float>;             // Density


    /**
     * @brief Functions relating to Quantity
     */

    // Addition, subtraction
    template<typename D, typename T>
    constexpr Quantity<D, T> operator+(const Quantity<D, T>& a, const Quantity<D, T>& b);

    template<typename D, typename T>
    constexpr Quantity<D, T> operator-(const Quantity<D, T>& a, const Quantity<D, T>& b);

    // Multiplication, Division (Quantity * Quantity)
    template<typename D1, typename D2, typename T>
    constexpr Quantity<
        Dimension<
        D1::length + D2::length,
        D1::mass + D2::mass,
        D1::time + D2::time,
        D1::angle + D2::angle
    >, T>
        operator*(const Quantity<D1, T>& a, const Quantity<D2, T>& b);

    template<typename D1, typename D2, typename T>
    constexpr Quantity<
        Dimension<
        D1::length - D2::length,
        D1::mass - D2::mass,
        D1::time - D2::time,
        D1::angle - D2::angle
    >, T>
        operator/(const Quantity<D1, T>& a, const Quantity<D2, T>& b);

    // Multiplication, Division with scalar
    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(const Quantity<D, T>& q, T scalar);

    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(T scalar, const Quantity<D, T>& q);

    template<typename D, typename T>
    constexpr Quantity<D, T> operator/(const Quantity<D, T>& q, T scalar);

    // Exponentiation, Inverse exponentiation, Log
    template<int power, typename D, typename T>
    constexpr Quantity <Dimension<D::length* power, D::mass* power, D::time* power, D::angle* power>, T>
        pow(const Quantity<D, T>& quantity);

    template<int n, typename D, typename T>
    constexpr Quantity<
        Dimension<
        D::length / n,
        D::mass / n,
        D::time / n,
        D::angle / n
    >, T>
        root(const Quantity<D, T>& quantity);

    template<typename T>
    constexpr Quantity<Scalar, T> log(const Quantity<Scalar, T>& quantity);

    // Comparison operators
    template<typename D, typename T>
    constexpr bool operator<(const Quantity<D, T>& lhs, const Quantity<D, T>& rhs);

    template<typename D, typename T>
    constexpr bool operator>(const Quantity<D, T>& lhs, const Quantity<D, T>& rhs);

    // Stream insertion for Dimension
    template<typename D, typename T>
    std::ostream& operator<<(std::ostream& os, const Quantity<D, T>& unit);

    /**
     * @brief Class representing a 3D vector of physical quantities with associated dimensions.
     *
     * @tparam D Dimension type.
     * @tparam T Underlying numerical type (default: float).
     */
    template<typename D, typename T = float>
    class Vector3Quantity {
    public:
        // Constructors
        constexpr Vector3Quantity();
        constexpr Vector3Quantity(T a);
        constexpr Vector3Quantity(glm::vec3 v);
        constexpr Vector3Quantity(T x, T y, T z);

        // Accessors
        constexpr T x() const;
        constexpr T y() const;
        constexpr T z() const;
        Quantity<D, T> xTyped() const;
        Quantity<D, T> yTyped() const;
        Quantity<D, T> zTyped() const;

        // Setters
        constexpr void setX(Quantity<D, T> val);
        constexpr void setY(Quantity<D, T> val);
        constexpr void setZ(Quantity<D, T> val);
        void reset();

        // Adjustors
        constexpr void addX(Quantity<D, T> val);
        constexpr void addY(Quantity<D, T> val);
        constexpr void addZ(Quantity<D, T> val);

        // Special to Vec3
        constexpr Scalar length() const;
        constexpr Quantity<D, T> lengthTyped() const;

        // Unary operators
        constexpr Vector3Quantity operator+() const;
        constexpr Vector3Quantity operator-() const;

        // Compound assignment operators
        constexpr Vector3Quantity& operator+=(const Vector3Quantity& other);
        constexpr Vector3Quantity& operator-=(const Vector3Quantity& other);
        constexpr Vector3Quantity& operator*=(T scalar);
        constexpr Vector3Quantity& operator/=(T scalar);
        constexpr bool operator<(const Vector3Quantity& other) const;
        constexpr bool operator>(const Vector3Quantity& other) const;

        // Getter for glm::vec3 representation (optional)
        glm::vec3 value() const;
    private:
        T x_;
        T y_;
        T z_;
    };


    /**
     * @brief Shorthands for referring to Vector3Quantity Types. Follows Quantity declarations.
     *
     * Syntax: Vec3_X2_Y -> Vec3(Units X^2 / Units Y)
     */

    // Scalar
    using Vec3_Scalar = Vector3Quantity<Dimensionless, float>;

    // Mass and Related
    using Vec3_Kg = Vector3Quantity<DimMass, float>;                   // Mass
    using Vec3_KgM = Vector3Quantity<DimKgM, float>;                   // kg·m
    using Vec3_KgM2 = Vector3Quantity<DimKgM2, float>;
    using Vec3_KgM3 = Vector3Quantity<DimM3, float>;
    using Vec3_KgM_S = Vector3Quantity<DimKgM_S, float>;
    using Vec3_KgM_S2 = Vector3Quantity<DimKgM_S2, float>;
    using Vec3_KgM2_S = Vector3Quantity<DimKgM2_S, float>;
    using Vec3_Kg_M = Vector3Quantity<DimKg_M, float>;
    using Vec3_Kg_M3 = Vector3Quantity<DimKg_M3, float>;               // kg/m^3

    // Motion
    using Vec3_M = Vector3Quantity<DimM, float>;                       // Length
    using Vec3_M2 = Vector3Quantity<DimM2, float>;                     // Area
    using Vec3_M3 = Vector3Quantity<DimM3, float>;                     // Volume
    using Vec3_M4 = Vector3Quantity<DimM4, float>;
    using Vec3_M5 = Vector3Quantity<DimM5, float>;
    using Vec3_M_S = Vector3Quantity<DimM_S, float>;                   // Velocity
    using Vec3_M_S2 = Vector3Quantity<DimM_S2, float>;                 // Acceleration
    using Vec3_M2_S2 = Vector3Quantity<DimM2_S2, float>;

    // Time
    using Vec3_S = Vector3Quantity<DimTime, float>;                    // Second

    // Angular
    using Vec3_R = Vector3Quantity<DimAngle, float>;                   // Angle (radian)
    using Vec3_R_S = Vector3Quantity<DimR_S, float>;                   // Angular velocity
    using Vec3_R_S2 = Vector3Quantity<DimR_S2, float>;                 // Angular acceleration
    using Vec3_RKgM2_S2 = Vector3Quantity<DimRKgM2_S2, float>;

    // Shorter Aliases
    using Vec3_Force = Vector3Quantity<DimKgM_S2, float>;              // Force 
    using Vec3_Torque = Vector3Quantity<DimKgM2_S2, float>;            // Torque
    using Vec3_Energy = Vector3Quantity<DimKgM2_S2, float>;            // Energy
    using Vec3_Momentum = Vector3Quantity<DimKgM_S, float>;            // Momentum
    using Vec3_AngularMomentum = Vector3Quantity<DimKgM2_S, float>;    // Angular momentum 
    using Vec3_Density = Vector3Quantity<DimKg_M3, float>;             // Density


    // Additional Vec3 Functions
    
    // Addition, Subtraction
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> operator+(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b);
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> operator-(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b);

    // Dot Product (returns combined dimension scalar)
    template<typename D, typename T>
    constexpr Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>
        dot(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b);

    // Cross Product (returns vector with combined dimensions)
    template<typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>
        cross(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b);

    // Projection (Same as dot product but projecting Dimension onto unitless)
    template<typename D, typename T>
    constexpr Quantity<D, T> proj(const Vector3Quantity<D, T>& a, const Vec3_Scalar& b);

    // Unitless normalized vector
    template<typename D, typename T>
    constexpr Vector3Quantity<Dimensionless, T>
        normalize(const Vector3Quantity<D, T>& a);
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>
        normalizeWithUnits(const Vector3Quantity<D, T>& a);

    // Multiplication, Division with scalar (Supposedly not needed if all physics calculations does within units)
    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator*(const Vector3Quantity<D, T>& vec, T scalar);
    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator*(T scalar, const Vector3Quantity<D, T>& vec);
    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator/(const Vector3Quantity<D, T>& vec, T scalar);

    // Multiplication, Division with quantity
    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>
        operator*(const Quantity<D1, T>& quantity, const Vector3Quantity<D2, T>& vec);
    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>
        operator*(const Vector3Quantity<D2, T>& vec, const Quantity<D1, T>& quantity);
    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D2::length - D1::length, D2::mass - D1::mass, D2::time - D1::time, D2::angle - D1::angle>, T>
        operator/(const Vector3Quantity<D2, T>& vec, const Quantity<D1, T>& quantity);

    // Exponentiation (term and unit-wise)
    template<int power, typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length* power, D::mass* power, D::time* power, D::angle* power>, T>
        pow(const Vector3Quantity<D, T>& vec);

    // nth Root (term and unit-wise 1/n-th power) for Vector3Quantity
    template<int n, typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length / n, D::mass / n, D::time / n, D::angle / n>, T>
        root(const Vector3Quantity<D, T>& vec);

    // Stream insertion for Vector3Quantity with Dimension
    template<typename D, typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3Quantity<D, T>& vec);


    constexpr Scalar operator"" __(long double value) {
        return Scalar(static_cast<float>(value));
    }
    constexpr M operator"" _m(long double value) {
        return M(static_cast<float>(value));
    }
    constexpr M operator"" _km(long double value) {
        return M(static_cast<float>(value) * 1000.0f);
    }
    constexpr M operator"" _cm(long double value) {
        return M(static_cast<float>(value) * 0.01f);
    }
    constexpr M2 operator"" _m2(long double value) {
        return M2(static_cast<float>(value));
    }
    constexpr M3 operator"" _m3(long double value) {
        return M3(static_cast<float>(value));
    }
    constexpr Kg operator"" _kg(long double value) {
        return Kg(static_cast<float>(value));
    }
    constexpr Kg operator"" _g(long double value) {
        return Kg(static_cast<float>(value) * 0.001f);
    }
    constexpr S operator"" _s(long double value) {
        return S(static_cast<float>(value));
    }
    constexpr S operator"" _ms(long double value) {
        return S(static_cast<float>(value) * 0.001f);
    }
    constexpr R operator"" _rad(long double value) {
        return R(static_cast<float>(value));
    }
    constexpr R_S operator"" _rad_s(long double value) {
        return R_S(static_cast<float>(value));
    }
    constexpr R_S2 operator"" _rad_s2(long double value) {
        return R_S2(static_cast<float>(value));
    }
    constexpr KgM_S2 operator"" _N(long double value) {
        return KgM_S2(static_cast<float>(value));
    }
    constexpr KgM2 operator"" _kg_m2(long double value) {
        return KgM2(static_cast<float>(value));
    }
    constexpr Kg_M3 operator"" _kg_m3(long double value) {
        return Kg_M3(static_cast<float>(value));
    }
    constexpr M_S operator"" _m_s(long double value) {
        return M_S(static_cast<float>(value));
    }
    constexpr M_S2 operator"" _m_s2(long double value) {
        return M_S2(static_cast<float>(value));
    }
    constexpr S2 operator"" _s2(long double value) {
        return S2(static_cast<float>(value));
    }

    const Scalar PI = 3.14159265358979__;

}

#include "Units.tpp"

#pragma once

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
        struct is_same : std::bool_constant<
            (length == Other::length) &&
            (mass == Other::mass) &&
            (time == Other::time) &&
            (angle == Other::angle)
        > {
        };
    };
}

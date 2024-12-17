namespace Units {
    // Quantity Member Function Definitions
    template<typename D, typename T>
    constexpr Quantity<D, T>::Quantity() : value_(0) {}

    template<typename D, typename T>
    constexpr Quantity<D, T>::Quantity(T value) : value_(value) {}

    template<typename D, typename T>
    template<typename OtherD>
    constexpr Quantity<D, T>::Quantity(const Quantity<OtherD, T>& other)
        : value_(other.value()) {
    }

    template<typename D, typename T>
    constexpr T Quantity<D, T>::value() const { return value_; }

    template<typename D, typename T>
    constexpr Quantity<D, T> Quantity<D, T>::operator+() const { return *this; }

    template<typename D, typename T>
    constexpr Quantity<D, T> Quantity<D, T>::operator-() const { return Quantity(-value_); }

    template<typename D, typename T>
    constexpr Quantity<D, T>& Quantity<D, T>::operator+=(const Quantity& other) {
        value_ += other.value_;
        return *this;
    }

    template<typename D, typename T>
    constexpr Quantity<D, T>& Quantity<D, T>::operator-=(const Quantity& other) {
        value_ -= other.value_;
        return *this;
    }

    template<typename D, typename T>
    constexpr Quantity<D, T>& Quantity<D, T>::operator*=(T scalar) {
        value_ *= scalar;
        return *this;
    }

    template<typename D, typename T>
    constexpr Quantity<D, T>& Quantity<D, T>::operator/=(T scalar) {
        value_ /= scalar;
        return *this;
    }

    // Operator Definitions
    template<typename D, typename T>
    constexpr Quantity<D, T> operator+(const Quantity<D, T>& a, const Quantity<D, T>& b) {
        return Quantity<D, T>(a.value() + b.value());
    }

    template<typename D, typename T>
    constexpr Quantity<D, T> operator-(const Quantity<D, T>& a, const Quantity<D, T>& b) {
        return Quantity<D, T>(a.value() - b.value());
    }

    template<typename D1, typename D2, typename T>
    constexpr Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>
        operator*(const Quantity<D1, T>& a, const Quantity<D2, T>& b) {
        return Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>(a.value() * b.value());
    }

    template<typename D1, typename D2, typename T>
    constexpr Quantity<Dimension<D1::length - D2::length, D1::mass - D2::mass, D1::time - D2::time, D1::angle - D2::angle>, T>
        operator/(const Quantity<D1, T>& a, const Quantity<D2, T>& b) {
        return Quantity<Dimension<D1::length - D2::length, D1::mass - D2::mass, D1::time - D2::time, D1::angle - D2::angle>, T>(a.value() / b.value());
    }

    // Multiplication and Division with Scalar
    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(const Quantity<D, T>& q, T scalar) {
        return Quantity<D, T>(q.value() * scalar);
    }
    template<typename D, typename T>
    constexpr Quantity<D, T> operator*(T scalar, const Quantity<D, T>& q) {
        return q * scalar;
    }

    template<typename D, typename T>
    constexpr Quantity<D, T> operator/(const Quantity<D, T>& q, T scalar) {
        return Quantity<D, T>(q.value() / scalar);
    }

    // Power, Root, Log
    template<int power, typename D, typename T>
    constexpr Quantity <Dimension<D::length* power, D::mass* power, D::time* power, D::angle* power>, T>
        pow(const Quantity<D, T>& quantity) {
        return Quantity<Dimension<D::length * power, D::mass* power, D::time* power, D::angle* power>, T>(T(std::pow(quantity.value(), power)));
    }
    template<int n, typename D, typename T>
    constexpr Quantity<Dimension<D::length / n, D::mass / n, D::time / n, D::angle / n>, T>
        root(const Quantity<D, T>& quantity) {
        return Quantity<Dimension<D::length / n, D::mass / n, D::time / n, D::angle / n>, T>(std::pow(quantity.value(), 1.0 / n));
    }
    template<typename T>
    constexpr Quantity<Scalar, T> log(const Quantity<Scalar, T>& quantity) {
        return Quantity<Scalar, T>(std::log(quantity.value()));
    }

    template<typename D, typename T>
    constexpr bool operator<(const Quantity<D, T>& lhs, const Quantity<D, T>& rhs) {
        return lhs.value() < rhs.value();
    }
    template<typename D, typename T>
    constexpr bool operator>(const Quantity<D, T>& lhs, const Quantity<D, T>& rhs) {
        return lhs.value() > rhs.value();
    }

    // Stream Insertion
    template<int Length, int Mass, int Time, int Angle>
    std::ostream& operator<<(std::ostream& os, const Dimension<Length, Mass, Time, Angle>&) {
        os << "Dimension(L=" << Length << ", M=" << Mass << ", T=" << Time << ", A=" << Angle << ")";
        return os;
    }


    /**************** Vector3Quantity *********************/

    // Constructors
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>::Vector3Quantity() : x_(0), y_(0), z_(0) {}

    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>::Vector3Quantity(T a) : x_(a), y_(a), z_(a) {}

    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>::Vector3Quantity(glm::vec3 v) : x_(v.x), y_(v.y), z_(v.z) {}

    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>::Vector3Quantity(T x, T y, T z) : x_(x), y_(y), z_(z) {}

    // Accessors
    template<typename D, typename T>
    constexpr T Vector3Quantity<D, T>::x() const { return x_; }
    template<typename D, typename T>
    constexpr T Vector3Quantity<D, T>::y() const { return y_; }
    template<typename D, typename T>
    constexpr T Vector3Quantity<D, T>::z() const { return z_; }
    template<typename D, typename T>
    Quantity<D, T> Vector3Quantity<D, T>::xTyped() const { return Quantity<D, T>(x_); }
    template<typename D, typename T>
    Quantity<D, T> Vector3Quantity<D, T>::yTyped() const { return Quantity<D, T>(y_); }
    template<typename D, typename T>
    Quantity<D, T> Vector3Quantity<D, T>::zTyped() const { return Quantity<D, T>(z_); }

    // Setters
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::setX(Quantity<D, T> val) { x_ = val.value(); }
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::setY(Quantity<D, T> val) { y_ = val.value(); }
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::setZ(Quantity<D, T> val) { z_ = val.value(); }
    template<typename D, typename T>
    void Vector3Quantity<D, T>::reset() {
        x_ = T(0); y_ = T(0); z_ = T(0);
    }

    // Adjustors
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::addX(Quantity<D, T> val) { x_ += val.value(); }
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::addY(Quantity<D, T> val) { y_ += val.value(); }
    template<typename D, typename T>
    constexpr void Vector3Quantity<D, T>::addZ(Quantity<D, T> val) { z_ += val.value(); }


    // Special to Vec3
    template<typename D, typename T>
    constexpr Scalar Vector3Quantity<D, T>::length() const {
        return Scalar(std::sqrt(x_ * x_ + y_ * y_ + z_ * z_));
    }

    template<typename D, typename T>
    constexpr Quantity<D, T> Vector3Quantity<D, T>::lengthTyped() const {
        return Quantity<D, T>(std::sqrt(x_ * x_ + y_ * y_ + z_ * z_));
    }

    // Unary operators
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> Vector3Quantity<D, T>::operator+() const { return *this; }
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> Vector3Quantity<D, T>::operator-() const { return Vector3Quantity(-x_, -y_, -z_); }

    // Compound assignment operators
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>& Vector3Quantity<D, T>::operator+=(const Vector3Quantity& other) {
        x_ += other.x_; y_ += other.y_; z_ += other.z_;
        return *this;
    }
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>& Vector3Quantity<D, T>::operator-=(const Vector3Quantity& other) {
        x_ -= other.x_; y_ -= other.y_; z_ -= other.z_;
        return *this;
    }
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>& Vector3Quantity<D, T>::operator*=(T scalar) {
        x_ *= scalar; y_ *= scalar; z_ *= scalar;
        return *this;
    }
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T>& Vector3Quantity<D, T>::operator/=(T scalar) {
        x_ /= scalar; y_ /= scalar; z_ /= scalar;
        return *this;
    }
    template<typename D, typename T>
    constexpr bool Vector3Quantity<D, T>::operator<(const Vector3Quantity& other) const {
        return this->length() < other.length();
    }
    template<typename D, typename T>
    constexpr bool Vector3Quantity<D, T>::operator>(const Vector3Quantity& other) const {
        return this->length() > other.length();
    }

    // Getter for glm::vec3 representation
    template<typename D, typename T>
    glm::vec3 Vector3Quantity<D, T>::value() const {
        return glm::vec3(x_, y_, z_);
    }

    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> operator+(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b) {
        return Vector3Quantity<D, T>(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
    }

    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> operator-(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b) {
        return Vector3Quantity<D, T>(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
    }

    template<typename D, typename T>
    constexpr Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>
        dot(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b) {
        T result = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
        return Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>(result);
    }

    template<typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>
        cross(const Vector3Quantity<D, T>& a, const Vector3Quantity<D, T>& b) {
        T cx = a.y() * b.z() - a.z() * b.y();
        T cy = a.z() * b.x() - a.x() * b.z();
        T cz = a.x() * b.y() - a.y() * b.x();
        return Vector3Quantity<Dimension<D::length * 2, D::mass * 2, D::time * 2, D::angle * 2>, T>(cx, cy, cz);
    }

    template<typename D, typename T>
    constexpr Quantity<D, T> proj(const Vector3Quantity<D, T>& a, const Vec3_Scalar& b)
    {
        T result = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
        return Quantity<D, T>(result);
    }

    template<typename D, typename T>
    constexpr Vector3Quantity<Dimensionless, T> normalize(const Vector3Quantity<D, T>& a) {
        T len = a.length().value();
        if (len <= static_cast<T>(0)) return Vector3Quantity<Dimensionless, T>(0, 0, 0);
        return Vector3Quantity<Dimensionless, T>(a.x() / len, a.y() / len, a.z() / len);
    }
    template<typename D, typename T>
    constexpr Vector3Quantity<D, T> normalizeWithUnits(const Vector3Quantity<D, T>& a) {
        T len = a.length();
        if (len <= static_cast<T>(0)) return Vector3Quantity<D, T>(0, 0, 0);
        return Vector3Quantity<D, T>(a.x() / len, a.y() / len, a.z() / len);
    }

    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator*(const Vector3Quantity<D, T>& vec, T scalar) {
    //    return Vector3Quantity<D, T>(vec.x() * scalar, vec.y() * scalar, vec.z() * scalar);
    //}

    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator*(T scalar, const Vector3Quantity<D, T>& vec) {
    //    return vec * scalar;
    //}

    //template<typename D, typename T>
    //constexpr Vector3Quantity<D, T> operator/(const Vector3Quantity<D, T>& vec, T scalar) {
    //    return Vector3Quantity<D, T>(vec.x() / scalar, vec.y() / scalar, vec.z() / scalar);
    //}

    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>
        operator*(const Quantity<D1, T>& quantity, const Vector3Quantity<D2, T>& vec) {
        using NewDim = Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>;
        return Vector3Quantity<NewDim, T>(quantity.value() * vec.x(), quantity.value() * vec.y(), quantity.value() * vec.z());
    }

    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>, T>
        operator*(const Vector3Quantity<D2, T>& vec, const Quantity<D1, T>& quantity) {
        using NewDim = Dimension<D1::length + D2::length, D1::mass + D2::mass, D1::time + D2::time, D1::angle + D2::angle>;
        return Vector3Quantity<NewDim, T>(quantity.value() * vec.x(), quantity.value() * vec.y(), quantity.value() * vec.z());
    }

    template<typename D1, typename D2, typename T>
    constexpr Vector3Quantity<Dimension<D2::length - D1::length, D2::mass - D1::mass, D2::time - D1::time, D2::angle - D1::angle>, T>
        operator/(const Vector3Quantity<D2, T>& vec, const Quantity<D1, T>& quantity) {
        using NewDim = Dimension<D2::length - D1::length, D2::mass - D1::mass, D2::time - D1::time, D2::angle - D1::angle>;
        return Vector3Quantity<NewDim, T>(vec.x() / quantity.value(), vec.y() / quantity.value(), vec.z() / quantity.value());
    }

    template<int power, typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length* power, D::mass* power, D::time* power, D::angle* power>, T>
        pow(const Vector3Quantity<D, T>& vec) {
        static_assert(power >= 0, "pow only supports non-negative powers.");
        using NewDim = Dimension<D::length* power, D::mass* power, D::time* power, D::angle* power>;
        T x_pow = std::pow(vec.x(), power);
        T y_pow = std::pow(vec.y(), power);
        T z_pow = std::pow(vec.z(), power);
        return Vector3Quantity<NewDim, T>(x_pow, y_pow, z_pow);
    }

    template<int n, typename D, typename T>
    constexpr Vector3Quantity<Dimension<D::length / n, D::mass / n, D::time / n, D::angle / n>, T>
        root(const Vector3Quantity<D, T>& vec) {
        static_assert(n > 0, "root requires n to be a positive integer.");
        static_assert(D::length % n == 0 && D::mass % n == 0 && D::time % n == 0 && D::angle % n == 0, "All dimensions (L, M, T, A) must be divisible by n for root.");
        using ResultDim = Dimension<D::length / n, D::mass / n, D::time / n, D::angle / n>;
        T x_root = std::pow(vec.x(), 1.0 / n);
        T y_root = std::pow(vec.y(), 1.0 / n);
        T z_root = std::pow(vec.z(), 1.0 / n);
        return Vector3Quantity<ResultDim, T>(x_root, y_root, z_root);
    }

    template<typename D, typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3Quantity<D, T>& vec) {
        os << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ") " << D{};
        return os;
    }

}
#include "Complex.h"

extern "C" {
    #include "lib/libc/math.h"
}

Complex::Complex(double real, double imaginary) : real(real), imaginary(imaginary) {

}

Complex::Complex(const Complex &other) : real(other.real), imaginary(other.imaginary) {

}

Complex Complex::operator+(const Complex &other) {

    return Complex(real + other.real, imaginary + other.imaginary);
}

Complex Complex::operator-(const Complex &other) {

    return Complex(real - other.real, imaginary - other.imaginary);
}

Complex Complex::operator*(const Complex &other) {

    return Complex(real * other.real - imaginary * other.imaginary, real * other.imaginary + imaginary * other.real);
}

Complex Complex::operator/(const Complex &other) {

    const double c = other.real;

    const double d = other.imaginary;

    double q;

    double denominator;

    if (abs(c) < abs(d)) {

        q = c / d;

        denominator = c * q  + d;

    } else {

        q = d / c;

        denominator = d * q + c;
    }

    return Complex((imaginary * q + real) / denominator, (imaginary - real * q) / denominator);
}

double Complex::absolute() {

    return sqrt(real * real + imaginary * imaginary);
}

double Complex::squaredAbsolute() {

    return real * real + imaginary * imaginary;
}

Complex &Complex::operator=(const Complex &other) = default;

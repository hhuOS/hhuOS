#ifndef __Complex_include__
#define __Complex_include__

class Complex {

public:

    Complex(double real, double imaginary);

    Complex(const Complex &other);

    Complex &operator=(const Complex &other);

    ~Complex() = default;

    Complex operator+(const Complex &other);

    Complex operator-(const Complex &other);

    Complex operator*(const Complex &other);

    Complex operator/(const Complex &other);

    double absolute();

    double squaredAbsolute();

private:

    double real;

    double imaginary;

};


#endif

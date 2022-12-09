#ifndef COMPLEX_H
#define COMPLEX_H
#include <vector>

    static constexpr double PI = 3.14159265358979323;
class Complex
{
public:
    double re;
    double im;

    Complex() { re=im=0.; }
    Complex(double x, double y) { re=x; im=y; }

    friend Complex operator * (Complex& x, Complex& y);
    friend Complex operator / (Complex& x, Complex& y);
    friend Complex operator / (Complex& x, double y);
    friend Complex operator + (Complex& x, Complex& y);
    friend Complex operator - (Complex& x, Complex& y);
    Complex &operator =(Complex c);
};

std::vector<Complex> fourier(std::vector<Complex> F, double is);

#endif // COMPLEX_H

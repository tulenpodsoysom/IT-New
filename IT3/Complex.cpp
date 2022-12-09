#include "complex.h"
#include <cmath>
#include <float.h>

Complex operator * (Complex& x, Complex& y)
{
    Complex z;
    z.re = x.re * y.re - x.im * y.im;
    z.im = x.re * y.im + y.re * x.im;
    return z;
}
Complex operator / (Complex& x, Complex& y)
{
    Complex z;

    double y2 = y.re*y.re + y.im*y.im;
    if( y2 < DBL_EPSILON ) return Complex(0,0);
    z.re = (x.re*y.re + x.im*y.im) / y2;
    z.im = (y.re*x.im - x.re*y.im) / y2;
    return z;
}


Complex operator / (Complex& x, double y)
{
    Complex z;
    if( y < DBL_EPSILON )  return Complex(0,0);
    z.re = x.re / y;
    z.im = x.im / y;
    return z;
}
Complex operator + (Complex& x, Complex& y)
{
    Complex z;
    z.re = x.re + y.re;
    z.im = x.im + y.im;
    return z;
}

Complex operator - (Complex& x, Complex& y)
{
    Complex z;
    z.re = x.re - y.re;
    z.im = x.im - y.im;
    return z;
}


Complex& Complex::operator = (Complex c)
{
    re = c.re;
    im = c.im;
    return *this;
}

std::vector<Complex> fourier(std::vector<Complex> F, double is)
{
    Complex temp, w, c;
    long i, j, istep;
    long m, mmax;
    long n = F.size();
    double fn, r1, theta;
    fn=(double)n;
    double r = PI * is;

    j = 1;
    for( i=1; i<=n; i++)
    {
        long i1 = i - 1;
        if(i<j)
        {
            int j1=j-1;
            temp = F[j1];
            F[j1] = F[i1];
            F[i1] = temp;
        }
        m=n/2;
        while( j > m ){	j -= m;	m = (m+1)/2; }
        j+=m;
    }
    mmax=1;
    while(mmax<n)
    {
        istep=2*mmax;
        r1=r/(double)mmax;
        for( m=1; m<=mmax; m++)
        {
            theta = r1*(double)(m-1);
            w = Complex(cos(theta), sin(theta));
            for( i=m-1; i<n; i+=istep)
            {
                j=i+mmax;
                c = F[j];
                temp = w * c;
                F[j] = F[i] - temp;
                F[i] = F[i] + temp;
            }
        }
        mmax=istep;
    }
    if (is>0) for (i=0; i<n; i++)
    {
        F[i].re /= fn;
        F[i].im /= fn;
    }
    return F;
}

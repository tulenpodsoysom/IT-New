#include "filterprogram.h"
#include <cmath>
#include <Complex.h>


FilterProgram::FilterProgram()
{
}

std::vector<double> FilterProgram::GenerateNoiseSignal(int N,std::mt19937& mt)
{

    std::vector<double> noiseSignal(N);

    for (int var = 0; var < N; ++var)
    {
        std::normal_distribution<double> dist(0,1);
        noiseSignal[var] = dist(mt);
    }
    return noiseSignal;
}

std::vector<double> FilterProgram::AddNoiseSignal(std::vector<double> signal, double energyPercentage,std::mt19937& mt)
{
    int N = signal.size();
    auto noiseSignal = GenerateNoiseSignal(N,mt);

    double E1 = 0;
    double E2 = 0;
    for (int var = 0; var < N; ++var) {
        E1 += signal[var]*signal[var];
        E2 += noiseSignal[var]*noiseSignal[var];
    }


    double a = E1/E2 * energyPercentage;

    for (int var = 0; var <N; ++var) {
        signal[var] += sqrt(a)*noiseSignal[var];
    }

    return signal;
}

double FilterProgram::WindowFunc(double width, double t, WindowType wt)
{
    if (fabs(t) > width/2) return 0.0;
    double t_ = t / width;
    switch (wt)
    {
        case WindowType::Rectangle: return 1.0; break;
        case WindowType::Triangle:  return 1 - 2 * fabs(t_); break;
        case WindowType::Hann:      return 0.5 * (1 + cos(2*PI*t_)); break;
        case WindowType::Hamming:   return 0.54 + 0.46 * cos(2*PI*t_); break;
        case WindowType::Gauss:     return exp(-0.5*(2*2.5*t_)*(2*2.5*t_)); break;
    }
}

std::vector<double> FilterProgram::CreatePolyHarmonicalSignal(std::vector<SineParams> sineParams, int N, double fd)
{
    std::vector<double> signal(N);
    for (auto var : sineParams) {
        auto& A = var.A;
        auto& f = var.f;
        auto& phi = var.phi;

        for (int i = 0; i < N; ++i) {
            signal[i] += A*sin(2*PI*f*i/fd + phi);
        }
    }

    return signal;
}

std::vector<double> FilterProgram::CreateWindowSignal(int N, double width, double fd, WindowType wt)
{
    int n = ceil(width*fd);

    std::vector<double> windowSignal(N);
    for (int var = 0; var < n; ++var) {
        windowSignal[var] = WindowFunc(width,(-n/2 + var)/fd,wt);
    }

    return windowSignal;
}


std::vector<double> FilterProgram::Filter(std::vector<double> signal, std::vector<double> windowSignal)
{
    int n = signal.size();

    std::vector<Complex> signalComplex(n);
    std::vector<Complex> windowComplex(n);
    std::vector<Complex> signalSpectreComplex(n);
    std::vector<Complex> windowSpectreComplex(n);
    std::vector<Complex> filteredSpectreComplex(n);
    std::vector<Complex> filteredSignalComplex(n);
    std::vector<double>  filteredSignal(n);

    for (int var = 0; var < n; ++var) {
        signalComplex[var] = Complex(signal[var],0.0);
        windowComplex[var] = Complex(windowSignal[var],0.0);
    }

    signalSpectreComplex = fourier(signalComplex,-1);
    windowSpectreComplex = fourier(windowComplex,-1);

    for (int var = 0; var < n; ++var) {
        filteredSpectreComplex[var] = signalSpectreComplex[var] * windowSpectreComplex[var];
    }

    filteredSignalComplex = fourier(filteredSpectreComplex,1);

    for (int var = 0; var < n; ++var) {
        filteredSignal[var] = filteredSignalComplex[var].re;
    }

    return filteredSignal;
}

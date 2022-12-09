#ifndef FILTERPROGRAM_H
#define FILTERPROGRAM_H

#include <random>
#include <vector>
#include "Complex.h"



class FilterProgram
{
public:
    FilterProgram();
    std::vector<Complex> freqSignal;

    enum class WindowType
    {
        Rectangle,
        Triangle,
        Hann,
        Hamming,
        Gauss
    };

    struct SineParams
    {
        double A, f, phi;
    };



    static double WindowFunc(double width, double t, WindowType wt);

    static std::vector<double> CreatePolyHarmonicalSignal(std::vector<SineParams> sineParams, int N, double fd);

    static std::vector<double> GenerateNoiseSignal(int N,std::mt19937& mt);
    static std::vector<double> AddNoiseSignal(std::vector<double> signal, double energyPercentage,std::mt19937& mt);

    static std::vector<double> CreateWindowSignal(int N, double width,double fd, WindowType wt);
    static std::vector<double> Filter(std::vector<double> signal, std::vector<double> windowSignal);

};

#endif // FILTERPROGRAM_H

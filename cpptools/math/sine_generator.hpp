#ifndef CPPTOOLS__MATH__SINE_GENERATOR_HPP
#define CPPTOOLS__MATH__SINE_GENERATOR_HPP

#include <chrono>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

namespace cpptools
{

template<typename P = float>
class SineGenerator
{
    private:
        P _frequency;
        P _runningCycle;
        std::chrono::system_clock::time_point _lastTime;
        bool _running;

        void runningCycleModulo2Pi();

    public:
        SineGenerator(P frequency, bool start = false);

        // Get the frequency (Hz) at which the generated sine runs
        P getFrequency();
        // Set the frequency (Hz) at which the generated sine runs
        void setFrequency(P freq);

        // Start the generator and get the current value of the generated sine
        P start();
        // Pause the generator and get the current value of the generated sine
        P pause();
        // Get the current value of the generated sine
        P get();
        // Set the state of the current generator cycle to a given value (will be modulo'ed inside ]-Pi;Pi])
        void setPhase(P value);
};

template<typename P>
SineGenerator<P>::SineGenerator(P frequency, bool start) :
    _frequency(frequency),
    _runningCycle(0.0),
    _running(start)
{
    _lastTime = std::chrono::system_clock::now();
}

template<typename P>
P SineGenerator<P>::getFrequency()
{
    return _frequency;    
}

template<typename P>
void SineGenerator<P>::setFrequency(P freq)
{
    // Update the current value using the current frequency up to this point
    get();
    // Register new frequency
    _frequency = freq;
}

// Start the sine generator and get its current value
template<typename P>
P SineGenerator<P>::start()
{
    P value = get();
    _running = true;
    return value;
}

// Pause the sine generator and get its current value
template<typename P>
P SineGenerator<P>::pause()
{
    P value = get();
    _running = false;
    return value;
}

// Get the current value of the sine generator
template<typename P>
P SineGenerator<P>::get()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    if (_running)
    {
        std::chrono::duration<P> delta = now - _lastTime;
        _runningCycle += _frequency * delta.count();
        runningCycleModulo2Pi();
    }
    _lastTime = now;
    return std::sin(_runningCycle);
}
// Set the state of the current generator cycle to a given value (will be modulo'ed inside ]-Pi;Pi])
template<typename P>
void SineGenerator<P>::setPhase(P value)
{
    _runningCycle = value;
    runningCycleModulo2Pi();
}

template<typename P>
void SineGenerator<P>::runningCycleModulo2Pi()
{
    while (_runningCycle > M_PI) _runningCycle -= (P)(2.0) * (P)M_PI;
}

}//namespace cpptools

#endif//CPPTOOLS__MATH__SINE_GENERATOR_HPP
#ifndef CPPTOOLS__MATH__SINE_GENERATOR_HPP
#define CPPTOOLS__MATH__SINE_GENERATOR_HPP

#include <chrono>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

namespace tools::math
{

template<typename prec = float>
class sine_generator
{
private:
    prec _frequency;
    prec _running_cycle;
    std::chrono::system_clock::time_point _last_time;
    bool _running;

    void _constrain_phase()
    {
        while (_running_cycle > M_PI) _running_cycle -= (prec)(2.0) * (prec)M_PI;
    }

public:
    sine_generator(prec frequency, bool start = false) :
        _frequency(frequency),
        _running_cycle(0.0),
        _running(start)
    {
        _last_time = std::chrono::system_clock::now();
    }

    // Get the frequency (Hz) at which the generated sine runs
    prec get_frequency()
    {
        return _frequency;
    }

    // Set the frequency (Hz) at which the generated sine runs
    void set_frequency(prec freq)
    {
        // Update the current value using the current frequency up to this point
        value();
        // Register new frequency
        _frequency = freq;
    }

    // Start the generator and get the current value of the generated sine
    prec start()
    {
        prec value = value();
        _running = true;
        return value;
    }

    // Pause the generator and get the current value of the generated sine
    prec pause()
    {
        prec value = value();
        _running = false;
        return value;
    }

    // Get the current value of the generated sine
    prec value()
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        if (_running)
        {
            std::chrono::duration<prec> delta = now - _last_time;
            _running_cycle += _frequency * delta.count();
            _constrain_phase();
        }
        _last_time = now;
        return std::sin(_running_cycle);
    }

    // Set the state of the current generator cycle to a given value (will be mod'ed inside ]-Pi;Pi])
    void set_phase(prec value)
    {
        _running_cycle = value;
        _constrain_phase();
    }
};

} // namespace tools::math

#endif//CPPTOOLS__MATH__SINE_GENERATOR_HPP
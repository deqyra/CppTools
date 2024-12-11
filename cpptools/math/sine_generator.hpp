#ifndef CPPTOOLS_MATH_SINE_GENERATOR_HPP
#define CPPTOOLS_MATH_SINE_GENERATOR_HPP

#include <cmath>
#include <chrono>
#include <numbers>

namespace tools {

template<typename prec = float>
class sine_generator {
private:
    using Clock = std::chrono::steady_clock;
    static constexpr prec pi = static_cast<prec>(std::numbers::pi);

    prec _frequency;
    prec _phase;
    Clock::time_point _last_time;
    bool _running;

public:
    sine_generator(prec frequency, bool start = false) :
        _frequency(frequency),
        _phase(0.0),
        _running(start)
    {
        _last_time = Clock::now();
    }

    // Get the frequency (Hz) at which the generated sine runs
    prec get_frequency() {
        return _frequency;
    }

    // Set the frequency (Hz) at which the generated sine runs
    void set_frequency(prec freq) {
        // Update the current value using the current frequency up to this point
        value();
        // Register new frequency
        _frequency = freq;
    }

    // Start the generator
    void start() {
        _running = true;
    }

    // Pause the generator
    void pause() {
        prec value = value();
        _running = false;
    }

    // Get the current value of the generated sine
    prec value() {
        if (_running) {
            auto now = Clock::now();
            auto delta = std::chrono::duration_cast<std::chrono::seconds>(now - _last_time);
            _last_time = now;
            _phase += _frequency * delta.count();
        }
        return std::sin(_phase);
    }

    // Set the state of the current generator cycle to a given value (will be mod'ed inside ]-Pi;Pi])
    void set_phase(prec value) {
        _phase = std::fmod(value + pi, pi * 2) - pi;
    }
};

} // namespace tools

#endif//CPPTOOLS_MATH_SINE_GENERATOR_HPP
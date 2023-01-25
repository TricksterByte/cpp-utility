#pragma once

#include <chrono>
#include <string>
#include <iostream>

namespace tmr
{

template<typename T = std::chrono::microseconds>
class timer
{
private:
    std::string _context;
    std::ostream& _os;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;

public:
    timer(const std::string& context = "", std::ostream& os = std::cout);
    ~timer();

    timer(const timer& other) = delete;
    timer& operator=(const timer&) = delete;

    friend std::ostream& operator<<(std::ostream& os, const timer& t);
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const timer<T>& t);

using timer_ms = timer<std::chrono::milliseconds>;
using timer_us = timer<std::chrono::microseconds>;
using timer_ns = timer<std::chrono::nanoseconds>;

}

template<typename T>
tmr::timer<T>::timer(const std::string& context, std::ostream& os) :
    _context(context),
    _os(os),
    _start(std::chrono::high_resolution_clock::now())
{
    // empty
}

template<typename T>
tmr::timer<T>::~timer()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<T>(end - _start).count();
    if (_context.empty())
    {
        _os << delta << std::endl;
    }
    else
    {
        _os << _context << ": " << delta << std::endl;
    }
}

template<typename T>
std::ostream& tmr::operator<<(std::ostream& os, const tmr::timer<T>& t)
{
    auto end = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<T>(end - t._start).count();
    if (t._context.empty())
    {
        os << delta;
    }
    else
    {
        os << t._context << ": " << delta;
    }
    return os;
}

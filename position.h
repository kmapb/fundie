#pragma once
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <numeric>

typedef std::chrono::time_point<std::chrono::system_clock> CalTime;
static inline CalTime YMD(int year, int month, int day) {
    std::tm tm = {};
    tm.tm_year = year;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        throw std::runtime_error("Failed to convert time");
    }
    return std::chrono::system_clock::from_time_t(time);
}

struct Holding {
    CalTime acquired;
    double ownership;
    double cost;
    double value;
};

struct Position {
    const std::vector<Holding> holdings;
    Position(const std::vector<Holding>& h) : holdings(h) {}

    double ownership() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.ownership; });
    }
    double cost() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.cost; });
    }
    double value() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.value; });
    }
};
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

class Asset {
    std::string id_;
    double value_;
public:
    Asset(const char* id, double value) : id_(id), value_(value) {}
    const double value() const { return value_; }
    const std::string& name() const { return id_; }
    void set_value(double new_value) { value_ = new_value; }
};

struct Holding {
    CalTime acquired;
    double ownership;
    double cost;
};

struct Position {
    Position(Asset &asset, const std::vector<Holding> &h)
        : asset(asset),
          holdings(h)
    {
        validate();
    }

    double ownership() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.ownership; });
    }
    double cost() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.cost; });
    }
    double value() const {
        return asset.value() * ownership();
    }

    void validate() const {
        if (ownership() <= 0.0 || ownership() >= 1.0) {
            throw std::runtime_error("impossible ownership");
        }
    }

    void add(const Holding& h) {
        holdings.push_back(h);
        validate();
    }

    void dilute(const double dilution) {
        // Doesn't affect cost, just ownership
        for (auto &h: holdings) {
            h.ownership *= (1.0 - dilution);
        }
    }

  protected:
    Asset& asset;
    std::vector<Holding> holdings;
};


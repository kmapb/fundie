#pragma once
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <numeric>
#include <cassert>

#include "asset.h"

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
extern CalTime CALTIME_EPOCH;

struct Position {
    Position(Asset &asset)
        : asset(asset),
          holdings({})
    {
        validate();
    }

    double ownership() const {
        auto outstanding = asset.outstanding_shares();
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [outstanding](double acc, const Holding& h) { return acc + double(h.shares) / outstanding; });
    }
    double cost() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
            [](double acc, const Holding& h) { return acc + h.cost; });
    }
    double value() const {
        return asset.value() * ownership();
    }

    void validate() const {
        if (holdings.size() == 0) {
            return;
        }
        if (ownership() <= 0.0 || ownership() >= 1.0) {
            throw std::runtime_error("impossible ownership");
        }
        for (const auto& h: holdings) {
            assert(h.shares > 0);
            assert(h.cost > 0);
        }
    }

    void buy(CalTime ymd, double cost, uint64_t shares) {
        holdings.emplace_back(ymd, cost, shares);
        validate();
    }

  protected:
    Asset& asset;
    struct Holding {
        Holding(CalTime ct, double cost, uint64_t shares) : acquired(ct), cost(cost), shares(shares) { }
        CalTime acquired;
        double cost;
        uint64_t shares;
    };
    std::vector<Holding> holdings;
};

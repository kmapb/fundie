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
    uint64_t outstanding_shares_;

public:
    Asset(const char* id, double value) : id_(id), value_(value), outstanding_shares_(10 * 1000 * 1000) {}
    const double value() const { return value_; }
    const std::string& name() const { return id_; }

    void set_value(double new_value) { value_ = new_value; }

    uint64_t accept_new_money(double money_in) {
        uint64_t new_shares = money_in / value_;
        outstanding_shares_ += new_shares;
        value_ += money_in;
        return new_shares;
    }

    uint64_t reserve_option_pool(double pool_ratio) {
        uint64_t new_shares = outstanding_shares_ * pool_ratio;
        outstanding_shares_ += new_shares;
        return new_shares;
    }
    uint64_t outstanding_shares() const { return outstanding_shares_; }
};


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

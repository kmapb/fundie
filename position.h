#pragma once
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <memory>
#include <vector>
#include <numeric>
#include <cassert>

#include "asset.h"
#include "types.h"

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

/**
 * @brief a Holding is an abstract claim on equity; could be shares, but could
 * also be a convertible, warrant, SAFE, etc.
 */
struct Holding {
    CalTime acquired;
    currency_t cost;
    Holding(CalTime acquired, currency_t cost) : acquired(acquired), cost(cost) {
    }
    virtual ~Holding() { }
    virtual double value() const = 0;
    virtual double ownership() const = 0;
};

struct ShareHolding : public Holding {
    uint64_t shares;
    const Asset &asset;
    ShareHolding(CalTime ct, currency_t cost, uint64_t shares, const Asset &asset)
        : Holding(ct, cost), shares(shares), asset(asset) {
    }
    virtual double value() const {
        return (double(shares) / asset.outstanding_shares()) * asset.value();
    }
    virtual double ownership() const {
        return (1.0 * shares) / asset.outstanding_shares();
    }
};

struct PostMoneySAFE : public Holding {
    PostMoneySAFE(CalTime ct, currency_t cost, currency_t post_money_cap = -1.0)
        : Holding(ct, cost), post_money_cap(post_money_cap) {
    }
    currency_t post_money_cap;
    virtual double value() const {
        return cost;
    }
    virtual double ownership() const {
        return cost / post_money_cap;
    }
};

struct Position {
  protected:
    Asset &asset;
    typedef std::unique_ptr<Holding> HoldingPtr;
    std::vector<HoldingPtr> holdings;

  public:
    Position(Asset &asset) : asset(asset) {
        validate();
    }

    Asset& underlying_asset() {
        return asset;
    }
    
    const Asset& underlying_asset() const {
        return asset;
    }

    double ownership() const {
        return std::accumulate(holdings.begin(), holdings.end(), 0.0,
                               [](pct_t acc, const HoldingPtr &h) {
                                   return acc + h->ownership();
                               });
    }
    double cost() const {
        return std::accumulate(
            holdings.begin(), holdings.end(), 0.0,
            [](currency_t acc, const HoldingPtr &h) { return acc + h->cost; });
    }
    double value() const {
        return std::accumulate(
            holdings.begin(), holdings.end(), 0.0,
            [](currency_t acc, const HoldingPtr &h) { return acc + h->value(); });
    }

    void validate() const {
        if (holdings.size() == 0) {
            return;
        }
        if (ownership() <= 0.0 || ownership() >= 1.0) {
            throw std::runtime_error("impossible ownership");
        }
        for (const auto &h : holdings) {
            assert(h->ownership() > 0);
            assert(h->ownership() < 1.0);
            assert(h->cost > 0);
        }
    }

    void buy(CalTime ymd, currency_t cost, uint64_t shares) {
        auto *h = new ShareHolding(ymd, cost, shares, asset);
        holdings.emplace_back(h);
        validate();
    }

    void post_money_safe(CalTime ymd, currency_t cost,
                         currency_t post_money_valuation) {
        auto *s = new PostMoneySAFE(ymd, cost, post_money_valuation);
        holdings.emplace_back(s);
        validate();
    }
};

#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

#include "types.h"

class Asset {
    std::string id_;
    currency_t value_;
    uint64_t outstanding_shares_;

  public:
    Asset(const char *id, double value)
        : id_(id), value_(value), outstanding_shares_(10 * 1000 * 1000) {
    }
    double value() const {
        return value_;
    }
    const std::string &name() const {
        return id_;
    }

    currency_t get_value() const {
        return value_;
    }
    void set_value(currency_t new_value) {
        value_ = new_value;
    }

    uint64_t accept_new_money(currency_t money_in) {
        uint64_t new_shares = (money_in / value_) * outstanding_shares_;
        outstanding_shares_ += new_shares;
        value_ += money_in;
        return new_shares;
    }

    uint64_t reserve_option_pool(pct_t pool_ratio) {
        uint64_t new_shares = outstanding_shares_ * pool_ratio;
        outstanding_shares_ += new_shares;
        return new_shares;
    }
    uint64_t outstanding_shares() const {
        return outstanding_shares_;
    }
};

typedef std::unordered_map<std::string, Asset> AssetMap;

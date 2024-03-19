#pragma once

#include <string>
#include <cstdint>

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

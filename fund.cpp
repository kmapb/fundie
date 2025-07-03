#include "fund.h"
#include "debug.h"
#include "stage.h"
#include <algorithm>

Position &Fund::get_position(Asset &a)
{
    auto p = positions_.find(a.name());
    if (p != positions_.end()) {
        return p->second;
    }
    positions_.emplace(a.name(), Position(a));
    auto ret = positions_.find(a.name());
    assert(ret != positions_.end());
    return ret->second;
}

void Fund::deploy(Asset &a, CalTime ymd, const double value) {
    assert(value <= dry_powder());
    auto &p = get_position(a);
    auto shares = a.accept_new_money(value);
    p.buy(ymd, value, shares);
    deployed_ += value;

    assert(get_position(a).ownership() <= 1.0);
    assert(get_position(a).cost() >= value);
}

void Fund::distribute(const Position& pos) {
    auto proceeds = pos.value();
    auto distribute_to = [&](double& dest, double amount) {
        dest += amount;
        proceeds -= amount;
    };

    auto distribute_lps = [&](double amount) {
        assert(amount > 0.0);
        distribute_to(distributed_to_lps_, amount);
    };
    auto distribute_gps = [&](double amount) {
        assert(amount > 0.0);
        distribute_to(distributed_to_gps_, amount);
    };
    auto distribute_pro_rata = [&](double amount) {
        assert(amount > 0.0);
        auto lp_rate = lp_commitments_ / commitments();
        auto gp_rate = gp_commitments_ / commitments();
        distribute_lps(amount * lp_rate);
        distribute_gps(amount * gp_rate);
    };
    auto distribute_with_carry = [&](double amount) {
        assert(amount > 0.0);
        auto carry_interest = amount * carry_;
        auto regular_returns = amount - carry_interest;
        distribute_pro_rata(regular_returns);
        distribute_to(carry_paid_, carry_interest);
    };

    // We're using the American waterfall system here. Prior to the carry
    // hurdle, 
    auto distributed_pre = distributed() / commitments();
    if (distributed_pre < carry_hurdle_) {
        auto remainder_to_hurdle = proceeds * carry_hurdle_ - distributed();
        assert(remainder_to_hurdle > 0.0);
        distribute_lps(std::min(proceeds, remainder_to_hurdle));
    }
    if (proceeds < 1.0) return;
    
    // After the fund hurdle, carried interest kicks in
    distribute_with_carry(proceeds);
    assert(proceeds < 1.0); // allow some rounding slop
}

// Run the fund for, like, a year or so.
void Fund::tick() {
    fees_paid_ += fees_ * commitments();
    for (auto& pairs: positions_) {
        auto &pos = pairs.second;
        auto& a = pos.underlying_asset();
        for (int si = 0; si < num_stages; si++) {
            auto result = stages[si].traverse_stage(a);
            if (result == Stage::EXIT) {
                debug("company exited: %s for %g", a.name().c_str(), a.value());
                distribute(pos);
                // TODO: remove from positions_ ?
                break;
            }
            if (result == Stage::DIE) {
                debug("company died: %s", a.name().c_str());
                break;
            }
            assert(result == Stage::CONTINUE);
        }
    }
}
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
    active_positions_.insert(a.name());

    assert(get_position(a).ownership() <= 1.0);
    assert(get_position(a).cost() >= value);
}

void Fund::distribute(const Position& pos) {
    dbg(fund, ("distributing %g from %s\n", pos.value(), pos.underlying_asset().name().c_str()));
    assert(active_positions_.find(pos.underlying_asset().name()) != active_positions_.end());
    auto proceeds = pos.value();
    // Some helpers
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
    // hurdle, pro rata to LPs and GPs.
    auto distributed_pre = distributed() / commitments();
    if (distributed_pre < carry_hurdle_) {
        auto remainder_to_hurdle = proceeds * carry_hurdle_ - distributed();
        assert(remainder_to_hurdle > 0.0);
        distribute_pro_rata(std::min(proceeds, remainder_to_hurdle));
    }
    if (proceeds < 1.0) return;
    
    // After the fund hurdle, carried interest kicks in
    distribute_with_carry(proceeds);
    assert(proceeds < 1.0); // allow some rounding slop
}

void Fund::tick() {
    // Run each position one step forward. Do pro rata if we're not
    // yet fully deployed. Pay fees.
    fees_paid_ += fees_ * commitments();
    for (auto posname: active_positions_) {
        dbg(fund, ("tick: %s\n", posname.c_str()));
        auto pairs = positions_.find(posname);
        auto& pos = pairs->second;
        auto& a = pos.underlying_asset();
        assert(a.name() == posname);
        auto result = stages[stage_index_].traverse_stage(a);
        if (result == Stage::EXIT) {
            dbg(fund, ("company exited: %s for %g\n", a.name().c_str(), a.value()));
            distribute(pos);
            active_positions_.erase(posname);
            continue;
        }
        if (result == Stage::DIE) {
            dbg(fund, ("company died: %s\n", a.name().c_str()));
            active_positions_.erase(posname);
            assert(active_positions_.find(posname) == active_positions_.end());
            continue;
        }
        assert(result == Stage::CONTINUE);
        dbg(fund, ("company continues: %s for %g\n", a.name().c_str(), a.value()));
    }
    stage_index_++;
    assert(stage_index_ <= num_stages);
}

// Run the fund to completion
void Fund::run_to_completion() {
    while (active_positions_.size() > 0) {
        // Eh.
        dbg(fund, ("stage: %s active positions: %zd\n", stages[stage_index_].name, active_positions_.size()));
        tick();
        stage_index_ = std::min(stage_index_ + 1, num_stages - 1);
    }
}
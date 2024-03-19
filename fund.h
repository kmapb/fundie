#pragma once
#include <unordered_map>
#include <string>

#include "position.h"

struct Fund {
    Fund(double lp_commitments, double gp_commitments, double carry )
        : lp_commitments_(lp_commitments), gp_commitments_(gp_commitments), carry_(carry), deployed_(0.0) {}

    double fund_size() const {
        return lp_commitments_ + gp_commitments_;
    }

    double deployed() const {
        return deployed_;
    }

    double dry_powder() const {
        return fund_size() - deployed_;
    }

    Position& get_position(Asset& a) {
        auto p = positions_.find(a.name());
        if (p != positions_.end()) {
            return p->second;
        }
        positions_.emplace(a.name(), Position(a)).second;
        auto ret = positions_.find(a.name());
        assert(ret != positions_.end());
        return ret->second;
    }

    void deploy(Asset& a, const double value) {
        assert(value <= dry_powder());
        auto& p = get_position(a);
        a.accept_new_money(value);
        deployed_ += value;
    }

  protected:
    typedef std::unordered_map<std::string, Position> PositionMap;
    double lp_commitments_;
    double gp_commitments_;
    double carry_;
    double deployed_;
    PositionMap positions_;
};
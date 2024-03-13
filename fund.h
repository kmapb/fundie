#pragma once
#include <unordered_map>
#include <string>

#include "position.h"


struct Fund {
    Fund(double lp_commitments, double gp_commitments, double carry )
        : lp_commitments_(lp_commitments), gp_commitments_(gp_commitments), carry_(carry) {}

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

  protected:
    typedef std::unordered_map<std::string, Position> PositionMap;
    double lp_commitments_;
    double gp_commitments_;
    double carry_;
    PositionMap positions_;
};
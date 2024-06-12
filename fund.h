#pragma once
#include <unordered_map>
#include <string>

#include "position.h"

struct Fund {
    Fund(double lp_commitments, double gp_commitments = 0.0, double carry = 0.2,
         double fees = 0.02)
        : lp_commitments_(lp_commitments), gp_commitments_(gp_commitments),
          carry_(carry), fees_(fees), deployed_(0.0), distributed_(0.0) {
    }

    double fund_size() const {
        return lp_commitments_ + gp_commitments_;
    }

    double deployed() const {
        return deployed_;
    }

    double distributed() const {
        return distributed_;
    }

    double dry_powder() const {
        return fund_size() - deployed_;
    }

    Position &get_position(Asset &a);
    void deploy(Asset &a, CalTime ymd, const double value);

  protected:
    typedef std::unordered_map<std::string, Position> PositionMap;
    double lp_commitments_;
    double gp_commitments_;
    double carry_;
    double fees_;
    double deployed_;
    double distributed_;
    PositionMap positions_;
};
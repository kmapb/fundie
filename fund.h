#pragma once
#include <unordered_map>
#include <string>

#include "position.h"

struct Fund {
    Fund(double lp_commitments, double gp_commitments = 0.0,
        double carry = 0.2, double carry_hurdle = 1.0,
         double fees = 0.02)
        : lp_commitments_(lp_commitments)
        , gp_commitments_(gp_commitments)
        , carry_(carry)
        , carry_hurdle_(carry_hurdle)
        , carry_paid_(0.0)
        , fees_(fees)
        , fees_paid_(0.0)
        , deployed_(0.0)
        , distributed_to_lps_(0.0)
        , distributed_to_gps_(0.0) {
    }

    double fund_size() const {
        return lp_commitments_ + gp_commitments_;
    }

    double deployed() const {
        return deployed_;
    }

    double dry_powder() const {
        return fund_size() - deployed_ - fees_paid_;
    }

    double distributed() const {
        return distributed_to_gps_ + distributed_to_lps_;
    }
    
    double distributed_to_lps() const {
        return distributed_to_lps_;
    }
    
    double distributed_to_gps() const {
        return distributed_to_gps_;
    }

    double carried_interest() const {
        return carry_paid_;
    } 

    double commitments() const {
        return lp_commitments_ + gp_commitments_;
    }
    
    Position &get_position(Asset &a);
    void deploy(Asset &a, CalTime ymd, const double value);
    
    // See which companies move forward
    void tick();

  protected:
    void distribute(const Position& pos);

    typedef std::unordered_map<std::string, Position> PositionMap;
    double lp_commitments_;
    double gp_commitments_;
    double carry_;
    double carry_hurdle_;
    double carry_paid_;
    double fees_;
    double fees_paid_;
    double deployed_;
    double distributed_to_lps_;
    double distributed_to_gps_;
    PositionMap positions_;
};
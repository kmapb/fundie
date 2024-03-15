#pragma once

#include <cassert>

struct Round {
    double prev_mv;
    double post_mv;
    double new_money() const {
        assert(post_mv > prev_mv);
        return post_mv - prev_mv;
    }

    void set_new_money(double new_money) {
        assert(new_money >= 0.0);
        prev_mv = post_mv - new_money;
    }

    double compute_pro_rata(double prev_ownership, double target_ownership=-1.0);
};


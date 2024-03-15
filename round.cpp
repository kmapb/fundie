#include "round.h"

double
Round::compute_pro_rata(double prev_ownership,
                        double target_ownership)
{
    if (target_ownership < 0.0) {
        target_ownership = prev_ownership;
    }
    const double money_in_so_far = new_money();

    double dilution = new_money() / post_mv;
    assert(dilution > -1.0 && dilution < 1.0);

    double dil_ownership = prev_ownership * (1.0 - dilution);
    assert(dil_ownership > -1.0 && dil_ownership < 1.0);

    double approx_pro_rata = (target_ownership - dil_ownership) * post_mv;
    set_new_money(money_in_so_far + approx_pro_rata);
    return approx_pro_rata;
}

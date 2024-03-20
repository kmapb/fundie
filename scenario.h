#pragma once

#include <string>
#include <unordered_map>

#include "fund.h"
#include "asset.h"

struct Scenario {
    protected:
    Fund fund_;
    uint32_t years_;
    uint32_t first_checks_years_;
    double follow_on_investment_;
    std::unordered_map<std::string, int32_t> stage_targets_;
    CalTime start_time_;

    public:
        Scenario(Fund &fund, uint32_t years = 10, uint32_t first_checks_years = 3,
                 double follow_on_investment = 0.2,
                 CalTime start_time=CALTIME_EPOCH)
                 : fund_(fund)
                 , years_(years)
                 , first_checks_years_(first_checks_years)
                 , follow_on_investment_(follow_on_investment) {}
};


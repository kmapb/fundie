#pragma once

#include <string>
#include <unordered_map>

#include "fund.h"
#include "asset.h"

struct Scenario {
    Fund& fund_;
    
    void age_one_year() {
        for (auto& p: fund_.positions()) {
            p.second.age_one_year();
        }
    }
};

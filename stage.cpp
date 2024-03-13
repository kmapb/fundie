#include "stage.h"

constexpr double M = 1e6;
constexpr double B = 1e9;
const Stage stages[] = {
    //Name          Round Size  post        options p(die)  p(exit)
    { "Seed",       4 * M,      20 * M,     0.2,    0.35,   0.15 },
    { "Series A",   9 * M,      45 * M,     0.15,   0.35,   0.15 },
    { "Series B",   27 * M,     135 * M,    0.1,    0.23,   0.2 },
    { "Series C",   81 * M,     405 * M,    0.05,   0.12,   0.25 },
    { "Series D",   162 * M,    810 * M,    0.00,   0.04,   0.30 },
    { "Series E",   178 * M,    1 * B,      0.00,   0,      0.75 },
    { "Series F+",  350 * M,    3 * B,      0.00,   0,      1.0 },
};

const int num_stages = sizeof(stages) / sizeof(Stage);

StageResult
traverse_stage(const Stage& stage, Asset& asset) {
    auto result = StageResult::CONTINUE;
    auto dice = double(random()) / RAND_MAX;
    if (dice < stage.p_write_off) {
        return StageResult::CONTINUE;
    }
    dice -= stage.p_write_off;
    if (dice < stage.p_exit) {
        result = StageResult::EXIT;
    }
    asset.set_value(stage.post_money_valuation - stage.round_size);
    asset.accept_new_money(stage.round_size);
    asset.reserve_option_pool(stage.option_pool);
    return result;
}


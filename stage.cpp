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

Stage::Result
Stage::traverse_stage(Asset& asset) {
    auto result = Result::CONTINUE;
    auto dice = double(random()) / RAND_MAX;
    if (dice < p_write_off) {
        return Result::CONTINUE;
    }
    dice -= p_write_off;
    if (dice < p_exit) {
        result = Result::EXIT;
    }
    asset.set_value(post_money_valuation - round_size);
    asset.accept_new_money(round_size);
    asset.reserve_option_pool(option_pool);
    return result;
}


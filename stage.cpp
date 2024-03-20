#include "stage.h"
#include "round.h"

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


const StageMap& get_stage_map() {
    static StageMap stageMap;
    if (stageMap.size() == num_stages) return stageMap;
    assert(stageMap.size() == 0);

    for (const auto& stage: stages) {
        std::string sn { stage.name };
        stageMap.emplace(sn, stage);
    }
    assert(stageMap.size() == num_stages);
    return stageMap;
}

Stage::Result
Stage::traverse_stage(Asset& asset) {
    auto result = Result::CONTINUE;
    auto dice = double(random()) / RAND_MAX;
    if (dice < p_write_off) {
        asset.set_value(0.0);
        return Result::DIE;
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

Stage::Result
Stage::pro_rata(Asset& asset, Position& position, currency_t& out_currency, double pro_rata_rate) {
    auto prev_ownership = position.ownership();
    auto target_ownership = prev_ownership * pro_rata_rate;

    Round round { asset.value(), post_money_valuation };
    out_currency = round.compute_pro_rata(prev_ownership, target_ownership);

    return traverse_stage(asset);
}


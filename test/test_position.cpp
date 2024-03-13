#include <gtest/gtest.h>
#include <cstdio>
#include "../position.h"
#include "../round.h"
#include "../fund.h"

TEST(Asset, Accessors) {
    Asset a1 {"Polyhive", 2.5e7 };
    EXPECT_EQ(a1.name(), "Polyhive");
    EXPECT_DOUBLE_EQ(a1.value(), 2.5e7);
}

TEST(Position, ValuationDilution) {
    Asset a1 { "AAPL", 1e9 };
    auto shares = a1.outstanding_shares();
    auto pct_of_shares = [&](int pct) {
        return uint64_t((pct / 100.0) * shares);
    };
    Position p1 { a1 };
    p1.buy(YMD(2025, 2, 1), 1e6, pct_of_shares(20));
    p1.buy(YMD(2026, 2, 1), 1e5, pct_of_shares(2));

    EXPECT_DOUBLE_EQ(p1.ownership(), 0.22);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.1e6);
    EXPECT_DOUBLE_EQ(p1.value(), 1e9 * 0.22);

    p1.buy(YMD(2027, 2, 1), 1e5, pct_of_shares(2));

    auto o = p1.ownership();
    EXPECT_DOUBLE_EQ(o, 0.24);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.2e6);
    EXPECT_DOUBLE_EQ(p1.value(), a1.value() * o);

    a1.set_value(1e10);
    EXPECT_EQ(a1.value(), 1e10);
    EXPECT_DOUBLE_EQ(p1.ownership(), o);
    EXPECT_DOUBLE_EQ(p1.value(), a1.value() * o);

    a1.reserve_option_pool(0.2);
    auto expected_new_ownership = o / 1.2;
    auto expected_new_value = a1.value() * expected_new_ownership;
    EXPECT_DOUBLE_EQ(p1.ownership(), expected_new_ownership);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.2e6);
    EXPECT_DOUBLE_EQ(p1.value(), expected_new_value);
}

TEST(Round, ProRata) {
    Round r = { 20e6, 25e6 };
    EXPECT_DOUBLE_EQ(r.preMV, 20e6);
    EXPECT_DOUBLE_EQ(r.newMoney(), 5e6);
    EXPECT_DOUBLE_EQ(computeProRata(r, 0.1), 5e5);
    EXPECT_DOUBLE_EQ(r.newMoney(), 5.5e6);
    EXPECT_DOUBLE_EQ(r.preMV, 19.5e6);
}

TEST(Portfolio, Basics) {
    Asset a1 { "AAPL", 2e12 };
    auto shares = a1.outstanding_shares();
    auto pct_of_shares = [&](int pct) {
        return uint64_t((pct / 100.0) * shares);
    };
    Fund f1 { 3e7, 1e6, 0.2 };
    auto& pos = f1.getPosition(a1);
    // pos.buy(YMD(2025, 2, 1), 1e6, pct_of_shares(20));
    // pos.buy(YMD(2026, 2, 1), 2e5, pct_of_shares(2));
    // pos.buy(YMD(2027, 2, 1), 4e5, pct_of_shares(2));
}


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

TEST(Position, Ctors) {
    Asset a1 { "AAPL", 1e9 };
    Holding h1 { YMD(2025, 2, 1), 0.2, 1e6, };
    Holding h2 { YMD(2026, 2, 1), 0.02, 1e5, };
    Position p1( a1, {h1, h2 });

    EXPECT_DOUBLE_EQ(p1.ownership(), 0.22);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.1e6);
    EXPECT_DOUBLE_EQ(p1.value(), 1e9 * 0.22);

    Holding h3 { YMD(2027, 2, 1), 0.02, 1e5, };
    p1.add(h3);

    auto o = p1.ownership();
    EXPECT_DOUBLE_EQ(o, 0.24);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.2e6);
    EXPECT_DOUBLE_EQ(p1.value(), a1.value() * o);

    a1.set_value(1e10);
    EXPECT_EQ(a1.value(), 1e10);
    EXPECT_DOUBLE_EQ(p1.ownership(), o);
    EXPECT_DOUBLE_EQ(p1.value(), a1.value() * o);

    p1.dilute(0.2);
    EXPECT_DOUBLE_EQ(p1.ownership(), o * 0.8);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.2e6);
    EXPECT_DOUBLE_EQ(p1.value(), 1e10 * 0.24 * 0.8);
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
    Fund f1 { 3e7, 1e6, 0.2 };
    f1.addHolding(a1, Holding{ YMD(2025, 2, 1), 0.2, 1e6 });
    f1.addHolding(a1, Holding{ YMD(2026, 2, 1), 0.02, 1e5 });
    f1.addHolding(a1, Holding{ YMD(2027, 2, 1), 0.02, 1e5 });
}


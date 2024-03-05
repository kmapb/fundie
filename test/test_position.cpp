#include <gtest/gtest.h>
#include "../position.h"

TEST(Position, Ctors) {
    Holding h1 { YMD(2025, 2, 1), 0.2, 1e6, 1e6 };
    Holding h2 { YMD(2026, 2, 1), 0.02, 1e5, 1e5 };
    Position p1( {h1, h2 });

    EXPECT_DOUBLE_EQ(p1.ownership(), 0.22);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.1e6);
    EXPECT_DOUBLE_EQ(p1.value(), 1.1e6);

    Holding h3 { YMD(2027, 2, 1), 0.02, 1e5, 1e5 };
    p1.add(h3);

    EXPECT_DOUBLE_EQ(p1.ownership(), 0.24);
    EXPECT_DOUBLE_EQ(p1.cost(), 1.2e6);
    EXPECT_DOUBLE_EQ(p1.value(), 1.2e6);
}
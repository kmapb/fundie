#include <gtest/gtest.h>
#include <cassert>

#include "../stage.h"
#include "../fund.h"

TEST(Fund, Init) {
    Fund f(38e6, 1.5e6, 0.2);
    Asset a{"Apple", 1e9};
    f.deploy(a, YMD(1997, 3, 1), 1e6);

    auto &pos = f.get_position(a);
    EXPECT_DOUBLE_EQ(pos.value(), 1e6);
    EXPECT_DOUBLE_EQ(pos.ownership(), 1.0 / 1.001e3);
    EXPECT_EQ(pos.cost(), 1e6);
}

TEST(Fund,age) {
    Fund f(1.25e7, 1.5e6, 0.2);
    Asset a{"Apple", 1e9};
    f.deploy(a, YMD(2025, 3, 1), 1e6);

    auto &pos = f.get_position(a);
    EXPECT_DOUBLE_EQ(pos.value(), 1e6);
    EXPECT_DOUBLE_EQ(pos.ownership(), 1.0 / 1.001e3);
    EXPECT_EQ(pos.cost(), 1e6);
    
    auto numreps = 500;
    for (int i = 0; i < numreps; i++) for (int i = 0; i < 10; i++) {
        f.run_to_completion();
    }
}

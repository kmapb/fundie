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

TEST(Fund, RecyclingDoesNotMintDryPowder) {
    Fund f(10e6, 0.0, 0.2, 1.0, 0.02, 1.2);
    Asset a{"Apple", 1e9};

    f.deploy(a, YMD(2025, 3, 1), 10e6);
    EXPECT_DOUBLE_EQ(f.dry_powder(), 0.0);
}

namespace {
struct TestFund : public Fund {
    using Fund::Fund;
    using Fund::distribute;
};
}

TEST(Fund, RecyclingComesFromLiquidityEvents) {
    TestFund f(9e6, 1e6, 0.2, 1.0, 0.02, 1.2);
    Asset a{"Apple", 1e9};

    f.deploy(a, YMD(2025, 3, 1), 10e6);
    a.set_value(2e9);
    auto& pos = f.get_position(a);

    f.distribute(pos);
    EXPECT_DOUBLE_EQ(f.dry_powder(), 2e6);
}

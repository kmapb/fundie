#include <gtest/gtest.h>

#include "../position.h"
#include "../stage.h"

TEST(Stage, evolve) {
    //Name, scale, options, p(die), p(exit)
    Stage stage {"Inception", 2.0, 0.1, 0.0, 0.0 };
    Asset asset { "Dropbox", 5e6 };
    stage.traverse_stage(asset);
    EXPECT_EQ(asset.value(), 12e6);
}

TEST(Stage, death) {
    Stage death {"Die", 2.0, 0.1, 0.0, 1.0 };
    Stage exit {"Exit", 2.0, 0.1, 1.0, 0.0 };
    Stage cont {"Continue", 2.0, 0.1, 0.0, 0.0 };
    
    Asset a1 { "Shmoogle", 5e6 };
    Asset a2 { "Shmopbox", 5e6 };
    Asset a3 { "Shamazon", 5e6 };
    EXPECT_EQ(death.traverse_stage(a1), Stage::EXIT);
    EXPECT_EQ(exit.traverse_stage(a2), Stage::DIE);
    EXPECT_EQ(cont.traverse_stage(a3), Stage::CONTINUE);
    EXPECT_EQ(a3.value(), 12e6);
}
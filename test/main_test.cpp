#include <gtest/gtest.h>
#include "loggermod/loggermod.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    initPLOG();
    return RUN_ALL_TESTS();
}
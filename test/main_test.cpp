#include <gtest/gtest.h>
#include "tools.hpp"
#include "loggermod/loggermod.hpp"

bool file_exists(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    initPLOG("info"); // INFO severity, less information
    // initPLOG("debug"); // DEBUG severity, all tiny details included
    return RUN_ALL_TESTS();
}
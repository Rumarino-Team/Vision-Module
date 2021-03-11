#include <gtest/gtest.h>
#include "loggermod/loggermod.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    //Logger
    std::string maxSeverity = "info";
    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg == "-d" || arg == "--debug"){
            maxSeverity = "debug";
        }
    }
    initPLOG(maxSeverity);

    return RUN_ALL_TESTS();
}
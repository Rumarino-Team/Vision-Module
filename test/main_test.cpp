#include <gtest/gtest.h>
#include "tools.hpp"

bool file_exists(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
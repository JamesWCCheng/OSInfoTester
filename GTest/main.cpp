#include <src/gtest-all.cc>
#include <src/gmock-all.cc>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
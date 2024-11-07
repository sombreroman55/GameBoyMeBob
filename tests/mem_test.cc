#include <gtest/gtest.h>
#include "mmu.hh"
#include "cartridge.hh"

// The fixture for testing class Foo.
class MemTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    MemTest()
    {
        // You can do set-up work for each test here.
        mem = new gameboymebob::Mmu();
    }

    ~MemTest() override
    {
        // You can do clean-up work that doesn't throw exceptions here.
        if (mem) delete mem;
        if (cart) delete cart;
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.
    gameboymebob::Mmu* mem = nullptr;
    gameboymebob::Cartridge* cart = nullptr;
};

TEST_F(MemTest, CartCopyWorks)
{
    cart = new gameboymebob::Cartridge("/home/droo/roms/gb/tetris.gb");
    mem->load_cartridge(cart);
    for (int i = 0; i < 0x8000; i++) {
        EXPECT_EQ(mem->memory[i], cart->rom_data[i]);
    }
}

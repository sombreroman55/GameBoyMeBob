#include <gtest/gtest.h>
#include "mmu.hh"
#include "cartridge.hh"
#include "serial.hh"

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
    mem->map_cartridge(cart);
    for (int i = 0; i < 0x8000; i++) {
        EXPECT_EQ(mem->read_byte(i), cart->rom[i]);
    }
}

TEST_F(MemTest, ReadWriteByte)
{
    mem->write_byte(0xC000, 0x43);
    EXPECT_EQ(mem->read_byte(0xC000), 0x43);

    mem->write_byte(0xC001, 0x21);
    EXPECT_EQ(mem->read_byte(0xC001), 0x21);
}

TEST_F(MemTest, ReadWriteWord)
{
    mem->write_word(0xC000, 0x4321);
    EXPECT_EQ(mem->read_word(0xC000), 0x4321);

    mem->write_word(0xC010, 0x2112);
    EXPECT_EQ(mem->read_word(0xC010), 0x2112);

    mem->write_word(0xC020, 0x1357);
    EXPECT_EQ(mem->read_word(0xC020), 0x1357);
}

TEST_F(MemTest, ByteWordMix)
{
    mem->write_byte(0xC000, 0x43);
    EXPECT_EQ(mem->read_word(0xC000), 0x0043);

    mem->write_word(0xC010, 0x4321);
    EXPECT_EQ(mem->read_byte(0xC010), 0x21);
    EXPECT_EQ(mem->read_byte(0xC011), 0x43);
}

TEST_F(MemTest, SerialMapperWorks)
{
    EXPECT_EQ(mem->serial, nullptr);
    gameboymebob::SerialController* s = new gameboymebob::SerialController(mem);
    EXPECT_EQ(mem->serial, s);
    delete s;
}



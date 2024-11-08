#include "cpu.hh"
#include "mmu.hh"
#include <cstdlib>
#include <ctime>
#include <gtest/gtest.h>

// The fixture for testing class Foo.
class CpuTest : public testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    CpuTest()
    {
        // You can do set-up work for each test here.
        mem = new gameboymebob::Mmu();
        cpu = new gameboymebob::Cpu(mem);
        srand(time(NULL));
    }

    ~CpuTest() override
    {
        // You can do clean-up work that doesn't throw exceptions here.
        if (cpu)
            delete cpu;
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
    gameboymebob::Cpu* cpu = nullptr;
    gameboymebob::Mmu* mem = nullptr;
    const int repetitions = 256;
};

TEST_F(CpuTest, Opcode00)
{
    int consumed = cpu->execute(0x00);
    EXPECT_EQ(cpu->reg->a, 0x01);
    EXPECT_EQ(consumed, 1);
}

TEST_F(CpuTest, Opcode02)
{
    cpu->reg->bc = 0x3000;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x02);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->bc], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode10)
{
    EXPECT_EQ(cpu->stopped, false);
    int consumed = cpu->execute(0x10);
    EXPECT_EQ(cpu->stopped, true);
    EXPECT_EQ(consumed, 1);
}

TEST_F(CpuTest, Opcode12)
{
    cpu->reg->de = 0x3000;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x12);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->de], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode22)
{
    cpu->reg->hl = 0x3000;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x22);
        EXPECT_EQ(cpu->reg->hl, hl_addr+1);
        EXPECT_EQ(cpu->mem->memory[hl_addr], rval);
        EXPECT_EQ(consumed, 2);
        hl_addr = cpu->reg->hl;
    }
}

TEST_F(CpuTest, Opcode32)
{
    cpu->reg->hl = 0x3000;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x32);
        EXPECT_EQ(cpu->reg->hl, hl_addr-1);
        EXPECT_EQ(cpu->mem->memory[hl_addr], rval);
        EXPECT_EQ(consumed, 2);
        hl_addr = cpu->reg->hl;
    }
}

TEST_F(CpuTest, Opcode40)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x40);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode41)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x41);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode42)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x42);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode43)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x43);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode44)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x44);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode45)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x45);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode46)
{
    cpu->reg->hl = 0x1234;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[0x1234] = rval;
        int consumed = cpu->execute(0x46);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode47)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x47);
        EXPECT_EQ(cpu->reg->b, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode48)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x48);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode49)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x49);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode4A)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x4A);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode4B)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x4B);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode4C)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x4C);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode4D)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x4D);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode4E)
{
    cpu->reg->hl = 0x1234;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[0x1234] = rval;
        int consumed = cpu->execute(0x4E);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode4F)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x4F);
        EXPECT_EQ(cpu->reg->c, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode50)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x50);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode51)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x51);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode52)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x52);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode53)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x53);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode54)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x54);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode55)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x55);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode56)
{
    cpu->reg->hl = 0x1234;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[0x1234] = rval;
        int consumed = cpu->execute(0x56);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode57)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x57);
        EXPECT_EQ(cpu->reg->d, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode58)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x58);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode59)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x59);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode5A)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x5A);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode5B)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x5B);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode5C)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x5C);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode5D)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x5D);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode5E)
{
    cpu->reg->hl = 0x1234;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[0x1234] = rval;
        int consumed = cpu->execute(0x5E);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode5F)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x5F);
        EXPECT_EQ(cpu->reg->e, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode60)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x60);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode61)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x61);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode62)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x62);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode63)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x63);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode64)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x64);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode65)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x65);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode66)
{
    cpu->reg->hl = 0x1234;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[hl_addr] = rval;
        int consumed = cpu->execute(0x66);
        hl_addr = cpu->reg->hl;
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode67)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x67);
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode68)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x68);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode69)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x69);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode6A)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x6A);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode6B)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x6B);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode6C)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x6C);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode6D)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x6D);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode6E)
{
    cpu->reg->hl = 0x1234;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[hl_addr] = rval;
        int consumed = cpu->execute(0x66);
        hl_addr = cpu->reg->hl;
        EXPECT_EQ(cpu->reg->h, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode6F)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x6F);
        EXPECT_EQ(cpu->reg->l, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode70)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x70);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->hl], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode71)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x71);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->hl], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode72)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x72);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->hl], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode73)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x73);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->hl], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode74)
{
    cpu->reg->hl = 0x1234;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x74);
        hl_addr = cpu->reg->hl;
        EXPECT_EQ(cpu->mem->memory[hl_addr], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode75)
{
    cpu->reg->hl = 0x1234;
    u16 hl_addr = cpu->reg->hl;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x75);
        hl_addr = cpu->reg->hl;
        EXPECT_EQ(cpu->mem->memory[hl_addr], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode76)
{
    EXPECT_EQ(cpu->halted, false);
    int consumed = cpu->execute(0x76);
    EXPECT_EQ(cpu->halted, true);
    EXPECT_EQ(consumed, 1);
}

TEST_F(CpuTest, Opcode77)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x77);
        EXPECT_EQ(cpu->mem->memory[cpu->reg->hl], rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode78)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->b = rval;
        int consumed = cpu->execute(0x78);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode79)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->c = rval;
        int consumed = cpu->execute(0x79);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode7A)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->d = rval;
        int consumed = cpu->execute(0x7A);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode7B)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->e = rval;
        int consumed = cpu->execute(0x7B);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode7C)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->h = rval;
        int consumed = cpu->execute(0x7C);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode7D)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->l = rval;
        int consumed = cpu->execute(0x7D);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

TEST_F(CpuTest, Opcode7E)
{
    cpu->reg->hl = 0x1234;
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->mem->memory[0x1234] = rval;
        int consumed = cpu->execute(0x7E);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 2);
    }
}

TEST_F(CpuTest, Opcode7F)
{
    for (int i = 0; i < repetitions; i++) {
        u8 rval = rand() & 0xFF;
        cpu->reg->a = rval;
        int consumed = cpu->execute(0x7F);
        EXPECT_EQ(cpu->reg->a, rval);
        EXPECT_EQ(consumed, 1);
    }
}

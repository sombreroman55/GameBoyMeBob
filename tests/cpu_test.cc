#include "cpu.hh"
#include "mmu.hh"
#include "registers.hh"
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

    void TestInc(uint8_t opcode, uint8_t* reg)
    {
        for (int i = 0; i < 0xFF; i++) {
            *reg = i;
            int consumed = cpu->execute(opcode);
            EXPECT_EQ(*reg, i + 1);
            EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
            EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
            EXPECT_EQ(cpu->reg->flag_is_set(gameboymebob::Flag::HALF), ((i & 0x0F) == 0x0F));
            EXPECT_EQ(consumed, 1);
        }

        // Test overflow
        *reg = 0xFF;
        int consumed = cpu->execute(opcode);
        EXPECT_EQ(*reg, 0x00);
        EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
        EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
        EXPECT_EQ(consumed, 1);
    }

    void TestInc(uint8_t opcode, uint16_t* reg)
    {
        for (int i = 0; i < 0xFFFF; i++) {
            *reg = i;
            int consumed = cpu->execute(opcode);
            EXPECT_EQ(*reg, i + 1);
            EXPECT_EQ(consumed, 2);
        }

        // Test overflow
        *reg = 0xFFFF;
        int consumed = cpu->execute(opcode);
        EXPECT_EQ(*reg, 0x0000);
        EXPECT_EQ(consumed, 2);
    }

    void TestDec(uint8_t opcode, uint8_t* reg)
    {
        for (int i = 0xFF; i > 0x01; i--) {
            *reg = i;
            int consumed = cpu->execute(opcode);
            EXPECT_EQ(*reg, i - 1);
            EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
            EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
            EXPECT_EQ(cpu->reg->flag_is_set(gameboymebob::Flag::HALF), !(i & 0x0f));
            EXPECT_EQ(consumed, 1);
        }
        // Test zero condition
        *reg = 0x01;
        int consumed = cpu->execute(opcode);
        EXPECT_EQ(*reg, 0x00);
        EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
        EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
        EXPECT_EQ(consumed, 1);

        // Test underflow
        *reg = 0x00;
        consumed = cpu->execute(opcode);
        EXPECT_EQ(*reg, 0xFF);
        EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
        EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
        EXPECT_EQ(consumed, 1);
    }

    void TestDec(uint16_t* reg)
    {
    }

    void TestDirectLd(uint8_t opcode, uint8_t* dest, uint8_t* src)
    {
        // Lots of times for good measure :)
        for (int i = 0; i < 200; i++) {
            u8 rval = rand() & 0xFF;
            *src = rval;
            int consumed = cpu->execute(opcode);
            EXPECT_EQ(*dest, rval);
            EXPECT_EQ(consumed, 1);
        }
    }

    void TestRlcs(uint8_t opcode, uint8_t* reg)
    {
        *reg = 0b00110011;
        int consumed = cpu->execute_cb(opcode);
        EXPECT_EQ(*reg, 0b01100110);
        EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
        EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::CARRY));
        EXPECT_EQ(consumed, 2);

        *reg = 0b11001100;
        consumed = cpu->execute_cb(opcode);
        EXPECT_EQ(*reg, 0b10011001);
        EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO));
        EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::CARRY));
        EXPECT_EQ(consumed, 2);
    }

    void TestBits(uint8_t opcode, uint8_t* reg)
    {
        for (int i = 0; i <= 0xFF; i++) {
            *reg = i;
            for (int j = 0; j < 8; j++) {
                cpu->execute_cb(opcode+(8*j));
                EXPECT_EQ(cpu->reg->flag_is_set(gameboymebob::Flag::ZERO), (!(*reg & (1 <<j))));
                EXPECT_FALSE(cpu->reg->flag_is_set(gameboymebob::Flag::NEG));
                EXPECT_TRUE(cpu->reg->flag_is_set(gameboymebob::Flag::HALF));
            }
        }
    }

    void TestResets(uint8_t opcode, uint8_t* reg)
    {
        for (int i = 0; i <= 0xFF; i++) {
            *reg = 0xFF;
            for (int j = 0; j < 8; j++) {
                if (!(i & (1 << j))) {
                    cpu->execute_cb(opcode+(8*j));
                }
            }
            EXPECT_EQ(*reg, i);
        }
    }

    void TestSets(uint8_t opcode, uint8_t* reg)
    {
        for (int i = 0; i <= 0xFF; i++) {
            *reg = 0x00;
            for (int j = 0; j < 8; j++) {
                if ((i & (1 << j))) {
                    cpu->execute_cb(opcode+(8*j));
                }
            }
            EXPECT_EQ(*reg, i);
        }
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

TEST_F(CpuTest, Opcode03)
{
    TestInc(0x03, &cpu->reg->bc);
}

TEST_F(CpuTest, Opcode04)
{
    TestInc(0x04, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode05)
{
    TestDec(0x05, &cpu->reg->b);
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
        EXPECT_EQ(cpu->reg->hl, hl_addr + 1);
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
        EXPECT_EQ(cpu->reg->hl, hl_addr - 1);
        EXPECT_EQ(cpu->mem->memory[hl_addr], rval);
        EXPECT_EQ(consumed, 2);
        hl_addr = cpu->reg->hl;
    }
}

TEST_F(CpuTest, Opcode40)
{
    TestDirectLd(0x40, &cpu->reg->b, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode41)
{
    TestDirectLd(0x41, &cpu->reg->b, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode42)
{
    TestDirectLd(0x42, &cpu->reg->b, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode43)
{
    TestDirectLd(0x43, &cpu->reg->b, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode44)
{
    TestDirectLd(0x44, &cpu->reg->b, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode45)
{
    TestDirectLd(0x45, &cpu->reg->b, &cpu->reg->l);
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
    TestDirectLd(0x47, &cpu->reg->b, &cpu->reg->a);
}

TEST_F(CpuTest, Opcode48)
{
    TestDirectLd(0x48, &cpu->reg->c, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode49)
{
    TestDirectLd(0x49, &cpu->reg->c, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode4A)
{
    TestDirectLd(0x4A, &cpu->reg->c, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode4B)
{
    TestDirectLd(0x4B, &cpu->reg->c, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode4C)
{
    TestDirectLd(0x4C, &cpu->reg->c, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode4D)
{
    TestDirectLd(0x4D, &cpu->reg->c, &cpu->reg->l);
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
    TestDirectLd(0x4F, &cpu->reg->c, &cpu->reg->a);
}

TEST_F(CpuTest, Opcode50)
{
    TestDirectLd(0x50, &cpu->reg->d, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode51)
{
    TestDirectLd(0x51, &cpu->reg->d, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode52)
{
    TestDirectLd(0x52, &cpu->reg->d, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode53)
{
    TestDirectLd(0x53, &cpu->reg->d, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode54)
{
    TestDirectLd(0x54, &cpu->reg->d, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode55)
{
    TestDirectLd(0x55, &cpu->reg->d, &cpu->reg->l);
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
    TestDirectLd(0x57, &cpu->reg->d, &cpu->reg->a);
}

TEST_F(CpuTest, Opcode58)
{
    TestDirectLd(0x58, &cpu->reg->e, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode59)
{
    TestDirectLd(0x59, &cpu->reg->e, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode5A)
{
    TestDirectLd(0x5A, &cpu->reg->e, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode5B)
{
    TestDirectLd(0x5B, &cpu->reg->e, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode5C)
{
    TestDirectLd(0x5C, &cpu->reg->e, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode5D)
{
    TestDirectLd(0x5D, &cpu->reg->e, &cpu->reg->l);
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
    TestDirectLd(0x5F, &cpu->reg->e, &cpu->reg->a);
}

TEST_F(CpuTest, Opcode60)
{
    TestDirectLd(0x60, &cpu->reg->h, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode61)
{
    TestDirectLd(0x61, &cpu->reg->h, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode62)
{
    TestDirectLd(0x62, &cpu->reg->h, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode63)
{
    TestDirectLd(0x63, &cpu->reg->h, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode64)
{
    TestDirectLd(0x64, &cpu->reg->h, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode65)
{
    TestDirectLd(0x65, &cpu->reg->h, &cpu->reg->l);
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
    TestDirectLd(0x67, &cpu->reg->h, &cpu->reg->a);
}

TEST_F(CpuTest, Opcode68)
{
    TestDirectLd(0x68, &cpu->reg->l, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode69)
{
    TestDirectLd(0x69, &cpu->reg->l, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode6A)
{
    TestDirectLd(0x6A, &cpu->reg->l, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode6B)
{
    TestDirectLd(0x6B, &cpu->reg->l, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode6C)
{
    TestDirectLd(0x6C, &cpu->reg->l, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode6D)
{
    TestDirectLd(0x6D, &cpu->reg->l, &cpu->reg->l);
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
    TestDirectLd(0x6F, &cpu->reg->l, &cpu->reg->a);
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
    TestDirectLd(0x78, &cpu->reg->a, &cpu->reg->b);
}

TEST_F(CpuTest, Opcode79)
{
    TestDirectLd(0x79, &cpu->reg->a, &cpu->reg->c);
}

TEST_F(CpuTest, Opcode7A)
{
    TestDirectLd(0x7A, &cpu->reg->a, &cpu->reg->d);
}

TEST_F(CpuTest, Opcode7B)
{
    TestDirectLd(0x7B, &cpu->reg->a, &cpu->reg->e);
}

TEST_F(CpuTest, Opcode7C)
{
    TestDirectLd(0x7C, &cpu->reg->a, &cpu->reg->h);
}

TEST_F(CpuTest, Opcode7D)
{
    TestDirectLd(0x7D, &cpu->reg->a, &cpu->reg->l);
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
    TestDirectLd(0x7F, &cpu->reg->a, &cpu->reg->a);
}

// CB Tests
TEST_F(CpuTest, OpcodeCB00toCB07)
{
    TestRlcs(0x00, &cpu->reg->b);
    TestRlcs(0x01, &cpu->reg->c);
    TestRlcs(0x02, &cpu->reg->d);
    TestRlcs(0x03, &cpu->reg->e);
    TestRlcs(0x04, &cpu->reg->h);
    TestRlcs(0x05, &cpu->reg->l);
    TestRlcs(0x07, &cpu->reg->a);

    cpu->reg->hl = 0x1000;
    TestRlcs(0x06, &cpu->mem->memory[cpu->reg->hl]);
}

TEST_F(CpuTest, OpcodeCB40toCB7F)
{
    TestBits(0x40, &cpu->reg->b);
    TestBits(0x41, &cpu->reg->c);
    TestBits(0x42, &cpu->reg->d);
    TestBits(0x43, &cpu->reg->e);
    TestBits(0x44, &cpu->reg->h);
    TestBits(0x45, &cpu->reg->l);
    TestBits(0x47, &cpu->reg->a);

    cpu->reg->hl = 0x1000;
    TestBits(0x46, &cpu->mem->memory[cpu->reg->hl]);
}

TEST_F(CpuTest, OpcodeCB80toCBBF)
{
    TestResets(0x80, &cpu->reg->b);
    TestResets(0x81, &cpu->reg->c);
    TestResets(0x82, &cpu->reg->d);
    TestResets(0x83, &cpu->reg->e);
    TestResets(0x84, &cpu->reg->h);
    TestResets(0x85, &cpu->reg->l);
    TestResets(0x87, &cpu->reg->a);

    cpu->reg->hl = 0x1000;
    TestResets(0x86, &cpu->mem->memory[cpu->reg->hl]);
}

TEST_F(CpuTest, OpcodeCBC0toCBFF)
{
    TestSets(0xC0, &cpu->reg->b);
    TestSets(0xC1, &cpu->reg->c);
    TestSets(0xC2, &cpu->reg->d);
    TestSets(0xC3, &cpu->reg->e);
    TestSets(0xC4, &cpu->reg->h);
    TestSets(0xC5, &cpu->reg->l);
    TestSets(0xC7, &cpu->reg->a);

    cpu->reg->hl = 0x1000;
    TestSets(0xC6, &cpu->mem->memory[cpu->reg->hl]);
}

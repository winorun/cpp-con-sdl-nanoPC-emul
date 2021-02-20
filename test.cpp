
#define BOOST_TEST_MODULE main
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "function.h"
#include <string.h>

void clearMemory(){
    for (int i = 0; i < 256; ++i) {
        memory[i]=0;
    }
}

BOOST_AUTO_TEST_CASE(getOptDefauld_test)
{
    const unsigned char OPT_PARAM_LINK=0b01;
    const unsigned char OPT_PARAM_VALUE=0b00;
    const unsigned char OPT_PARAM_NEXT=0b10;
    const unsigned char OPT_PARAM_STACK=0b11;

    clearMemory();
    memory[0]=5;
    memory[1]=1;
    memory[2]=0;

    memory[5]=9;

    unsigned char *opt;

    BOOST_TEST(*getOptDefauld(memory,OPT_PARAM_VALUE)==5);
    BOOST_TEST(*getOptDefauld(memory,OPT_PARAM_LINK)==9);
    BOOST_TEST(*getOptDefauld(memory,OPT_PARAM_NEXT)==9);
    BOOST_TEST(*getOptDefauld(memory,0b100)==9);
    BOOST_TEST(*getOptDefauld(&memory[2],OPT_PARAM_STACK)==memory[memory[STACK_HEAD_MEMORY]]);
}


BOOST_AUTO_TEST_CASE(first_test)
{
    unsigned char TEST_PROGRAMM[256]={0x00};
    unsigned char RESULT[256]={0x00};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+1;
    //RESULT[START_PROGRAMM_MEMORY]=0x01;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}

BOOST_AUTO_TEST_CASE(mov_value_value)
{
    unsigned char TEST_PROGRAMM[256]={0x10,0x3,0x5};
    unsigned char RESULT[256]={0x00};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+3;
    RESULT[START_PROGRAMM_MEMORY]=0x10;
    RESULT[START_PROGRAMM_MEMORY+1]=0x05;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}

BOOST_AUTO_TEST_CASE(mov_next_value)
{
    unsigned char TEST_PROGRAMM[256]={0x12,0x3,0x5};
    unsigned char RESULT[256]={0x00};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+3;
    RESULT[START_PROGRAMM_MEMORY]=0x12;
    RESULT[START_PROGRAMM_MEMORY+1]=0x03;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;
    RESULT[START_PROGRAMM_MEMORY+4]=0x05;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    //BOOST_CHECK_MESSAGE(runOneStep(),"pc_status:" << std::hex << memory[PC_STATUS_MEMORY]);
    BOOST_CHECK_MESSAGE(runOneStep(),"pc_status:" << std::hex << (int)memory[PC_STATUS_MEMORY] );
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}

BOOST_AUTO_TEST_CASE(mov_link_value)
{
    unsigned char TEST_PROGRAMM[256]={0x11,0x1,'h'};
    unsigned char RESULT[256]={0x0,'h',0x0};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+3;
    RESULT[START_PROGRAMM_MEMORY]=0x11;
    RESULT[START_PROGRAMM_MEMORY+1]=0x01;
    RESULT[START_PROGRAMM_MEMORY+2]='h';

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_CHECK_MESSAGE(runOneStep(),"pc_status:" << std::hex << memory[PC_STATUS_MEMORY]);
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}
/*
BOOST_AUTO_TEST_CASE(cmp_value_value)
{
    unsigned char TEST_PROGRAMM[256]={0x40,0x5,0x5};
    unsigned char RESULT[256]={0x00};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+3;
    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x05;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;
    RESULT[CMP_BUF_MEMORY]=0x03;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);

    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x05;
    RESULT[START_PROGRAMM_MEMORY+2]=0x00;
    TEST_PROGRAMM[2]=0x00;
    RESULT[CMP_BUF_MEMORY]=0x02;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);

    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x03;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;
    TEST_PROGRAMM[2]=0x00;
    RESULT[CMP_BUF_MEMORY]=0x01;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}

BOOST_AUTO_TEST_CASE(jmp_value)
{
    unsigned char TEST_PROGRAMM[256]={0x40,0x5,0x5};
    unsigned char RESULT[256]={0x00};
    RESULT[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY+3;
    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x05;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;
    RESULT[CMP_BUF_MEMORY]=0x03;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);

    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x05;
    RESULT[START_PROGRAMM_MEMORY+2]=0x00;
    TEST_PROGRAMM[2]=0x00;
    RESULT[CMP_BUF_MEMORY]=0x02;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);

    RESULT[START_PROGRAMM_MEMORY]=0x40;
    RESULT[START_PROGRAMM_MEMORY+1]=0x03;
    RESULT[START_PROGRAMM_MEMORY+2]=0x05;
    TEST_PROGRAMM[2]=0x00;
    RESULT[CMP_BUF_MEMORY]=0x01;

    clearMemory();
    loadProgramm(TEST_PROGRAMM); 
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    BOOST_TEST(runOneStep());
    BOOST_TEST(memcmp(RESULT,memory, 256)==0);
}
*/


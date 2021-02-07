
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

const int START_DISPLAY_MEMORY=0;
const int END_DISPLAY_MEMORY=START_DISPLAY_MEMORY+9;
const int PC_STATUS_MEMORY=0xA;
const int NEXT_COMMAND_MEMORY=0xB;
const int KEY_STOP_VECTOR_MEMORY=0xC;
const int KEY_BUF_MEMORY=0xD;
const int CMP_BUF_MEMORY=0xE;
const int STACK_HEAD_MEMORY=0xF;
const int START_PROGRAMM_MEMORY=0x10;

//PC_STATUS
const unsigned char PC_STATUS_ERROR_MAX_VALUE=0x01;
const unsigned char PC_STATUS_NODEFINE_COMMAND=0x10;
const unsigned char PC_STATUS_NODEFINE_PARAM_COMMAND=0x11;
const unsigned char PC_STATUS_ERROR_READ_FILE=0xf0;

unsigned char memory[257];

unsigned char TEST_PROGRAMM[256]={0,0x10,0x10,0,1};

unsigned char HDD[256][256];

void printMemory(){
    puts(
"╔════╤═════════════════════════╤═════════════════════════╗\n║    │ 00 01 02 03 04 05 06 07 │ 08 09 0A 0B 0C 0D 0E 0F ║\n╠════╪═════════════════════════╪═════════════════════════╣" );
    for(int i=0;i<=0xf;i++){
        printf("║ %X0 │ ",i);
        for (int y = 0; y <= 0xf; y++){
            if(y==8)printf("│ ");
            printf("%02X ",memory[(i*0x10+y)]);
            //printf("%02X ",i*0x10+y);
        }
        puts("║");
    }
    puts("╚════╧═════════════════════════╧═════════════════════════╝" ); 
}

void printDisplay(){
    //char *display = &memory[0xf6];
    puts("Display:\n  0123456789\n╔═══════════╗" );
    printf(       "║ ");
    for (int i = 0; i < 10; ++i) {
        if((memory[i]>32)&&(memory[i]<127)){
            char a=memory[i];
            putchar(a);
        }else
            putchar(' ');
    }
    puts("║\n╚═══════════╝"); 
}

void gotoHomeConsole(){
    printf("\033[1;1H");
}

void clearConsole(){
    printf("\033[2J\033[1;1H");
}

unsigned char * const getOptDefauld(unsigned char * const opt, unsigned char optParam){
const unsigned char OPT_PARAM_VALUE=0b00;
const unsigned char OPT_PARAM_LINK=0b01;
const unsigned char OPT_PARAM_PREV=0b10;
const unsigned char OPT_PARAM_STACK=0b11;

    switch(optParam){
        case OPT_PARAM_VALUE:return opt;break;
        case OPT_PARAM_LINK :return &memory[*opt]; break;
        case OPT_PARAM_PREV :return opt-*opt; break;
        case OPT_PARAM_STACK :return &memory[STACK_HEAD_MEMORY-*opt]; break;
    }
    return opt;
}

void stk_pop(unsigned char * const opt){
    unsigned char * const head = &memory[STACK_HEAD_MEMORY]; 
    *opt=memory[*head];
    *head=*head-1;
}

void stk_push(unsigned char * const opt){
    unsigned char * const head = &memory[STACK_HEAD_MEMORY]; 
    memory[*head]=*opt;
    *head=*head+1;
}

void cmd_stk(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char * const opt = getOptDefauld(&memory[i+1],cmdb%0b100);

    switch(cmdb/0b100){
        case 0:stk_push(opt);break;
        case 1:stk_pop(opt);break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_PARAM_COMMAND;
    }

    memory[NEXT_COMMAND_MEMORY]+=2;
}


void cmd_jmp(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char * const opt = getOptDefauld(&memory[i+1],cmdb%0b100);
    unsigned char cmp = cmdb/0b100; 

    if(cmp==memory[CMP_BUF_MEMORY]){
        memory[NEXT_COMMAND_MEMORY]=*opt;
        return;
    }
    if(cmp==0){
        memory[NEXT_COMMAND_MEMORY]=*opt;
        return;
    }
    memory[NEXT_COMMAND_MEMORY]+=2;
}

void cmd_cmp(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char * const opt1 = getOptDefauld(&memory[i+1],cmdb%0b100);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],cmdb/0b100); 

    if(*opt1 >  *opt2)memory[CMP_BUF_MEMORY]=1;
    if(*opt1 == *opt2)memory[CMP_BUF_MEMORY]=2;
    if(*opt1 <  *opt2)memory[CMP_BUF_MEMORY]=3;
    memory[NEXT_COMMAND_MEMORY]+=3;
}

void sub(unsigned char * const opt1, unsigned char * const opt2){
    if(*opt1<*opt2){
        memory[PC_STATUS_MEMORY]=PC_STATUS_ERROR_MAX_VALUE;
        memory[memory[NEXT_COMMAND_MEMORY]]=0x01;
    }
    *opt1-=*opt2;
}

void cmd_sub(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char * const opt1 = getOptDefauld(&memory[i+1],cmdb%0b100);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],cmdb/0b100); 

    sub(opt1,opt2);
    memory[NEXT_COMMAND_MEMORY]+=3;
}

void add(unsigned char * const opt1, unsigned char * const opt2){
    *opt1+=*opt2;
    if(*opt1<*opt2){
        memory[PC_STATUS_MEMORY]=PC_STATUS_ERROR_MAX_VALUE;
        memory[memory[NEXT_COMMAND_MEMORY]]=0x01;
    }
}

void cmd_add(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],cmdb%0b100);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],cmdb/0b100); 
    add(opt1,opt2);
/*
    unsigned char * const opt1 = &memory[i+1];
    unsigned char * const opt2 = &memory[i+2]; 

    switch(cmdb){
        case 0:add(opt1,opt2);break;
        case 1:add(&memory[*opt1],opt2); break;
        case 4:add(opt1,&memory[*opt2]); break;
        case 5:add(&memory[*opt1],&memory[*opt2]); break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_PARAM_COMMAND;
    }
*/

    memory[NEXT_COMMAND_MEMORY]+=3;

}

void cmd_mov(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char * const opt1 = getOptDefauld(&memory[i+1],cmdb%0b100);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],cmdb/0b100); 

    memory[NEXT_COMMAND_MEMORY]+=3;
    
    /*
    switch(cmdb){
        case 0:*opt1=*opt2;break;
        case 1:memory[*opt1]=*opt2; break;
        case 4:*opt1=memory[*opt2]; break;
        case 5:memory[*opt1]=memory[*opt2]; break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_PARAM_COMMAND;
    }
    */
    *opt1=*opt2;
}

bool runOneStep(){
    if(memory[memory[NEXT_COMMAND_MEMORY]]==0x01)return false;
    unsigned char cmdl=memory[memory[NEXT_COMMAND_MEMORY]]/0x10;

    switch(cmdl){
        case 0: memory[NEXT_COMMAND_MEMORY]++; break;
        case 1: cmd_mov(); break;
        case 2: cmd_add(); break;
        case 3: cmd_sub(); break;
        case 4: cmd_cmp(); break;
        case 5: cmd_jmp(); break;
        case 6: cmd_stk(); break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_COMMAND;
    }
    if(memory[PC_STATUS_MEMORY]>=0x10)return false;
    return true;
}

void loadProgramm(unsigned char *programm,unsigned char start=START_PROGRAMM_MEMORY,unsigned char size=240){
    for (int i = 0; i < size; ++i) {
        memory[i+start]=programm[i];
        //printf("copy %02X\n", programm[i]);
    }
}

void debug(){
    getchar();
}

bool loadFile(){
    FILE *file;

    if((file = fopen("program.bin", "rb")) == NULL) {
        memory[PC_STATUS_MEMORY]=PC_STATUS_ERROR_READ_FILE;
        return false;
    }

    size_t size = fread(HDD,256,256,file);
    printf("load: %ld byte\n",size);
    if(size==0){
        memory[PC_STATUS_MEMORY]=PC_STATUS_ERROR_READ_FILE;
        return false;
    }
    
    fclose(file);
    return true;
}

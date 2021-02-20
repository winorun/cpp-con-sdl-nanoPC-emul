const int START_DISPLAY_MEMORY=0;
const int END_DISPLAY_MEMORY=START_DISPLAY_MEMORY+9;
const int PC_STATUS_MEMORY=0xA;
const int NEXT_COMMAND_MEMORY=0xB;
const int KEY_STOP_VECTOR_MEMORY=0xC;
const int KEY_BUF_MEMORY=0xD;
const int CMP_AND_LINK_STATUS_MEMORY=0xE;
const int STACK_HEAD_MEMORY=0xF;
const int START_PROGRAMM_MEMORY=0x10;

//PC_STATUS
const unsigned char PC_STATUS_ERROR_MAX_VALUE=0x01;
const unsigned char PC_STATUS_NODEFINE_COMMAND=0x10;
const unsigned char PC_STATUS_NODEFINE_PARAM_COMMAND=0x11;
const unsigned char PC_STATUS_ERROR_READ_FILE=0xf0;

unsigned char memory[257];

struct Status1{
    unsigned status_cmp:2;
    unsigned status_link:2;
};

Status1 * const status_pc =(Status1 *)&memory[CMP_AND_LINK_STATUS_MEMORY];

unsigned char TEST_PROGRAMM[256]={0,0x10,0x10,0,1};

unsigned char HDD[256][256];

void printMemory(){
    puts(
"╔════╤═════════════════════════╤═════════════════════════╗\n║    │ 00 01 02 03 04 05 06 07 │ 08 09 0A 0B 0C 0D 0E 0F ║\n╠════╪═════════════════════════╪═════════════════════════╣" );
    for(int i=0;i<=0xf;i++){
        printf("║ %X0 │ ",i);
        for (int y = 0; y <= 0xf; y++){
            int b = i*0x10+y;
            if(y==8)printf("│ ");
            if(b==memory[NEXT_COMMAND_MEMORY]){
                printf("%02X<",memory[b]);
            }else
                printf("%02X ",memory[b]);
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

void printGraphicDisplay(){
    //char *display = &memory[0xf6];
    std::string str;
    str="╔════════╗\n";
    for (int i = 0; i < 10; ++i) {
        str+="║";
        std::string spaceChar="□";
        std::string trueChar="■";
        str+=(memory[i]&0b00000001)?trueChar:spaceChar;
        str+=(memory[i]&0b00000010)?trueChar:spaceChar;
        str+=(memory[i]&0b00000100)?trueChar:spaceChar;
        str+=(memory[i]&0b00001000)?trueChar:spaceChar;
        str+=(memory[i]&0b00010000)?trueChar:spaceChar;
        str+=(memory[i]&0b00100000)?trueChar:spaceChar;
        str+=(memory[i]&0b01000000)?trueChar:spaceChar;
        str+=(memory[i]&0b10000000)?trueChar:spaceChar;
        str+=("║\n");
    }
    str+="╚════════╝";
   printf("%s\n",str.c_str()); 
}

void gotoHomeConsole(){
    printf("\033[1;1H");
}

void clearConsole(){
    printf("\033[2J\033[1;1H");
}

void loadProgramm(unsigned char *programm,unsigned char start=START_PROGRAMM_MEMORY,unsigned char size=240){
//    fprintf(stderr,"copy memory start\n size:%02X  start:%02X\n", size, start);
    for (int i = 0; i < size; ++i) {
        memory[i+start]=programm[i];
//        fprintf(stderr,"copy %02X\n", programm[i]);
    }
}

unsigned char * const getOptDefauld(unsigned char * const opt, unsigned char optParam){
const unsigned char OPT_PARAM_VALUE=0b00;
const unsigned char OPT_PARAM_LINK=0b01;
const unsigned char OPT_PARAM_NEXT=0b10;
const unsigned char OPT_PARAM_STACK=0b11;

unsigned char * o;
    switch(optParam%0b100){
        case OPT_PARAM_VALUE:o=opt;break;
        case OPT_PARAM_LINK :o=&memory[*opt]; break;
        case OPT_PARAM_NEXT:o=opt+*opt; break;
        case OPT_PARAM_STACK :o=&memory[memory[STACK_HEAD_MEMORY]-*opt]; break;
    }

    if(optParam/0b100)return &memory[*o];

    return o;
}

void cmd_ldm(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char head = memory[STACK_HEAD_MEMORY]; 
    const unsigned char size = memory[head];
    
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    unsigned char opt2_mod=(status_pc->status_link&0b01)?0b100:0;

    const unsigned char cmdb = memory[i]%0x10;
    opt1_mod+=cmdb%0b100;
    opt2_mod+=cmdb/0b100;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],opt1_mod);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],opt2_mod); 

    loadProgramm(&HDD[*opt1][*opt2],head,size);
    memory[NEXT_COMMAND_MEMORY]+=3;
}

void stk_pop(unsigned char * const opt){
    unsigned char * const head = &memory[STACK_HEAD_MEMORY]; 
    *opt=memory[*head];
    *head=*head-1;
}

void stk_push(unsigned char * const opt){
    unsigned char * const head = &memory[STACK_HEAD_MEMORY]; 
    *head=*head+1;
    memory[*head]=*opt;
}

void cmd_slk(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;


    status_pc->status_link=cmdb%0b100;

    memory[NEXT_COMMAND_MEMORY]+=1;
}

void cmd_stk(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    opt1_mod+=cmdb%0b100;
    unsigned char * const opt = getOptDefauld(&memory[i+1],opt1_mod);


    switch(cmdb/0b100){
        case 0:stk_push(opt);break;
        case 1:stk_pop(opt);break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_PARAM_COMMAND;
    }

    memory[NEXT_COMMAND_MEMORY]+=2;
}
void cmd_invert_jmp(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char cmp = cmdb/0b100; 

    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    opt1_mod+=cmdb%0b100;
    unsigned char * const opt = getOptDefauld(&memory[i+1],opt1_mod);

    if(cmp==status_pc->status_cmp){
        memory[NEXT_COMMAND_MEMORY]+=2;
        if(cmp==0)memory[NEXT_COMMAND_MEMORY]=*opt;
        return;
    }
    memory[NEXT_COMMAND_MEMORY]=*opt;
}



void cmd_jmp(){
    const unsigned char i = memory[NEXT_COMMAND_MEMORY]; 
    const unsigned char cmdb = memory[i]%0x10;
    unsigned char cmp = cmdb/0b100; 

    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    opt1_mod+=cmdb%0b100;
    unsigned char * const opt = getOptDefauld(&memory[i+1],opt1_mod);

    if(cmp==status_pc->status_cmp){
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
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    unsigned char opt2_mod=(status_pc->status_link&0b01)?0b100:0;

    const unsigned char cmdb = memory[i]%0x10;
    opt1_mod+=cmdb%0b100;
    opt2_mod+=cmdb/0b100;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],opt1_mod);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],opt2_mod);

    if(*opt1 <  *opt2)status_pc->status_cmp=1;
    if(*opt1 >  *opt2)status_pc->status_cmp=2;
    if(*opt1 == *opt2)status_pc->status_cmp=3;
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
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    unsigned char opt2_mod=(status_pc->status_link&0b01)?0b100:0;

    const unsigned char cmdb = memory[i]%0x10;
    opt1_mod+=cmdb%0b100;
    opt2_mod+=cmdb/0b100;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],opt1_mod);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],opt2_mod); 

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
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    unsigned char opt2_mod=(status_pc->status_link&0b01)?0b100:0;

    const unsigned char cmdb = memory[i]%0x10;
    opt1_mod+=cmdb%0b100;
    opt2_mod+=cmdb/0b100;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],opt1_mod);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],opt2_mod); 
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
    unsigned char opt1_mod=(status_pc->status_link&0b10)?0b100:0;
    unsigned char opt2_mod=(status_pc->status_link&0b01)?0b100:0;

    const unsigned char cmdb = memory[i]%0x10;
    opt1_mod+=cmdb%0b100;
    opt2_mod+=cmdb/0b100;

    unsigned char * const opt1 = getOptDefauld(&memory[i+1],opt1_mod);
    unsigned char * const opt2 = getOptDefauld(&memory[i+2],opt2_mod);
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
        case 6: cmd_invert_jmp(); break;
        case 7: cmd_stk(); break;
        case 8: cmd_ldm(); break;
        case 9: cmd_slk(); break;
        default: memory[PC_STATUS_MEMORY]=PC_STATUS_NODEFINE_COMMAND;
    }
    if(memory[PC_STATUS_MEMORY]>=0x10)return false;
    return true;
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
    fprintf(stderr,"load: %ld byte\n",size);
    if(size==0){
        memory[PC_STATUS_MEMORY]=PC_STATUS_ERROR_READ_FILE;
        return false;
    }
    
    fclose(file);
    return true;
}

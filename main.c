
typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

static inline void WriteVDPRegister(u32 v);
void _start();
void main();

void _start() { main(); }

static inline void WriteVDPRegister(u32 v)
{
    asm("move.w %0,(0xC00004).l"
    :
    : "g"(v)
    :);
}

void main()
{
    WriteVDPRegister((u32)0x8000);
    while(1){ }
}


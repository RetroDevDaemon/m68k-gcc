//debug.h
extern struct _counters Counters;

void DO_DEBUG(void);

static struct _debugvars \
{ 
    s32 cycles;
    s32 vcycles;
    u8 vch[3];
    u8 vcl[3];
    u8 _zero;
    u8 ch[3];
    u8 cl[3];
    u8 _zeroa;
    u8 zh[3];
    u8 zl[3];
    bool debug_text_enabled;
} debugVars;

void __attribute__((optimize("O3"))) UpdateDebugText()
{
	extern u8 zcyclesl;
	extern u8 zcyclesh;

	// Every word write to the VDP is ~2 cycles. This takes up 24c!
	print(BG_A, 5, 1, (u8*)debugVars.ch);
	print(BG_A, 7, 1, (u8*)debugVars.cl);
    print(BG_A, 5, 3, (u8*)debugVars.vch);
	print(BG_A, 7, 3, (u8*)debugVars.vcl);
	print(BG_A, 5, 5, (u8*)debugVars.zh);
	print(BG_A, 7, 5, (u8*)debugVars.zl);
	
}

void __attribute__((optimize("O3"))) DO_DEBUG(void)
{
    /* Debug Menu */   
    if(1)
    {
        if(Joy1Down(BTN_A_PRESSED) && Joy1Down(BTN_C_PRESSED))
        {
            if(debugVars.debug_text_enabled) 
            {
                debugVars.debug_text_enabled = false;
                print(BG_A, 0, 0, (u8*)"                ");
                print(BG_A, 0, 1, (u8*)"         ");
                print(BG_A, 0, 2, (u8*)"                ");
                print(BG_A, 0, 3, (u8*)"         ");
                print(BG_A, 0, 4, (u8*)"                ");
                print(BG_A, 0, 5, (u8*)"         ");   
            }
            else 
            {
                print(BG_A, 0, 0, (u8*)"CPU Cycles left:");
                print(BG_A, 0, 2, (u8*)"VDP Cycles left:");
                print(BG_A, 0, 4, (u8*)"Z80 Cycles left:");
                debugVars.debug_text_enabled = true;
            }
        }
    }
    UpdateBGScroll();
    if(debugVars.debug_text_enabled)
    {
        if(Counters.thirtyFrameCounter == 0)
            UpdateDebugText();
    }
    debugVars.vcycles = 0;
    while(*((u32*)0xc00004) & 0b1000)
        debugVars.vcycles++;
}

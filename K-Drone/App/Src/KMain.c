#include "KDefine.h"
#include "KMain.h"

void KMain_Init(void)
{
}

void KMain_Update(void)
{
}

void KMain_DeInit(void)
{
}

void KMain_Run(void)
{
    KMain_Init();
    
    while(1)
    {
        KMain_Update();
    }

    KMain_DeInit();
}


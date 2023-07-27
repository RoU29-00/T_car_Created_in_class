#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define INT3    3

#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define PAFR3    (GPIOA_BASE + 0x010)

#define GPIOE_BASE (0x400C0400)
#define PE_DATA  (GPIOE_BASE + 0x00)
#define PE_CR    (GPIOE_BASE + 0x04)
#define PE_IE    (GPIOE_BASE + 0x38)

EXPORT INT usermain(void)
{
    DI(intsts);
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PAFR3 |= (1<<3);

    SetIntMode(INT3, IM_EDGE | IM_LOW);
    
    ClearInt(INT3);
    EnableInt(INT3, 5);
    EI(intsts);

    *(_UW*)PE_CR |= (1<<3);

    while(1) {
        *(_UB*)(PE_DATA) |= (1<<3);
        tk_dly_tsk(500);
        *(_UB*)(PE_DATA) &= ~(1<<3);
        tk_dly_tsk(500);
    }
    return 0;
}
#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートE
#define PE_DATA 0x400C0400
#define PE_CR   0x400C0404

ID cycid;

void cychdr(void *exinf)
{
    static BOOL is_on = FALSE;

    if(is_on){
      *(_UB*)(PE_DATA) &= ~(1<<3);
      is_on = FALSE;
    } 
    else{
      *(_UB*)(PE_DATA) |= (1<<3);
      is_on = TRUE;
    }
    return;
}

EXPORT INT usermain()
{
    T_CCYC  ccyc;

    *(_UW*)PE_CR |= (1<<3);

    ccyc.cycatr = TA_HLNG | TA_STA;
    ccyc.cychdr = cychdr;
    ccyc.cyctim = 500;
    ccyc.cycphs = 500;
    cycid = tk_cre_cyc(&ccyc);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
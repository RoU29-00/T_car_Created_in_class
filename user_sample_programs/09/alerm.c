#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

void almhdr(void *exinf)
{
    tm_printf("hello");
    return;
}

EXPORT INT usermain(void)
{
    T_CALM  calm;
    ID almid;

    calm.almatr = TA_HLNG;
    calm.almhdr = almhdr;
    almid = tk_cre_alm(&calm);
    tk_sta_alm(almid, 3000);
    tk_slp_tsk(TMO_FEVR);

    return 0;
}
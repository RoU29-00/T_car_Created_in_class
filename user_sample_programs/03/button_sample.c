#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)


/* 初期化処理 */
EXPORT INT usermain(void)
{
    UW  sw3, sw4;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    while(1) {
        sw3 = *(_UW*)PADATA & (1<<3);
        sw4 = *(_UW*)PEDATA & (1<<7);
        tm_printf("SW3 = %d  SW4 = %d\n", sw3, sw4);
        tk_dly_tsk(500);
    }
    return 0;       // 本関数は終了しない
}

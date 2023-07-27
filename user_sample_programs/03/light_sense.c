#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// ADC定義
#define ADA_BASE 0x40050000
#define ADACLK  (ADA_BASE + 0x00)
#define ADAMOD0 (ADA_BASE + 0x04)
#define ADAMOD1 (ADA_BASE + 0x08)
#define ADAMOD3 (ADA_BASE + 0x10)
#define ADAMOD4 (ADA_BASE + 0x14)
#define ADAMOD5 (ADA_BASE + 0x18)
#define ADAREG00 (ADA_BASE + 0x34)
#define ADAREG01 (ADA_BASE + 0x38)
#define ADAREG02 (ADA_BASE + 0x3c)
#define ADAREG03 (ADA_BASE + 0x40)

EXPORT INT usermain(void)
{
    UINT d;

    /* ADC */
    *(_UW*)ADACLK  = 0x01;  // クロック供給
    *(_UW*)ADAMOD1 = 0x80;  // ADC有効化
    tk_dly_tsk(1);   // 3マイクロ秒以上待つ
    *(_UW*)ADAMOD3 = 0x01;  // チャンネルスキャンモード
    *(_UW*)ADAMOD4 = 0x00;  // AINA0の1チャンネルのみ

    int count = 0;

    while(1) {
        
        if(count == 5)break;
        /* A/D変換を行う */
        *(_UW*)ADAMOD0 = 0x01; // 変換開始
        while ((*(_UW*)ADAMOD5 & (1 << 1) ) == 0);  // 変換完了待ち
        d = *(_UW*)ADAREG00 & 0xFFF;          // AINA0の変換結果 12ビット分のみを読み出す

        /* 変換結果を表示する */
        tm_printf((UB*)"AINA0: %d\n", d);

        tk_dly_tsk(500); // 500 ミリ秒待つ
        count++;
    }

    return 0;       /* このタスクは終了しない */
}



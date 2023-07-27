#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

// GPIOポートA (SW3)
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define	PAFR1	(GPIOA_BASE + 0x08)
#define	PAFR3	(GPIOA_BASE + 0x10)

#define INT3	(3)

ID	itskid;

void sw3_inthdr(UINT intno)
{
	ClearInt(INT3);			// 要因クリア
	tk_wup_tsk(itskid);		// LED制御タスクを起床

	return;
}

LOCAL void init_pmd(ID dd)
{
	UW	data;
	SZ	asz;
	ER	err;

	data = 14000;
	err = tk_swri_dev( dd, PMD_DATA_PERIOD, &data, 1, &asz);
	data = 14000-1500;
	err = tk_swri_dev( dd, PMD_DATA_PULSE0, &data, 1, &asz);
	data = 1500;
	err = tk_swri_dev( dd, PMD_DATA_PULSE1, &data, 1, &asz);
	data = 1;
	err = tk_swri_dev( dd, PMD_DATA_EN, &data, 1, &asz);
}


ER set_speed(ID dd, INT speed)
{
	UW	data;
	SZ	asz;
	ER	err;
	
	if(speed < -500 || speed > 500) return E_PAR;
	data = 14000 - (1500 - speed);
	err = tk_swri_dev( dd, PMD_DATA_PULSE0, &data, 1, &asz);

	return err;
}


EXPORT	INT	usermain( void )
{
	T_DINT	dint;				/* 割込みハンドラ定義情報 */
	UINT	intsts;
	INT	speed;
	ER	err;
	ID	dd;

	itskid = tk_get_tid();

	/* 割込みハンドラ定義 */
	dint.intatr	= TA_HLNG;
	dint.inthdr	= sw3_inthdr;
	tk_def_int( INT3, &dint);

	/* 割込み設定 */
	DI(intsts);				// 割込み禁止
	*(_UW*)PACR &= ~(1<<3);			// 出力禁止
	*(_UW*)PAIE |= (1<<3);			// 入力許可
	*(_UW*)PAFR3 |= (1<<3);			// INT3選択

	SetIntMode(INT3, IM_EDGE | IM_LOW);	// 割込み検出モード設定
	
	ClearInt(INT3);				// 割込み要因クリア
	EnableInt(INT3, 5);			// 割込み有効＆優先度設定
	EI(intsts);

	dd = tk_opn_dev("pmda", TD_UPDATE);
	init_pmd(dd);
	tk_dly_tsk(500);
	set_speed(dd, 0);

	tk_dly_tsk(3000);
	tm_printf((UB *) "\nPlease press the SET button on the speed controller.\n");
	tm_printf((UB *) "Make sure the LED flashing and Press SW3 to continue setting.\n");
	tk_slp_tsk(TMO_FEVR);
	tm_printf("\nPlease wait...\n");

	for(speed = 25; speed <= 500; speed+=25){
		set_speed(dd, speed);
		tk_dly_tsk(50);
	}
	tk_dly_tsk(3000);
	tm_printf((UB *) "\nPlease press the SET button again on the speed controller.\n");
	tm_printf((UB *) "Make sure the LED flash 2 times and Press SW3 to continue setting.\n");
	tk_slp_tsk(TMO_FEVR);
	tm_printf("\nPlease wait...\n");
	
	set_speed(dd, 0);
	tk_dly_tsk(1000);	
	for(speed = -25; speed >= -500; speed-=25){
		set_speed(dd,speed);
		tk_dly_tsk(50);
	}
	tk_dly_tsk(3000);
	tm_printf((UB *) "\nPlease press the SET button again on the speed controller.\n");
	tm_printf((UB *) "Make sure the LED is turned off and Press SW3 to reset.\n");
	tk_slp_tsk(TMO_FEVR);

	set_speed(dd, 0);
	tm_printf("finish\n");

	tk_slp_tsk(TMO_FEVR);		// このタスクは終了（無限スリープ）
	return 0;
}

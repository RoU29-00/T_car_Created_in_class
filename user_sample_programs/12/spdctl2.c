 #include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#define SENSOR_MAX   4
#define I2C_ATR_EXEC  -100
#define       SNS_TH 10
const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

ID drv_tskid;

typedef struct {
    UH red;
    UH green;
    UH blue;
    UH clear;
}T_COLOR_DATA;

T_COLOR_DATA  cdata[SENSOR_MAX];



ER read_sensor(ID dd, T_COLOR_DATA cdata[]){
    INT i;

    typedef struct {
          UW  sadr;
          W   snd_size;
          UB  *snd_data;
          W   rcv_size;
          UB  *rcv_data;
    }T_I2C_EXEC;

    UH sens_data[4];
    UB data[2];
    T_I2C_EXEC    exec;
    UB            snd_data;
    UB            rcv_data;
    SZ            asz;
    ER            err;

    for(i=0; i<4; i++){
      while(1){
        exec.sadr = sensor_addr[i];
        snd_data = 0x42;
        exec.snd_size = 1;
        exec.snd_data = &snd_data;
        exec.rcv_size = 1;
        exec.rcv_data = &rcv_data;
        
        err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

        if((rcv_data & 0x80) != 0){
          exec.sadr = sensor_addr[i];
          snd_data = 0x50;
          exec.snd_size = 1;
          exec.snd_data = &snd_data;
          exec.rcv_size = sizeof(sens_data);
          exec.rcv_data = (UB*)(&sens_data);

          err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
          cdata[i].clear = sens_data[3];
          break;
        }
      }
    }
}

ER set_speed(ID dd, INT speed){
    UW  data;
    SZ  asz;
    ER  err;

    if(speed < -500 || speed > 500) return E_PAR;
    data = 14000 - (1500 - speed);
    err = tk_swri_dev(dd, PMD_DATA_PULSE0, &data, 1, &asz);
    return err;
}

ER set_steer(ID dd, INT steer){
    UW data;
    SZ asz;
    ER err;

    if(steer < -500 || steer > 500) return E_PAR;
    data = steer + 1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);

    return err;
}

LOCAL INT pid_control(INT e){
    static INT last=0, last_diff=0, steer=0;
    INT diff;
    INT PID_KP=500, PID_KI=0, PID_KD=0;

    diff = e - last;
    steer += ((PID_KP * diff + PID_KI * e + PID_KD * (diff - last_diff)) / 100);

    last = e;
    last_diff = diff;

    return steer;
}

LOCAL void drv_task(void *exinf){
   
    ID sns_dd, pmd_dd;

    sns_dd = tk_opn_dev("iicb", TD_UPDATE);
    UB data[2];
    SZ asz;
    INT i;
    ER err; 
   
    for(i = 0; i < SENSOR_MAX; i++){
      data[0] = 0x40;
      data[1] = 0x80;
      err = tk_swri_dev(sns_dd, sensor_addr[i], &data, 2, &asz);

      data[0] = 0x40;
      data[1] = 0x00;
      err = tk_swri_dev(sns_dd, sensor_addr[i], &data, 2, &asz);

      data[0] = 0x42;
      data[1] = 0x10;
      err = tk_swri_dev(sns_dd, sensor_addr[i], &data, 2, &asz);
    }

    pmd_dd = tk_opn_dev("pmda", TD_UPDATE);
    UW data1;

    data1 = 14000;
    err = tk_swri_dev(pmd_dd, PMD_DATA_PERIOD, &data1, 1, &asz);

    data1 = 14000-1500;
    err = tk_swri_dev(pmd_dd, PMD_DATA_PULSE0, &data1, 1, &asz);

    data1 = 1500;
    err = tk_swri_dev(pmd_dd, PMD_DATA_PULSE1, &data1, 1, &asz);

    data1 = 1;
    err = tk_swri_dev(pmd_dd, PMD_DATA_EN, &data1, 1, &asz);

    INT sns_max, sns_min;

    for(i=0, sns_max=0; i<10; i++){
      err = read_sensor(sns_dd, cdata);
      sns_max +=  cdata[0].clear + cdata[1].clear + cdata[2].clear + cdata[3].clear;
      tk_dly_tsk(100);
    }
    sns_max /= 10;
    sns_min = 100;

    set_speed(pmd_dd, 48);

    while(1){
      INT           diff, steer, sns_now, dir, e;

      err = read_sensor(sns_dd, cdata);
      diff = (cdata[2].clear + cdata[3].clear) - (cdata[0].clear + cdata[1].clear);
      sns_now = cdata[0].clear + cdata[1].clear + cdata[2].clear + cdata[3].clear;

      if(diff > SNS_TH){
        dir = 1;
      }else if (diff < -SNS_TH){
        dir = -1;
      }else {
        dir = 0;
      }

      e = ((sns_max - sns_min)-(sns_now - sns_min))*100 / (sns_max - sns_min);
      steer = pid_control(e);

      if(steer > 500) steer = 500;
      steer *= dir;
      set_steer(pmd_dd, steer * dir);

      tk_dly_tsk(100);
    } 
}

EXPORT  INT usermain(void){
    T_CTSK  drv_ctsk;
    ER      err;

    drv_ctsk.tskatr = TA_HLNG | TA_RNG3;
    drv_ctsk.task   = (FP)drv_task;
    drv_ctsk.itskpri= 10;
    drv_ctsk.stksz  = 1024;

    drv_tskid = tk_cre_tsk( &drv_ctsk );
    tk_sta_tsk(drv_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
/*******************************************************************************
Raspberry Pi用 GPIO 入力プログラム  raspi_gpi

指定したGPIOのポートを入力に設定し、入力値を取得するプログラムです。

    使い方：

        $ ./raspi_gpi ポート番号 設定値

    使用例：

        $ ./raspi_gpi 4           GIPOポート4の入力値を取得
        $ ./raspi_gpi 18          GIPOポート18の入力値を取得
        $ ./raspi_gpi 18 PUP      GIPOポート18をプルアップに設定する(PUP／2)
        $ ./raspi_gpi 18 PDOWN    GIPOポート18をプルダウンに設定する(PDOWN／3)
        $ ./raspi_gpi 18 NC       GIPOポート18を非使用に戻す(NCまたは-1)
        $ ./raspi_gpi 18 0        GIPOポート18が0になるまで待つ
        $ ./raspi_gpi 18 1        GIPOポート18が1になるまで待つ
        $ ./raspi_gpi 18 PUP 0    GIPOポート18をプルアップし、0になるまで待つ
        $ ./raspi_gpi 18 PUP 1    GIPOポート18をプルアップし、1になるまで待つ
        
        ※WiringPi未インストール時のプルアップは一時的に出力を設定することにより
        　実現する模擬方式となります。

    応答値(stdio)
        0       Lレベルを取得
        1       Hレベルを取得
        -1      非使用に設定完了
        9       エラー(エラー内容はstderr出力)
        時間    値待ちで待機したときは待機時間(100ms単位)が戻る
        
    戻り値
        0       正常終了
        -1      異常終了
                                        Copyright (c) 2015-2017 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         // usleep用
#include <string.h>         // strcmp用

#define RasPi_1_REV 2       // 初代Raspberry Pi Tyep B のときのリビジョン
#define RasPi_PORTS 40      // Raspberry Pi GPIO ピン数 初代=26
#define GPIO_RETRY  3       // GPIO 切換え時のリトライ回数
#define S_NUM       8       // 文字列の最大長
//  #define DEBUG               // デバッグモード

int main(int argc,char **argv){
    FILE *fgpio;
    //           0123456789012345678901234567890                // ポート番号
    char gpio[]="/sys/class/gpio/gpio00/value";                 // gpio[20-21]
    char dir[] ="/sys/class/gpio/gpio00/direction";             // dir[20-21]
    char wipi[]="/usr/local/bin/gpio -g mode 00 up/down/tri 2>/dev/null";
    char wip2[]="/usr/bin/gpio -g mode 00 up/down/tri 2>/dev/null";
                                                                // wipi[28-29]
    char s[S_NUM];
    int i;                  // ループ用
    int port;               // GPIOポート
    int value;              // 応答値
    int trig=-1;            // GPIOがtrig値に変化するまで待つ（-1は待たない）
    int pseudoPUpDown=-1;   // 疑似プルアップ・ダウン処理
    
    #if RasPi_1_REV == 1
        /* RasPi      pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16    */
        int pin_ports[]={-1,-1, 0,-1, 1,-1, 4,14,-1,15,17,18,21,-1,22,23,
        /*               17 18 19 20 21 22 23 24 25 26                      */
                         -1,24,10,-1, 9,25,11, 8,-1, 7,
                         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    #else
        /* Pi B Rev1  pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16    */
        int pin_ports[]={-1,-1, 2,-1, 3,-1, 4,14,-1,15,17,18,27,-1,22,23,
        /*               17 18 19 20 21 22 23 24 25 26                      */
                         -1,24,10,-1, 9,25,11, 8,-1, 7,
        /*               27 28 29 30 31 32 33 34 35 36 37 38 39 40          */
                         -1,-1, 5,-1, 6,12,13,-1,19,16,26,20,-1,21};
    #endif
    
    if( argc < 2 || argc > 4 ){
        fprintf(stderr,"usage: %s port [value]\n",argv[0]);
        printf("9\n");
        return -1;
    }
    /* 第1引数portの内容確認 */
    port = atoi(argv[1]);
    for(i=0;i<RasPi_PORTS;i++){
        if( pin_ports[i] == port ){
            #ifdef DEBUG
                printf("Pin = %d, Port = %d\n",i+1,port);
            #endif
            break;
        }
    }
    if( i==RasPi_PORTS || port<0 ){
        fprintf(stderr,"Unsupported Port Error, %d\n",port);
        printf("9\n");
        return -1;
    }
    /* ポート番号の設定 */
    gpio[20]='\0';
    dir[20] ='\0';
    sprintf(gpio,"%s%d/value",gpio,port);
    sprintf(dir,"%s%d/direction",dir,port);
    
    /* ポート開始処理 */
    fgpio = fopen(gpio, "r");
    if( fgpio==NULL ){
        /* ポートの有効化(エラー処理あり) */
        fgpio = fopen("/sys/class/gpio/export","w");
        if(fgpio==NULL ){
            fprintf(stderr,"IO Error\n");
            printf("9\n");
            return -1;
        }
        fprintf(fgpio,"%d\n",port);
        fclose(fgpio);
        #ifdef DEBUG
            printf("Enabled Port\n");
        #endif
        /* ポートの入力設定(リトライ処理・エラー処理あり) */
        for(i=0;i<GPIO_RETRY;i++){
            fgpio = fopen(dir, "w");
            if( fgpio ) break;
            usleep(50000);
        }
        if(i==GPIO_RETRY){
            fprintf(stderr,"IO Error %s\n",dir);
            printf("9\n");
            return -1;
        }
        fprintf(fgpio,"in\n");
        fclose(fgpio);
        #ifdef DEBUG
            printf("Set Direction to IN (tried %d)\n",i);
        #endif
        /* 入力ポートの確認 */
        fgpio = fopen(dir, "r");
        if(fgpio==NULL){
            fprintf(stderr,"IO Error %s\n",gpio);
            printf("9\n");
            return -1;
        }
        fgets(s, S_NUM, fgpio);
        if(strcmp(s,"in\n")){
            fprintf(stderr,"IO Error (IO Mode = %s)\n",s);
            printf("9\n");
            fclose(fgpio);
            return -1;
        }
        fclose(fgpio);
    }
    fclose(fgpio);
    
    /* 第2引数valueの内容確認 */
    if( argc >= 3 ){
        if(!strcmp(argv[2],"NC"))         value=-1;
        else if(!strcmp(argv[2],"LOW"))   value=0;
        else if(!strcmp(argv[2],"HIGH"))  value=1;
        else if(!strcmp(argv[2],"PUP"))   value=2;
        else if(!strcmp(argv[2],"PDOWN")) value=3;
        else value = atoi(argv[2]);
        switch( value ){
            case -1:
                fgpio = fopen("/sys/class/gpio/unexport","w");
                if(fgpio){
                    fprintf(fgpio,"%d\n",port);
                    fclose(fgpio);
                    #ifdef DEBUG
                        printf("Disabled Port\n");
                    #else
                        printf("-1\n");
                    #endif
                    return 0;
                }else{
                    fprintf(stderr,"IO Error\n");
                    printf("9\n");
                    return -1;
                }
                break;
            case 0:
            case 1:
                trig=value;
                break;
            case 2: // PULL UP
                wipi[28]='\0'; wip2[22]='\0';
                sprintf(wip2,"%s %d up 2> /dev/null",wip2,port);
                sprintf(wipi,"%s %d up",wipi,port);
                if(system(wip2)){
                    if(system(wipi)){
                        fgpio = fopen(dir, "w");
                        if(fgpio){
                            fprintf(fgpio,"high\n");
                            fclose(fgpio);
                            pseudoPUpDown=1;
                            #ifdef DEBUG
                                printf("Port Pulled Up\n");
                            #endif
                        }else{
                            fprintf(stderr,"IO Error\n");
                            printf("9\n");
                            return -1;
                        }
                    }
                }
                if( argc == 4 ){
                    if(!strcmp(argv[3],"LOW")) trig=0;
                    else if(!strcmp(argv[3],"HIGH")) trig=1;
                    else trig = atoi(argv[3]);
                    if( trig%2 != trig) trig=-1;
                }
                break;
            case 3: // PULL DOWN
                wipi[28]='\0'; wip2[22]='\0';
                sprintf(wip2,"%s %d down 2> /dev/null",wip2,port);
                sprintf(wipi,"%s %d down",wipi,port);
                if(system(wip2)){
                    if(system(wipi)){
                        fgpio = fopen(dir, "w");
                        if(fgpio){
                            fprintf(fgpio,"low\n");
                            fclose(fgpio);
                            pseudoPUpDown=0;
                            #ifdef DEBUG
                                printf("Port Pulled Down\n");
                            #endif
                        }else{
                            fprintf(stderr,"IO Error\n");
                            printf("9\n");
                            return -1;
                        }
                    }
                }
                if( argc == 4 ){
                    if(!strcmp(argv[3],"LOW")) trig=0;
                    else if(!strcmp(argv[3],"HIGH")) trig=1;
                    else trig = atoi(argv[3]);
                    if( trig%2 != trig) trig=-1;
                }
                break;
            default:
                fprintf(stderr,"Unsupported Value Error, %d\n",value);
                printf("9\n");
                return -1;
        }
    }
    if( pseudoPUpDown >= 0){
        fprintf(stderr,"Pseudo Pull Up / Down mode (%d)\n",pseudoPUpDown);
        fprintf(stderr,"WiringPiをインストールしてください。\n");
        fprintf(stderr,"git clone git://git.drogon.net/wiringPi\n");
    }
    
    /* ポート入力処理 */
    fgpio = fopen(gpio, "r");
    if(fgpio==NULL ){
        fprintf(stderr,"IO Error\n");
        printf("9\n");
        return -1;
    }
    fgets(s, S_NUM, fgpio);
    value = atoi(s);
    fclose(fgpio);
    
    /* 期待値trigの待ち受け処理 */
    if( trig >= 0 ){
        i=0;
        if(pseudoPUpDown<0){
            while( value != trig ){
                fgpio = fopen(gpio, "r");
                fgets(s, S_NUM, fgpio);
                value = atoi(s);
                i++;
                fclose(fgpio);
                usleep(100000);
            }
        }else{
            while( value != trig ){
                fgpio = fopen(dir, "w");
                if(pseudoPUpDown==0)fprintf(fgpio,"low\n");
                else fprintf(fgpio,"high\n");
                fclose(fgpio);
                fgpio = fopen(gpio, "r");
                fgets(s, S_NUM, fgpio);
                value = atoi(s);
                i++;
                fclose(fgpio);
                fgpio = fopen(dir, "w");
                fprintf(fgpio,"in\n");
                fclose(fgpio);
                usleep(100000);
            }
        }
        value = i;
    }
    
    /* 疑似プルアップ終了処理(リトライ処理・エラー処理あり) */
    for(i=0;i<GPIO_RETRY;i++){
        fgpio = fopen(dir, "r");
        if( fgpio ) break;
        usleep(50000);
    }
    if(i==GPIO_RETRY){
        fprintf(stderr,"IO Error %s\n",dir);
        printf("9\n");
        return -1;
    }
    fgets(s, S_NUM, fgpio);
    if(strcmp(s,"in\n")){
        for(i=0;i<GPIO_RETRY;i++){
            fgpio = fopen(dir, "w");
            if( fgpio ) break;
            usleep(50000);
        }
        if(i==GPIO_RETRY){
            fprintf(stderr,"IO Error %s\n",dir);
            printf("9\n");
            return -1;
        }
        if(fgpio){
            fprintf(fgpio,"in\n");
            fclose(fgpio);
        }
    }
    
    /* ポート状態の出力 */
    #ifdef DEBUG
        if( trig < 0 ) printf("%s = ",gpio);
        else printf("Time = ");
    #endif
    printf("%d\n",value);
    return 0;
}

/* 

参考資料(1) 標準ファイル入出力でGPIOを制御する

    ポート利用の開始：exportへ使用するポート番号を入力する
    echo 5 > /sys/class/gpio/export
         ~
    入力設定：gpio27/directionへinを入力する
    echo in  > /sys/class/gpio/gpio27/direction
                                   ~~
    入力方法：gpio27/valueの値を読み取る
    cat /sys/class/gpio/gpio27/value
                            ~~
    出力設定：gpio5/directionへin／outを入力する
    echo out > /sys/class/gpio/gpio5/direction
                                   ~
    出力方法：gpio5/valueへ値を入力する
    /sys/class/gpio/gpio5/value
                        ~
    ポート利用の終了：unexportへ中止するポート番号を入力する
    echo 18 > /sys/class/gpio/unexport
         ~~
    参考文献
    https://tool-lab.com/2013/12/raspi-gpio-controlling-command-1/

参考資料(2) Wiring Piの命令を使用する ※Wiring Piのインストールが必要

    入力設定：
    gpio -g mode 27 in
                 ~~
    入力方法：
    gpio -g read 27
                 ~~
    出力設定：
    gpio -g mode 5 out
                 ~
    出力方法：
    gpio -g write 5 1
                  ~
    参考文献
    https://tool-lab.com/make/raspberrypi-startup-24/
    
参考資料(3) Command-Line access to Raspberry Pi's GPIO  // wiringPi September 2015

    gpio オプション

    -g     Use the BCM_GPIO pins numbers rather than wiringPi pin numbers.

    gpio mode <pin> <mode>
        Set a pin into input, output or pwm mode. Can also use the literals up, down or tri  to
        set the internal pull-up, pull-down or tristate (off) controls.
    
    EXAMPLES
       gpio mode 4 output               # Set pin 4 to output
       gpio -g mode 23 output           # Set GPIO pin 23 to output (same as WiringPi pin 4)
       gpio mode 1 pwm                  # Set pin 1 to PWM mode
       gpio pwm 1 512                   # Set pin 1 to PWM value 512 - half brightness
       gpio export 17 out               # Set GPIO Pin 17 to output
       gpio export 0 in                 # Set GPIO Pin 0 (SDA0) to input.
       gpio -g read 0                   # Read GPIO Pin 0 (SDA0)

    権利情報
        WiringPi's home page
            http://wiringpi.com/
        AUTHOR
            Gordon Henderson
        REPORTING BUGS
            Please report bugs to <projects@drogon.net>
        COPYRIGHT
            Copyright (c) 2012-2015 Gordon Henderson This is free software; see  the  source  for  copying
            conditions.  There  is  NO  warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR
            PURPOSE.

*/

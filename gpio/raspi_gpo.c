/*******************************************************************************
Raspberry Pi用 GPIO 出力プログラム  raspi_gpo

指定したGPIOのポートを出力に設定し、指定した値に変更するためのプログラムです。

    使い方：

        $ raspi_gpo ポート番号 設定値

    使用例：

        $ raspi_gpo 4 1         GIPOポート4に1(Hレベル)を出力
        $ raspi_gpo 18 0        GIPOポート18に0(Lレベル)を出力
        $ raspi_gpo 18 -1       GIPOポート18を非使用に戻す

    応答値(stdio)
        0       Lレベルを出力完了
        1       Hレベルを出力完了
        -1      非使用に設定完了
        9       エラー(エラー内容はstderr出力)

    戻り値
        0       正常終了
        -1      異常終了
                                        Copyright (c) 2015-2016 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         // usleep用

#define RasPi_1_REV 2       // Raspberry Pi 1 Type B の場合のリビジョン 通常=2
#define RasPi_PORTS 26      // Raspberry Pi GPIO ピン数 26 固定
#define GPIO_RETRY  3       // GPIO 切換え時のリトライ回数
//  #define DEBUG               // デバッグモード

int main(int argc,char **argv){
    FILE *fgpio;
    //           012345678901234567890      gpio[20]と[21]にポート番号が入る
    char gpio[]="/sys/class/gpio/gpio00/value";
    char dir[] ="/sys/class/gpio/gpio00/direction";
    int i;              // ループ用
    int port;           // GPIOポート
    int value;          // 応答値
    
    #if RasPi_1_REV == 1
        /* RasPi      pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16    */
        int pin_ports[]={-1,-1, 0,-1, 1,-1, 4,14,-1,15,17,18,21,-1,22,23,
        /*               17 18 19 20 21 22 23 24 25 26                      */
                         -1,24,10,-1, 9,25,11, 8,-1, 7};
    #else
        /* Pi B Rev1  pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16    */
        int pin_ports[]={-1,-1, 2,-1, 3,-1, 4,14,-1,15,17,18,27,-1,22,23,
        /*               17 18 19 20 21 22 23 24 25 26                      */
                         -1,24,10,-1, 9,25,11, 8,-1, 7};
    #endif
    
    if( argc != 3 ){
        fprintf(stderr,"usage: %s port value\n",argv[0]);
        printf("9\n");
        return -1;
    }
    /* 第1引数portと第2引数valueの内容確認 */
    port = atoi(argv[1]);
    value = atoi(argv[2]);
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
    if( value<-1 || value>1 ){
        fprintf(stderr,"Unsupported Value Error, %d\n",value);
        printf("9\n");
        return -1;
    }
    if( value == -1 ){
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
    }
    /* ポート番号の設定 */
    gpio[20]='\0';
    dir[20]='\0';
    sprintf(gpio,"%s%d/value",gpio,port);
    sprintf(dir,"%s%d/direction",dir,port);
    /* ポート開始処理 */
    fgpio = fopen(gpio, "w");
    if( fgpio==NULL ){
        fgpio = fopen("/sys/class/gpio/export","w");
        if(fgpio==NULL ){
            fprintf(stderr,"IO Error\n");
            printf("9\n");
            return -1;
        }else{
            fprintf(fgpio,"%d\n",port);
            fclose(fgpio);
            #ifdef DEBUG
                printf("Enabled Port\n");
            #endif
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
            fprintf(fgpio,"out\n");
            fclose(fgpio);
            #ifdef DEBUG
                printf("Set Direction to OUT (tried %d)\n",i);
            #endif
            fgpio = fopen(gpio, "w");
            if(fgpio==NULL){
                fprintf(stderr,"IO Error %s\n",gpio);
                printf("9\n");
                return -1;
            }
        }
    }
    /* ポート出力処理 */
    fprintf(fgpio,"%d\n",value);
    fclose(fgpio);
    #ifdef DEBUG
        printf("%s = ",gpio);
    #endif
    printf("%d\n",value);
    return 0;
}

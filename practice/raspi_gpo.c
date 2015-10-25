/*******************************************************************************
Raspberry Pi用 GPIO 出力プログラム

                                                Copyright (c) 2015 Wataru KUNINO
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define RasPi_PORTS	26				
#define GPIO_RETRY	1023			

int main(int argc,char **argv){
    FILE *fgpio;
    //           012345678901234567890      gpio[20]と[21]にポート番号が入る
    char gpio[]="/sys/class/gpio/gpio00/value";
    char dir[] ="/sys/class/gpio/gpio00/direction";
    int i;
    int port;
    int value;
    /* RasPi      pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26*/
    int pin_ports[]={-1,-1, 2,-1, 3,-1, 4,14,-1,15,17,18,27,-1,22,23,-1,24,10,-1, 9,25,11, 8,-1, 7};
    /* Pi B Rev1  pin 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26*/
//  int pin_ports[]={-1,-1, 0,-1, 1,-1, 4,14,-1,15,17,18,21,-1,22,23,-1,24,10,-1, 9,25,11, 8,-1, 7};
    
    if( argc != 3 ){
        fprintf(stderr,"usage: %s port value\n",argv[0]);
        return -1;
    }
    port = atoi(argv[1]);
    value = atoi(argv[2]);
    for(i=0;i<RasPi_PORTS;i++){
        if( pin_ports[i] == port ){
            printf("Pin = %d, Port = %d\n",i+1,port);
            break;
        }
    }
    if( i==RasPi_PORTS || port<0 ){
        fprintf(stderr,"Unsupported Port Error, %d\n",port);
        return -1;
    }
    if( value<-1 || value>1 ){
        fprintf(stderr,"Unsupported Value Error, %d\n",value);
        return -1;
    }
    if( value == -1 ){
        fgpio = fopen("/sys/class/gpio/unexport","w");
        if(fgpio){
            fprintf(fgpio,"%d\n",port);
            fclose(fgpio);
            printf("Disabled Port\n");
            return 0;
        }else{
            fprintf(stderr,"IO Error\n");
            return -1;
        }
    }
    
    gpio[20]='\0';
    dir[20]='\0';
    sprintf(gpio,"%s%d/value",gpio,port);
    sprintf(dir,"%s%d/direction",dir,port);
    
    fgpio = fopen(gpio, "w");
    if( fgpio==NULL ){
        fgpio = fopen("/sys/class/gpio/export","w");
        if(fgpio==NULL ){
            fprintf(stderr,"IO Error\n");
            return -1;
        }else{
            fprintf(fgpio,"%d\n",port);
            fclose(fgpio);
            printf("Enabled Port\n");
            for(i=0;i<GPIO_RETRY;i++){
                fgpio = fopen(dir, "w");
                if( fgpio ) break;
            }
            if(i==GPIO_RETRY){
                fprintf(stderr,"IO Error %s\n",dir);
                return -1;
            }
            fprintf(fgpio,"out\n");
            fclose(fgpio);
            printf("Set Direction (tryed %d)\n",i);
            fgpio = fopen(gpio, "w");
            if(fgpio==NULL){
                fprintf(stderr,"IO Error %s\n",gpio);
                return -1;
            }
        }
    }
    fprintf(fgpio,"%d\n",value);
    printf("%s = %d\n",gpio,value);
    fclose(fgpio);
    return 0;
}

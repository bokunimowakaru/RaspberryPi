#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用
#include<string.h>
int TEMPERATURE_OFFSET = 25.0;  // 補正値(RaspPi温度上昇)

int main(int argc,char **argv){
    FILE *pp;                   // コマンド出力用の変数pp
    char cmd[]=                 // コマンドをcmdに代入
    "cat /sys/devices/virtual/thermal/thermal_zone0/temp";
    float in;                   // 数値変数inを定義
    char s[8];                  // 文字列変数sを定義
    
    if( argc == 2 ){
        in = atoi(argv[1]);
        if(strcmp(argv[1],"pi3")==0) in=25.0;
        if(strcmp(argv[1],"pi2")==0) in=21.0;
        if(strcmp(argv[1],"pi1")==0) in=26.0;
        if(strcmp(argv[1],"zerow")==0) in=11.0;
        if(strcmp(argv[1],"zero")==0) in=7.0;
        if(in)TEMPERATURE_OFFSET=in;
        else if(strcmp(argv[1],"0")==0){
            TEMPERATURE_OFFSET=0.0;
        }else{
            fprintf(stderr,"usage: %s [offset]\n",argv[0]);
            return -1;
        }
    }
    
    pp = popen(cmd, "r");       // GPIO用ファイルを開く
    if( pp == NULL ){           // 失敗時
        printf("ERROR\n");      // エラー表示
        return -1;              // 異常終了
    }
    fgets(s, 8, pp);            // コマンドの戻り値を取得
    in = (float)atoi(s);        // 数値に変換してinに代入
    printf("%3.1f\n",in/1000.-TEMPERATURE_OFFSET);
    pclose(pp);                 // コマンド出力を閉じる
    return 0;                   // 関数mainの正常終了(0)
}

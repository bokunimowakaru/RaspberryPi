#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用
#define TEMPERATURE_OFFSET 25.0 // 補正値(RaspPi温度上昇)

int main(){                     // プログラムのメイン関数
    FILE *pp;                   // コマンド出力用の変数pp
    char cmd[]=                 // コマンドをcmdに代入
    "cat /sys/devices/virtual/thermal/thermal_zone0/temp";
    float in;                   // 数値変数inを定義
    char s[8];                  // 文字列変数sを定義
    
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

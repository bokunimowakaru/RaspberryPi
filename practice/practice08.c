#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用

int main(){                     // プログラムのメイン関数
    FILE *pp;                   // コマンド出力用の変数pp
    char cmd[]=                 // コマンドをcmdに代入
    "cat /sys/devices/virtual/thermal/thermal_zone0/temp";
    int in = 0;                 // 数値変数inを定義
    float temp;                 // 温度保存用の変数
    float prev = -99.9;         // 前回の温度の保存用
    int count=0;                // 温度低下回数カウント用
    char s[8];                  // 文字列変数sを定義
    
    printf("Practice 08\n");    // 「Practice 08」を表示

    do{                         // 繰り返す
        pp = popen(cmd, "r");   // GPIO用ファイルを開く
        if( pp == NULL ){       // 失敗時
            printf("ERROR\n");  // エラー表示
            return -1;          // 異常終了
        }
        fgets(s, 8, pp);        // コマンドの戻り値を取得
        in = atoi(s);           // 数値に変換してinに代入
        temp = (float)in/1000.; // tempにin÷1000を代入
        printf("Temp=%.1f[C]\n",temp);  // 温度を表示
        pclose(pp);             // コマンド出力を閉じる
        if( temp < prev ){      // 温度が低下した時
            count++;            // countの値を増やす
        }else{
            count=0;            // countを0にリセット
        }
        prev = temp;            // 測定値を前回値へ代入
        system("sleep 1");      // 1秒待ち
    }while( count < 2 );        // count2回未満で繰り返し
    return 0;                   // 関数mainの正常終了(0)
}

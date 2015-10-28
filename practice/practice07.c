#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用

int main(){
    FILE *pp;                   // ファイル出力用の変数fp
    char gpo[]="./raspi_gpo 4"; // raspi_gpo コマンド
    char cmd[sizeof(gpo)+2];	// コマンド保存用
    int in = 0;                 // 数値変数inを定義
    char s[5];                  // 文字列変数sを定義
    
    printf("Practice 07\n");	// 「Practice 07」を表示

    while(1){                   // 繰り返す
        fputs("in > ",stdout);  // 「in > 」を標準出力へ
        fgets(s, 5, stdin);     // 標準入力から取得
        in = atoi(s);           // 数値に変換してinに代入
        printf("in=%d ",in);    // inの値を表示
        if(in<0 || in>1) break; // 0～1以外の時に抜ける
        sprintf(cmd, "%s %d", gpo, in);	// コマンド作成
        pp = popen(cmd, "r");  	// GPIO用ファイルを開く
        if( pp == NULL ){       // 失敗時
            printf("ERROR\n");  // エラー表示
            return -1;          // 異常終了
        }
        fgets(s, 5, pp);     	// シェルから戻り値を取得
        in = atoi(s);           // 数値に変換してinに代入
        printf("ret=%d\n",in);  // 戻り値を表示
        pclose(pp);             // ファイルを閉じる
    }
    return 0;                   // 関数mainの正常終了(0)
}

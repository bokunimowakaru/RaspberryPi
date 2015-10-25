#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用

int main(){                     // プログラムのメイン関数
    FILE *fp;                   // ファイル出力用の変数fp
    char fname[]="data.csv";    // ファイル名
    int in = 0;                 // 数値変数inを定義
    char s[5];                  // 文字列変数sを定義
    
    printf("Practice 05\n");    // 「Practice 05」を表示
    
    do{                         // do～while間を繰り返す
        fputs("in > ",stdout);  // 「in > 」を標準出力へ
        fgets(s, 5, stdin);     // 標準入力から取得
        in = atoi(s);           // 数値に変換してinに代入
        printf("in=%d\n",in);   // inの値を表示
        fp = fopen(fname, "a"); // ファイル作成(上書き)
        if( fp == NULL ){       // 作成失敗時
            printf("ERROR\n");  // エラー表示
            return -1;          // 異常終了
        }
        fprintf(fp,"%d\n",in);  // inの値を書き込み
        fclose(fp);             // ファイルを閉じる
    }while( in > 0 );           // 0より大の時に繰り返す
    return 0;                   // 関数mainの正常終了(0)
}

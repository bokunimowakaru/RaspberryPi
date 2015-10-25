#include <stdio.h>              // 標準入出力を使用
#include <stdlib.h>             // 型変換(atoi)を使用

int main(){                     // プログラムのメイン関数
    FILE *fp;                   // ファイル出力用の変数fp
    char fname[]="data.csv";    // ファイル名
    int in = 0;                 // 数値変数inを定義
    int i;                      // 数値変数iを定義
    char s[16];                 // 文字列変数sを定義
    
    printf("Practice 06\n");    // 「Practice 06」を表示

    fp = fopen(fname, "r");     // ファイル作成(読み取り)
    if( fp == NULL ){           // 作成失敗時
        printf("ERROR\n");      // エラー表示
        return -1;              // 異常終了
    }
    while( 1 ){                 // 繰り返し
        fgets(s, 16, fp);       // ファイルからデータ取得
        if( feof(fp) ){         // データ終了の時に
            printf("EOF\n");    // 「EOF」を表示してから
            break;              // whileの繰り返しを抜ける
        }
        in = atoi(s);           // 数値に変換してinに代入
        for(i=0; i < in; i++){  // inの値の回数だけ繰り返す
            printf("#");        // 「#」を表示
        }
        printf("\tin=%d\n",in); // inの値を表示
    }
    fclose(fp);                 // ファイルを閉じる
    return 0;                   // 関数mainの正常終了(0)
}

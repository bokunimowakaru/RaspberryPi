#include <stdio.h>              // 標準入出力を使用

int main(){                     // プログラムのメイン関数
    int a = 12345;              // 整数型の変数aを定義
    char c = 'R';               // 文字変数cを定義
    char s[] = "Hello, World!"; // 文字列変数sを定義
    float v = 1.2345;           // 浮動小数点数型変数v
    int size;                   // 変数sizeを定義
    
    printf("Practice 02\n");    // 「Practice 02」を表示
    
    printf("a=%d",a);           // 整数値変数aの値を表示
    size = sizeof(a);           // aのサイズをsizeに代入
    printf("\tsize=%d\n",size); // sizeの値を表示して改行
    
    printf("c=%c",c);           // 文字変数cの値を表示
    size = sizeof(c);           // cのサイズをsizeに代入
    printf("\tsize=%d\n",size); // sizeの値を表示して改行
    
    printf("s=%s",s);           // 文字列変数sの値を表示
    size = sizeof(s);           // sのサイズをsizeに代入
    printf("\tsize=%d\n",size); // sizeの値を表示して改行
    
    printf("v=%f",v);           // 数値変数vの値を表示
    size = sizeof(v);           // vのサイズをsizeに代入
    printf("\tsize=%d\n",size); // sizeの値を表示して改行
    
    return 0;                   // 関数mainの正常終了(0)
}

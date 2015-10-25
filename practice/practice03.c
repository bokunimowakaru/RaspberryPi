#include <stdio.h>              // 標準入出力を使用

int main(){                     // プログラムのメイン関数
    int a = 12345;              // 整数型の変数aを定義
    float v = 1.2345;           // 浮動小数点数型変数v
    
    printf("Practice 03\n");    // 「Practice 03」を表示
    
    printf("%d ",a);            // 整数値変数aの値を表示
    a = a - 345;				// a-345を計算してaに代入
    printf("- 345 = %d\n",a);   // aの値を表示
    
    printf("%d ",a);            // 整数値変数aの値を表示
    a = a / 1000;				// a÷1000をaに代入
    printf("/ 1000 = %d\n",a);  // aの値を表示
    
    v = (float) a;				// 浮動小数変数vにaを代入
    printf("(int) %d ",a);      // 整数値変数aの値を表示
    a = a / 10;					// a÷10をaに代入
    printf("/ 10 = %d\n",a);  	// aの値を表示
    printf("(float) %f ",v);    // 浮動小数変数vの値を表示
    v = v / 10;					// v÷10をvに代入
    printf("/ 10 = %f\n",v);    // 浮動小数変数vの値を表示
    
    return 0;                   // 関数mainの正常終了(0)
}

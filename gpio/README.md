# Raspberry Pi用 GPIO プログラム 集
- Raspberry Pi用 GPIO 出力プログラム raspi_gpo
- Raspberry Pi用 GPIO 入力プログラム raspi_gpi
- Raspberry Pi用 I2C SPI UART センサ制御プログラム raspi_*
- Raspberry Pi + Apple Pi 用 デモ プログラム
- Raspberry Pi用 GPIO 出力プログラム  raspi_gpo

指定したGPIOのポートを出力に設定し、指定した値に変更するためのプログラムです。

    コンパイル方法

        $ make

    使い方：

        $ raspi_gpo ポート番号 設定値

    使用例：

        $ raspi_gpo 4 1         GIPOポート4に1(Hレベル)を出力
        $ raspi_gpo 18 0        GIPOポート18に0(Lレベル)を出力
        $ raspi_gpo 18 -1       GIPOポート18を非使用に戻す

    応答値
        0       Lレベルを出力完了
        1       Hレベルを出力完了
        -1      非使用に設定完了
        9        エラー(内容はstderr出力)

    http://blogs.yahoo.co.jp/bokunimowakaru/55117684.html

# Raspberry Pi用 GPIO 入力プログラム  raspi_gpi

指定したGPIOのポートを入力に設定し、入力値を取得するプログラムです。

    使い方：

        $ raspi_gpi ポート番号 設定値

    使用例：

        $ raspi_gpi 4           GIPOポート4の入力値を取得
        $ raspi_gpi 18          GIPOポート18の入力値を取得
        $ raspi_gpi 18 -1       GIPOポート18を非使用に戻す

    応答値
        0       Lレベルを取得
        1       Hレベルを取得
        -1      非使用に設定完了
        9        エラー(内容はstderr出力)

    http://blogs.yahoo.co.jp/bokunimowakaru/55117684.html

# Raspberry Pi用 (I2C等) 制御プログラム  raspi_*
# 各種I2C等の温度センサ・湿度センサ・気圧センサに対応

    使い方(BME280)：

        $ raspi_bme280 I2Cアドレス(16進数)

    使用例(BME280)：

        $ raspi_bme280          デフォルトせって
        $ raspi_bme280 76       SDOピンをGNDに接続した場合
        $ raspi_bme280 77       SDOピンをVDDIOに接続した場合

    温  湿  気  電  メーカ・型番    プログラム
    度  度  圧  圧
    〇  〇  －  －  TI HDC1000      raspi_hdc1000.c 
    〇  〇  －  －  Silicon Si7021  raspi_si7021.c
    〇  〇  〇  －  BOSCH BME280    raspi_bme280.c 
    〇  －  〇  －  BOSCH BMP280    raspi_bme280.c 
    〇  〇  －  －  Aosong AM2320   raspi_am2320.c 
    〇  －  〇  －  STMicro LPS25H  raspi_lps25h.c 
    〇  －  －  －  STMicro STTS751 raspi_stts751.c 
    －  －  －  〇  TI ADS1115      raspi_ads1115.c 
    〇  －  －  －  MAXIM MAX6675   raspi_max6675.c 
    －  〇  －  －  RasPi 内蔵      raspi_temp.c 
    加速度 Analog Devices ADXL345   raspi_adxl345.c 
    二酸化炭素      AMS CCS811      raspi_ccs811.c
    二酸化炭素      MH-Z19          raspi_mhz19.c

# Raspberry Pi + Apple Pi 用 デモ プログラム

    使用例：

        $ ./apple_pi.sh

    http://blogs.yahoo.co.jp/bokunimowakaru/55431206.html

----------------------------------------------------------------
ライセンス・著作権表示

本資料を複製・改変・再配布する場合は著作権表示が必要です。

                  Copyright (C) 2015-2017 国野亘 (Wataru KUNINO)
               http://www.geocities.jp/bokunimowakaru/diy/raspi/
----------------------------------------------------------------

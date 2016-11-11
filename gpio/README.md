# Raspberry Pi用 GPIO 出力プログラム  raspi_gpo

指定したGPIOのポートを出力に設定し、指定した値に変更するためのプログラムです。

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

# Web Site

	http://blogs.yahoo.co.jp/bokunimowakaru/55117684.html

----------------------------------------------------------------
ライセンス・著作権表示

本資料を複製・改変・再配布する場合は著作権表示が必要です。

                  Copyright (C) 2015-2016 国野亘 (Wataru KUNINO)
                       http://www.geocities.jp/bokunimowakaru/
----------------------------------------------------------------

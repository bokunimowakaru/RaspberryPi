#!/bin/bash
# HALTコマンドをUDPで送信する
# Copyright (c) 2023 Wataru KUNINO

cd `dirname $0`

echo "UDP HALTER Client"         # タイトル表示
echo "uhalt_0,1" | ../udp_sender.py                 # 送信実行

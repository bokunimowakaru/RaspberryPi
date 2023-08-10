#!/bin/bash
# HALTコマンドをUDPで送信する
# Copyright (c) 2023 Wataru KUNINO

cd `dirname $0`

echo "UDP HALTER Client"             # タイトル表示
echo "HALTのブロードキャスト送信(255.255.255.255)"
ping -c 1 -b 255.255.255.255 > /dev/null
sleep 1
echo "uhalt_0,1" | ../udp_sender.py
sleep 1

IP=`hostname -I|tr " " "\n"|grep -Eo '([0-9]*\.){3}[0-9]*'|grep -v "127.0."`
for ip in $IP; do
	broad_ip=`echo $ip|cut -d. -f1-3`".255"
	echo "HALTのブロードキャスト送信("${broad_ip}")"
	sleep 1
	ping -c 1 -b ${broad_ip} > /dev/null
	# echo "uhalt_0,1" | ../udp_sender_uni.py ${broad_ip} 1024 # 送信実行
	echo "uhalt_0,1" | ../udp_sender.py
	sleep 1
	((i++))
done


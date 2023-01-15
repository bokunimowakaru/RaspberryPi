#!/usr/bin/env python3
# coding: utf-8

import sys
import os
import subprocess
import datetime
from time import sleep									# スリープ実行モジュールの取得

class RaspiLcd:

	def __init__(self,ignoreError=False,x=16,reset=0):	# コンストラクタ作成
		self.restoreUsedGpio = False					# 使用したGPIOを終了時に開放しない
		self.title = "ﾎﾞｸﾆﾓﾜｶﾙ Rasp.Pi by bokunimo.net"
		self.dir = os.path.dirname(__file__)
		self.ignoreError = ignoreError
		self.reset_port  = reset						# GPIO ポート番号
		self.width = x									# LCD Digits
		self.bar = None									# 棒グラフの初期化状態
		print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), end=' ') # 日時
		print('LCD initialized')

	def help(self):
		path = self.dir + '/raspi_lcd'					# raspi_lcd モジュールのパス
		app = [path, '-h']								# 起動オプション
		res = subprocess.run(app, stdout=subprocess.PIPE) # サブプロセスとして起動
		data = res.stdout.decode().strip()				# 結果をdataへ代入
		ret = res.returncode							# 終了コードをdataへ代入
		if ret != 0:									# エラー時
			print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), end=' ') # 日時
			print('LCD ERROR in def help')
			print('app =', app) 						# サブ起動する設定内容を表示
			print('ret=', ret)							# 結果データを表示
			print('data=', data)						# 結果データを表示
			raise Exception('ERROR: raspi_lcd, return data='+ret)
		return data

	def print(self, data=None, y=1):
		if self.width != 8 and self.width != 16:
			raise Exception('ERROR: LCD width')
		if data is None:
			data = self.title
		path = self.dir + '/raspi_lcd'					# raspi_lcd モジュールのパス
		app = [path]	# 起動設定
		if self.ignoreError == True:
			app.append('-i')
		if self.width > 8:
			app.append('-w'+str(self.width))
		if y == 2:
			app.append('-y'+str(y))
		elif self.reset_port > 0:
			app.append('-r'+str(self.reset_port))
		app.append(data)
		# print(app)									# DEBUG app引数確認用
		res = subprocess.run(app,input=None,stdout=subprocess.PIPE)# サブプロセスとして起動
		ret = res.returncode							# 終了コードをretへ代入
		print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), end=' ') # 日時
		if ret != 0:
			data = res.stdout.decode().strip()			# 結果をdataへ代入
			print('LCD ERROR in def print') 			# サブ起動する設定内容を表示
			print('app =', app) 						# サブ起動する設定内容を表示
			print('ret =', data)						# 結果データを表示
			print('data=', data)						# 結果データを表示
			raise Exception('ERROR: print')
		else:
			if len(data) > self.width:
				print('LCD', data[0:self.width], '/', data[self.width:2*self.width])
			else:
				print('LCD', data[0:self.width])
		return ret

	def printBar(self, data=[0], y=1):
		if self.width != 8 and self.width != 16:
			raise Exception('ERROR: LCD width')
		path = self.dir + '/raspi_lcd'					# raspi_lcd モジュールのパス
		app = [path]	# 起動設定
		if self.ignoreError == True:
			app.append('-i')
		if self.bar is None:
			if self.reset_port > 0:
				app.append('-r'+str(self.reset_port))
		else:
			app.append('-n')
		self.bar = 'inited'
		app.append('-b')
		if self.width > 8:
			app.append('-w'+str(self.width))
		if y == 2:
			app.append('-y'+str(y))
		app.append(str(data[0]))
		if len(data) >= 2:
			app.append(str(data[1]))
		# print(app)									# DEBUG app引数確認用
		res = subprocess.run(app,input=None,stdout=subprocess.PIPE)# サブプロセスとして起動
		ret = res.returncode							# 終了コードをretへ代入
		if ret != 0:
			raise Exception('ERROR: bar')
		print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), end=' ') # 日時
		print('LCD printBar', data)
		return ret

	def __del__(self):									# インスタンスの削除
		if self.restoreUsedGpio and self.reset_port > 0:
			print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), end=' ') # 日時
			print('LCD restore GPIO'+str(self.reset_port), 'to free')
			sleep(5)
			path = self.dir + '/raspi_lcd'				# IR 受信ソフトモジュールのパス
			app = [path, '-q'+str(self.reset_port)]		# ポートの開放
			# print(app)								# DEBUG app引数確認用
			res = subprocess.run(app,stdout=subprocess.PIPE)  # サブプロセスとして起動
			if res.returncode != 0: 					# 終了コードを確認
				print(res.stdout.decode().strip())		# 結果を表示
				print('WARN: Failed to Disable Port',self.reset_port)
		if sys.meta_path is not None:
			print(datetime.datetime.today().strftime('%Y/%m/%d %H:%M:%S'), 'LCD Done')

def main():
	s = ''
	raspiLcd = RaspiLcd(ignoreError=True,x=16,reset=16) # raspiLcdの生成
	if len(sys.argv) >= 2 and sys.argv[1].isnumeric():
		s = [int(sys.argv[1])]
		if len(sys.argv) >= 3 and sys.argv[2].isnumeric():
			s.append(int(sys.argv[2]))
		raspiLcd.printBar(s)
		return
	for word in sys.argv[1:]:                       	# 引数を変数 word へ代入
		s += word + ' '
	if len(s) > 0:
		raspiLcd.print(s)
	else:
		print(raspiLcd.help())							# ヘルプメッセージを表示
		raspiLcd.print(raspiLcd.title)					# タイトル表示

if __name__ == "__main__":
	main()

'''
実行結果例

'''

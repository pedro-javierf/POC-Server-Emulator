import socket #Main python networking lib
import sys
import os
from random import randint
from time import sleep
from threading import Thread

global version
version=1

itemPrice = 499 #Not So expensive

def debug(text):
	print("[>]"+text)


def openChest(command):
	tempString = ""
	tempString = command.strip("OpenChest#")
	chestNum = int(tempString)
	moneyPerChest = [0]*chestNum
	for i in range(0,chestNum):#As much times as chests we have
		moneyPerChest[i] = randint(1,10)
	return moneyPerChest


def buyItem(command):
	tempString = ""
	tempString = command.strip("BuyItem#")
	clientMoney = int(tempString)
	if clientMoney>=itemPrice:
		return 1
	else:
		return 0




def Client(sc,addr):
	print("Connection From: "+str(addr[0]))
	OpenChest = "GetCoins#"
	tempStr = ""

	while True:
		try:
			data = sc.recv(32)#Receives 32 bytes
		except ConnectionResetError:
			print(str(addr[0])+" has disconnected.")
			break
		if data!="":
			command = data.decode('utf-8')
			if 'OpenChest' in command:
				coins = openChest(command)
				for value in coins:
					tempStr = OpenChest+str(value)
					print(str(addr[0])+" opened a chest with "+str(value)+" coins")
					sc.send(bytes(tempStr,'utf-8'))

			elif 'BuyItem' in command:
				x=buyItem(command)
				if x==1:
					print(str(addr[0])+" bought an item")
					sc.send(bytes("GetItem#1",'utf-8')) 
				elif x==0:
					print(str(addr[0])+" is poor and couldn't buy an item!")
					sc.send(bytes("BuyDennied#0",'utf-8'))
				else:
					pass



			else:
				debug("Unknown Command")
				sleep(1)
		else:
			pass
			


def main():
	print(" Server Emulator: v"+str(version))
	print()

	try:
		s = socket.socket()
		debug("Socket creado correctamente")
	except:
		debug("No se pudo crear el socket")
		sys.exit()
 
	try:
		s.bind(("localhost", 47799))
		debug("Servidor Bindeado Correctamente!")
	except:
		debug("No se pudo bindear el servidor")
		sys.exit()

 
	try:
		s.listen(1)
	except:
		debug("Error Al Escuchar conexiones")
		debug("El programa se cerrara")
		input()
		sys.exit()


	os.system("cls")
	print("[SERVER STARTED] ")
	print()
	print("[>]Listening...")

	while True:
		sc, addr = s.accept()#Sets a breakpoint, returns when a connection is done
		t = Thread(target=Client, args=(sc, addr))
		t.start()
		


if __name__ == '__main__':
    main()
else:
	main()
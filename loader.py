#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import json
from urtx.urtxserial import SerialUrtx
import sys
import getopt
import threading

configFilePath = 'conf.json'       # путь до файла конфигурации
speexFilePath = 'test.speex'        # путь до speex файла
speexFileHeadSize = 160     # размер заголовка спиксового файла в байтах
portPath = "COM4"   # порт и
baud = 9600                 # боды по умолчанию
textCodec = "utf-8"
speexCodec = "utf-8"

""" чтение опций с коммандной строки """
try:
    opts, _ = getopt.getopt(sys.argv[1:], "hc:s:p:b:t:", ["help", "configfile=", "speexfile=", "port=", "baudrate=", "textcodec="])
    for opt, arg in opts:
        if (opt == '-h') or (opt == "--help"):
            print("Используйте: \n", sys.argv[0] + """ -c <configfilepath> -s <speexfilepath> \n\t -p <port> -b <baudrate> \n\t"""
                                                   """ --configfile=<configfilepath> --speexfile=<speexfilepath> \n\t"""
                                                   """ --port=<port> --baudrate=<baudrate>""")
            sys.exit(0)
        elif (opt == '-c') or (opt == "--configfile"):
            configFilePath = arg
        elif (opt == '-s') or (opt == "--speexfile"):
            speexFilePath = arg
        elif (opt == '-p') or (opt == "--port"):
            portPath = arg
        elif (opt == '-b') or (opt == "--baudrate"):
            baud = arg
        elif (opt == '-t') or (opt == "--textcodec"):
            baud = arg
except getopt.GetoptError:
    print("Используйте: \n", sys.argv[0] + " --help")
    sys.exit(2)

if configFilePath is None:
    print("Не был указан конфигурационный файл, используйте: \n ",
          sys.argv[0] + " -c <configfilepath> -s <speexfilepath>")
    sys.exit(2)

if speexFilePath is None:
    print("Не был указан speex файл, используйте: \n", sys.argv[0] + " -c <configfilepath> -s <speexfilepath>")
    sys.exit(2)

""" Проверка наличия файлов """
data = {}
try:
    with open(configFilePath, "r", encoding='utf-8') as file:
        data.update(json.load(file))
except FileNotFoundError:
    print(configFilePath, ": такого файла не найдено")
    sys.exit(2)

try:
    with open(speexFilePath, 'r', encoding='utf-8') as file:
        file.seek(speexFileHeadSize)
        data.update({"speex": file.read()})
except FileNotFoundError:
    print(speexFilePath, ": такого файла не найдено")
    sys.exit(2)


ser = SerialUrtx()

ack = threading.Event()     # блокировщик основного потока
ackCode = None      # код ошибки, приходящий с платы


def loadPackage(desc, data):
    """ загружает данные, пока не придет подтверждение с платы """
    global ackCode
    while True:
        ser.sendPackage(desc, data)     # отправляем пакет и ждем
        ack.wait()  # ждем, пока не придет ответ
        ack.clear()     # снимаем флаг
        if ackCode == 0:    # если пришел 0 - сообщение доставлено
            break
        elif ackCode == 1:  # если пришла 1 - сообщение не доставлено
            continue


def recvFeedBack(data):
    """ обратная связь с платы  """
    global ackCode
    ackCode = data[0]
    ack.set()


def splitSpeex(s, tokensize=20):
    """ разбиваем строку на токены по 20 символов, лишнее выбрасываем """
    return list(map(''.join, zip(*[iter(s)]*tokensize)))


ser.subscribe(4, recvFeedBack)      # подключаемся к обработчику
ser.connect(portPath, baud)   # подключаемся к порту
ser.start()

if data.get("state"):
    print("state send...")
    loadPackage(1, (data["state"],))
    print("done!")

if data.get("texts"):
    print("texts send...")
    for idx, text in enumerate(data["texts"]):
        btext = bytearray(text, textCodec)
        if len(btext) > 20:
            raise ValueError("Длина строки: " + text + "слишком большая")
        else:
            btext = btext + b'\n'
            btext = btext.ljust(21, b'\x00')
            loadPackage(2, bytes([idx]) + btext)
    print("done!")

data["speex"] = splitSpeex(data["speex"], tokensize=20)

print("speex send...")
for token in data["speex"]:
    loadPackage(3, bytes(token, encoding=speexCodec))
print("done!")

print("waiting disconnection...")
loadPackage(4, (0, ))   # сообщение об окончании связи
print("done!")
time.sleep(1)
ser.exit()

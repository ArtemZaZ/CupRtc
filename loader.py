#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time
import json
from urtx.urtxserial import SerialUrtx
import sys
import getopt

configFilePath = None
speexFilePath = None

try:
    opts, _ = getopt.getopt(sys.argv[1:], "hc:s:", ["help", "configfile=", "speexfile="])
    for opt, arg in opts:
        if (opt == '-h') or (opt == "--help"):
            print("Используйте: \n", sys.argv[0] + """ -c <configfilepath> -s <speexfilepath>"""
                                                """ --configfile=<configfilepath> --speexfile=<speexfilepath>""")
            sys.exit(0)
        if (opt == '-c') or (opt == "--configfile"):
            configFilePath = arg
        if (opt == '-s') or (opt == "--speexfile"):
            speexFilePath = arg
except getopt.GetoptError:
    print("Используйте: \n", sys.argv[0] + " --help")
    sys.exit(2)

if configFilePath is None:
    print("Не был указан конфигурационный файл, используйте: \n ", sys.argv[0] + " -c <configfilepath> -s <speexfilepath>")
    sys.exit(2)

if speexFilePath is None:
    print("Не был указан speex файл, используйте: \n", sys.argv[0] + " -c <configfilepath> -s <speexfilepath>")
    sys.exit(2)


data = {}
try:
    with open(configFilePath, "r") as file:
        data.update(json.load(file))
except FileNotFoundError:
    print(configFilePath, ": такого файла не найдено")
    sys.exit(2)

try:
    with open(speexFilePath, 'r') as file:
        data.update({"speex": file.read()})
except FileNotFoundError:
    print(speexFilePath, ": такого файла не найдено")
    sys.exit(2)

ser = SerialUrtx()


def recvBlock(data):
    print("block", data)


def recvError(data):
    print("error", data)


ser.subscribe(3, recvBlock)
ser.subscribe(5, recvError)
ser.connect("/dev/ttyUSB1", 9600)
ser.start()

if data.get("state"):
    ser.sendPackage(1, (data["state"],))

if data.get("texts"):
    for idx, text in enumerate(data["texts"]):
        btext = bytearray(text, "utf-8")
        if len(btext) > 20:
            raise ValueError("длина строки не должна превышать 20 символов латиницы или 10 символов кириллицы")
        else:
            btext = btext + b'\n'
            btext = btext.ljust(21, b'\x00')
            ser.sendPackage(2, bytes([idx]) + btext)

# ser.sendPackage(0, (1,))
# ser.sendPackage(1, (2,))
# ser.sendPackage(2, bytes([21]) + b"123456789!123456789!")
# ser.sendPackage(3, (333,))
# ser.sendPackage(4, b"90909090909090909090")
# ser.sendPackage(5, (3,))

time.sleep(3)

import time
import json
from urtx.urtxserial import SerialUrtx

data = {}
with open("conf.json", "r") as file:
    data = json.load(file)

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
    ser.sendPackage(1, (data["state"], ))

if data.get("texts"):
    for idx, text in enumerate(data["texts"]):
        btext = bytearray(text, "utf-8")
        if len(btext) > 20:
            raise ValueError("длина строки не должна превышать 20 символов латиницы или 10 символов кириллицы")
        else:
            btext = btext + b'\n'
            btext = btext.ljust(21, b'\x00')
            print(btext)
            ser.sendPackage(2, bytes([idx]) + btext)



#ser.sendPackage(0, (1,))
#ser.sendPackage(1, (2,))
#ser.sendPackage(2, bytes([21]) + b"123456789!123456789!")
#ser.sendPackage(3, (333,))
#ser.sendPackage(4, b"90909090909090909090")
#ser.sendPackage(5, (3,))

time.sleep(3)



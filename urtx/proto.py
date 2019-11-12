import struct
import crc16

protocolConfig = {
    "startBytes": b'\xaa\xaa',
    "checksum": "crc16Xmodem",
    "formatDesc": 'B',  # дескриптор пакета, определяет, как будет читаться пакет
    "formatChecksum": 'H'  # 2 байта
}

protocolFormatDescription = {  # описание форматов пакетов по дескриптору
    1: '=B',  # пакет данных c местом участника
    2: '=B21B',   # пакет данных с номером текста и 20 символами текста (юзайте в конце нуль-терминальную строку)
    3: '=50B',   # 50 байт speex файла
    4: '=B',    # обратная связь
}

_headFormat = '=' + protocolConfig["formatChecksum"] + protocolConfig["formatDesc"]
_headSize = struct.calcsize(_headFormat)


def readPackage(readArrFun, crc=False):
    """ Чтение пакета по протоколу """
    temp = bytearray(len(protocolConfig["startBytes"]))
    while temp != protocolConfig["startBytes"]:  # ищем вхождение
        temp.pop(0)
        temp += readArrFun(1)
    head = struct.unpack(_headFormat, readArrFun(_headSize))  # читаем заголовок

    fmt = protocolFormatDescription[head[1]]  # получаем формат пакета
    raw = readArrFun(struct.calcsize(fmt))
    data = struct.unpack(fmt, raw)  # читаем данные
    if check(head[0], raw, crc):  # проверяем данные
        return head, data
    else:
        return None


def sendPackage(sendArrFun, desc, data, crc=False):
    """ Отправка пакета по протоколу """
    pack = struct.pack(protocolFormatDescription[desc], *data)
    ch = checksum(pack, crc)
    head = protocolConfig["startBytes"] + struct.pack(_headFormat, ch, desc)
    sendArrFun(head + pack)


def check(checksum, data, crc=False):
    if not crc:
        return True
    else:
        return checksum == crc16.crc16xmodem(data)


def checksum(data, crc=False):
    if not crc:
        return 0
    else:
        return crc16.crc16xmodem(data)


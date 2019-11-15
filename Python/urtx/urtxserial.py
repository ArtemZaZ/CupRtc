import serial
import threading
import time
from urtx import proto
from urtx import eventmaster


class SerialUrtx(threading.Thread):
    """ Класс, использующий сериал порт и протокол urtx """
    def __init__(self):
        threading.Thread.__init__(self, daemon=True)
        self.__exit = False  # метка выхода
        self._port = None   # serial port
        self._eventDict = {}    # создаем словарь событий
        self._eventMaster = eventmaster.EventMaster()   # создаем мастера событий

        for desc in proto.protocolFormatDescription.keys():
            self._eventDict.update({desc: eventmaster.Event(desc)})  # добавляем события приема сообщений с
            # определенным дескриптором
            self._eventMaster.append(self._eventDict[desc])
        self._eventMaster.start()

    def connect(self, *args, **kwargs):
        """ метод подключения к сериал портк """
        if self._port is not None:
            raise IOError("Предыдущий порт не был закрыт")
        else:
            self._port = serial.Serial(*args, **kwargs)

    def disconnect(self):
        """ метод закрытия порта """
        self._port.close()
        del self._port
        self._port = None

    def exit(self):
        """ метод выхода из потока """
        self.__exit = True
        self.disconnect()

    def _readArray(self, size):
        """ метод чтения массива данных """
        return self._port.read(size)

    def _sendArray(self, data):
        """ метод отправки массива данных """
        self._port.write(data)

    def _readPackage(self):
        """ Читаем пакет и вызываем событие для определенного дескриптора """
        package = proto.readPackage(self._readArray, crc=True)
        desc = package[0][1]
        data = package[1]
        self._eventDict[desc].push(data)    # вызываем событие о приеме пакета по определенному дескриптору
        return package

    def sendPackage(self, desc, data):
        """ отправляем пакет форматом, описываемым дескриптором """
        proto.sendPackage(self._sendArray, desc, data, crc=True)

    def run(self):
        while not self.__exit:
            try:
                self._readPackage()
            except Exception as e:
                print(str(e))

    def subscribe(self, event, handler):
        ev = self._eventDict[event]
        if not ev:
            raise eventmaster.EventError(str(event) + ": такого события нет")
        ev.connect(handler)



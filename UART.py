import serial
import gi
import time
import crc16
import threading
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GLib, GObject

#176
def read_speex_fromfile(filename):                   
    file=open(filename, "rb")
    file.seek(160, 0)
    filestr=''
    outstr=''
    n=0
    i=0
    
    tempc=file.read(1)
    while (len(tempc)!=0):                                          # чтение speex файла за исключением первых 176 байт, перевод их в строку( строка представляет числа в шестнадцатиричной системе )
        filestr=filestr+hex(ord(tempc))
        tempc=file.read(1)
        n=n+1
    outstr=filestr.upper()                                          # перевод всех символов в строке в верхний регистр
    outlist=list(outstr.split('0X'))                                # перевод строки в список, с помощью удаления разделителя 0X(подробнее читать про ф-ию split)
    outlist.append('EOSP')                                          # добавление в конец списка значение EOSP, означающее конец списка
    while(outlist[i]!='EOSP'):                                     
        if(len(outlist[i])==0):                                     # если элемент в списке пустой, удалить его
            del(outlist[i])   
        if(len(outlist[i])==1):                                     # если элемент в списке имеет только 1 символ, добавить ноль перед ним 
            outlist[i]='0'+outlist[i][0]         
        i=i+1 

    file.close()
    print(len(outlist))
    return outlist


def convert_hesh(data):                                                 # рассчет хеш-суммы для data и перевод ее в строку
    temp=(hex(crc16.crc16xmodem(bytes(data, "utf-8")))[2:]).upper()
    while(len(temp)<4):
        temp='0'+temp                                                   # добавление перед хеш-суммой необходимое кол-во нулей
    return temp


def convert_blocknumber(blocknumber):                           # перевод в строку количество блоков, представленное целым числом
    temp=(hex(blocknumber)[2:]).upper()
    while(len(temp)<4):
        temp='0'+temp
    return temp


def pack_state(state):
    while(len(state)<2):                                        # Добавляем нулей перед местом, если там 1 цифра
        state='0'+state
    out="<STATE" + ' ' + state + ' ' + convert_hesh(state) + '>'
    return bytes(out, "utf-8")

def pack_text(text, numberOfText):                                      # символы & взяты для удобства распарсивания на Си
    return bytes( "<TEXT" + numberOfText + ' ' + '&' + text + '&' + ' ' + convert_hesh(text) + '>' , "utf-8")
    
def convert_datalist(datalist):                                 # перевод списка данных, полученного из файла в список комманд, готовых к отправке(<SPEEX block field hesh>)
    i=0
    j=0
    blocknumber=0
    command='<SPEEX'
    tempstr=command+' '+ convert_blocknumber(blocknumber) +' '
    templist=[]
    blocknumber=blocknumber+1
    while(datalist[i]!='EOSP'):
        if(j<20):
            tempstr=tempstr+datalist[i]
            j=j+1
            i=i+1
        else:
            #print(hex(crc16.crc16xmodem(bytes(tempstr[10:], "utf-8"))))                 
            #print(tempstr[7:53])
            tempstr=tempstr + ' ' + convert_hesh(tempstr[7:11]+tempstr[12:]) + '>'          # хеш-сумма считается, как номер блока + поле данных(без пробела между ними) 
            templist.append(tempstr)
            tempstr=command+ ' ' + convert_blocknumber(blocknumber) + ' '
            blocknumber=blocknumber+1
            j=0
               
        
    if(j!=0):
        while(len(tempstr[12:])<40):
              tempstr=tempstr+'0'
        templist.append(tempstr+' '+ convert_hesh(tempstr[7:11]+tempstr[12:]) + '>')
    templist.append(b'EOSP')
    
    templist2=['<BLOCK' + ' ' + convert_blocknumber(blocknumber-1) + ' ' +  convert_hesh(convert_blocknumber(blocknumber-1)) + '>']  # добавление в начало списка комманды BLOCK, содержащей в себе количество блоков
    #print(str(convert_blocknumber(blocknumber-1)))
    templist2.extend(templist)

    i=0
    while(templist2[i]!=b'EOSP'):
        templist2[i]=bytes(templist2[i],"utf-8")
        i=i+1
    
    return templist2






class SPEEX_UART:                                                       # класс для графического приложения 
    def __init__(self):
        self.port = serial.Serial()      
        self.Exit = False               # Выход из потока
        self.speexdown = ''             # Переменные для отправки speex файла
        self.lenSpeexDown = 1           # 
        self.state = None               # Переменные из ГП
        self.text1 = None
        self.text2 = None
        self.text3 = None
        self.text4 = None
        self.text5 = None
        self.text6 = None
        self.text7 = None
        self.text8 = None
        self.text9 = None
        self.text10 = None
        self.text11 = None
        self.text12 = None
        self.text13 = None
        self.text14 = None
        self.text15 = None
        self.text16 = None        
        self.Done = True                # Флаг успешной отправки
        self.Connected = False          # Флаг успешного подключения
        self.q = 0                      # Cчетчик
        self.out = False                # Флаг отмены
        
        self.builder = Gtk.Builder()
        self.builder.add_from_file("interface.glade")                   # билдим приложение, созданное в Glade 
        self.window = self.builder.get_object("window1")                # и вытаскиваем из него все интересующие элементы
        self.combobox = self.builder.get_object("comboboxtext3")
        self.downloadButton = self.builder.get_object("button2")
        self.connectButton = self.builder.get_object("button1")
        self.canselButton = self.builder.get_object("button3")
        self.portEntry = self.builder.get_object("portEntry")
        self.fileChooserButton = self.builder.get_object("filechooserbutton")
        self.progressbar = self.builder.get_object("progressbar1")
        self.textview = self.builder.get_object("textview1")
        self.textbuffer = self.textview.get_buffer()
        
        self.text1Entry = self.builder.get_object("entry1")
        self.text2Entry = self.builder.get_object("entry2")
        self.text3Entry = self.builder.get_object("entry3")
        self.text4Entry = self.builder.get_object("entry4")
        self.text5Entry = self.builder.get_object("entry5")
        self.text6Entry = self.builder.get_object("entry6")
        self.text7Entry = self.builder.get_object("entry7")
        self.text8Entry = self.builder.get_object("entry8")
        self.text9Entry = self.builder.get_object("entry9")
        self.text10Entry = self.builder.get_object("entry10")
        self.text11Entry = self.builder.get_object("entry11")
        self.text12Entry = self.builder.get_object("entry12")
        self.text13Entry = self.builder.get_object("entry13")
        self.text14Entry = self.builder.get_object("entry14")
        self.text15Entry = self.builder.get_object("entry15")
        self.text16Entry = self.builder.get_object("entry16")
        
        self.checkStateButton = self.builder.get_object("checkStateButton")
        self.checkText1Button = self.builder.get_object("checkbutton1")
        self.checkText2Button = self.builder.get_object("checkbutton2")
        self.checkText3Button = self.builder.get_object("checkbutton3")
        self.checkText4Button = self.builder.get_object("checkbutton4")
        self.checkText5Button = self.builder.get_object("checkbutton5")
        self.checkText6Button = self.builder.get_object("checkbutton6")
        self.checkText7Button = self.builder.get_object("checkbutton7")
        self.checkText8Button = self.builder.get_object("checkbutton8")
        self.checkText9Button = self.builder.get_object("checkbutton9")
        self.checkText10Button = self.builder.get_object("checkbutton10")
        self.checkText11Button = self.builder.get_object("checkbutton11")
        self.checkText12Button = self.builder.get_object("checkbutton12")
        self.checkText13Button = self.builder.get_object("checkbutton13")
        self.checkText14Button = self.builder.get_object("checkbutton14")
        self.checkText15Button = self.builder.get_object("checkbutton15")
        self.checkText16Button = self.builder.get_object("checkbutton16")
        self.checkFileButton = self.builder.get_object("checkFileButton")        
        
        self.checkStateButton.connect("clicked", self.checkState)
        self.checkText1Button.connect("clicked", self.checkText1)
        self.checkText2Button.connect("clicked", self.checkText2)
        self.checkText3Button.connect("clicked", self.checkText3)
        self.checkText4Button.connect("clicked", self.checkText4)
        self.checkText5Button.connect("clicked", self.checkText5)
        self.checkText6Button.connect("clicked", self.checkText6)
        self.checkText7Button.connect("clicked", self.checkText7)
        self.checkText8Button.connect("clicked", self.checkText8)
        self.checkText9Button.connect("clicked", self.checkText9)
        self.checkText10Button.connect("clicked", self.checkText10)
        self.checkText11Button.connect("clicked", self.checkText11)
        self.checkText12Button.connect("clicked", self.checkText12)
        self.checkText13Button.connect("clicked", self.checkText13)
        self.checkText14Button.connect("clicked", self.checkText14)
        self.checkText15Button.connect("clicked", self.checkText15)
        self.checkText16Button.connect("clicked", self.checkText16)
        self.checkFileButton.connect("clicked", self.checkFile)
        
        self.connectButton.connect("clicked", self.Connect)             # при нажанию на кнопку connectButton запускается ф-ия Connect
        self.downloadButton.connect("clicked", self.Download)
        self.canselButton.connect("clicked", self.Cansel)
        
        self.timeout_id = GObject.timeout_add(100, self.on_timeout, None) # дергаем раз в 50 мс ф-ию on_timeout
        self.window.show_all()


    def delete_event(self, widget, event, data=None):                   # ф-ия закрытия окна
        self.Exit=True
        Gtk.main_quit()
        self.port.close()

    def on_timeout(self, user_data):
        new_value = self.q/self.lenSpeexDown
        if new_value > 1:
            new_value = 0
        self.progressbar.set_fraction(new_value)
        return True

    def setSensetiveToCheck(self, b):
        self.checkStateButton.set_property("sensitive", b)              # включаем все флаги проверки загружаемых объектов
        time.sleep(0.01)
        self.checkText1Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText2Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText3Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText4Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText5Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText6Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText7Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText8Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText9Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText10Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText11Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText12Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText13Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText14Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText15Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkText16Button.set_property("sensitive", b)
        time.sleep(0.01)
        self.checkFileButton.set_property("sensitive", b) 


    ############ Трешачок ###########################################
    def checkState(self, w):                                            # функция, запускающаяся при активации флага напротив Места в ГП
        self.combobox.set_property("sensitive", self.checkStateButton.get_active())

    def checkText1(self, w):
        self.text1Entry.set_property("sensitive", self.checkText1Button.get_active())

    def checkText2(self, w):
        self.text2Entry.set_property("sensitive", self.checkText2Button.get_active())

    def checkText3(self, w):
        self.text3Entry.set_property("sensitive", self.checkText3Button.get_active())

    def checkText4(self, w):
        self.text4Entry.set_property("sensitive", self.checkText4Button.get_active())

    def checkText5(self, w):
        self.text5Entry.set_property("sensitive", self.checkText5Button.get_active())

    def checkText6(self, w):
        self.text6Entry.set_property("sensitive", self.checkText6Button.get_active())

    def checkText7(self, w):
        self.text7Entry.set_property("sensitive", self.checkText7Button.get_active())

    def checkText8(self, w):
        self.text8Entry.set_property("sensitive", self.checkText8Button.get_active())

    def checkText9(self, w):
        self.text9Entry.set_property("sensitive", self.checkText9Button.get_active())

    def checkText10(self, w):
        self.text10Entry.set_property("sensitive", self.checkText10Button.get_active())

    def checkText11(self, w):
        self.text11Entry.set_property("sensitive", self.checkText11Button.get_active())

    def checkText12(self, w):
        self.text12Entry.set_property("sensitive", self.checkText12Button.get_active())

    def checkText13(self, w):
        self.text13Entry.set_property("sensitive", self.checkText13Button.get_active())

    def checkText14(self, w):
        self.text14Entry.set_property("sensitive", self.checkText14Button.get_active())

    def checkText15(self, w):
        self.text15Entry.set_property("sensitive", self.checkText15Button.get_active())

    def checkText16(self, w):
        self.text16Entry.set_property("sensitive", self.checkText16Button.get_active())            
            
    def checkFile(self, w):                                             # функция, запускающаяся при активации флага напротив выбора файла
        self.fileChooserButton.set_property("sensitive", self.checkFileButton.get_active())          

    def Connect(self, w):                                               # ф-ия для кнопки connect.  открывает порт по адресу, записанному в portEntry
        if(not self.Connected):                                         ##### Добавить исключене на открытие порта
            portname = self.portEntry.get_text()
            baudrate0=115200
            try:
                self.port=serial.Serial(portname, baudrate=baudrate0, timeout=1)
                self.printLog("Connect to " + portname + ";  baudrate:  " + str(baudrate0) + "\n")
                self.connectButton.set_label("Disconnect")
                self.Connected=True
            except serial.serialutil.SerialException:
                self.printLog("Could not open port" + portname + "\n");
        else:
            self.port.close()
            self.connectButton.set_label("Connect")
            self.printLog("Disconnect\n")
            self.Connected=False

    def Cansel(self, w):
        self.out=True
        time.sleep(1)
        self.Done=True        
        self.printLog("Cancel.\n")
        self.setSensetiveToCheck(True) 

    def printLog(self, string):                                                   # Печатает логи в текстбокс
        end_iter = self.textbuffer.get_end_iter()                           # получение итератора конца строки
        self.textbuffer.insert(end_iter, string)
        
    def Download(self, w):                                                      # ф-ия для кнопки downloadButton. Отправка данных по uart
        try:
            if(self.Connected):
                #self.downloadButton.set_sensitive(False)                               # из-за того, что кнопка становится неактивной приложение отрубается без каких-либо логов, пришлось костылить
                if(self.Done):
                    self.setSensetiveToCheck(False)
                    self.out=False
                    self.Done=False

                    if(self.checkFileButton.get_active()):
                        speexfilename = self.fileChooserButton.get_uri()                      # а также производит чтение из файлов и перевод SPEEX файла в список готовый к отправке
                        self.printLog("Файл: " + str(speexfilename) + "\n")              # запись строки в конец файла
                        if(speexfilename!=None):                                            ##### Добавить исключение на отсутствие файла
                            if(speexfilename[:6] == "file:/"):                                # из пути к файлу обрезается часть file:/
                                self.speex_list = read_speex_fromfile(speexfilename[6:])
                                self.speexdown = convert_datalist(self.speex_list)
                                self.lenSpeexDown = len(self.speexdown)

                    if(self.checkStateButton.get_active()):     ####################################### ТУТЬ №№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
                        self.state=self.combobox.get_active_text()                          # получение места из комбобокса
                        self.printLog("Место: " + self.state + "\n")                        # запись строки в конец файла
                        self.state=pack_state(self.state)                                   # упаковка места для отправки

                    ##################### Трешачок №2, упаковка текстов #############################################
                    if(self.checkText1Button.get_active()):                
                        self.text1=self.text1Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №1: " + self.text1 + "\n")                     
                        self.text1 = pack_text(self.text1, "1")                                                    

                    if(self.checkText2Button.get_active()):                  
                        self.text2 = self.text2Entry.get_text()
                        self.printLog("Текст №2: " + self.text2 + "\n")     
                        self.text2 = pack_text(self.text2, "2")

                    if(self.checkText3Button.get_active()):                  
                        self.text3 = self.text3Entry.get_text()
                        self.printLog("Текст №3: " + self.text3 + "\n")     
                        self.text3 = pack_text(self.text3, "3")

                    if(self.checkText4Button.get_active()):                
                        self.text4=self.text4Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №4: " + self.text4 + "\n")                     
                        self.text4 = pack_text(self.text4, "4")

                    if(self.checkText5Button.get_active()):                
                        self.text5=self.text5Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №5: " + self.text5 + "\n")                     
                        self.text5 = pack_text(self.text5, "5")

                    if(self.checkText6Button.get_active()):                
                        self.text6=self.text6Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №6: " + self.text6 + "\n")                     
                        self.text6 = pack_text(self.text6, "6")

                    if(self.checkText7Button.get_active()):                
                        self.text7=self.text7Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №7: " + self.text7 + "\n")                     
                        self.text7 = pack_text(self.text7, "7")

                    if(self.checkText8Button.get_active()):                
                        self.text8=self.text8Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №8: " + self.text8 + "\n")                     
                        self.text8 = pack_text(self.text8, "8")

                    if(self.checkText9Button.get_active()):                
                        self.text9=self.text9Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №9: " + self.text9 + "\n")                     
                        self.text9 = pack_text(self.text9, "9")

                    if(self.checkText10Button.get_active()):                
                        self.text10=self.text10Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №10: " + self.text10 + "\n")                     
                        self.text10 = pack_text(self.text10, "10")

                    if(self.checkText11Button.get_active()):                
                        self.text11=self.text11Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №11: " + self.text11 + "\n")                     
                        self.text11 = pack_text(self.text11, "11")

                    if(self.checkText12Button.get_active()):                
                        self.text12=self.text12Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №12: " + self.text12 + "\n")                     
                        self.text12 = pack_text(self.text12, "12")

                    if(self.checkText13Button.get_active()):                
                        self.text13=self.text13Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №13: " + self.text13 + "\n")                     
                        self.text13 = pack_text(self.text13, "13")

                    if(self.checkText14Button.get_active()):                
                        self.text14=self.text14Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №14: " + self.text14 + "\n")                     
                        self.text14 = pack_text(self.text14, "14")

                    if(self.checkText15Button.get_active()):                
                        self.text15=self.text15Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №15: " + self.text15 + "\n")                     
                        self.text15 = pack_text(self.text15, "15")

                    if(self.checkText16Button.get_active()):                
                        self.text16=self.text16Entry.get_text()                                # Получение первого текста из Entry
                        self.printLog("Текст №16: " + self.text16 + "\n")                     
                        self.text16 = pack_text(self.text16, "16")

                    ##############################################################
                                            
                    self.printLog("Download...\n")
                    
                    thr = threading.Thread(target=self.Write)                           # ф-ия отправки данных в отдельном потоке    
                    thr.start()
                else:                    
                    self.printLog("Wait...\n")
            else:
                self.printLog("Port not open!\n")

        except TypeError:
            self.Done=True
            self.printLog("TypeError. Check the fields.\n")
            self.setSensetiveToCheck(True)
            
        
    def Write(self):                                                        ##### Сюда добавить кучу исключений, а также отмену отправки
        tempstr=b''
        while(tempstr!=b's'):                                               # пока не пришло g отправка строки   <*For reading say g>
            #self.port.write(b'\n<*For reading say g>\n')                    
            tempstr=self.port.read()
            if(self.out):
                return
        
        if(self.checkStateButton.get_active()):
            self.port.write(self.state)                                         # отправка места            
            tempstr=self.port.read()        
            while(tempstr!=b'g'):                                               # пока не пришло g отправка того же элемента
                if(tempstr==b'f'):                                              # если пришел символ f, отправить снова 
                    self.port.write(self.state)
                tempstr=self.port.read()
                if(self.out):
                    return

        ######################## Трешачок №3, Отправка текстов ########################################
        if(self.checkText1Button.get_active()):
            self.port.write(self.text1)                                      # отправка первого текста
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               # пока не пришло g отправка того же элемента
                if(tempstr==b'f'):                                              # если пришел символ f, отправить снова 
                    self.port.write(self.text1)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText2Button.get_active()):
            self.port.write(self.text2)                                      # отправка второго текста
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               # пока не пришло g отправка того же элемента
                if(tempstr==b'f'):                                              # если пришел символ f, отправить снова 
                    self.port.write(self.text2)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText3Button.get_active()):
            self.port.write(self.text3)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text3)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText4Button.get_active()):
            self.port.write(self.text4)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text4)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText5Button.get_active()):
            self.port.write(self.text5)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text5)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText6Button.get_active()):
            self.port.write(self.text6)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text6)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText7Button.get_active()):
            self.port.write(self.text7)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text7)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText8Button.get_active()):
            self.port.write(self.text8)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text8)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText9Button.get_active()):
            self.port.write(self.text9)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text9)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText10Button.get_active()):
            self.port.write(self.text10)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text10)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText11Button.get_active()):
            self.port.write(self.text11)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text11)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText12Button.get_active()):
            self.port.write(self.text12)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text12)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText13Button.get_active()):
            self.port.write(self.text13)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text13)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText14Button.get_active()):
            self.port.write(self.text14)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text14)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText15Button.get_active()):
            self.port.write(self.text15)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text15)
                tempstr=self.port.read()
                if(self.out):
                    return

        if(self.checkText16Button.get_active()):
            self.port.write(self.text16)                                     
            tempstr=self.port.read()            
            while(tempstr!=b'g'):                                               
                if(tempstr==b'f'):                                              
                    self.port.write(self.text16)
                tempstr=self.port.read()
                if(self.out):
                    return

        ##############################################################
        
        self.q=0
        if(self.checkFileButton.get_active()):
            while(self.speexdown[self.q]!=b'EOSP'):                             # пока не конец списка 
                self.port.write(self.speexdown[self.q])                         # последовательная отравка каждого элемента speex файла, если ему приходит g, если ему приходит другой символ, происходит переотправка элемента
                tempstr=self.port.read()
                while(tempstr!=b'g'):
                    print(tempstr)
                    if(tempstr==b'f'):
                        self.port.write(self.speexdown[self.q])
                    tempstr=self.port.read()
                    if(self.out):
                        return
                self.q=self.q+1
                print(self.q)
        #self.downloadButton.set_sensitive(True)
        self.port.write(bytes("<0>", "utf-8"))
        self.printLog("Done!\n")
        self.Done=True
        self.q=0
        self.setSensetiveToCheck(True)

        
def th():                               # в отдельном потоке запускаем GTK
    Gtk.main()

t = threading.Thread(target=th)


SPEEX_UART()

t.start()



import ctypes
from ctypes import wintypes


# калибровочные коэффициенты
class QMX_CC_F(ctypes.Structure):
    _fields_ = [
        ("Offset", ctypes.c_float),
        ("Scale", ctypes.c_float),
        ("Units", ctypes.c_char * 6),
        ("UnitsCode",  wintypes.BYTE),
    ]


class QMX_CC_D(ctypes.Structure):
    _fields_ = [
        ("Offset", ctypes.c_double),
        ("Scale", ctypes.c_double),
        ("Units", ctypes.c_char * 6),
        ("UnitsCode", wintypes.BYTE),
    ]


# кольцевой буфер
class QMX_CIRC_BUF_CONFIG(ctypes.Structure):
    _fields_ = [
        ("buf_start", ctypes.c_ushort),  # Указатель на начало буфера
        ("buf_ptr", ctypes.c_ulong),  # Указатель записи в буфер [0..buf_size-1]
        ("buf_cntr", ctypes.c_ulong),  # Счетчик записи в буфер [0..2^32]
        ("buf_size", ctypes.c_ulong),  # Размер буфера в словах (WORD)
        ("event", wintypes.HANDLE),
    ]


# структура конфигурации крейта
class QMX_CONFIG(ctypes.Structure):
    _fields_ = [
        # Поля доступные после QMX_Open()
        ("UsbSpeed", wintypes.BYTE),  # Текущая скорость USB
        ("CtlName", wintypes.CHAR * 11),  # Название контроллера
        ("CtlBIOSversion", wintypes.CHAR * 6),  # Версия БИОСа контроллера
        ("CtlSerialNumber", wintypes.CHAR * 10),  # Серийный номер контроллера
        ("ModQuantity", wintypes.BYTE),  # Количество модулей в системе
        ("ModName", (wintypes.CHAR * 8) * 10),  # Имена модулей
        ("ModType", wintypes.WORD * 8),  # Типы модулей
        ("ModRevision", wintypes.BYTE * 8),  # Ревизия модуля
        ("ModSerialNumber", (wintypes.CHAR * 8) * 10),  # Серийный номер модуля
        ("DevHandle", wintypes.HANDLE),  # Хэндл драйвера
        ("DevHandle", wintypes.HANDLE),  # Хэндл драйвера
         # Поля доступные после QMX_Prepare()
         ("RespTime", wintypes.WORD),  # Рассчетное время реакции в мс
         ("ReadFileSize", wintypes.DWORD),  # Размер ридфайла в БАЙТАХ
         ("WriteFileSize", wintypes.DWORD),  # Размер райтфайла в БАЙТАХ
         ("ModLocalBufferSize", wintypes.DWORD),  # Размер половины локального буфера-1 (в словах)
         ("InputRate", ctypes.c_double),  # Скорость ввода данных (слов) в кГц
         ("OutputRate", ctypes.c_double),  # Скорость вывода данных (слов) в кГц
         ("CBInput", QMX_CIRC_BUF_CONFIG),  # Конфигурация кольцевого буфера ввода
         ("CBOutput", QMX_CIRC_BUF_CONFIG),  # Конфигурация кольцевого буфера вывода
    ]

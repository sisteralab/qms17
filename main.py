import subprocess
from time import sleep
import ctypes
from ctypes import cdll, wintypes

from constants import *
from structures import QMX_CC_F, QMX_CONFIG

lib = cdll.LoadLibrary("./qmx.dll")

SD: wintypes.HANDLE = wintypes.HANDLE(0)  # Дескриптор системы
Gain: wintypes.WORD
QMS15Table: wintypes.WORD * 16
Data1: wintypes.WORD
Data2: wintypes.WORD
Channel: wintypes.WORD
CC_Adc: QMX_CC_F = QMX_CC_F()
CFG: QMX_CONFIG = QMX_CONFIG()  # Конфигурация системы

DataPtr: ctypes.c_uint
DataPtrOld: ctypes.c_uint
CurrentPtr: ctypes.c_uint
LengthDataBuf: ctypes.c_uint
Data: ctypes.POINTER(ctypes.c_ushort)
# QMS_data: (ctypes.c_ulonglong * 8) * 32
QMS_data = [[0 for _ in range(8)] for _ in range(32)]
# QMS_iter: (ctypes.c_ulonglong * 8) * 32
QMS_iter = [[0 for _ in range(8)] for _ in range(32)]
time: ctypes.c_ulonglong
temp: ctypes.c_ulonglong

QMX_QMS17_GAIN_1 = 0
QMX_QMS17_GAIN_2 = 1


def main():
	global SD
	global CFG
	global DataPtr
	global DataPtrOld
	global Data
	s_wrk: ctypes.c_char_p * 64
	mod: ctypes.c_int
	ch: ctypes.c_int

	print("Please wait....\n")
	
	# Запустим сервер
	subprocess.Popen(["qmbox_ds.exe", "-p3"])
	sleep(0.5)

	# Получим дискриптор системы
	SD = lib.QMX_CircBufAttach(wintypes.WORD(0))
	if not SD:
		print("ERROR: No QMBox found.")
		_quit()

	# Проинициализуем систему
	f = lib.QMX_InitModules(wintypes.HANDLE(SD), QMX_LOG_NONE, None)
	CheckErr()


	# Фиктивный вызов QMX_Prepare - выясним конфигурацию устройства т.е. из каких модулей оно состоит
	lib.QMX_MOD_SetActive(SD, 0, QMX_ON)
	lib.QMX_Prepare(SD, 100, ctypes.byref(CFG))
	CheckErr()
	lib.QMX_MOD_SetActive(SD, 0, QMX_OFF)
	
	# Убедимся, что в нулевом слоте устройства - модуль QMS17
	s_wrk = CFG.ModName[0]
	s_wrk[5] = 0
	if "QMS17" in s_wrk.value.decode():
		print("Module name OK\n")
	else:
		print("Module Name error\n")
		_quit()


	# Зададим усиление
	Gain = QMX_QMS17_GAIN_1

	# Считаем калибр. коэффициенты для нужного усиления
	lib.QMX_QMS17_ReadCC(SD, 0, Gain, ctypes.byref(CC_Adc))

	# Запишем таблицу опроса, состоящую из 4х каналов - 0, 1, 2 и 3
	lib.QMX_QMS17_SetTable(SD, 0, 0xF, Gain)


	# Установим частоту работы АЦП 1 MГц (частота ввода 16-битных слов - 2 МГц)
	lib.QMX_QMS17_SetInputRate(SD, 0, ctypes.c_double(2000000.0), None)
	CheckErr()

	# Подготовимся к сбору данных
	lib.QMX_MOD_SetActive(SD, 0, QMX_ON)
	lib.QMX_Prepare(SD, 100, ctypes.byref(CFG))
	CheckErr()
	
	LengthDataBuf = CFG.CBInput.buf_size  # размер кольцевого буфера данных, заполняемого сервером
	Data = CFG.CBInput.buf_start			# указатель на начало кольцевого буфера данных,
																# заполняемого сервером
	DataPtrOld = 0
	time = 0

	# Запустим сбор данных
	lib.QMX_Start(SD)
	CheckErr()
	try:
		while True:

			DataPtr = CFG.CBInput.buf_ptr  # получим значение указателя на последний полученный элемент данных
			# for CurrentPtr in range(DataPtrOld, DataPtr, 2):
			CurrentPtr = DataPtrOld
			while CurrentPtr == DataPtr:  # цикл считывания всех полученных элементов данных
				CurrentPtr += 2
				# Проверки, необходимые чтобы текущий указатель не перескочил через границу буфера
				if CurrentPtr >= int(CFG.CBInput.buf_size[0].value):
					CurrentPtr = 0
				if not CurrentPtr and not DataPtr:
					break

				mod = (Data[CurrentPtr] >> 5) & 0x07  # в четном (младшем) 16-ти битном слове элемента данных содержится
				ch = Data[CurrentPtr] & 0x1f  # информация о номерах канала (младщие 5-ть бит) и модуля (сл. 3-ри бита).

				QMS_data[mod][ch] += Data[CurrentPtr+1]
				# в нечетном (старшем) 16-ти битном слове элемента данных содержится
				# информация о величине сигнала измеренного в данном канале данного модуля
				# (в кодах АЦП).
				# при этом в канале i*2 содержатся старшие 16 бит данных i-го физического канала
				# а в канале i*2 + 1 - младшие 2 бита i-го физического канала
				# в настоящем примере измерения интегрируются в соответствующей ([модуль][канал])
				# ячейке массива накопления. В реальной программе пользователя измерения могут
				# перемещаться в поканальные массивы без усреднения, для дальнейшей обработки или
				# записи в выходные файлы.
				QMS_iter[mod][ch] += 1  # счетчик количества суммирований для данного канала



			DataPtrOld = DataPtr  # запоминание предыдущего указателя на последний полученный элемент данных

			time += 1  # счетчик количества 100 мс интервалов
			if time >= 5:  # если накопление длится более 0.5 с,
				DisplayData()  # произвести отображение результатов измерений
				time = 0  # обнуление счетчика количества 100 мс интервалов

			sleep(0.05)  # Ожидание следующей порции данных от сервера в течении 100 мс
			# В реальной программе в этом месте вместо Sleep может производится
			# обработка полученных данных и их запись в файл
	except KeyboardInterrupt:
		pass

	_quit()
	return 0  # выход из программы


# // Выход из программы
def _quit():
	print("\n\n Quiting program, please wait....\r")
	lib.QMX_Stop(SD, QMX_STOP_ALL)  # Остановим систему
	lib.QMX_MOD_SetStartMode(SD, 0xFF, QMX_START_MODE_MANUAL)

	lib.QMX_CircBufDetach(SD)  # Освободим виртуальный слот USB
	# Остановим сервер
	subprocess.Popen(["qmbox_ds.exe", "-s0"])
	exit(0)


# // Проверка на ошибки
def CheckErr():
	tmp_str = ctypes.create_string_buffer(1024)
	check_error = lib.QMX_GetLastError(SD, tmp_str, 1024)
	if check_error == QMX_OK.value:
		return
	else:
		print(f"Error {check_error}; Tmp {tmp_str.value.decode()};\n")
		_quit()


# Отображение данных
def DisplayData():
	d_result = []

	# Пересчет в Вольты
	for i in range(4):
		d_result[i] = (QMS_data[0][i*2] * 4 + QMS_data[0][i*2 + 1] / QMS_iter[0][i*2]) * CC_Adc.Scale + CC_Adc.Offset

	# Печать
	for i in range(3):
		print(f"{d_result[i]} ")

	# обнуление рабочих ячеек усреднения результатов измерений :
	for i in range(32):
		QMS_data[0][i] = 0
		QMS_iter[0][i] = 0


if __name__ == "__main__":
	main()

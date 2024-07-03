//На примере устройства, состоящего из одного модуля QMS17, 
//иллюстрируется применения основных библиотечных
//функций и правильная очередность их вызова.


#include "qmx.h"
// #include "qms17.h"
// #include "qmx_core.h"
#include <shellapi.h>
#include <stdio.h>
#include <conio.h> 


HANDLE SD;                              // Дескриптор системы
WORD Gain, QMS15Table[16], Data1, Data2, Channel;
QMX_CC_F CC_Adc;
QMX_CONFIG CFG;                         // Конфигурация системы

static unsigned int DataPtr, DataPtrOld, CurrentPtr, LengthDataBuf;
static unsigned short int *Data;
static unsigned long long int QMS_data[8][32], QMS_iter[8][32], time, temp;


void CheckErr(void);
void quit(void);
void DisplayData(void);


int main(int argc, char* argv[])
{
	char s_wrk[64];
	int mod, ch;;

	printf("Please wait....\r");
	
	// Запустим сервер
	ShellExecuteW(NULL, TEXT(L"open"), TEXT(L"qmbox_ds.exe"), TEXT(L"-p3"), NULL, SW_SHOWNORMAL);
	Sleep(500);

	// Получим дискриптор системы
	SD = 0;
	SD=QMX_CircBufAttach(0);
	if (SD == NULL) {
		printf("ERROR: No QMBox found.");
		quit(); 
	}

	// Проинициализуем систему
	QMX_InitModules(SD, QMX_LOG_NONE, NULL);
	CheckErr();


	// Фиктивный вызов QMX_Prepare - выясним конфигурацию устройства
	// т.е. из каких модулей оно состоит
	QMX_MOD_SetActive(SD, 0, QMX_ON);
	QMX_Prepare(SD, 100, &CFG);
	CheckErr();
	QMX_MOD_SetActive(SD, 0, QMX_OFF);
	
	// Убедимся, что в нулевом слоте устройства - модуль QMS17
	strncpy_s (s_wrk, CFG.ModName[0], 5); s_wrk[5] = 0;
	if (!_stricmp(s_wrk, "QMS17"))	printf("Module name OK\n");
	else{ printf("Module Name error\n");	quit();	}


	// Зададим усиление
	Gain = QMX_QMS17_GAIN_1;

	// Считаем калибр. коэффициенты для нужного усиления
	QMX_QMS17_ReadCC(SD, 0, Gain, &CC_Adc);

	// Запишем таблицу опроса, состоящую из 4х каналов - 0, 1, 2 и 3
	QMX_QMS17_SetTable(SD, 0, 0xF, Gain);



	// Установим частоту работы АЦП 1 MГц (частота ввода 16-битных слов - 2 МГц)
	QMX_QMS17_SetInputRate(SD, 0, 2000000.0, NULL);
	CheckErr();

	// Подготовимся к сбору данных
	QMX_MOD_SetActive(SD, 0, QMX_ON);
	QMX_Prepare(SD, 100, &CFG);
	CheckErr();
	
	LengthDataBuf = *CFG.CBInput.buf_size;						// размер кольцевого буфера данных, заполняемого сервером
	Data = (unsigned short int*)CFG.CBInput.buf_start;			// указатель на начало кольцевого буфера данных,
																// заполняемого сервером
	DataPtrOld = 0;
	time = 0;

	// Запустим сбор данных
	QMX_Start(SD);
	CheckErr();

	while (!_kbhit()){											// цикл непрерывного сбора данных
																// (до нажатия кнопки клавиатуры)

		DataPtr = *CFG.CBInput.buf_ptr;							//получим значение указателя на последний 
																// полученный элемент данных						
		for (CurrentPtr = DataPtrOld; CurrentPtr != DataPtr; CurrentPtr += 2) { // цикл считывания всех полученных элементов данных
			
			if(CurrentPtr >= *CFG.CBInput.buf_size)				// Проверки, необходимые
				CurrentPtr = 0;									// чтобы текущий указатель 
			if ((!CurrentPtr)&&(!DataPtr))break;				// не перескочил через границу буфера

			mod = (Data[CurrentPtr] >> 5) & 0x07;				// в четном (младшем) 16-ти битном слове элемента данных содержится 
			ch  = Data[CurrentPtr]  & 0x1f;						// информация о номерах канала (младщие 5-ть бит) и модуля (сл. 3-ри бита).

	
			QMS_data[mod][ch] += Data[CurrentPtr+1];
																// в нечетном (старшем) 16-ти битном слове элемента данных содержится
																// информация о величине сигнала измеренного в данном канале данного модуля
																// (в кодах АЦП).
																// при этом в канале i*2 содержатся старшие 16 бит данных i-го физического канала
																// а в канале i*2 + 1 - младшие 2 бита i-го физического канала


																// в настоящем примере измерения интегрируются в соответствующей ([модуль][канал])
																// ячейке массива накопления. В реальной программе пользователя измерения могут
																// перемещаться в поканальные массивы без усреднения, для дальнейшей обработки или
																// записи в выходные файлы.
			QMS_iter[mod][ch]++;								// счетчик количества суммирований для данного канала 
	
		}

		DataPtrOld = DataPtr;						// запоминание предыдущего указателя на последний полученный
													// элемент данных

		time++;										// счетчик количества 100 мс интервалов
		if (time >=5){								// если накопление длится более 0.5 с,
			DisplayData();	int(CFG.CBInput.buf_size)						// произвести отображение результатов измерений
			time = 0;								// обнуление счетчика количества 100 мс интервалов
		}

		Sleep(50);									// Ожидание следующей порции данных от сервера в течении 100 мс
													// В реальной программе в этом месте вместо Sleep может производится
													// обработка полученных данных и их запись в файл

 }

	quit();
	return 0;											// выход из программы
}



// Выход из программы
void quit(void){
	
	printf("\n\n Please wait....\r");
	QMX_Stop(SD, QMX_STOP_ALL);			// Остановим систему
	QMX_MOD_SetStartMode(SD, 0xFF, QMX_START_MODE_MANUAL); 									

	QMX_CircBufDetach(SD);              // Освободим виртуальный слот USB
										// Остановим сервер
	ShellExecute(NULL, TEXT("open"), TEXT("qmbox_ds.exe"), TEXT("-s0"), NULL, SW_SHOWNORMAL);
	printf("\n\n");
	ExitProcess(0);
}


// Проверка на ошибки
void CheckErr(void) {
char tmp_str[1024];

if (QMX_GetLastError(SD, tmp_str, sizeof(tmp_str))==QMX_OK){ return;}
else  {	printf("%s\n", tmp_str);	quit();}
}



// Отображение данных
void DisplayData(void){

	double d_wrk, d_result[16];
	unsigned int i_result[16], i, k = 0;

	// Пересчет в Вольты
	for (i = 0; i < 4; i++) d_result[i] = ((double)(QMS_data[0][i*2] * 4 + QMS_data[0][i*2 + 1]) / (double)QMS_iter[0][i*2]) * CC_Adc.Scale + CC_Adc.Offset;

	// Печать
	for (i = 0; i < 3; i++) printf("%2.6f  ", d_result[i]);	
	printf("%2.6f  \r", d_result[i]);	


	// обнуление рабочих ячеек усреднения результатов измерений :
	for (i = 0; i < 32; i++)	{QMS_data[0][i] = 0; QMS_iter[0][i] = 0; }

}



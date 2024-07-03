#ifndef __qmx_h__
#define __qmx_h__
#include <windows.h>

#if defined(QMX_BUILD_DLL)
 #define QMX_API __declspec(dllexport)
 #define QMX_EXT extern "C"
#elif defined(QMX_BUILD_APP)
 #define QMX_API
 #if defined(_CVI_)
  #define QMX_EXT extern
 #else
  #define QMX_EXT extern "C"
 #endif
#else
 #define QMX_API __declspec(dllimport)
 #if defined(_CVI_)
  #define QMX_EXT extern
 #else
  #define QMX_EXT extern "C"
 #endif
#endif

typedef struct QMX_CONFIG QMX_CONFIG;		// Конфигурация крейта, заполняется QMX_Open() и QMX_Prepare()
typedef struct QMX_CC_F QMX_CC_F;		// Калибровочные коэффициенты float
typedef struct QMX_CC_D QMX_CC_D;		// Калибровочные коэффициенты double
typedef struct QMX_CIRC_BUF QMX_CIRC_BUF;	// Кольцевой буфер
typedef struct QMX_CIRC_BUF_CONFIG QMX_CIRC_BUF_CONFIG;

QMX_EXT QMX_API int __stdcall QMX_GetDllVersion(void); // ToDo: !!!
QMX_EXT QMX_API int __stdcall QMX_GetLastError(HANDLE SD, char *str, DWORD len);
QMX_EXT QMX_API int __stdcall QMX_GetLastErrorForSlot(BYTE Slot, char *str, DWORD len);
QMX_EXT QMX_API HANDLE __stdcall QMX_Open(WORD VirtualSlot);
QMX_EXT QMX_API int __stdcall QMX_InitModules(HANDLE SD, WORD LogLevel, BYTE *FaulSlot);
QMX_EXT QMX_API int __stdcall QMX_GetConfiguration(HANDLE SD, QMX_CONFIG *Config);
QMX_EXT QMX_API int __stdcall QMX_Close(HANDLE SD);
QMX_EXT QMX_API int __stdcall QMX_Prepare(HANDLE SD, WORD ResponseTime, QMX_CONFIG *Config);
QMX_EXT QMX_API int __stdcall QMX_Start(HANDLE SD); // (crate (transfer) + modules)
QMX_EXT QMX_API int __stdcall QMX_Stop(HANDLE SD, WORD Mode); //(crate (transfer))
QMX_EXT QMX_API int __stdcall QMX_MOD_OutputPrestage(HANDLE SD, BYTE Slot);
QMX_EXT QMX_API int __stdcall QMX_MOD_SetActive(HANDLE SD, BYTE Slot, BYTE IsActive);
QMX_EXT QMX_API int __stdcall QMX_MOD_SetStartMode(HANDLE SD, BYTE Slot, BYTE Mode);

// Работа с кольцевыми буферами ============================================================
QMX_EXT QMX_API HANDLE __stdcall QMX_CircBufAttach(WORD VirtualSlot);
QMX_EXT QMX_API int __stdcall QMX_CircBufDetach(HANDLE SD);
QMX_EXT QMX_API int __stdcall QMX_CircBufSetChannels(HANDLE SD, BYTE Slot, BYTE Channels);
QMX_EXT QMX_API int __stdcall QMX_CircBufSetStorePath(HANDLE SD, char *Path);
QMX_EXT QMX_API int __stdcall QMX_CircBufOutputUpdate(HANDLE SD);

// Низкоуровневые функции ==================================================================
//QMX_EXT QMX_API int __stdcall QMX_PutQMbus(QMX_SYSTEM *SD, WORD BusAddress, WORD N, WORD *Data);
//QMX_EXT QMX_API int __stdcall QMX_GetQMbus(QMX_SYSTEM *SD, WORD BusAddress, WORD N, WORD *Data);


#pragma pack(push)
#pragma pack(1)

struct QMX_CIRC_BUF_CONFIG {
 volatile WORD *buf_start;				// Указатель на начало буфера
 volatile DWORD *buf_ptr;				// Указатель записи в буфер [0..buf_size-1]
 volatile DWORD *buf_cntr;				// Счетчик записи в буфер [0..2^32]
 volatile DWORD *buf_size;				// Размер буфера в словах (WORD)
 volatile HANDLE event;
};


// структура конфигурации крейта
struct QMX_CONFIG {
// Поля доступные после QMX_Open();
 BYTE 		  	UsbSpeed;	       		// Текущая скорость USB
 CHAR		  	CtlName[11];         		// Название контроллера
 CHAR		  	CtlBIOSversion[6];   		// Версия БИОСа контроллера
 CHAR		  	CtlSerialNumber[10]; 		// Серийный номер контроллера
 BYTE 		  	ModQuantity;         		// Количество модулей в системе
 CHAR		  	ModName[8][10];      		// Имена модулей
 WORD		  	ModType[8];      		// Типы модулей
 BYTE		  	ModRevision[8];   		// Ревизия модуля
 CHAR		  	ModSerialNumber[8][10];		// Серийный номер модуля
 HANDLE           	DevHandle;			// Хэндл драйвера
// Поля доступные после QMX_Prepare();
 WORD   	  	RespTime;	   		// Рассчетное время реакции в мс
 DWORD  	  	ReadFileSize;	   		// Размер ридфайла в БАЙТАХ
 DWORD  	  	WriteFileSize;   		// Размер райтфайла в БАЙТАХ
 DWORD		  	ModLocalBufferSize[8];		// Размер половины локального буфера-1 (в словах)
 double		  	InputRate;			// Скорость ввода данных (слов) в кГц
 double		  	OutputRate;			// Скорость вывода данных (слов) в кГц
 QMX_CIRC_BUF_CONFIG    CBInput;			// Конфигурация кольцевого буфера ввода
 QMX_CIRC_BUF_CONFIG    CBOutput;			// Конфигурация кольцевого буфера вывода
};

// калибровочные коэффициенты
struct QMX_CC_F {
 float Offset;
 float Scale;
 char Units[6];
 BYTE UnitsCode;				// зарезервировано
};

struct QMX_CC_D {
 double Offset;
 double Scale;
 char Units[6];
 BYTE UnitsCode;				// зарезервировано
};

#pragma pack(pop)

//варианты останова системы. для функции QMX_Stop()
#define QMX_STOP_ALL		0
#define QMX_STOP_TRANSFER_IN	1
#define QMX_STOP_TRANSFER_OUT	2

//типы модулей. QMX_CONFIG->ModType[]
#define QMX_MOD_NONE		0x0000		// такого номера гарантировано не будет выпускаться
#define QMX_MOD_QM14		0x000E
#define QMX_MOD_QMS301		0x012D
#define QMX_MOD_QMS10		0x000A
#define QMX_MOD_QMS15		0x000F
#define QMX_MOD_QMS17		0x0011
#define QMX_MOD_QMS20		0x0014
#define QMX_MOD_QMS40		0x0028
#define QMX_MOD_QMS45		0x002D
#define QMX_MOD_QMS50		0x0032
#define QMX_MOD_QMS60		0x003C
#define QMX_MOD_QMS70		0x0046
#define QMX_MOD_QMS75		0x004B
#define QMX_MOD_QMS80		0x0050
#define QMX_MOD_QMS82		0x0052
#define QMX_MOD_QMS85		0x0055
#define QMX_MOD_QMS90		0x005A
#define QMX_MOD_QMS92		0x005C
#define QMX_MOD_QMS93		0x005D

//==============================================================================
#define QMX_OFF     		0
#define QMX_ON	                1

//способы запуска модуля. для QMX_MOD_SetStartMode
#define QMX_START_MODE_MANUAL	0		// Запуск модуля или контроллера функцией QMX_Start()
#define QMX_START_MODE_EXTERNAL	1		// Запуск модуля или контроллера по внешнему сигналу после QMX_Start()

// Настройка способа вывода отладочной информации для функции QMX_Open()
#define QMX_LOG_NONE		0		// Не выводить отчет
#define QMX_LOG_TO_FILE		1		// Выводить отчет в файл

// Коды ошибок =================================================================
#define QMX_OK         		        	0L	//
#define QMX_ERR         	        	-1L	//
#define QMXE_INTERNAL_ERROR			0x80000000//
#define QMXE_INVALID_OS                		2L	//
#define QMXE_DEVICE_OPEN                	3L	//
#define QMXE_DEVICE_CLOSE               	4L	//
#define QMXE_FPGA_FIRMWARE_FILE_OPEN    	5L	//
#define QMXE_WRITE_PROHIBITED_REGION    	7L	// ToDo: !!!
#define QMXE_NO_ACTIVE_MODULES          	8L	//
#define QMXE_ACTIVE_OUTPUT_EXCEEDING    	9L	//
#define QMXE_MAX_SPEED_EXCEEDING        	10L	//
#define QMXE_ACTIVE_MODULE_ZERO_SPEED		11L	//
#define QMXE_BAD_PARAMETER                  	12L	//
#define QMXE_INVALID_DESCRIPTOR                 13L	//
#define QMXE_DATA_FLOW_CORRUPTED               	14L	//
#define QMXE_BUFFER_OVERFLOW		       	15L	//
#define QMXE_DISK_WRITE_ERROR		       	16L	//
#define QMXE_DATA_SERVER_ATTACH		       	17L	//
#define QMXE_ALREADY_STARTED			18L	//

// Специализированные функции ==================================================
#include "qms3.h"
#include "qms10.h"
#include "qms15.h"
#include "qms17.h"
#include "qms20.h"
#include "qms45.h"
#include "qms50.h"
#include "qms70.h"
#include "qms75.h"
#include "qms80.h"
#include "qms82.h"
#include "qms85.h"
#include "qms90.h"
#include "qms93.h"
#include "qm14.h"

#endif

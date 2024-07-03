#ifndef __qms17_h__
#define __qms17_h__
#include "qmx.h"


#define QMX_QMS17_GAIN_1	0
#define QMX_QMS17_GAIN_2	1

QMX_EXT QMX_API int __stdcall QMX_QMS17_SetInputRate(HANDLE SD, BYTE Slot, double DRate, double *CRate);
QMX_EXT QMX_API int __stdcall QMX_QMS17_SetTable(HANDLE SD, BYTE Slot, WORD ChannelMask, WORD Gain);
QMX_EXT QMX_API int __stdcall QMX_QMS17_GetAdc(HANDLE SD, BYTE Slot, WORD Mode, WORD *Data);	// Async
QMX_EXT QMX_API int __stdcall QMX_QMS17_WriteCC(HANDLE SD, BYTE Slot, WORD Gain, QMX_CC_F *CC);
QMX_EXT QMX_API int __stdcall QMX_QMS17_ReadCC(HANDLE SD, BYTE Slot, WORD Gain, QMX_CC_F *CC);

#endif
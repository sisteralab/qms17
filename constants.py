# варианты останова системы. для функции QMX_Stop()
from ctypes import c_long

QMX_STOP_ALL = 0
QMX_STOP_TRANSFER_IN = 1
QMX_STOP_TRANSFER_OUT = 2

# типы модулей. QMX_CONFIG->ModType[]
QMX_MOD_NONE = 0x0000  # такого номера гарантировано не будет выпускаться
QMX_MOD_QM14 = 0x000E
QMX_MOD_QMS301 = 0x012D
QMX_MOD_QMS10 = 0x000A
QMX_MOD_QMS15 = 0x000F
QMX_MOD_QMS17 = 0x0011
QMX_MOD_QMS20 = 0x0014
QMX_MOD_QMS40 = 0x0028
QMX_MOD_QMS45 = 0x002D
QMX_MOD_QMS50 = 0x0032
QMX_MOD_QMS60 = 0x003C
QMX_MOD_QMS70 = 0x0046
QMX_MOD_QMS75 = 0x004B
QMX_MOD_QMS80 = 0x0050
QMX_MOD_QMS82 = 0x0052
QMX_MOD_QMS85 = 0x0055
QMX_MOD_QMS90 = 0x005A
QMX_MOD_QMS92 = 0x005C
QMX_MOD_QMS93 = 0x005D

# ==============================================================================
QMX_OFF = 0
QMX_ON = 1

# способы запуска модуля. для QMX_MOD_SetStartMode
QMX_START_MODE_MANUAL = 0  # Запуск модуля или контроллера функцией QMX_Start()
QMX_START_MODE_EXTERNAL = 1  # Запуск модуля или контроллера по внешнему сигналу после QMX_Start()

# Настройка способа вывода отладочной информации для функции QMX_Open()
QMX_LOG_NONE = 0  # Не выводить отчет
QMX_LOG_TO_FILE = 1  # Выводить отчет в файл

# Коды ошибок =================================================================
QMX_OK = c_long(0)
QMX_ERR = c_long(-1)
QMXE_INTERNAL_ERROR = 0x8000000
QMXE_INVALID_OS = c_long(2)
QMXE_DEVICE_OPEN = c_long(3)
QMXE_DEVICE_CLOSE = c_long(4)
QMXE_FPGA_FIRMWARE_FILE_OPEN = c_long(5)
QMXE_WRITE_PROHIBITED_REGION = c_long(7)
QMXE_NO_ACTIVE_MODULES = c_long(8)
QMXE_ACTIVE_OUTPUT_EXCEEDING = c_long(9)
QMXE_MAX_SPEED_EXCEEDING = c_long(10)
QMXE_ACTIVE_MODULE_ZERO_SPEED = c_long(11)
QMXE_BAD_PARAMETER = c_long(12)
QMXE_INVALID_DESCRIPTOR = c_long(13)
QMXE_DATA_FLOW_CORRUPTED = c_long(14)
QMXE_BUFFER_OVERFLOW = c_long(15)
QMXE_DISK_WRITE_ERROR = c_long(16)
QMXE_DATA_SERVER_ATTACH = c_long(17)
QMXE_ALREADY_STARTED = c_long(18)

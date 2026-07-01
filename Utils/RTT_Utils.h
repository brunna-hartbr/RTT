#ifndef RTT_UTILS_H
#define RTT_UTILS_H

/*
these macros should be defined as symbols depending on the project using them

LTE_MODULE=1
LORA_RADIO=1
CPU=1
*/
#include <stdio.h>
#include <stdbool.h>

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#if LTE_MODULE
#include "lte_com.h"
#include "lteContext.h"
#include "modemStateHandler.h"
#include "CommandCircularBuffer.h"

extern const char* IoTStateNames[IOTSTATE_NUMBER_STATES];
extern const char* ModemStatesNames[MODEM_STATES_MAX];
#endif
extern bool g_rtt_enabled;
extern bool g_debug_present;

// Public functions
void RTT_Utils_init(void);
void RTT_Utils_set_enabled(bool enable);
void RTT_Utils_toggle(void);
void RTT_Logs_Test(void);
bool isDebugAttached(void);
void RTT_Debug_Check(uint32_t timestamp);

#ifdef RTT_LOGS

#define MAIN_LOGS_ENABLE            1
#define LTE_LOGS_ENABLE             1
#define MODEM_STATES_LOGS_ENABLE    1
#define RTT_TIME_LOGS_ENABLE		1

#else

#define MAIN_LOGS_ENABLE            0
#define LTE_LOGS_ENABLE             0
#define MODEM_STATES_LOGS_ENABLE    0
#define RTT_TIME_LOGS_ENABLE		0

#endif

#ifdef RTT_LOGS

// Internal macro for checking if logs are enabled (compile-time AND runtime)
#define IS_LOG_ENABLED(compile_flag) \
((compile_flag) && (g_rtt_enabled) && (g_debug_present))


// RTT_LOG_MSG(1, "Sys Start"); - only text
#define RTT_LOG_MSG(enable, str) do { \
	if (IS_LOG_ENABLED(enable)) \
	{ \
		SEGGER_RTT_WriteString(0, str); \
		SEGGER_RTT_WriteString(0, "\n"); \
	} \
} while(0)

// RTT_LOG_VAR(1, "counter", "%d", count); - integer or string values
#define RTT_LOG_VAR(enable, name, fmt, val) do { \
	if (IS_LOG_ENABLED(enable)) \
	{ \
		SEGGER_RTT_printf(0, "%s " fmt "\n", name, val); \
	} \
} while(0)

// RTT_LOG_FLOAT(1, "counter", count); - float values
#define RTT_LOG_FLOAT(enable, name, val) do { \
	if (IS_LOG_ENABLED(enable)) \
	{ \
		float v = (float)(val); \
		int integer = (int)v; \
		int frac = (int)((v - integer) * 100 + 0.5); \
		if (frac >= 100) { integer++; frac -= 100; } \
		if (frac < 0) { integer--; frac = 0; } \
		SEGGER_RTT_printf(0, "%s = %d.%02d\n", name, integer, frac); \
	} \
} while(0)

/*
mode = 0 ? HEX
mode = 1 ? ASCII (char)

Example:
uint8_t data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"

Hexadecimal:
RTT_LOG_BUFFER(1, "data", data, sizeof(data), 0);
Output: data [5 bytes]: 48 65 6C 6C 6F

ASCII:
RTT_LOG_BUFFER(1, "data", data, sizeof(data), 1);
Output: data [5 bytes]: Hello
*/
#define RTT_LOG_BUFFER(enable, name, buf, len, mode) do { \
	if (IS_LOG_ENABLED(enable)) { \
		SEGGER_RTT_printf(0, "%s [%d bytes]: ", name, (int)(len)); \
		for (int __i = 0; __i < (len); __i++) { \
			unsigned char __c = ((unsigned char*)(buf))[__i]; \
			if (mode == 0) { \
				SEGGER_RTT_printf(0, "%02X ", __c); \
				} else { \
				if (__c >= 32 && __c <= 126) { \
					SEGGER_RTT_printf(0, "%c", __c); \
					} else { \
					SEGGER_RTT_printf(0, "."); \
				} \
			} \
		} \
		SEGGER_RTT_WriteString(0, "\n"); \
	} \
} while(0)

//extern volatile uint32_t tickBase;
#define RTT_LOG_TIME(enable, label, tickbase) do { \
	if (IS_LOG_ENABLED(enable)) { \
		SEGGER_RTT_printf(0, "%s: %lu ms\n", label, (unsigned long)(tickbase)); \
	} \
} while(0)

#if LTE_MODULE
/*
RTT_LOG_LTE_STATE(1, lteContext.currentState);
Output: LTE current state: MODEM_CONNECTED
*/
#define RTT_LOG_LTE_STATE(enable, state) do { \
	if (IS_LOG_ENABLED(enable)) { \
		if ((state) < IOTSTATE_NUMBER_STATES) { \
			SEGGER_RTT_printf(0, "LTE current state: %s\n", IoTStateNames[state]); \
			} else { \
			SEGGER_RTT_WriteString(0, "Invalid LTE state\n"); \
		} \
	} \
} while(0)

/*
RTT_LOG_LTE_TRANSITION(1, SYSTEM_STARTUP, MODEM_OPERATIONAL);
Output: LTE state transition: SYSTEM_STARTUP -> MODEM_OPERATIONAL
*/
#define RTT_LOG_LTE_TRANSITION(enable, lastState, currentState) do { \
	if (IS_LOG_ENABLED(enable)) { \
		if ((lastState) < IOTSTATE_NUMBER_STATES && (currentState) < IOTSTATE_NUMBER_STATES) { \
			SEGGER_RTT_printf(0, "LTE state transition: %s -> %s\n", \
			IoTStateNames[lastState], IoTStateNames[currentState]); \
			} else { \
			SEGGER_RTT_WriteString(0, "Invalid LTE state transition\n"); \
		} \
	} \
} while(0)

/*
RTT_LOG_MODEM_STATE(1, MODEM_OPERATIONAL);
Output: Modem State: MODEM_OPERATIONAL
*/
#define RTT_LOG_MODEM_STATE(enable, state) do { \
	if (IS_LOG_ENABLED(enable)) { \
		if ((state) < MODEM_STATES_MAX) { \
			SEGGER_RTT_printf(0, "Modem State: %s\n", ModemStatesNames[state]); \
			} else { \
			SEGGER_RTT_WriteString(0, "Modem State Invalid\n"); \
		} \
	} \
} while(0)

/*
RTT_LOG_MODEM_TRANSITION(1, SYSTEM_STARTUP, MODEM_CONNECTED);
Output: State Transition: SYSTEM_STARTUP -> MODEM_CONNECTED
*/
#define RTT_LOG_MODEM_TRANSITION(enable, lastState, currentState) do { \
	if (IS_LOG_ENABLED(enable)) { \
		if ((lastState) < MODEM_STATES_MAX && (currentState) < MODEM_STATES_MAX) { \
			SEGGER_RTT_printf(0, "State Transition: %s -> %s\n", \
			ModemStatesNames[lastState], ModemStatesNames[currentState]); \
			} else { \
			SEGGER_RTT_WriteString(0, "Invalid Transition\n"); \
		} \
	} \
} while(0)

/*
Print first command (IMEI)
RTT_LOG_LTECMD(1, 0);
output: LTECmd[0]: IMEI | AT: AT+GSN | Timeout: 300 ms | ResponseType: 0 | CmdType: 1

Print operator query command
RTT_LOG_LTECMD(1, 10);
output: LTECmd[10]: Connection Operator | AT: AT+COPS? | Timeout: 300 ms | ResponseType: 2 | CmdType: 1
*/

LTECmdConfig_t auxCmd;
extern const LTECmdConfig_t LTECmds[LTE_CMD_NUMBER];

#define RTT_LOG_LTECMD(enable, idx) do { \
	if (IS_LOG_ENABLED(enable)) { \
		if ((idx) < LTE_CMD_NUMBER) { \
			auxCmd = LTECmds[idx]; \
			SEGGER_RTT_printf(0, \
			"LTECmd[%d]: %s | AT: %s | Timeout: %lu ms | isSet: %d | CmdId: %d\n", \
			(idx), \
			auxCmd.descriptor, \
			auxCmd.cmd, \
			(unsigned long)auxCmd.waitResponseTime, \
			auxCmd.isSet, \
			auxCmd.cmdId); \
			} else { \
			SEGGER_RTT_WriteString(0, "Invalid LTECmd index\n"); \
		} \
	} \
} while(0)

#endif


#else

#define RTT_LOG_MSG(enable, str)									((void)0)
#define RTT_LOG_VAR(enable, name, fmt, val)							((void)0)
#define RTT_LOG_FLOAT(enable, name, val)							((void)0)
#define RTT_LOG_BUFFER(enable, name, buf, len, mode)				((void)0)
#define RTT_LOG_TIME(enable, label, tickbase)						((void)0)
#define RTT_LOG_LTE_STATE(enable, state)							((void)0)
#define RTT_LOG_LTE_TRANSITION(enable, lastState, currentState)		((void)0)
#define RTT_LOG_MODEM_STATE(enable, state)							((void)0)
#define RTT_LOG_MODEM_TRANSITION(enable, lastState, currentState)	((void)0)
#define RTT_LOG_LTECMD(enable, idx)									((void)0)

#endif


#endif // RTT_UTILS_H
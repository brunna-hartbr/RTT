
#include "RTT_Utils.h"
#include "SEGGER_RTT.h"

#if LTE_MODULE

#include "lteContext.h"
#include "modemStateHandler.h"
#include "CommandCircularBuffer.h"


const char* IoTStateNames[IOTSTATE_NUMBER_STATES] =
{
	[IOTSTATE_WAIT_POWER_CONDITION]     = "IOTSTATE_WAIT_POWER_CONDITION",
	[IOTSTATE_WAIT_INITIALIZATION]      = "IOTSTATE_WAIT_INITIALIZATION",
	[IOTSTATE_CONFIGURE_NWKLED]         = "IOTSTATE_CONFIGURE_NWKLED",
	[IOTSTATE_CONFIGURE_ECHO]           = "IOTSTATE_CONFIGURE_ECHO",
	[IOTSTATE_CONFIGURE_WDISABLEPIN]    = "IOTSTATE_CONFIGURE_WDISABLEPIN",
	[IOTSTATE_CONFIGURE_GPSOFF]         = "IOTSTATE_CONFIGURE_GPSOFF",
	[IOTSTATE_CONFIGURE_USBENABLE]      = "IOTSTATE_CONFIGURE_USBENABLE",
	[IOTSTATE_CONFIGURE_CHECKTECH]      = "IOTSTATE_CONFIGURE_CHECKTECH",
	[IOTSTATE_CHECK_RADIOIMEI]          = "IOTSTATE_CHECK_RADIOIMEI",
	[IOTSTATE_CHECK_RADIOMODEL]         = "IOTSTATE_CHECK_RADIOMODEL",
	[IOTSTATE_CHECK_RADIOFWVERSION]     = "IOTSTATE_CHECK_RADIOFWVERSION",
	[IOTSTATE_CHECK_SIMCARD_STATE]      = "IOTSTATE_CHECK_SIMCARD_STATE",
	[IOTSTATE_CONFIGURE_APN_CONTEXT]    = "IOTSTATE_CONFIGURE_APN_CONTEXT",
	[IOTSTATE_CLEAR_PREV_MQTT_CONN]     = "IOTSTATE_CLEAR_PREV_MQTT_CONN",
	[IOTSTATE_CLEAR_PREV_MQTT_CONN_WAIT]= "IOTSTATE_CLEAR_PREV_MQTT_CONN_WAIT",
	[IOTSTATE_CONFIGURE_CERTS]          = "IOTSTATE_CONFIGURE_CERTS",
	[IOTSTATE_CONFIGURE_SSL]            = "IOTSTATE_CONFIGURE_SSL",
	[IOTSTATE_CONFIGURE_MQTT_FULL]      = "IOTSTATE_CONFIGURE_MQTT_FULL",
	[IOTSTATE_CONFIGURE_MQTT_SSL]       = "IOTSTATE_CONFIGURE_MQTT_SSL",
	[IOTSTATE_WAIT_NWK_CONNECTION]      = "IOTSTATE_WAIT_NWK_CONNECTION",
	[IOTSTATE_ACTIVATE_APN_CONTEXT]     = "IOTSTATE_ACTIVATE_APN_CONTEXT",
	[IOTSTATE_OBTAIN_IP]                = "IOTSTATE_OBTAIN_IP",
	[IOTSTATE_MQTT_OPEN_REQ]            = "IOTSTATE_MQTT_OPEN_REQ",
	[IOTSTATE_MQTT_OPEN_WAIT]           = "IOTSTATE_MQTT_OPEN_WAIT",
	[IOTSTATE_MQTT_CONNECT_REQ]         = "IOTSTATE_MQTT_CONNECT_REQ",
	[IOTSTATE_MQTT_CONNECT_WAIT]        = "IOTSTATE_MQTT_CONNECT_WAIT",
	[IOTSTATE_MQTT_SUBSCRIBE_REQ]       = "IOTSTATE_MQTT_SUBSCRIBE_REQ",
	[IOTSTATE_MQTT_SUBSCRIBE_WAIT]      = "IOTSTATE_MQTT_SUBSCRIBE_WAIT",
	[IOTSTATE_MQTT_WAIT_TX]             = "IOTSTATE_MQTT_WAIT_TX",
	[IOTSTATE_MQTT_TX]                  = "IOTSTATE_MQTT_TX",
	[IOTSTATE_MQTT_END_RX_WINDOW]       = "IOTSTATE_MQTT_END_RX_WINDOW",
	[IOTSTATE_KEEP_LTE_SILENT]          = "IOTSTATE_KEEP_LTE_SILENT",
	[IOTSTATE_FORCE_POWER_CYCLE_MODEM]  = "IOTSTATE_FORCE_POWER_CYCLE_MODEM",
};

const char* ModemStatesNames[MODEM_STATES_MAX] =
{
	[SYSTEM_STARTUP]           = "SYSTEM_STARTUP",
	[MODEM_KEPT_OFF]           = "MODEM_KEPT_OFF",
	[POWER_BANK_WAIT]          = "POWER_BANK_WAIT",
	[VOLTAGE_3V8_STABILIZATION]= "VOLTAGE_3V8_STABILIZATION",
	[MODEM_OPERATIONAL]        = "MODEM_OPERATIONAL",
	[MODEM_CONNECTED]          = "MODEM_CONNECTED",
	[SERVER_WARN_SHUTDOWN]     = "SERVER_WARN_SHUTDOWN",
};

#endif

bool g_rtt_enabled = false;

void RTT_Utils_init(void)
{
#ifdef RTT_LOGS
	SEGGER_RTT_Init();
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
	SEGGER_RTT_WriteString(0, "RTT Start\r\n\r\n");
	// Default: logs disabled until explicitly enabled
	g_rtt_enabled = false;
#endif
	return;
}

#if CPU_SAML21
#include "hri/hri_dsu_l21.h"
#include "saml21e18b.h"
#elif CPU_SAME53
#include "hri/hri_dsu_e53.h"
#include "same53j20a.h"
#endif
/**
 * @brief  Verifica se existe um J-Link (ou outro depurador) ativamente ligado via SWD.
 * @return true se o J-Link estiver ligado, false caso contrário.
*/
bool isDebugAttached(void) 
{
	//return hri_dsu_get_STATUSB_DBGPRES_bit(DSU);
	//return hri_dsu_get_STATUSB_DBGPRES_bit(DSU);
	return DSU->STATUSB.bit.DBGPRES;
}


// Function to enable/disable all logs at runtime
void RTT_Utils_set_enabled(bool enable) {
	g_rtt_enabled = enable;
}

// Function to toggle logs at runtime
void RTT_Utils_toggle(void) {
	g_rtt_enabled = !g_rtt_enabled;
}

void RTT_Logs_Test(void) 
{
	// Inicializar RTT Utils
	//RTT_Utils_init();
	
	// Habilitar logs em runtime quando necessário
	RTT_Utils_set_enabled(true);
	
	// Agora os logs serão exibidos
	RTT_LOG_MSG(1, "System Started");
	RTT_LOG_VAR(1, "counter", "%d", 42);
	
	// Desabilitar logs quando não precisar mais
	RTT_Utils_set_enabled(false);
	
	// Logs abaixo não serão exibidos
	RTT_LOG_MSG(1, "This won't appear");
	
	// Alternar estado (ligar/desligar)
	RTT_Utils_toggle(); // Liga novamente
	RTT_LOG_MSG(1, "This will appear again");
	
	return;
}
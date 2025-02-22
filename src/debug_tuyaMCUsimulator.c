#ifdef WINDOWS

#include "new_common.h"

const char *dataToSimulate[] =
{
	// dummy entry in order to avoid problems with empty table
	"",
#if 1

#elif 0
	"55AA00050005010400010110",
	"55AA0005000501040001000F"
#elif 0
	"55AA03070014060000080916000099000023010200040000000310",
	"55AA0307000C03000008160C160C0000000367",
	"55AA0307000C040000080C010C01000000033E",
	"55AA030700140600000809350000F800003A010200040000002FD1"
#elif 0
	"55AA030000010003",
	"55AA0301002A7B2270223A226C696834766A656F79616F346A656B75222C2276223A22322E302E30222C226D223A307D41",
	"55AA0302000004",
	"55AA0303000005",
	"55AA030000010104",
	"55AA0303000005",
	"55AA03070014060000080928000000000000010200040000000467",
	"55AA0307000C03000008160B160B0000000466",
	"55AA0307000C040000080B1D0B1D0000000071",
	"55AA030700060B00000201001D",
	"55AA030700111001000101120300083232343930303638E9",
	"55AA0324000026",
	"55AA031C00001E",
	"55AA031C00001E",
	"55AA031C00001E",
	"55AA030000010104",
	"55AA0324000026",
	"55AA030000010104",
#elif 0
	"55AA0307000801020004000000041C",
	"55AA030700130600000F0000000C0000001001C3000077091CAD",
	"55AA0307000509050001001D",
	"55AA03070005100100010121",
	"55AA030700221100001E00000000000000000000006400010E0000AA00000000000A00000000000081",
	"55AA0307001512000011000500640005001E003C0000000000000009",
	"55AA03070008650200040000000480",
	"55AA0307000866020004000000007D",
	"55AA03070008670200040000000C8A",
	"55AA0307000869020004000013881B",
	"55AA030700086D0200040000000387",
	"55AA030700086E0200040000001095",
	"55AA030700086F020004000001BE45",
#elif 1

#else
	"55AA030700130600000F000000BF0000013C01F800064F0930B4",
	"55AA030700130600000F000000BD0000013C01F40006450936AA",
	"55AA030700130600000F000000C10000013C01FD000653092EBD",
	"55AA030700130600000F000000BD0000013B01F6000646092EA4",
	"55AA030700130600000F000000BD0000013B01F5000646092DA2",
#endif
};
int g_totalStrings = sizeof(dataToSimulate) / sizeof(dataToSimulate[0]);

int delay_between_packets = 20;
int max_bytes_per_frame = 200;
int curString = 0;
const char *curP = 0;
int current_delay_to_wait_ms = 100;

void NewTuyaMCUSimulator_RunQuickTick(int deltaMS) {
	byte b;
	int c_added = 0;

	if (g_totalStrings <= 0) {
		return;
	}
	if (current_delay_to_wait_ms > 0) {
		current_delay_to_wait_ms -= deltaMS;
		return;
	}
	if (curP == 0) {
		curP = dataToSimulate[curString];
	}

#if 1

#elif 0
	if (DRV_IsRunning("TuyaMCU") == 0) {
		CMD_ExecuteCommand("startDriver TuyaMCU", 0);
		CMD_ExecuteCommand("startDriver tmSensor ", 0);
		CMD_ExecuteCommand("setChannelType 1 readonly", 0);
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 1 val 1", 0);
	}
#else
	if (DRV_IsRunning("TuyaMCU") == 0) {
		CMD_ExecuteCommand("startDriver TuyaMCU", 0);
		CMD_ExecuteCommand("startDriver NTP", 0);
		CMD_ExecuteCommand("setChannelType 1 toggle", 0);
		CMD_ExecuteCommand("setChannelType 2 Voltage_div10", 0);
		CMD_ExecuteCommand("setChannelType 3 Power", 0);
		CMD_ExecuteCommand("setChannelType 4 Current_div1000", 0);
		CMD_ExecuteCommand("setChannelType 5 EnergyTotal_kWh_div100", 0);
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 16 bool 1", 0);
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 1 val 5", 0);
		// TAC2121C VoltageCurrentPower Packet
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 6 RAW_TAC2121C_VCP", 0);
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 4 RAW_TAC2121C_Yesterday", 0);
		CMD_ExecuteCommand("linkTuyaMCUOutputToChannel 3 RAW_TAC2121C_LastMonth", 0);
	}
#endif
	// make sure that buffer has free size
	if (UART_GetDataSize() != 0) {
		return;
	}
	while (*curP != 0) {
		c_added++;
		if (c_added >= max_bytes_per_frame) {
			break;
		}
		b = hexbyte(curP);
		UART_AppendByteToCircularBuffer(b);
		curP += 2;
	}
	if (*curP == 0) {
		curString++;
		if (curString >= g_totalStrings)
			curString = 0;
		curP = 0;
		current_delay_to_wait_ms = delay_between_packets;
	}

}


#endif



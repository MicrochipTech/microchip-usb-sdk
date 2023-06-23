#pragma once

#include "typedef.h"

#define VERSION_NUM "1.0.0"

// Error Code
#define Error_Success   0x0000
#define Error_Fail      0x0001

void MchpEnableLogging(INT LoggingLevel);
void MchpUsbGetVersion(PCHAR VersionNum);
BOOL MchpSetConfigFilePath(PCHAR INIFile);
INT MchpUsbGetAllHubs(PCHAR HubInfo);
HANDLE MchpUSBOpen(BYTE byHubIndex);
BOOL MchpUsbClose (HANDLE hDevice);

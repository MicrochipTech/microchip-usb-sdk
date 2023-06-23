#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "Mchp_Usb_Api.h"
#include "MchpUsbInterface.h"

using namespace std;

// TODO - Update Logfile based on Logging Level
// This is basic implementation. Needs to optimize the code
void MchpEnableLogging(INT LoggingLevel)
{
    createLogFile();
} 

void MchpUsbGetVersion (PCHAR VersionNumber)
{
    string strVersionNum;
    strVersionNum = string ("\"lib_version\":\"") + string(VERSION_NUM) + string("\"");
    Logger("%s",strVersionNum.c_str());
    sprintf(VersionNumber, "%s", strVersionNum.c_str());
}

BOOL MchpSetConfigFilePath(PCHAR INIFile)
{
    return (mchp_read_config_file(INIFile));
}

INT MchpUsbGetAllHubs(PCHAR HubInfo)
{
    INT iMchpHubCnt = 0;
    iMchpHubCnt = mchp_usb_get_all_hubs(HubInfo);
    return iMchpHubCnt;
}

HANDLE MchpUSBOpen(BYTE byHubIndex)
{
   HANDLE hDevice = mchp_usb_open(byHubIndex);
   return hDevice;
}

BOOL MchpUsbClose (HANDLE hDevice)
{
    return (mchp_usb_close (hDevice));
}
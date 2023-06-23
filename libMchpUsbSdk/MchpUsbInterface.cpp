#include "MchpUsbInterface.h"
#include "PAL/PAL_Libusb/MchpUsbLibusbAbstraction.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdarg>
#include <time.h>
#include <unistd.h>

using namespace std;

WORD gwHUBVID[6]    =   {   0x0424, 0x00, 0x00, 0x00, 0x00, 0x00}; // default 0x0424
WORD gwHFCPID[32]   =   {   0x2530, 0x2740, 0x2840, 0x4940, 0x7040, 0x7240, //Common HFC PID 
				            0x274E, 0x274F,0x274C, //USB57xx HFC PID
						    0x284C, 0x284E, //USB58xx/USB59xx HFC PID
	                        0x724A, 0x724B, 0x724C, 0x724E, 0x724F,  //USB72xx HFC PID
					    };

INT giHubVidCnt = 1;
INT giHfcPidCnt = 16;
INT giHubMaxCnt = 0; /* Global Variable to store Maximum Hub count*/

FILE* LogFile = NULL;

HINFO gasHubInfo [MAX_HUBS];

/* Functions related to Logging - Todo Optimize*/
inline std::string NowTime()
{
    char buffer[200];
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);

    std::sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d", (tstruct.tm_year + 1900), (tstruct.tm_mon + 1), tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec);
    return buffer;
}
void Logger (const char * format, ...) {
    static char err_string[2000];
    va_list v;
    va_start(v, format);
    vsprintf(err_string, format, v);

    if(LogFile != NULL)
    {
        fprintf(LogFile,"%s - ", NowTime().c_str());
        fprintf(LogFile, "%s\n", err_string);
    }
    
}
void createLogFile ()
{
    string Log = "MchpUsbSdk.log";
    LogFile = fopen(Log.c_str(), "a+");
}
/* Functions related to logging - End*/

BOOL mchp_read_config_file(PCHAR INIFile)
{
    BOOL bReturn = FALSE;
    string strKey, strValue, line;
    BOOL bINIHubVID = FALSE, bINIHfcPID = FALSE;
    int iLine;

    ifstream InputFile;
    InputFile.open(INIFile);
    if (InputFile) //Find if file is available
	{
        for (iLine = 0; InputFile.eof() != TRUE; iLine++)
        {
            getline(InputFile, line);
            if(line.rfind(";") == 0){
                continue;
            }
            else
            {
                if(line.find("[HUB_VID_LIST]") == 0)
                {
                    bINIHubVID = TRUE;
                }
                else if(line.find("[HFC_PID_LIST]") == 0)
                {
                    bINIHfcPID = TRUE;
                }
                int pos_equal = line.find("=");
                strKey = line.substr(0, pos_equal);
                strValue = line.substr(pos_equal + 1, line.length());
                if((TRUE == bINIHubVID))
                {
                    if(!strncmp(strKey.c_str(),"HUB_VID", 7))
                    {
                        if(giHubVidCnt > 5)
                        {
                            Logger ("Reached Maximum Hub VID List. Only first 5 Values will be considered");
                            continue;
                        }
                        else
                        {
                            sscanf(strValue.c_str(), "%x", (unsigned int*)&gwHUBVID[giHubVidCnt]);
                            giHubVidCnt++;
                        }
                        
                    }
                }
                if((TRUE == bINIHfcPID))
                {
                    if(!strncmp(strKey.c_str(),"HFC_PID", 7))
                    {
                        if(giHfcPidCnt > 31)
                        {
                            Logger ("Reached Maximum Hfc PID List. Only first 15 Values will be considered");
                            continue;
                        }
                        else
                        {
                            sscanf(strValue.c_str(), "%x",(unsigned int*)&gwHFCPID[giHfcPidCnt]);
                            giHfcPidCnt++;
                        }
                        
                    }
                }
            }
        }
        bReturn = TRUE;
    }
    else
    {
        Logger("Failed to read Configuration file: %s", INIFile);
        bReturn = FALSE;
    }
    return bReturn;
}

INT mchp_usb_get_all_hubs(char *pHubInfo)
{
    INT iMchpHubCnt = 0;
    char pchTemp[2048];
    *pHubInfo = '\0';   
    iMchpHubCnt  = usb_get_hub_list (&gasHubInfo[0]);
    giHubMaxCnt = iMchpHubCnt;
    Logger("%s: Number of MCHP USB Hubs Detected: %d", __FUNCTION__, iMchpHubCnt);
    
    for(int i=0; i< iMchpHubCnt; i++)
    {
    	sprintf (pchTemp,"\"Index\":\"%d\",\"PortChain\":\"%s\",\"VID\":\"%04X\",\"PID\":\"%04X\"", i,gasHubInfo[i].PortChain,gasHubInfo[i].wVID, gasHubInfo[i].wPID);
        Logger("%s", pchTemp);
        strcat(pHubInfo,pchTemp);
    }
    return iMchpHubCnt;
}

HANDLE mchp_usb_open(BYTE byHubIndex){
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bReturn = FALSE;

    if((giHubMaxCnt <= 0) || (byHubIndex >= giHubMaxCnt))
    {
        Logger ("%s Failed: Invalid Hub Index %d, Total Hubs found: %d", __FUNCTION__, byHubIndex, giHubMaxCnt);
        return hDevice;
    }
    // Get HFC Device Handle of the Hub
    Logger ("%s Get HFC Device Handle for Hub Index %d", __FUNCTION__, byHubIndex);
    bReturn = usb_open_HFC_device(byHubIndex);
    if(!bReturn)
    {
        Logger ("%s Enable HFC Device for Hub Index %d", __FUNCTION__, byHubIndex);
        bReturn = usb_enable_HFC_device(byHubIndex);
        if(bReturn)
        {
            sleep(2);
            // Now try to get HFC Handle
            bReturn = usb_open_HFC_device(byHubIndex);
        }
        
    }

    if(bReturn)
    {
        Logger ("%s: Success", __FUNCTION__);
        // Now we have HFC Handle
        // Get Hub related details
        // ToDo
        hDevice = (HANDLE)byHubIndex;
    }
    else{
        Logger ("%s: Fail", __FUNCTION__);
        hDevice = INVALID_HANDLE_VALUE;
    }
    return hDevice;
}

BOOL mchp_usb_close(HANDLE hDevice)
{
    BOOL bReturn = FALSE;
    if((giHubMaxCnt <= 0) || (hDevice >= giHubMaxCnt))
    {
        Logger ("%s Failed: Invalid Hub Index %d, Total Hubs found: %d", __FUNCTION__, hDevice, giHubMaxCnt);
        return bReturn;
    }
    else
    {
        usb_close (hDevice);
        bReturn = TRUE;
    }
    return bReturn;
}

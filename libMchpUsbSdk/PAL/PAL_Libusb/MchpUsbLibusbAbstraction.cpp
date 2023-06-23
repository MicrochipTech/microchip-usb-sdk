#include <iostream>
#include <libusb.h>
#include <cstring>
#include "MchpUsbLibusbAbstraction.h"

extern WORD gwHUBVID[6];
extern WORD gwHFCPID[32];
extern INT giHubVidCnt;
extern INT giHfcPidCnt;

/* Context Variable used for initializing LibUSB session */
libusb_context *ctx = NULL; 

INT usb_get_hub_list(PHINFO pHubInfoList)
{
    libusb_device **devices;
    libusb_device *dev;
	libusb_device_descriptor desc;

    ssize_t devCnt = 0, mchpUsbHubCnt = 0;
    INT dRetVal = 0;
    char chHubPath[256], pchTemp[256];

    dRetVal = libusb_init(&ctx);
    if(dRetVal < 0)
    {
        Logger("%s: Failed to Initialise LibUSB", __FUNCTION__);
        return -1;
    }
    
    devCnt = libusb_get_device_list(ctx, &devices);
    
    for (int i=0; i < devCnt; i++)
    {
        dev = devices[i];
        dRetVal = libusb_get_device_descriptor(dev, &desc);
        if(LIBUSB_CLASS_HUB == desc.bDeviceClass)
        {  
            for(int iIniVID = 0; iIniVID < giHubVidCnt; iIniVID++)
            {
                // check if Hub VID matches MCHP VID list
                if(gwHUBVID[iIniVID] == desc.idVendor)
                {
                    pHubInfoList->port_max = libusb_get_port_numbers(dev, pHubInfoList->port_list, 7);
                    memset(chHubPath, 0, 50);
                    if (pHubInfoList->port_max  > 0) {
                        sprintf(chHubPath, "%d", pHubInfoList->port_list[0]); 
                
                        for (int j = 1; j < pHubInfoList->port_max ; j++)
                        {
                            sprintf(pchTemp, "-%d",pHubInfoList->port_list[j]);
                            strcat(chHubPath, pchTemp);
                        }
                       
                    }
                    pHubInfoList->byHubIndex 	= mchpUsbHubCnt;
                    pHubInfoList->wVID	 	= desc.idVendor;
		            pHubInfoList->wPID 		= desc.idProduct;
		            strcpy(pHubInfoList->PortChain, chHubPath);
			
                    pHubInfoList++; 
                    mchpUsbHubCnt++;
                    break;
		    
                } 
            }
        }

    }
    libusb_free_device_list(devices, 1);
    return mchpUsbHubCnt;
}

BOOL usb_open_HFC_device(BYTE byHubIndex)
{
    BOOL bReturn = FALSE;
    libusb_device_handle *handle = NULL;
    libusb_device **devices;
    libusb_device *dev;
	libusb_device_descriptor desc;

    ssize_t devCnt = 0;
    INT dRetVal = 0, port_cnt =0;
    BYTE byPath[8];
    char chHubPath[256], pchTemp[256];
    BOOL bHFCMatch = FALSE, bHFCOpen = FALSE;

    dRetVal = libusb_init(&ctx);
    if(dRetVal < 0)
    {
        Logger("Failed to Initialize LibUSB");
        return bReturn;
    }
    
    devCnt = libusb_get_device_list(ctx, &devices);
    
    for (int i=0; i < devCnt; i++)
    {
        dev = devices[i];
        dRetVal = libusb_get_device_descriptor(dev, &desc);

        // We want HFC Device, so now compare desc.PID with list of supported PID in our .ini
        for (int j=0; j< giHfcPidCnt; j++)
        {
            if(gwHFCPID[j] == desc.idProduct)
            {
                port_cnt = libusb_get_port_numbers(dev, byPath, sizeof(byPath));

                if(port_cnt <= 1)
                {
                    Logger("Failed to get port number\n");
                    continue;
                }
                if(gasHubInfo[byHubIndex].port_max != (port_cnt -1))
                {
                    continue;
                }

                for(int k=0; k < gasHubInfo[byHubIndex].port_max; k++)
                {
                    if(gasHubInfo[byHubIndex].port_list[k] != byPath[k])
                    {
                        bHFCMatch = FALSE;
                        break;
                    }
                    bHFCMatch = TRUE;
                }
                if(bHFCMatch)
                {
                    gasHubInfo[byHubIndex].wHFCVID = desc.idVendor;
                    gasHubInfo[byHubIndex].wHFCPID = desc.idProduct;

                    memset(chHubPath, 0, 50);

                    if (port_cnt  > 0) {
                        sprintf(chHubPath, "%d", byPath[0]); 
                
                        for (int j = 1; j < port_cnt ; j++)
                        {
                            sprintf(pchTemp, "-%d",byPath[j]);
                            strcat(chHubPath, pchTemp);
                        }
                       
                    }
                    strcpy(gasHubInfo[byHubIndex].HFCPortChain, chHubPath);

                    // get Handle
                    dRetVal = libusb_open(dev, &handle);
                    if(dRetVal < 0)
                    {
                        Logger("Failed to Get Device Handle");
                        bHFCOpen = FALSE;
                    }
                    else
                    {
                        gasHubInfo[byHubIndex].hHFCHandle = handle;
                        bHFCOpen = TRUE;
                        Logger("Get Device Handle Success");
                        break;
                    }
                }
            }
        }
    }
    libusb_free_device_list(devices, 1);
    return bHFCOpen;
}

BOOL usb_enable_HFC_device(BYTE byHubIndex)
{
    BOOL bReturn = FALSE;
    libusb_device_handle *handle = NULL;
    libusb_device **devices;
    libusb_device *dev;
	libusb_device_descriptor desc;

    ssize_t devCnt = 0;
    INT dRetVal = 0, port_cnt =0;
    BYTE byPath[8];
    BOOL bHubMatch = FALSE;

    dRetVal = libusb_init(&ctx);
    if(dRetVal < 0)
    {
        Logger("Failed to Initialize Libusb");
        return bReturn;
    }
    
    devCnt = libusb_get_device_list(ctx, &devices);
    
    for (int i=0; i < devCnt; i++)
    {
        dev = devices[i];
        dRetVal = libusb_get_device_descriptor(dev, &desc);
        if((desc.idVendor == gasHubInfo[byHubIndex].wVID) && \
        (desc.idProduct == gasHubInfo[byHubIndex].wPID))
        {
            port_cnt = libusb_get_port_numbers(dev, byPath, sizeof(byPath));

            if(port_cnt <= 1)
            {
                Logger ("Failed to get port number");
                continue;
            }
            if(gasHubInfo[byHubIndex].port_max != (port_cnt))
            {
                continue;
            }

            for(int k=0; k < gasHubInfo[byHubIndex].port_max; k++)
            {
                if(gasHubInfo[byHubIndex].port_list[k] != byPath[k])
                {
                    bHubMatch = FALSE;
                    break;
                }
                bHubMatch = TRUE;
            }

            if(bHubMatch)
            {
                // get Hub handle and send command to enable HFC
                dRetVal = libusb_open(dev, &handle);
                if(dRetVal < 0)
                {
                    Logger("Failed to get Hub Handle\n");
                    return FALSE;
                }
                gasHubInfo[byHubIndex].hHubHandle = handle;

                WORD wVal = 0x0001;
                dRetVal = usb_send_vsm_command (byHubIndex, (BYTE*)&wVal);
                if(dRetVal < 0)
                {
                    Logger ("Failed to send VSM Command to Hub at Index: %d", byHubIndex);
                    return bReturn;
                }

                wVal = 0x0201;
                dRetVal = usb_send_vsm_command (byHubIndex, (BYTE*)&wVal);

                if(dRetVal < 0)
                {
                    Logger ("Failed to send VSM Command to Hub at Index: %d", byHubIndex);
                    return bReturn;
                }
                bReturn = TRUE;
                Logger ("Enable HFC Success for Hub at Index: %d", byHubIndex);
                break;
            }

        }
    }
    libusb_free_device_list(devices, 1);
    return bReturn;
}

INT usb_send_vsm_command(HANDLE handle, BYTE *byValue)
{
    INT dRetVal;

    dRetVal = libusb_control_transfer ( (libusb_device_handle*)gasHubInfo[handle].hHubHandle,
        (LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE),
        WRITE_MEMORY,
        0,
        0,
        byValue,
        2,
        CTRL_TIMEOUT
    );
    return dRetVal;
}

void usb_close(HANDLE hDevice)
{
    if(gasHubInfo[hDevice].hHFCHandle != NULL)
    {
        libusb_close((libusb_device_handle*) gasHubInfo[hDevice].hHFCHandle);
        gasHubInfo[hDevice].hHFCHandle = NULL;
    }
    if(gasHubInfo[hDevice].hHubHandle != NULL)
    {
        libusb_close((libusb_device_handle*) gasHubInfo[hDevice].hHubHandle);
        gasHubInfo[hDevice].hHFCHandle = NULL;
    }
}
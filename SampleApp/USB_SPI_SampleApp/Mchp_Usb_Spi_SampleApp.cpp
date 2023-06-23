#include <iostream>
#include "../../libMchpUsbSdk/Mchp_Usb_Api.h"

#define INVALID_HANDLE_VALUE	0xFF

int main(int argc, char* argv[])
{
    CHAR sztext[2048];
    BYTE byHubIndex = 0;
    int iMchpHubCnt = 0;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    std::string strMchpUsbSdkConfigfile = "MchpUsbSdkConfig.ini";

    printf("USB SPI Sample Application\n");
    // Todo Sample App will be update when implementing SPI Api
    /*if(((0 == strcmp(argv[1],"--help"))) || (0 == strcmp(argv[1],"--help")))
    {
        printf("Operation: Program SPI Firmware\n");
        printf("Usage: ./usbSpiSampleApp /pspi <FirmwareFile.bin> /id <HubId>\n");
        printf("Example: ./usbSpiSampleApp /pspi USB58xx_SPI_FW.bin /id 0\n\n");
        return -1;
    }
    else
    {
        
    }*/

    // Enable Logging
    MchpEnableLogging(1);

    // Get Library Version
    MchpUsbGetVersion(sztext);


    if(FALSE == MchpSetConfigFilePath((PCHAR)strMchpUsbSdkConfigfile.c_str()))
    {
        printf ("Error accessing File: %s\n", strMchpUsbSdkConfigfile.c_str());
    }

    // Get List of MCHP USB HUbs Connected 
    iMchpHubCnt = MchpUsbGetAllHubs(sztext);
    if(0 == iMchpHubCnt)
    {
        printf ("Error: Zero MCHP USB Hubs found\n");
        return EXIT_FAILURE;
    }
    else
    {
        printf ("Number of MCHP USB Hubs Found: %d\n", iMchpHubCnt );
        printf ("%s\n", sztext);
    }
    
    // Get Device Handle
    hDevice = MchpUSBOpen(byHubIndex);
    if(INVALID_HANDLE_VALUE == hDevice)
    {
        printf ("Error: MchpUSBOpen Failed for Hub Index %d\n", byHubIndex);
        return EXIT_FAILURE;
    }
    else
    {
        printf ("MchpUSBOpen Success for Hub Index %d\n", byHubIndex);
    }

    // SPI Operation 
    // Todo



    return 0;
}

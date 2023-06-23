#pragma once

#include "typedef.h"
#include <vector>
#include <string>


#define INVALID_HANDLE_VALUE	0xFF
#define MAX_HUBS	20
#define WRITE_MEMORY	0x02


//enum logging_level {info, warning, error, logDebug}

typedef struct tagHubInfo
{
    WORD	wVID;			/* USB Vendor ID of the hub */
	WORD	wPID;			/* USB Product ID of the hub */
 	WORD	wDID;           /* USB device ID of the hub */
 	BYTE    byHubIndex;
    BYTE 	port_list[7];
	BYTE 	port_max;
	char	PortChain[256];

	WORD wHFCVID;
	WORD wHFCPID;
	char HFCPortChain[256];

	void* hHubHandle;
	void* hHFCHandle;
}HINFO, DEVICE_INFO, *PHINFO, *PDEVICE_INFO;

typedef struct _USB_SETUP_PACKET {
	BYTE byRequest;
	WORD wValue;
	WORD wIndex;
	BYTE* byBuffer;
	WORD wLength;
}USB_SETUP_PACKET, *PUSB_SETUP_PACKET;

extern HINFO gasHubInfo [MAX_HUBS];

void createLogFile ();
void Logger (const char * format, ...) ;
BOOL mchp_read_config_file(PCHAR INIFile);
INT mchp_usb_get_all_hubs(char *HubInfo);
HANDLE mchp_usb_open(BYTE byHubIndex);
BOOL mchp_usb_close(HANDLE hDevice);

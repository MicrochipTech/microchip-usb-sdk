#pragma once

#include "../../typedef.h"
#include "../../MchpUsbInterface.h"

#define CTRL_TIMEOUT    (5 * 1000) /* milliseconds */

INT usb_get_hub_list(PHINFO pHubInfoList);
BOOL usb_open_HFC_device(BYTE byHubIndex);
BOOL usb_enable_HFC_device(BYTE byHubIndex);
INT usb_send_vsm_command(HANDLE handle, BYTE *byValue);
void usb_close(HANDLE hDevice);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>	// required by usbdrv.h
#include <util/delay.h>

#include <stdio.h>

#include "hw_setup.h"
#include "HID_Joystick.h"
#include "usbdrv.h"
#include "usbconfig.h"
#include "neRX.h"
#include "utils.h"

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] =
{
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x34,                    //     USAGE (Ry)
    0x09, 0x33,                    //     USAGE (Rx)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x81, 0x82,                    //     INPUT (Data,Var,Abs,Vol)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

// this is the data buffer that is sent through USB to the OS
typedef struct 
{
    uint8_t	pitch;
    uint8_t	rudder;
    uint8_t	elevator;
    uint8_t	aileron;
} HID_report_t;

HID_report_t reportBuffer;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	static uchar idleRate;   // repeat rate for keyboards
	usbRequest_t* rq = (void*) data;

    // The following requests are never used. But since they are required by
    // the specification, we implement them in this example.

    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS)
	{   
		// class request type
        if (rq->bRequest == USBRQ_HID_GET_REPORT)
		{	
			// wValue: ReportType (highbyte), ReportID (lowbyte)
            // we only have one report type, so don't look at wValue
            usbMsgPtr = (void*) &reportBuffer;
            return sizeof(reportBuffer);
        } else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
            usbMsgPtr = &idleRate;
            return 1;
        } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
            idleRate = rq->wValue.bytes[1];
        }
    } else {
        // no vendor specific requests implemented
    }

    return 0;	// default for not implemented requests: return no data back to host
}

// run device as HID Joystick
void HID_Joystick()
{
	// init v-usb
    usbInit();
    usbDeviceDisconnect();	// enforce re-enumeration, do this while interrupts are disabled!
    int i = 260;
    while (--i)				// fake USB disconnect for > 250 ms
        _delay_ms(1);

    usbDeviceConnect();

	sei();

	// init the RX
	RX_Init();
	
	for (;;)
	{
        usbPoll();

        if (usbInterruptIsReady())
		{
            // this sends the data to V-USB
            usbSetInterrupt((void*) &reportBuffer, sizeof(reportBuffer));
		}

		// do we have a data packet?
		if (RX_ReadSticks())
		{
			// copy the data into the report buffer
			reportBuffer.pitch		= neSticks.pitch;
			reportBuffer.rudder		= neSticks.rudder;
			reportBuffer.elevator	= neSticks.elevator;
			reportBuffer.aileron	= neSticks.aileron;

			// the channel data
			//printf("%02x %02x %02x %02x\n", neSticks.pitch, neSticks.rudder, neSticks.elevator, neSticks.aileron);
			
			// binding data
			//printf("%02x %02x %02x\n", nRF_data[5], nRF_data[6], nRF_data[7]);
		}
	}
}

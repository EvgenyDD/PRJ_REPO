I wanted a have a TX to control an RC simulator and I only have a Nine Eagles Solo Pro transmitter. So I decided to make a receiver that will read the stick position information sent out by the SoloPro TX, and pass that over to a simulator throught either USB or PPM.

The device draws power from USB and has two modes of operation which are controller by the PPM/HID jumper.

HID Joystick

If the PPM/HID jumper is open when powering up the device or after reset, the device will run in HID joystick mode. It introduces itself to the operating system as a standard HID Joystick. It doesn't need any drivers since Windows/Linux/MacOS already have the necessary drivers for HID joysticks. However, you have to go through the joystick calibration procedure of the OS before using the device. NOTE: remember to do the joystick calibration in double rates (full circle) on the TX!!!

I have tested this mode with ClearView and Heli-X and it works as expected. I have also tried FMS, but for some misterious reason FMS does not recognize one of the channels during calibration.


PPM mode

If the PPM/HID jumper is closed after power up or reset, the device will run in PPM mode. In this mode neRecv receives data from the TX and converts it to PPM. The USB connection will not send or receive any data but the device still needs to be plugged in USB for power. The generated PPM signal is sent out through a 3.5mm jack. The voltage of the signal is limited with a 3.6V zenner diode. The PPM frame duration is 22ms, with the 4 channels mapped to pitch, rudder, elevator and aileron in that order. The duration for each channel is the standard 1-2ms with a 0.3 ms pulse. neRecv can generate both positive and negative PPM. This is configured in the firmware with the NEGATIVE_PPM preprocessor definition.

I have tested the PPM mode with Phoenix RC and RealFlight G5. It works.


Hardware

The hardware is built around a 28 pin AVR micro controller running at 12MHz. I use an ATmega88, but it should also work with ATmega48, ATmega168 or ATmega328. The device uses the SparkFun nRF24L01+ module for communicating with the TX. I have used the version with the chip antenna http://www.sparkfun.com/products/691. The version with the duck antenna can also be used and should have a better range. http://www.sparkfun.com/products/705 However, this extra range is not really necessary since you are not likely to stand more than 10 meters from the PC and the receiver.

The USB connection in HID joystick mode is implemented with V-USB.

The device has 3 LEDs. One is the power indicator, the other indicates that the device is receiving data from the TX. The third LED is not currently used. This could change in a future update. For instance it could be a bootloader indicator.

One jumper is for PPM/HID mode selection and the other is not used. It could be used in future updates to activate a bootloader.

Please note that since version 1.0 I have rotated the orienatation of the SparkFun module to simplify PCB routing. I have also changed the AVR programming interface from 6 pins to 10 pins. I have also added a reset push button.

The design also contains a 4 pin serial interface that I used for debugging. This is not needed for normal operation of the device.


The TX protocol

The TX sends data out in 7 byte packets on address 0x3443101001 about 110 times each second. Is uses the 250kbps data rate and it cycles through channels 10, 30 and 50. The data packet can be represented with the following C structure:

typedef struct
{
    // stick position. the range of these is 0 - 0x7f
    uint8_t pitch;
    uint8_t rudder;
    uint8_t elevator;
    uint8_t aileron;
	
    uint8_t     packet_type;    // 0x55 for sticks data
	
    uint16_t    tx_id;          // id of the TX
} nePacket_t;

The first 4 bytes are pitch, rudder, elevator and aileron respectively. The TX uses the 7 least significant bits of each byte to send out the data.

The next byte contains the packet type. This is 0x55 for normal stick position. The last 2 bytes contain the ID of the TX. This is used for binding to the heli receiver. Binding the TX to the neRecv device is done automatically by the firmware. neRecv will 'bind' to the first TX it sees after power up and listen to only this TX until reset. The signal from any other TX received after this will be ignored.

neRecv should work with any NE Solo Pro or compatible TX. I have tested with a standard Nine Eagle TX and a TX that comes with the rebranded Merlin Tracer 90.


The firmware

Brief description of the source code files:

neRecv.c

This file contains the main() function. In turns on the power LED, reads the status of the PPM/HID jumper and branches into funtions PPM_Output() or HID_Joystick() accordingly.

nRF24L01.h and nRF24L01.c

These contain functions and definitions used for communication with the SparkFun module. 

neRX.c and neRX.h

Contains functions RX_Init() and RX_ReadSticks(). The first puts the SparkFun module in receive mode. The seconds reads the data from the last received packet into the neSticks structure. It returs 0 if no data was available.

HID_Joystick.c and HID_Joystick.h

Contains the HID_Joystick() function which is called by main(). It configures the USB connection and runs neRecv in HID joystick mode.

PPM_Output.c and PPM_Output.h

Contains the PPM_Output() function which is called by main(). It configures and runs neRecv in PPM output mode.

avr_serial.c and avr_serial.h

Function init_serial() initializes the serial port of the AVR micro to 57600 BAUD. This allows debug output with the printf() function. These are not necessary for the normal functioning of the device.


Although neRecv works perfectly for me, I'm sure that people with more experience can spot flaws in the hardware design (or maybe even firmware). I appreciate any advice you might have about things that could be improved. Come to think of it, my main motivation to build this was to learn about electronics and microcontrollers. Flying the virtual helicopters is just an added bonus. :)

I have attached the EAGLE schematics, the PCB board and the firmware source code.


Enjoy!
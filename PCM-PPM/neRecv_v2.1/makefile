# change the next line for a different AVR microcontroller.
# this could be ATmega88, ATmega168 and ATmega328p
DEVICE  = atmega88

F_CPU   = 12000000

# Fuses for ATmega88 and ATmega168:
# high     0xDF
# low      0xDF
# extended 0xF9

# Fuses for ATmega328p:
# high     0xDF
# low      0xDF
# extended 0xFF

# change the next line to suit your programmer
AVRDUDE = avrdude -c stk500 -P com4 -p $(DEVICE)

CFLAGS  = -I. -Iusbdrv -DDEBUG_LEVEL=0
OBJECTS = avr_serial.o neRecv.o nRF24L01.o neRX.o PPM_Output.o HID_Joystick.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

neRecv.hex: $(OBJECTS)
	$(COMPILE) -o neRecv.elf $(OBJECTS)
	rm -f neRecv.hex neRecv.eep.hex
	avr-objcopy -j .text -j .data -O ihex neRecv.elf neRecv.hex
	avr-size neRecv.hex

flash: neRecv.hex
	$(AVRDUDE) -U flash:w:neRecv.hex:i

clean:
	rm -f *.hex *.elf $(OBJECTS)

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.o:
	$(COMPILE) -c $< -o $@

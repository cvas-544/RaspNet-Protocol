#Configuring the Makefile
 
TARGET=main/main
SOURCES_DIR=Source/
OUTFILE=main/main
MMCU=atmega328p
F_CPU=12000000L
AVRDUDE_PORT=/dev/ttyAMA0
 
#Compiling the C script with avr-gcc.
 
AVRDUDE = avrdude
CC = avr-gcc
OBJCOPY = avr-objcopy
ARVCONFIG = $(CC) -g -Os -mmcu=$(MMCU) -DF_CPU=12000000L -c
 
all:build link copy flash clean

build:
	$(AVRCONFIG) $(TARGET).c -o $(OUTFILE).o
	$(AVRCONFIG) SOURCES_DIR pinConfig.c -o pinConfig.o
	$(AVRCONFIG) SOURCES_DIR CRC/CRC.c -o CRC.o
	$(AVRCONFIG) SOURCES_DIR UART/uart.c -o uart.o
	$(AVRCONFIG) SOURCES_DIR ISQ/InterruptServiceRoutine.c -o InterruptServiceRoutine.o
	$(AVRCONFIG) SOURCES_DIR Layer1/PhysicalLayer.c -o PhysicalLayer.o
	$(AVRCONFIG) SOURCES_DIR Layer2/DataLinkLayer.c -o DataLinkLayer.o
	$(AVRCONFIG) SOURCES_DIR Layer3/NetworkLayer.c -o NetworkLayer.o
   
link:
	$(CC) -mmcu=$(MMCU) -o $(OUTFILE).elf $(OUTFILE).o pinConfig.o CRC.o uart.o InterruptServiceRoutine.o PhysicalLayer.o DataLinkLayer.o NetworkLayer.o

copy:
	$(OBJCOPY) -O ihex -R .eeprom $(OUTFILE)/main.elf $(OUTFILE)/main.hex

flash:
	$(AVRDUDE) -F -V -c gpio -p $(MMCU) -P $(AVRDUDE_PORT) -b 9600 -U flash:w:$(OUTFILE)/main.hex

clean:
	rm $(OUTFILE).elf $(OUTFILE).hex $(OUTFILE).o PhysicalLayer.o DataLinkLayer.o NetworkLayer.o CRC.o uart.o InterruptServiceRoutine.o pinConfig.o

doc:
	doxygen DoxyConfig

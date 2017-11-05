MCU = attiny841
AVRDUDE_MCU = t841

SERIAL = /dev/ttyUSB0
CONF = ~/.arduino15/packages/ATTinyCore/hardware/avr/1.1.4/avrdude.conf

CFLAGS = -g -Os -mmcu=${MCU}
LDFLAGS = -g -mmcu=${MCU}

OUTPUT = lasertag
SOURCES != find . -name '*.c'
OBJECTS = ${SOURCES:.c=.o}

${OUTPUT}.hex: ${OUTPUT}.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

${OUTPUT}.elf: ${OBJECTS}
	avr-gcc ${LDFLAGS} -o $@ $^

include ${SOURCES:.c=.deps}

%.deps: %.c
	@set -e; rm -f $@; \
	avr-gcc -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.c
	avr-gcc ${CFLAGS} -o $@ -c $<

all: ${OUTPUT}.hex

flash:
	avrdude -c buspirate -C ${CONF} -P ${SERIAL} -p${AVRDUDE_MCU} -U flash:w:${OUTPUT}.hex

clean:
	@rm -f ${OUTPUT}.hex
	@rm -f ${OUTPUT}.elf
	@rm -f *.deps
	@rm -f *.deps.*
	@rm -f *.o

.PHONY: clean flash

NAME = pebble-striangle

BIN = build/$(NAME).pbw

default: quick install

quick: pebble-striangle.c.o

pebble-striangle.c.o: src/pebble-striangle.c
	~/pebble-dev/PebbleSDK-3.0-dp6/arm-cs-tools/bin/arm-none-eabi-gcc -std=c99 -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -g -Os -D_TIME_H_ -Wall -Wextra -Werror -Wno-unused-parameter -Wno-error=unused-function -Wno-error=unused-variable -fPIE -I/home/cel/pebble-dev/pebble-striangle/PebbleSDK-3.0-dp6/Pebble/basalt/include -I/home/cel/pebble-dev/PebbleSDK-3.0-dp6/Pebble/basalt/include -I/home/cel/pebble-dev/pebble-striangle/build -I/home/cel/pebble-dev/pebble-striangle -I/home/cel/pebble-dev/pebble-striangle/build/src -I/home/cel/pebble-dev/pebble-striangle/src -I/home/cel/pebble-dev/pebble-striangle/build/basalt -I/home/cel/pebble-dev/pebble-striangle/basalt -DRELEASE -DPBL_PLATFORM_BASALT -DPBL_COLOR -D__FILE_NAME__=\"pebble-striangle.c\" $< -c -o $@

all: install

$(BIN): src/pebble-striangle.c
	pebble build

install: $(BIN)
	pebble install

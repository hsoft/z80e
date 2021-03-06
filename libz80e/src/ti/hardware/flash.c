#include "ti/asic.h"
#include "log/log.h"
#include "ti/memory.h"
#include "ti/hardware/flash.h"

#include <stdlib.h>
#include <string.h>

uint8_t read_mirror(void *device) {
	flash_state_t *state = device;
	z80iodevice_t port4 = state->asic->cpu->devices[0x04];
	return port4.read_in(port4.device);
}

uint8_t read_zero(void *device) {
	return 0;
}

void write_control_port(void *device, uint8_t data) {
	flash_state_t *state = device;
	// TODO: check permissions
	state->asic->mmu->flash_unlocked = data & 1;
}

void write_exclusion_port(void *device, uint8_t data) {
	flash_state_t *state = device;
	// TODO: implement
}

uint8_t read_size_port(void *device) {
	flash_state_t *state = device;
	return state->chip_size;
}

void write_size_port(void *device, uint8_t data) {
	flash_state_t *state = device;
	state->chip_size = data & 0x33;
}

void init_flash_ports(asic_t *asic) {
	flash_state_t *state = malloc(sizeof(flash_state_t));

	memset(state, 0, sizeof(flash_state_t));
	state->asic = asic;
	state->chip_size = 0x33;

	uint8_t (*read_port)(void *) = asic->device == TI83p ? read_mirror : read_zero;
	z80iodevice_t chip_control_port = { state, read_port, write_control_port };
	z80iodevice_t chip_exclusion_port = { state, read_port, write_exclusion_port };
	z80iodevice_t chip_size_port = { state, read_size_port, write_size_port };

	asic->cpu->devices[0x14] = chip_control_port;
	asic->cpu->devices[0x21] = chip_size_port;
}

void free_flash_ports(asic_t *asic) {
	free(asic->cpu->devices[0x22].device);
}

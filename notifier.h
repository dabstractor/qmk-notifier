#pragma once

typedef void (*callback_t)(void);

typedef struct {
    const char *command;
    callback_t callback;
} command_map_t;

#define SERIAL_COMMAND_MAP(...) \
    command_map_t command_map[] = __VA_ARGS__; \
    const size_t command_map_size = sizeof(command_map) / sizeof(command_map[0])

extern command_map_t command_map[];
extern const size_t command_map_size;

// From QMK
void raw_hid_receive(uint8_t* data, uint8_t length);

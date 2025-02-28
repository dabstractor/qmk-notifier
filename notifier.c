#include "notifier.h"
#include "print.h"
#include <string.h>

typedef void (*callback_t)(void);

#define COMMAND_MAP_SIZE (sizeof(command_map) / sizeof(command_map[0]))

void raw_hid_receive(uint8_t* data, uint8_t length) {
    char received_command[64] = {0};
    if (length < sizeof(received_command)) {
        memcpy(received_command, data, length);
        received_command[length] = '\0';
        #ifdef CONSOLE_ENABLE
        bool found = false;
        #endif

        for (uint8_t i = 0; i < command_map_size; i++) {
            if (strcmp(received_command, command_map[i].command) == 0) {
                #ifdef CONSOLE_ENABLE
                found = true;
                #endif

                command_map[i].callback();
                break;
            }
        }

        #ifdef CONSOLE_ENABLE
        if (found) {
            uprintf("Received command: %s\n", received_command);
        } else {
            uprintf("Unknown command: %s\n", received_command);
        }
        #endif
    }
}


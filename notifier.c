#include QMK_KEYBOARD_H
#include "pattern_match.c"
#include "notifier.h"
#include "print.h"
#include <string.h>

// External command_map defined by the user with SERIAL_COMMAND_MAP macro
extern command_map_t command_map[];
extern const size_t command_map_size;

#define LAYER_UNSET 255
uint8_t activated_layer = LAYER_UNSET;

// reference to currently active command:
command_map_t *current_command = {0};

void activate_layer(uint8_t layer) {
    #ifdef CONSOLE_ENABLE
    uprintf("Activating layer %d\n", layer);
    #endif
    layer_on(layer);
    activated_layer = layer;
}

void deactivate_layer(void) {
    if (activated_layer == LAYER_UNSET) {
        return;
    }

    #ifdef CONSOLE_ENABLE
    uprintf("Deactivating layer %d\n", activated_layer);
    #endif

    layer_off(activated_layer);
    activated_layer = LAYER_UNSET;
}

void deactivate_all_commands(void) {
}

void enable_command(command_map_t *command) {
    current_command = command;
    command->on_enable();
}

void disable_command(void) {
    if (current_command != NULL && current_command->on_disable != NULL) {
        current_command->on_disable();
    }

    current_command = NULL;
}

void raw_hid_receive(uint8_t* data, uint8_t length) {
    char received_command[64] = {0};
    if (length < sizeof(received_command)) {
        memcpy(received_command, data, length);
        received_command[length] = '\0';

        #ifdef CONSOLE_ENABLE
        bool found_command = false;
        bool found_layer = false;
        #endif

        disable_command();

        for (uint8_t i = 0; i < command_map_size; i++) {
            if (pattern_match(command_map[i].pattern, received_command)) {
                #ifdef CONSOLE_ENABLE
                found_command = true;
                #endif
                enable_command(&command_map[i]);
                break;
            }
        }

        for (uint8_t i = 0; i < layer_map_size; i++) {
            if (pattern_match(layer_map[i].pattern, received_command)) {
                #ifdef CONSOLE_ENABLE
                found_layer = true;
                #endif

                if (!layer_state_is(layer_map[i].layer)) {
                    activate_layer(layer_map[i].layer);
                }
                break;
            }
        }

        if(!found_command) {
            deactivate_all_commands();
        }

        if (!(found_command || found_layer)) {
            deactivate_layer();
        }

        #ifdef CONSOLE_ENABLE
        if (found_command) {
            uprintf("Received command: %s\n", received_command);
        } else {
            uprintf("Unknown command: %s\n", received_command);
        }

        if (found_layer) {
            uprintf("Enabling layer for %s\n", received_command);
        }
        #endif
    }
}

#include QMK_KEYBOARD_H
#include "pattern_match.c"
#include "notifier.h"
#include "raw_hid.h"
#include "print.h"
#include <string.h>

    // Maximum size for the assembled command.
#define MSG_BUFFER_SIZE 256

    // Buffer to accumulate incoming data.
    static char msg_buffer[MSG_BUFFER_SIZE];
    // Current write index into the buffer.
    static uint16_t msg_index = 0;

    // Default empty maps if user doesn't define them
#if !defined(COMMAND_MAP_DEFINED)
    command_map_t command_map[1] = {0};
    const size_t command_map_size = 0;
#endif

#if !defined(CUSTOM_LAYER_MAP)
    layer_map_t layer_map[1] = {0};
    const size_t layer_map_size = 0;
#endif

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


    bool process_full_message(char *data) {

        }

        char received_command[256] = {0};
        int length = strlen(data);
        if (length < sizeof(received_command)) {
            memcpy(received_command, data, length);
            received_command[length] = '\0';

            bool found_command = false;
            bool found_layer = false;

            disable_command();

            for (uint8_t i = 0; i < command_map_size; i++) {
                if (pattern_match(command_map[i].pattern, received_command, command_map[i].case_sensitive)) {
                    found_command = true;
                    enable_command(&command_map[i]);
                    break;
                }
            }

            for (uint8_t i = 0; i < layer_map_size; i++) {
                if (pattern_match(layer_map[i].pattern, received_command, layer_map[i].case_sensitive)) {
                    found_layer = true;

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
                return false;
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

        return true;
    }

    void raw_hid_receive(uint8_t *data, uint8_t length) {
        #define MAX_LOG_SIZE 32

        // Process each byte of the incoming packet.
        bool match = false;

        for (uint8_t i = 0; i < length; i++) {
            char c = (char)data[i];

            // End of text (ASCII 3) indicates the end of the message.
            if (c == ETX_TERMINATOR[0]) {
                msg_buffer[msg_index] = '\0'; // Ensure the buffer is properly terminated
                msg_index = 0; // Reset the buffer for the next message
                match = process_full_message(msg_buffer);
                break;
            } else {
                // Append character if space is available.
                if (msg_index < (MSG_BUFFER_SIZE - 1)) {
                    msg_buffer[msg_index++] = c;
                } else {
                    // Buffer overflow – reset the buffer to prevent corruption.
                    msg_index = 0;
                }
            }
        }

        }
    }



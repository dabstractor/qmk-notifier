#pragma once

typedef void (*callback_t)(void);

typedef struct {
    const char *pattern;
    callback_t on_enable;
    callback_t on_disable;
    const bool case_sensitive;
} command_map_t;

typedef struct {
    const char *pattern;
    // needs to be a normal string
    const int layer;
    const bool case_sensitive;
} layer_map_t;

// Declare external linkage for the maps
extern command_map_t command_map[];
extern const size_t command_map_size;
extern layer_map_t layer_map[];
extern const size_t layer_map_size;

#define GS_DELIMITER "\x1D"  // ASCII 31 (Unit Separator)
#define ETX_TERMINATOR "\x03"  // ASCII 3 (End of Text)

#define WINDOW_TITLE(classname, title) classname GS_DELIMITER title
#define WT(...) WINDOW_TITLE(__VA_ARGS__)

#if !defined(COMMAND_MAP_DEFINED)
#define DEFINE_SERIAL_COMMANDS(...) \
    command_map_t command_map[] = __VA_ARGS__; \
    const size_t command_map_size = sizeof(command_map) / sizeof(command_map[0]);
#define COMMAND_MAP_DEFINED
#else
#define DEFINE_SERIAL_COMMANDS(...) \
    /* Empty expansion, already defined */
#endif

#if !defined(CUSTOM_LAYER_MAP)
#define DEFINE_SERIAL_LAYERS(...) \
    layer_map_t layer_map[] = __VA_ARGS__; \
    const size_t layer_map_size = sizeof(layer_map) / sizeof(layer_map[0]);
#define CUSTOM_LAYER_MAP
#else
#define DEFINE_SERIAL_LAYERS(...) \
    /* Empty expansion, already defined */
#endif

// From QMK
void raw_hid_receive(uint8_t* data, uint8_t length);

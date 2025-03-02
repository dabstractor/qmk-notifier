#pragma once

typedef void (*callback_t)(void);

typedef struct {
    const char *pattern;
    callback_t on_enable;
    callback_t on_disable;
} command_map_t;

typedef struct {
    const char *pattern;
    // needs to be a normal string
    const int layer;
} layer_map_t;

#if !defined(COMMAND_MAP_DEFINED)
#define DEFINE_SERIAL_COMMANDS(...) \
    command_map_t command_map[] = __VA_ARGS__; \
    const size_t command_map_size = sizeof(command_map) / sizeof(command_map[0]); \
    const bool COMMAND_MAP_DEFINED = true
#else
#define DEFINE_SERIAL_COMMANDS(...) \
#endif

#if !defined(LAYER_MAP_DEFINED)
#define DEFINE_SERIAL_LAYERS(...) \
    layer_map_t layer_map[] = __VA_ARGS__; \
    const size_t layer_map_size = sizeof(layer_map) / sizeof(layer_map[0]); \
    const bool LAYER_MAP_DEFINED = true
#else
#define DEFINE_SERIAL_LAYERS(...) \
    /* Empty expansion, already defined */
#endif
#define WINDOW_TITLE(classname, title) classname "\x1F" title
#define WT(...) WINDOW_TITLE(__VA_ARGS__)

extern command_map_t command_map[];
extern const size_t command_map_size;

extern layer_map_t layer_map[];
extern const size_t layer_map_size;

// From QMK
void raw_hid_receive(uint8_t* data, uint8_t length);

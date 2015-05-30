/*
    Copyright (c) 2015
    vurtun <polygone@gmx.net>
    MIT licence
*/
#ifndef GUI_H_
#define GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
#define GUI_UTF_SIZE 4
#define GUI_INPUT_MAX 16
#define GUI_MAX_COLOR_STACK 32
#define GUI_MAX_ATTRIB_STACK 32
#define GUI_UTF_INVALID 0xFFFD
#define GUI_PTR_ADD(t, p, i) ((t*)((void*)((gui_size)(p) + (i))))
#define GUI_PTR_SUB(t, p, i) ((t*)((void*)((gui_size)(p) - (i))))

/* Types */
#ifdef GUI_USE_FIXED_TYPES
#include <stdint.h>
typedef int32_t gui_int;
typedef int32_t gui_bool;
typedef int16_t gui_short;
typedef int64_t gui_long;
typedef float gui_float;
typedef uint16_t gui_ushort;
typedef uint32_t gui_uint;
typedef uint64_t gui_ulong;
typedef uint32_t gui_flags;
typedef uint8_t gui_char;
typedef uint8_t gui_byte;
typedef uint32_t gui_flag;
typedef uint64_t gui_size;
#else
typedef int gui_int;
typedef int gui_bool;
typedef short gui_short;
typedef long gui_long;
typedef float gui_float;
typedef unsigned short gui_ushort;
typedef unsigned int gui_uint;
typedef unsigned long gui_ulong;
typedef unsigned int gui_flags;
typedef unsigned char gui_char;
typedef unsigned char gui_byte;
typedef unsigned int gui_flag;
typedef unsigned long gui_size;
#endif

/* Utilities */
enum {gui_false, gui_true};
enum gui_heading {GUI_UP, GUI_RIGHT, GUI_DOWN, GUI_LEFT};
struct gui_color {gui_byte r,g,b,a;};
struct gui_vec2 {gui_float x,y;};
struct gui_rect {gui_float x,y,w,h;};
struct gui_key {gui_bool down, clicked;};
struct gui_font;

/* Callbacks */
typedef void* gui_image;
typedef gui_char gui_glyph[GUI_UTF_SIZE];
typedef gui_bool(*gui_filter)(gui_long unicode);
typedef gui_size(*gui_text_width_f)(void*, const gui_char*, gui_size);

/*
 * ==============================================================
 *
 *                          Input
 *
 * ===============================================================
 */
enum gui_keys {
    GUI_KEY_SHIFT,
    GUI_KEY_CTRL,
    GUI_KEY_DEL,
    GUI_KEY_ENTER,
    GUI_KEY_BACKSPACE,
    GUI_KEY_ESCAPE,
    GUI_KEY_SPACE,
    GUI_KEY_MAX
};

struct gui_input {
    struct gui_key keys[GUI_KEY_MAX];
    gui_char text[GUI_INPUT_MAX];
    gui_size text_len;
    struct gui_vec2 mouse_pos;
    struct gui_vec2 mouse_prev;
    struct gui_vec2 mouse_delta;
    gui_bool mouse_down;
    gui_uint mouse_clicked;
    struct gui_vec2 mouse_clicked_pos;
};

/* UTF8 */
gui_size gui_utf_decode(const gui_char*, gui_long*, gui_size);
gui_size gui_utf_encode(gui_long, gui_char*, gui_size);

/* Input */
void gui_input_begin(struct gui_input*);
void gui_input_motion(struct gui_input*, gui_int x, gui_int y);
void gui_input_key(struct gui_input*, enum gui_keys, gui_bool down);
void gui_input_button(struct gui_input*, gui_int x, gui_int y, gui_bool down);
void gui_input_char(struct gui_input*, const gui_glyph);
void gui_input_end(struct gui_input*);

/*
 * ==============================================================
 *
 *                          Buffer
 *
 * ===============================================================
 */
struct gui_memory_status {
    void *memory;
    gui_uint type;
    gui_size size;
    gui_size allocated;
    gui_size needed;
    gui_size calls;
};

struct gui_allocator {
    void *userdata;
    void*(*alloc)(void *usr, gui_size);
    void*(*realloc)(void *usr, void*, gui_size);
    void(*free)(void *usr, void*);
};

struct gui_memory {
    void *ptr;
    gui_size size;
};

enum gui_buffer_type {
    GUI_BUFFER_FIXED,
    GUI_BUFFER_DYNAMIC
};

struct gui_buffer {
    struct gui_allocator pool;
    enum gui_buffer_type type;
    struct gui_memory memory;
    gui_float grow_factor;
    gui_size allocated;
    gui_size needed;
    gui_size calls;
};


/* General purpose buffer */
void gui_buffer_init(struct gui_buffer*, const struct gui_allocator*,
                    gui_size initial_size, gui_float grow_factor);
void gui_buffer_init_fixed(struct gui_buffer*, void *memory, gui_size size);
void gui_buffer_info(struct gui_memory_status*, struct gui_buffer*);
void* gui_buffer_alloc(struct gui_buffer*, gui_size size, gui_size align);
void gui_buffer_reset(struct gui_buffer*);
void gui_buffer_clear(struct gui_buffer*);

/*
 * ==============================================================
 *
 *                          Commands
 *
 * ===============================================================
 */
enum gui_command_type {
    GUI_COMMAND_NOP,
    GUI_COMMAND_SCISSOR,
    GUI_COMMAND_LINE,
    GUI_COMMAND_RECT,
    GUI_COMMAND_CIRCLE,
    GUI_COMMAND_TRIANGLE,
    GUI_COMMAND_TEXT,
    GUI_COMMAND_IMAGE,
    GUI_COMMAND_MAX
};

struct gui_command {
    enum gui_command_type type;
    gui_size offset;
};

struct gui_command_scissor {
    struct gui_command header;
    gui_short x, y;
    gui_ushort w, h;
};

struct gui_command_line {
    struct gui_command header;
    gui_short begin[2];
    gui_short end[2];
    struct gui_color color;
};

struct gui_command_rect {
    struct gui_command header;
    gui_short x, y;
    gui_ushort w, h;
    struct gui_color color;
};

struct gui_command_circle {
    struct gui_command header;
    gui_short x, y;
    gui_ushort w, h;
    struct gui_color color;
};

struct gui_command_image {
    struct gui_command header;
    gui_short x, y;
    gui_ushort w, h;
    gui_image img;
};

struct gui_command_triangle {
    struct gui_command header;
    gui_short a[2];
    gui_short b[2];
    gui_short c[2];
    struct gui_color color;
};

struct gui_command_text {
    struct gui_command header;
    void *font;
    gui_short x, y;
    gui_ushort w, h;
    struct gui_color bg;
    struct gui_color fg;
    gui_size length;
    gui_char string[1];
};

typedef struct gui_buffer gui_command_buffer;
#define gui_command_buffer_init(b, a, i, g) gui_buffer_init(b, a, i, g);
#define gui_command_buffer_init_fixed(b, m, s) gui_buffer_init_fixed(b, m ,s)
#define gui_command_buffer_reset(b) gui_buffer_reset(b)
#define gui_command_buffer_clear(b) gui_buffer_clear(b)
void *gui_command_buffer_push(gui_command_buffer*, gui_uint type, gui_size size);
void gui_command_buffer_push_scissor(gui_command_buffer*, gui_float,
                    gui_float, gui_float, gui_float);
void gui_command_buffer_push_line(gui_command_buffer*, gui_float, gui_float,
                    gui_float, gui_float, struct gui_color);
void gui_command_buffer_push_rect(gui_command_buffer *buffer, gui_float x,
                    gui_float y, gui_float w, gui_float h, struct gui_color c);
void gui_command_buffer_push_circle(gui_command_buffer*, gui_float, gui_float,
                    gui_float, gui_float, struct gui_color);
void gui_command_buffer_push_triangle(gui_command_buffer*, gui_float, gui_float,
                    gui_float, gui_float, gui_float, gui_float, struct gui_color);
void gui_command_buffer_push_image(gui_command_buffer*, gui_float,
                    gui_float, gui_float, gui_float, gui_image);
void gui_command_buffer_push_text(gui_command_buffer*, gui_float, gui_float,
                    gui_float, gui_float, const gui_char*, gui_size,
                    const struct gui_font*, struct gui_color, struct gui_color);


#define gui_command(t, c) ((const struct gui_command_##t*)c)
#define gui_command_buffer_begin(b)\
    ((const struct gui_command*)(b)->memory.ptr)
#define gui_command_buffer_end(b)\
    (GUI_PTR_ADD(const struct gui_command, (b)->memory.ptr, (b)->allocated))
#define gui_command_buffer_next(b, c)\
    ((GUI_PTR_ADD(const struct gui_command, c, c->offset) < gui_command_buffer_end(b)) ?\
     GUI_PTR_ADD(const struct gui_command, c, c->offset) : NULL)
#define gui_foreach_command(i, b)\
    for((i) = gui_command_buffer_begin(b); (i) != NULL; (i) = gui_command_buffer_next(b,i))

/*
 * ==============================================================
 *
 *                          Widgets
 *
 * ===============================================================
 */
struct gui_font {
    void *userdata;
    gui_float height;
    gui_text_width_f width;
};

enum gui_text_align {
    GUI_TEXT_LEFT,
    GUI_TEXT_CENTERED,
    GUI_TEXT_RIGHT
};

struct gui_text {
    struct gui_vec2 padding;
    struct gui_color foreground;
    struct gui_color background;
};

enum gui_button_behavior {
    GUI_BUTTON_DEFAULT,
    GUI_BUTTON_REPEATER,
    GUI_BUTTON_MAX
};

struct gui_button {
    gui_float border;
    struct gui_vec2 padding;
    struct gui_color background;
    struct gui_color foreground;
    struct gui_color content;
    struct gui_color highlight;
    struct gui_color highlight_content;
};

enum gui_toggle_type {
    GUI_TOGGLE_CHECK,
    GUI_TOGGLE_OPTION
};

struct gui_toggle {
    struct gui_vec2 padding;
    struct gui_color font;
    struct gui_color background;
    struct gui_color foreground;
    struct gui_color cursor;
};

struct gui_progress {
    struct gui_vec2 padding;
    struct gui_color background;
    struct gui_color foreground;
};

struct gui_slider {
    struct gui_vec2 padding;
    struct gui_color bar;
    struct gui_color border;
    struct gui_color bg;
    struct gui_color fg;
};

struct gui_scroll {
    struct gui_color background;
    struct gui_color foreground;
    struct gui_color border;
};

enum gui_input_filter {
    GUI_INPUT_DEFAULT,
    GUI_INPUT_ASCII,
    GUI_INPUT_FLOAT,
    GUI_INPUT_DEC,
    GUI_INPUT_HEX,
    GUI_INPUT_OCT,
    GUI_INPUT_BIN
};

struct gui_edit {
    struct gui_vec2 padding;
    gui_bool show_cursor;
    struct gui_color cursor;
    struct gui_color background;
    struct gui_color foreground;
};

enum gui_graph_type {
    GUI_GRAPH_LINES,
    GUI_GRAPH_COLUMN,
    GUI_GRAPH_MAX
};

struct gui_graph {
    gui_bool valid;
    enum gui_graph_type type;
    gui_float x, y;
    gui_float w, h;
    gui_float min, max;
    struct gui_vec2 last;
    gui_size index;
    gui_size count;
};

void gui_text(gui_command_buffer*, gui_float x, gui_float y, gui_float w, gui_float h,
                    const char *text, gui_size len, const struct gui_text*, enum gui_text_align,
                    const struct gui_font*);
gui_bool gui_button_text(gui_command_buffer*, gui_float x, gui_float y,
                    gui_float w, gui_float h, const char*, enum gui_button_behavior,
                    const struct gui_button*, const struct gui_input*, const struct gui_font*);
gui_bool gui_button_image(gui_command_buffer*, gui_float x, gui_float y,
                    gui_float w, gui_float h, gui_image, enum gui_button_behavior,
                    const struct gui_button*, const struct gui_input*);
gui_bool gui_button_triangle(gui_command_buffer*, gui_float x, gui_float y,
                    gui_float w, gui_float h, enum gui_heading, enum gui_button_behavior,
                    const struct gui_button*, const struct gui_input*);
gui_bool gui_toggle(gui_command_buffer*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_bool, const char*, enum gui_toggle_type,
                    const struct gui_toggle*, const struct gui_input*, const struct gui_font*);
gui_float gui_slider(gui_command_buffer*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_float min, gui_float val, gui_float max, gui_float step,
                    const struct gui_slider*, const struct gui_input*);
gui_size gui_progress(gui_command_buffer*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_size value, gui_size max, gui_bool modifyable,
                    const struct gui_progress*, const struct gui_input*);
gui_size gui_edit(gui_command_buffer*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_char*, gui_size, gui_size max, gui_bool*,
                    const struct gui_edit*, enum gui_input_filter filter,
                    const struct gui_input*, const struct gui_font*);
gui_size gui_edit_filtered(gui_command_buffer*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_char*, gui_size, gui_size max, gui_bool*,
                    const struct gui_edit*, gui_filter filter, const struct gui_input*,
                    const struct gui_font*);
gui_float gui_scroll(gui_command_buffer*, gui_float x, gui_float y,
                    gui_float w, gui_float h, gui_float offset, gui_float target,
                    gui_float step, const struct gui_scroll*, const struct gui_input*);

/*
 * ==============================================================
 *
 *                          Config
 *
 * ===============================================================
 */
enum gui_config_colors {
    GUI_COLOR_TEXT,
    GUI_COLOR_PANEL,
    GUI_COLOR_HEADER,
    GUI_COLOR_BORDER,
    GUI_COLOR_BUTTON,
    GUI_COLOR_BUTTON_BORDER,
    GUI_COLOR_BUTTON_HOVER,
    GUI_COLOR_BUTTON_TOGGLE,
    GUI_COLOR_BUTTON_HOVER_FONT,
    GUI_COLOR_CHECK,
    GUI_COLOR_CHECK_BACKGROUND,
    GUI_COLOR_CHECK_ACTIVE,
    GUI_COLOR_OPTION,
    GUI_COLOR_OPTION_BACKGROUND,
    GUI_COLOR_OPTION_ACTIVE,
    GUI_COLOR_SLIDER,
    GUI_COLOR_SLIDER_BAR,
    GUI_COLOR_SLIDER_BORDER,
    GUI_COLOR_SLIDER_CURSOR,
    GUI_COLOR_PROGRESS,
    GUI_COLOR_PROGRESS_CURSOR,
    GUI_COLOR_INPUT,
    GUI_COLOR_INPUT_CURSOR,
    GUI_COLOR_INPUT_BORDER,
    GUI_COLOR_SPINNER,
    GUI_COLOR_SPINNER_BORDER,
    GUI_COLOR_SPINNER_TRIANGLE,
    GUI_COLOR_SELECTOR,
    GUI_COLOR_SELECTOR_BORDER,
    GUI_COLOR_SELECTOR_TRIANGLE,
    GUI_COLOR_HISTO,
    GUI_COLOR_HISTO_BARS,
    GUI_COLOR_HISTO_NEGATIVE,
    GUI_COLOR_HISTO_HIGHLIGHT,
    GUI_COLOR_PLOT,
    GUI_COLOR_PLOT_LINES,
    GUI_COLOR_PLOT_HIGHLIGHT,
    GUI_COLOR_SCROLLBAR,
    GUI_COLOR_SCROLLBAR_CURSOR,
    GUI_COLOR_SCROLLBAR_BORDER,
    GUI_COLOR_TABLE_LINES,
    GUI_COLOR_SHELF,
    GUI_COLOR_SHELF_TEXT,
    GUI_COLOR_SHELF_ACTIVE,
    GUI_COLOR_SHELF_ACTIVE_TEXT,
    GUI_COLOR_SCALER,
    GUI_COLOR_COUNT
};

enum gui_config_properties {
    GUI_PROPERTY_ITEM_SPACING,
    GUI_PROPERTY_ITEM_PADDING,
    GUI_PROPERTY_PADDING,
    GUI_PROPERTY_SCALER_SIZE,
    GUI_PROPERTY_SCROLLBAR_WIDTH,
    GUI_PROPERTY_SIZE,
    GUI_PROPERTY_MAX
};

struct gui_saved_property {
    enum gui_config_properties type;
    struct gui_vec2 value;
};

struct gui_saved_color {
    enum gui_config_colors type;
    struct gui_color value;
};

struct gui_config {
    struct gui_font font;
    struct gui_vec2 properties[GUI_PROPERTY_MAX];
    struct gui_color colors[GUI_COLOR_COUNT];
    struct gui_saved_property property_stack[GUI_MAX_ATTRIB_STACK];
    struct gui_saved_color color_stack[GUI_MAX_COLOR_STACK];
    gui_size color, property;
};

void gui_config_default(struct gui_config*, const struct gui_font*);
struct gui_vec2 gui_config_property(const struct gui_config*, enum gui_config_properties);
struct gui_color gui_config_color(const struct gui_config*, enum gui_config_colors);
void gui_config_push_property(struct gui_config*, enum gui_config_properties, gui_float, gui_float);
void gui_config_push_color(struct gui_config*, enum gui_config_colors,
                    gui_byte, gui_byte, gui_byte, gui_byte);
void gui_config_pop_color(struct gui_config*);
void gui_config_pop_property(struct gui_config*);
void gui_config_reset_colors(struct gui_config*);
void gui_config_reset_properties(struct gui_config*);
void gui_config_reset(struct gui_config*);

/*
 * ==============================================================
 *
 *                          Panel
 *
 * ===============================================================
 */
enum gui_table_lines {
    GUI_TABLE_HHEADER = 0x01,
    GUI_TABLE_VHEADER = 0x02,
    GUI_TABLE_HBODY = 0x04,
    GUI_TABLE_VBODY = 0x08
};

enum gui_panel_tab {
    GUI_MAXIMIZED = gui_false,
    GUI_MINIMIZED = gui_true
};

enum gui_panel_flags {
    GUI_PANEL_HIDDEN = 0x01,
    GUI_PANEL_BORDER = 0x02,
    GUI_PANEL_MINIMIZABLE = 0x04,
    GUI_PANEL_CLOSEABLE = 0x08,
    GUI_PANEL_MOVEABLE = 0x10,
    GUI_PANEL_SCALEABLE = 0x20,
    GUI_PANEL_NO_HEADER = 0x40,
    GUI_PANEL_BORDER_HEADER = 0x80,
    /* internal */
    GUI_PANEL_ACTIVE = 0x100,
    GUI_PANEL_SCROLLBAR = 0x200,
    GUI_PANEL_TAB = 0x400
};

struct gui_layout;
struct gui_stack;
struct gui_panel {
    gui_float x, y;
    gui_float w, h;
    gui_flags flags;
    gui_float offset;
    gui_bool minimized;
    const struct gui_config *config;
    gui_command_buffer *buffer;
    struct gui_panel* next;
    struct gui_panel* prev;
};

struct gui_panel_layout {
    gui_float x, y, w, h;
    gui_float offset;
    gui_bool is_table;
    gui_flags tbl_flags;
    gui_bool valid;
    gui_float at_x;
    gui_float at_y;
    gui_size index;
    gui_float width, height;
    gui_float header_height;
    gui_float row_height;
    gui_size row_columns;
    struct gui_rect clip;
    const struct gui_config *config;
    const struct gui_input *input;
    gui_command_buffer *buffer;
};

/* Panel */
void gui_panel_init(struct gui_panel*, gui_float x, gui_float y, gui_float w,
                    gui_float h, gui_flags, gui_command_buffer*, const struct gui_config*);
gui_bool gui_panel_begin(struct gui_panel_layout *layout, struct gui_panel*,
                    const char *title, const struct gui_input*);
gui_bool gui_panel_begin_stacked(struct gui_panel_layout*, struct gui_panel*,
                    struct gui_stack*, const char*, const struct gui_input*);
gui_bool gui_panel_begin_tiled(struct gui_panel_layout*, struct gui_panel*,
                    struct gui_layout*, gui_uint slot, gui_size index,
                    const char*, const struct gui_input*);
gui_size gui_panel_row_columns(const struct gui_panel_layout *layout, gui_size widget_size);
void gui_panel_row(struct gui_panel_layout*, gui_float height, gui_size cols);
gui_bool gui_panel_widget(struct gui_rect*, struct gui_panel_layout*);
void gui_panel_spacing(struct gui_panel_layout*, gui_size cols);
void gui_panel_text(struct gui_panel_layout*, const char*, gui_size, enum gui_text_align);
void gui_panel_text_colored(struct gui_panel_layout*, const char*, gui_size,
                    enum gui_text_align, struct gui_color color);
void gui_panel_label(struct gui_panel_layout*, const char*, enum gui_text_align);
void gui_panel_label_colored(struct gui_panel_layout*, const char*, enum gui_text_align,
                    struct gui_color color);
gui_bool gui_panel_check(struct gui_panel_layout*, const char*, gui_bool active);
gui_bool gui_panel_option(struct gui_panel_layout*, const char*, gui_bool active);
gui_size gui_panel_option_group(struct gui_panel_layout*, const char**, gui_size cnt, gui_size cur);
gui_bool gui_panel_button_text(struct gui_panel_layout*, const char*, enum gui_button_behavior);
gui_bool gui_panel_button_color(struct gui_panel_layout*, const struct gui_color,
                    enum gui_button_behavior);
gui_bool gui_panel_button_triangle(struct gui_panel_layout*, enum gui_heading,
                    enum gui_button_behavior);
gui_bool gui_panel_button_image(struct gui_panel_layout*, gui_image image,
                    enum gui_button_behavior);
gui_bool gui_panel_button_toggle(struct gui_panel_layout*, const char*, gui_bool value);
gui_float gui_panel_slider(struct gui_panel_layout*, gui_float min, gui_float val,
                    gui_float max, gui_float step);
gui_size gui_panel_progress(struct gui_panel_layout*, gui_size cur, gui_size max,
                    gui_bool modifyable);
gui_size gui_panel_edit(struct gui_panel_layout*, gui_char *buffer, gui_size len,
                    gui_size max, gui_bool *active, enum gui_input_filter);
gui_size gui_panel_edit_filtered(struct gui_panel_layout*, gui_char *buffer, gui_size len,
                    gui_size max, gui_bool *active, gui_filter);
gui_bool gui_panel_shell(struct gui_panel_layout*, gui_char *buffer, gui_size *len,
                    gui_size max, gui_bool *active);
gui_int gui_panel_spinner(struct gui_panel_layout*, gui_int min, gui_int value,
                    gui_int max, gui_int step, gui_bool *active);
gui_size gui_panel_selector(struct gui_panel_layout*, const char *items[],
                    gui_size item_count, gui_size item_current);
void gui_panel_graph_begin(struct gui_panel_layout*, struct gui_graph*, enum gui_graph_type,
                    gui_size count, gui_float min_value, gui_float max_value);
gui_bool gui_panel_graph_push(struct gui_panel_layout *layout,struct gui_graph*,gui_float);
void gui_panel_graph_end(struct gui_panel_layout *layout, struct gui_graph*);
gui_int gui_panel_graph(struct gui_panel_layout*, enum gui_graph_type,
                    const gui_float *values, gui_size count, gui_size offset);
gui_int gui_panel_graph_ex(struct gui_panel_layout*, enum gui_graph_type, gui_size count,
                    gui_float(*get_value)(void*, gui_size), void *userdata);
void gui_panel_table_begin(struct gui_panel_layout*, gui_flags flags,
                    gui_size row_height, gui_size cols);
void gui_panel_table_row(struct gui_panel_layout*);
void gui_panel_table_end(struct gui_panel_layout*);
gui_bool gui_panel_tab_begin(struct gui_panel_layout*, struct gui_panel_layout *tab,
                    const char*, gui_bool);
void gui_panel_tab_end(struct gui_panel_layout*, struct gui_panel_layout *tab);
void gui_panel_group_begin(struct gui_panel_layout*, struct gui_panel_layout *tab,
                    const char*,gui_float offset);
gui_float gui_panel_group_end(struct gui_panel_layout*, struct gui_panel_layout* tab);
gui_size gui_panel_shelf_begin(struct gui_panel_layout*, struct gui_panel_layout *shelf,
                    const char *tabs[], gui_size size, gui_size active, gui_float offset);
gui_float gui_panel_shelf_end(struct gui_panel_layout*, struct gui_panel_layout *shelf);
void gui_panel_end(struct gui_panel_layout*, struct gui_panel*);


/*
 * ==============================================================
 *
 *                          Stack
 *
 * ===============================================================
 */
struct gui_stack {
    gui_size count;
    struct gui_panel *begin;
    struct gui_panel *end;
};

void gui_stack_clear(struct gui_stack*);
void gui_stack_push(struct gui_stack*, struct gui_panel*);
void gui_stack_pop(struct gui_stack*, struct gui_panel*);
#define gui_foreach_panel(i, s) for (i = (s)->begin; i != NULL; i = (i)->next)

/*
 * ==============================================================
 *
 *                          Layout
 *
 * ===============================================================
 */
enum gui_layout_state {
    GUI_LAYOUT_INACTIVE,
    GUI_LAYOUT_ACTIVE
};

enum gui_layout_slot_index {
    GUI_SLOT_TOP,
    GUI_SLOT_BOTTOM,
    GUI_SLOT_LEFT,
    GUI_SLOT_CENTER,
    GUI_SLOT_RIGHT,
    GUI_SLOT_MAX
};

enum gui_layout_format {
    GUI_LAYOUT_HORIZONTAL,
    GUI_LAYOUT_VERTICAL
};

struct gui_layout_config {
    gui_float left;
    gui_float right;
    gui_float centerh;
    gui_float centerv;
    gui_float bottom;
    gui_float top;
};

struct gui_layout_slot {
    gui_size capacity;
    struct gui_vec2 ratio;
    struct gui_vec2 offset;
    enum gui_layout_format format;
};

struct gui_layout {
    gui_flags flags;
    gui_size width, height;
    enum gui_layout_state state;
    struct gui_stack stack;
    struct gui_layout_slot slots[GUI_SLOT_MAX];
};

void gui_layout_init(struct gui_layout*, const struct gui_layout_config*,
                    gui_size width, gui_size height);
void gui_layout_set_size(struct gui_layout*, gui_size width, gui_size height);
void gui_layout_slot(struct gui_layout*, enum gui_layout_slot_index,
                    enum gui_layout_format, gui_size panel_count);

#ifdef __cplusplus
}
#endif

#endif

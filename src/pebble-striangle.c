#include <pebble.h>

#define WIDTH 144

static Window *window;
static Layer *window_layer;
static uint8_t factor = 0b11;

static void multiply(uint8_t *from, uint8_t *to, uint8_t factor, uint8_t len) {
  uint16_t carry = 0;
  for (int i = 0; i < len; i++) {
    uint16_t val = from[i] * factor;
    to[i] = val | carry;
    carry = val >> 8;
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  factor = 1;
  layer_mark_dirty(window_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  factor++;
  layer_mark_dirty(window_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  factor--;
  layer_mark_dirty(window_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50,
      down_click_handler);
}

static void update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);
  GBitmap *buffer = graphics_capture_frame_buffer(ctx);
  if (!buffer) return;
  uint8_t *data = gbitmap_get_data(buffer);
  uint8_t row_size = gbitmap_get_bytes_per_row(buffer);

  // assume buffer->bounds is the whole layer

  // clear first row except for one pixel
  for (int j = 0; j < WIDTH / 8; j++)
    data[j] = 0;
  data[4] = 1;

  for (int y = 1; y < bounds.size.h; y++) {
      uint8_t *row_curr = &data[row_size * y],
              *row_prev = &data[row_size * (y-1)];
      multiply(row_prev, row_curr, factor, WIDTH / 8);
  }

  graphics_release_frame_buffer(ctx, buffer);
}

static void window_load(Window *window) {
  window_layer = window_get_root_layer(window);

  layer_set_update_proc(window_layer, update_proc);
  /*
  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  */
}

static void window_unload(Window *window) {
  // text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

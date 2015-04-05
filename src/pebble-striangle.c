#include <pebble.h>

#define WIDTH 144

static Window *window;
static Layer *window_layer;
static uint8_t seed[WIDTH/8] = {0},
               last[WIDTH/8] = {0};

static void iterate(uint8_t *from, uint8_t *to, uint8_t len) {
  uint8_t carry = 0;
  for (int x = 0; x < len; x++) {
    uint16_t value = carry ^ from[x] ^ (from[x] << 1);
    to[x] = value;
    carry = value >> 8;
  }
}

static void reset_seed() {
  for (int i = 0; i < WIDTH/8; i++)
    seed[i] = 0;
  seed[0] = 1;
}

static void copy_row(uint8_t *from, uint8_t *to) {
  for (int i = 0; i < WIDTH / 8; i++)
    to[i] = from[i];
}

static void select_long_click_handler(ClickRecognizerRef recognizer,
    void *context) {
  reset_seed();
  layer_mark_dirty(window_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  seed[0] ^= 1;
  layer_mark_dirty(window_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  reset_seed();
  layer_mark_dirty(window_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  copy_row(seed, last);
  iterate(last, seed, WIDTH / 8);
  layer_mark_dirty(window_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500,
      select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 30, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30,
      down_click_handler);
}

static void update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);
  GBitmap *buffer = graphics_capture_frame_buffer(ctx);
  if (!buffer) return;
  uint8_t *data = gbitmap_get_data(buffer);
  uint8_t row_size = gbitmap_get_bytes_per_row(buffer);

  // assume buffer->bounds is the whole layer

  // set first row to seed
  for (int i = 0; i < row_size; i++)
    data[i] = seed[i];

  // update the rest of the rows
  for (int y = 1; y < bounds.size.h; y++) {
    iterate(&data[row_size * (y-1)], &data[row_size * y], row_size);
  }

  graphics_release_frame_buffer(ctx, buffer);
}

static void window_load(Window *window) {
  window_layer = window_get_root_layer(window);

  layer_set_update_proc(window_layer, update_proc);
}

static void window_unload(Window *window) {
}

static void init(void) {
  reset_seed();

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

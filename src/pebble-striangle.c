#include <pebble.h>

#define WIDTH 144

static Window *window;
static Layer *window_layer;
int16_t rows[2][WIDTH];

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(text_layer, "Select");
  for (int16_t i = 0; i < WIDTH; i++)
    for (int16_t j = 0; j < 2; j++)
      rows[j][i] = 1;
  layer_mark_dirty(window_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_mark_dirty(window_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_mark_dirty(window_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);

  // erase layer
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  for (int16_t y = 0; y < bounds.size.h; y++) {
    int16_t y_prev = (y+1) % 2;
    for (int16_t x = 0; x < WIDTH; x++) {
      int16_t /*up_left = x > 1 ? rows[y_prev][x-1] : 0,*/
              up_middle = rows[y_prev][x],
              up_right = x < WIDTH-1 ? rows[y_prev][x+1] : 0,
              pixel = (up_middle + up_right) % 2;
      rows[y % 2][x] = pixel;
      if (pixel)
        graphics_draw_pixel(ctx, GPoint(x, y));
    }
  }
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
  for (int16_t i = 0; i < WIDTH; i++)
    for (int16_t j = 0; j < 2; j++)
      rows[j][i] = 0;
  rows[0][WIDTH/2] = 1;
  //rows[1][1] = 1;

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

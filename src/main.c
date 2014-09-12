#include <pebble.h>
	
#define KEY_WORD 0
#define KEY_DEFINITION 1
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *word_layer;
static TextLayer *definition_layer;

static GFont time_font;
static GFont word_font;
static GFont definition_font;

static void update_time(){
  
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";
  
  if(clock_is_24h_style() == true){
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  }
  else{
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  
	s_time_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Sharpie_64));
	
	text_layer_set_font(s_time_layer, time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	word_layer = text_layer_create(GRect(0, 85, 144, 168));
	text_layer_set_background_color(word_layer, GColorWhite);
	text_layer_set_text_color(word_layer, GColorBlack);
	text_layer_set_text_alignment(word_layer, GTextAlignmentCenter);
	text_layer_set_text(word_layer, "Word: Loading...");
	
	word_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Futura_14));
	
	text_layer_set_font(word_layer, word_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(word_layer));
  
  definition_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Futura_12));
	
	definition_layer = text_layer_create(GRect(0, 100, 144, 168));
	text_layer_set_background_color(definition_layer, GColorWhite);
	text_layer_set_text_color(definition_layer, GColorBlack);
	text_layer_set_text_alignment(definition_layer, GTextAlignmentCenter);
	text_layer_set_text(definition_layer, "Definition: Loading...");
	text_layer_set_font(definition_layer, definition_font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(definition_layer));
	
	
	update_time();
	
}

static void main_window_unload(Window *window) {

  text_layer_destroy(s_time_layer);
	fonts_unload_custom_font(time_font);
	text_layer_destroy(word_layer);
	text_layer_destroy(definition_layer);
	fonts_unload_custom_font(word_font);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
	
	if(tick_time->tm_min % 30 == 0) {
	
	DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
	
	dict_write_uint8(iter, 0, 0);
		
	app_message_outbox_send();
	
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
	static char word_buffer[64];
	static char definition_buffer[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla porta venenatis volutpat. Vestibulum vel sem scelerisque, pellentesque urna non, rhoncus ipsum. Fusce in augue consectetur, faucibus quam vitae, consectetur dui. Vivamus efficitur scelerisque gravida. Proin consectetur tristique mauris, in dictum enim semper eget. Nullam finibus mauris vel urna hendrerit, a volutpat lectus porttitor. Morbi aliquet et nibh nec tincidunt. Cras vel justo diam. ";
	static char word_layer_buffer[64];
	
	Tuple *t = dict_read_first(iterator);
	
	while(t != NULL){
		switch(t->key){
			case KEY_WORD:
				snprintf(word_buffer, sizeof(word_buffer), "%s", t->value->cstring);
				text_layer_set_text(word_layer, (char*) &word_buffer);
				break;
			case KEY_DEFINITION:
				snprintf(definition_buffer, sizeof(definition_buffer), "%s", t->value->cstring);
			  text_layer_set_text(definition_layer, (char*) &definition_buffer);
				break;
			default:
				APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
			break;
		}
		//snprintf(word_layer_buffer, sizeof(word_layer_buffer), "%s, %s", word_buffer, definition_buffer);
		//text_layer_set_text(word_layer, word_layer_buffer);
		t = dict_read_next(iterator);
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");	
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send Failed!");
}

static void outbox_sent_callback(DictionaryIterator * iterator, void *context){
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	app_message_register_inbox_received(inbox_received_callback);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
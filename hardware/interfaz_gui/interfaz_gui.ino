#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// Definir pines del Touch
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

TFT_eSPI tft = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);

// 📌 Función de lectura del touch
void touchscreen_read(lv_indev_drv_t * indev, lv_indev_data_t * data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    int x = map(p.x, 200, 3700, 0, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 0, SCREEN_HEIGHT);

    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// 📌 Función para refrescar la pantalla
void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t*)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

// 📌 Función cuando se presiona un botón
static void btn_event_handler(lv_event_t * e) {
  lv_obj_t * btn = lv_event_get_target(e);
  
  // Cambia el color del botón al presionarlo
  lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
  lv_obj_set_style_bg_color(btn, lv_palette_darken(LV_PALETTE_RED, 3), LV_STATE_PRESSED);

  Serial.println("¡Botón presionado!");
}

void lv_create_main_gui(void) {
  // 📌 Texto superior
  lv_obj_t * text_label = lv_label_create(lv_scr_act());
  lv_label_set_text(text_label, "Escoja un modo");
  lv_obj_align(text_label, LV_ALIGN_TOP_MID, 0, 20);

  // 📌 Botón "Enroll"
  lv_obj_t * btn_enroll = lv_btn_create(lv_scr_act());
  lv_obj_set_size(btn_enroll, 120, 50);
  lv_obj_align(btn_enroll, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_bg_color(btn_enroll, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
  lv_obj_add_event_cb(btn_enroll, btn_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t * label_enroll = lv_label_create(btn_enroll);
  lv_label_set_text(label_enroll, "Enroll");
  lv_obj_center(label_enroll);

  // 📌 Botón "Fingerprint"
  lv_obj_t * btn_fingerprint = lv_btn_create(lv_scr_act());
  lv_obj_set_size(btn_fingerprint, 120, 50);
  lv_obj_align(btn_fingerprint, LV_ALIGN_CENTER, 0, 40);
  lv_obj_set_style_bg_color(btn_fingerprint, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
  lv_obj_add_event_cb(btn_fingerprint, btn_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t * label_fingerprint = lv_label_create(btn_fingerprint);
  lv_label_set_text(label_fingerprint, "Fingerprint");
  lv_obj_center(label_fingerprint);
}

void setup() {
  Serial.begin(115200);

  lv_init();

  tft.begin();
  tft.setRotation(2);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(2);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchscreen_read;
  lv_indev_drv_register(&indev_drv);

  lv_create_main_gui();
}

void loop() {
  lv_timer_handler();
  delay(5);
}

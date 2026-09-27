#include "awtk.h"
#include "../common/navigator.h"

static ret_t on_home_page_btn_click(void* ctx, event_t* e) {
  return navigator_replace("home_page");
}

static ret_t on_download_page_btn_click(void* ctx, event_t* e) {
  return navigator_replace("download_page");
}

static ret_t on_settings_page_btn_click(void* ctx, event_t* e) {
  return navigator_replace("settings_page");
}

static ret_t on_more_page_btn_click(void* ctx, event_t* e) {
  return navigator_replace("more_page");
}

/**
 * 初始化窗口的子控件
 */
static ret_t visit_init_child(void* ctx, const void* iter) {
  widget_t* win = WIDGET(ctx);
  widget_t* widget = WIDGET(iter);
  const char* name = widget->name;

  // 初始化指定名称的控件（设置属性或注册事件），请保证控件名称在窗口上唯一
  if (name != NULL && *name != '\0') {
    if (tk_str_eq(name, "home_page_btn")) {
      widget_on(widget, EVT_CLICK, on_home_page_btn_click, win);
    } else if (tk_str_eq(name, "download_page_btn")) {
      widget_on(widget, EVT_CLICK, on_download_page_btn_click, win);
    } else if (tk_str_eq(name, "settings_page_btn")) {
      widget_on(widget, EVT_CLICK, on_settings_page_btn_click, win);
    } else if (tk_str_eq(name, "more_page_btn")) {
      widget_on(widget, EVT_CLICK, on_more_page_btn_click, win);
    }

  }

  return RET_OK;
}

/**
 * 初始化窗口
 * @note
 * 使用 common/navigator.h 中的 API 打开窗口时，会自动调用该初始化函数
 * 使用 AWTK 原生的 window_open 函数打开窗口时，须手动调用该初始化函数
 */
ret_t more_page_init(widget_t* win, void* ctx) {
  (void)ctx;
  return_value_if_fail(win != NULL, RET_BAD_PARAMS);

  widget_foreach(win, visit_init_child, win);

  return RET_OK;
}

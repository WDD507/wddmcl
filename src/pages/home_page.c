#include "../../third_party/awtk/include_flat/awtk.h"
#include "../common/navigator.h"
#include "../mutually.h"

static ret_t on_start_game_click(void* ctx, event_t* e) {
  pointer_event_t* evt = pointer_event_cast(e);
  // TODO: 在此添加控件事件处理程序代码

  return AWTKLaunchGame("", "")/*RET_OK*/;
}

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

static ret_t on_choose_ver_click(void* ctx, event_t* e) {
  return navigator_to("");
}

static ret_t on_ver_settings_click(void* ctx, event_t* e) {
  return navigator_to("");
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
    if (tk_str_eq(name, "start_game")) {
      widget_on(widget, EVT_CLICK, on_start_game_click, win);
    } else if (tk_str_eq(name, "home_page_btn")) {
      widget_on(widget, EVT_CLICK, on_home_page_btn_click, win);
    } else if (tk_str_eq(name, "download_page_btn")) {
      widget_on(widget, EVT_CLICK, on_download_page_btn_click, win);
    } else if (tk_str_eq(name, "settings_page_btn")) {
      widget_on(widget, EVT_CLICK, on_settings_page_btn_click, win);
    } else if (tk_str_eq(name, "more_page_btn")) {
      widget_on(widget, EVT_CLICK, on_more_page_btn_click, win);
    } else if (tk_str_eq(name, "choose_ver")) {
      widget_on(widget, EVT_CLICK, on_choose_ver_click, win);
    } else if (tk_str_eq(name, "ver_settings")) {
      widget_on(widget, EVT_CLICK, on_ver_settings_click, win);
    }

  }

  return RET_OK;
}

/**
 * 初始化窗口
 */
ret_t home_page_init(widget_t* win, void* ctx) {
  (void)ctx;
  return_value_if_fail(win != NULL, RET_BAD_PARAMS);

  widget_foreach(win, visit_init_child, win);

  return RET_OK;
}

/*
** window-binding.cpp
**
** This file is part of HiddenChest.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com> - mkxp project
** 2018-2019 Extended by Kyonides-Arkanthes
**
** HiddenChest is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** HiddenChest is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with HiddenChest. If not, see <http://www.gnu.org/licenses/>.
*/

#include "window.h"
#include "viewport.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "sceneelement-binding.h"
#include "disposable-binding.h"
#include "binding-util.h"
#include "hcextras.h"

DEF_TYPE(Window);

template<class C>
static VALUE viewportElementGetViewport(VALUE self)
{
  checkDisposed<C>(self);
  return rb_iv_get(self, "viewport");
}

template<class C>
static VALUE viewportElementSetViewport(VALUE self, VALUE rviewport)
{
  ViewportElement *ve = getPrivateData<C>(self);
  Viewport *viewport = 0;
  if (!RB_NIL_P(rviewport))
    viewport = getPrivateDataCheck<Viewport>(rviewport, ViewportType);
  GUARD_EXC( ve->setViewport(viewport); );
  rb_iv_set(self, "viewport", rviewport);
  return rviewport;
}

template<class C>
void viewportElementBindingInit(VALUE klass)
{
  sceneElementBindingInit<C>(klass);
  rb_define_method(klass, "viewport", RUBY_METHOD_FUNC(viewportElementGetViewport<C>), 0);
  rb_define_method(klass, "viewport=", RUBY_METHOD_FUNC(viewportElementSetViewport<C>), 1);
}

static VALUE window_initialize(int argc, VALUE *v, VALUE self)
{
  int cmode;
  Viewport *view = 0;
  VALUE rbView = Qnil, mode = rb_iv_get(self, "@open_mode");
  if ( mode == hc_sym("top") )
    cmode = 1;
  else if ( mode == hc_sym("center") )
    cmode = 2;
  else if ( mode == hc_sym("bottom") )
    cmode = 3;
  else
    cmode = 0;
  rb_get_args(argc, v, "|o", &rbView RB_ARG_END);
  if (!RB_NIL_P(rbView)) {
    view = getPrivateDataCheck<Viewport>(rbView, ViewportType);
    if (rgssVer == 1) disposableAddChild(rbView, self);
  }
  rb_iv_set(self, "viewport", rbView);
  Window *w = new Window(view, cmode);
  setPrivateData(self, w);
  w->initDynAttribs();
  rb_iv_set(self, "@area", rb_ary_new());
  wrapProperty(self, &w->getCursorRect(), "cursor_rect", RectType);
  return self;
}

static VALUE window_update(VALUE self)
{
  Window *w = getPrivateData<Window>(self);
  w->update();
  return Qnil;
}

static VALUE window_get_windowskin(VALUE self)
{
  return rb_iv_get(self, "windowskin");
}

static VALUE window_set_windowskin(VALUE self, VALUE bmap)
{
  Window *w = getPrivateData<Window>(self);
  VALUE propObj = rb_obj_dup(bmap);
  Bitmap *prop;
  if (RB_NIL_P(propObj))
    prop = 0;
  else
    prop = getPrivateDataCheck<Bitmap>(propObj, BitmapType);
  GUARD_EXC( w->setWindowskin(prop); )
  rb_iv_set(self, "windowskin", propObj);
  return propObj;
}

static VALUE window_get_open_mode(VALUE self)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return rb_iv_get(self, "@open_mode");
  return rb_iv_get(self, "@open_mode");
}

static VALUE window_set_open_mode(VALUE self, VALUE mode)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return rb_iv_set(self, "@open_mode", mode);
  int result = 0;
  VALUE top = hc_sym("top"), center = hc_sym("center");
  VALUE bottom = hc_sym("bottom");
  if (mode == top) {
    rb_iv_set(self, "@open_mode", top);
    result = 1;
  } else if (mode == center) {
    rb_iv_set(self, "@open_mode", center);
    result = 2;
  } else if (mode == bottom) {
    rb_iv_set(self, "@open_mode", bottom);
    result = 3;
  } else {
    if (mode == Qtrue) mode = Qnil;
    rb_iv_set(self, "@open_mode", mode);
    result = 0;
  }
  w->setOpenMode(result);
  return rb_iv_get(self, "@open_mode");
}

static VALUE window_get_openness(VALUE self)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return RB_INT2FIX(100);
  int result = w->getOpenness();
  return RB_INT2FIX(result);
}

static VALUE window_set_openness(VALUE self, VALUE open)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return RB_INT2FIX(100);
  w->setOpenness(RB_FIX2INT(open));
  return RB_INT2FIX( w->getOpenness() );
}

static VALUE window_is_open(VALUE self)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return Qfalse;
  return w->isOpen() ? Qtrue : Qfalse;
}

static VALUE window_is_closed(VALUE self)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return Qtrue;
  return w->isClosed() ? Qtrue : Qfalse;
}

static VALUE window_set_xy(VALUE self, VALUE rx, VALUE ry)
{
  Window *w = getPrivateData<Window>(self);
  if (w == 0) return rb_ary_new3(2, RB_INT2FIX(0), RB_INT2FIX(0));
  w->setXY(RB_FIX2INT(rx), RB_FIX2INT(ry));
  return rb_ary_new3(2, rx, ry);
}

static VALUE window_is_mouse_inside(VALUE self, VALUE pos)
{
  Window *win = getPrivateData<Window>(self);
  if (win == 0) return Qnil;
  int index = RB_FIX2INT(pos);
  VALUE rect = rb_ary_entry(rb_iv_get(self, "@area"), index);
  if (RB_NIL_P(rect)) return Qnil;
  int x = RB_FIX2INT(rb_ary_entry(rect, 0));
  int y = RB_FIX2INT(rb_ary_entry(rect, 1));
  int w = RB_FIX2INT(rb_ary_entry(rect, 2));
  int h = RB_FIX2INT(rb_ary_entry(rect, 3));
  return win->isMouseInside(x, y, w, h) ? Qtrue : Qfalse;
}

DEF_PROP_OBJ_REF(Window, Bitmap, Contents,   "contents")
DEF_PROP_OBJ_VAL(Window, Rect,   CursorRect, "cursor_rect")
DEF_PROP_B(Window, Stretch)
DEF_PROP_B(Window, Active)
DEF_PROP_B(Window, Pause)
DEF_PROP_I(Window, X)
DEF_PROP_I(Window, Y)
DEF_PROP_I(Window, Width)
DEF_PROP_I(Window, Height)
DEF_PROP_I(Window, OX)
DEF_PROP_I(Window, OY)
DEF_PROP_I(Window, Opacity)
DEF_PROP_I(Window, BackOpacity)
DEF_PROP_I(Window, ContentsOpacity)

void windowBindingInit()
{
  VALUE klass = rb_define_class("Window", rb_cObject);
  rb_iv_set(klass, "@open_mode", hc_sym("center"));
  rb_define_alloc_func(klass, classAllocate<&WindowType>);
  disposableBindingInit     <Window>(klass);
  viewportElementBindingInit<Window>(klass);
  rb_define_method(klass, "initialize", RMF(window_initialize), -1);
  rb_define_method(klass, "update", RMF(window_update), 0);
  rb_define_method(klass, "windowskin", RMF(window_get_windowskin), 0);
  rb_define_method(klass, "windowskin=", RMF(window_set_windowskin), 1);
  rb_define_method(klass, "open_mode", RMF(window_get_open_mode), 0);
  rb_define_method(klass, "open_mode=", RMF(window_set_open_mode), 1);
  rb_define_method(klass, "openness", RMF(window_get_openness), 0);
  rb_define_method(klass, "openness=", RMF(window_set_openness), 1);
  rb_define_method(klass, "open?", RMF(window_is_open), 0);
  rb_define_method(klass, "close?", RMF(window_is_closed), 0);
  rb_define_method(klass, "set_xy", RMF(window_set_xy), 2);
  rb_define_method(klass, "mouse_inside?", RMF(window_is_mouse_inside), 1);
  rb_define_method(klass, "mouse_above?", RMF(window_is_mouse_inside), 1);
  rb_define_attr(klass, "area", 1, 0);
  INIT_PROP_BIND( Window, Contents,        "contents"         );
  INIT_PROP_BIND( Window, Stretch,         "stretch"          );
  INIT_PROP_BIND( Window, CursorRect,      "cursor_rect"      );
  INIT_PROP_BIND( Window, Active,          "active"           );
  INIT_PROP_BIND( Window, Pause,           "pause"            );
  INIT_PROP_BIND( Window, X,               "x"                );
  INIT_PROP_BIND( Window, Y,               "y"                );
  INIT_PROP_BIND( Window, Width,           "width"            );
  INIT_PROP_BIND( Window, Height,          "height"           );
  INIT_PROP_BIND( Window, OX,              "ox"               );
  INIT_PROP_BIND( Window, OY,              "oy"               );
  INIT_PROP_BIND( Window, Opacity,         "opacity"          );
  INIT_PROP_BIND( Window, BackOpacity,     "back_opacity"     );
  INIT_PROP_BIND( Window, ContentsOpacity, "contents_opacity" );
}

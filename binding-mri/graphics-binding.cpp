/*
** graphics-binding.cpp
**
** This file is part of mkxpplus and mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** 2018 Modified by Kyonides-Arkanthes
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "exception.h"

static VALUE graphicsUpdate(VALUE self)
{
  shState->graphics().update();
  return Qnil;
}

static VALUE graphicsFreeze(VALUE self)
{
  shState->graphics().freeze();
  return Qnil;
}

RB_METHOD(graphicsTransition)
{
  RB_UNUSED_PARAM;
  int duration = 8;
  const char *filename = "";
  int vague = 40;
  rb_get_args(argc, argv, "|izi", &duration, &filename, &vague RB_ARG_END);
  GUARD_EXC( shState->graphics().transition(duration, filename, vague); )
  return Qnil;
}

static VALUE graphicsFrameReset(VALUE self)
{
  shState->graphics().frameReset();
  return Qnil;
}

static VALUE graphicsWidth(VALUE self)
{
  return rb_fix_new(shState->graphics().width());
}

static VALUE graphicsHeight(VALUE self)
{
  return rb_fix_new(shState->graphics().height());
}

static VALUE graphicsDimensions(VALUE self)
{
  VALUE width = RB_INT2NUM( shState->graphics().width() );
  VALUE height = RB_INT2NUM( shState->graphics().height() );
  return rb_ary_new3(2, width, height);
}

RB_METHOD(graphicsWait)
{
  RB_UNUSED_PARAM;
  int duration;
  rb_get_args(argc, argv, "i", &duration RB_ARG_END);
  shState->graphics().wait(duration);
  return Qnil;
}

RB_METHOD(graphicsFadeout)
{
  RB_UNUSED_PARAM;
  int duration;
  rb_get_args(argc, argv, "i", &duration RB_ARG_END);
  shState->graphics().fadeout(duration);
  return Qnil;
}

RB_METHOD(graphicsFadein)
{
  RB_UNUSED_PARAM;
  int duration;
  rb_get_args(argc, argv, "i", &duration RB_ARG_END);
  shState->graphics().fadein(duration);
  return Qnil;
}

void bitmapInitProps(Bitmap *b, VALUE self);

static VALUE graphicsSnapToBitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snapToBitmap(); );
  VALUE obj = wrapObject(result, BitmapType);
  bitmapInitProps(result, obj);
  return obj;
}

static VALUE graphicsSaveScreenShot(VALUE self)
{
  bool result = false;
  GUARD_EXC( result = shState->graphics().saveScreenShot(); );
  return result;
}

static VALUE graphicsResizeScreen(VALUE self, VALUE w, VALUE h)
{
  int width = RB_FIX2INT(w), height = RB_FIX2INT(h);
  shState->graphics().resizeScreen(width, height);
  return Qnil;
}

static VALUE graphicsReset(VALUE self)
{
  shState->graphics().reset();
  return Qnil;
}

static VALUE graphicsPlayMovie(VALUE self, VALUE filename)
{
  const char *fn = StringValueCStr(filename);
  shState->graphics().playMovie(fn);
  return Qnil;
}

static VALUE graphicsGetFrameRate(VALUE self)
{
  return rb_fix_new(shState->graphics().getFrameRate());
}

static VALUE graphicsSetFrameRate(VALUE self, VALUE num)
{
  shState->graphics().setFrameRate(RB_FIX2INT(num));
  return rb_fix_new(shState->graphics().getFrameRate());
}

static VALUE graphicsGetFrameCount(VALUE self)
{
  return rb_fix_new(shState->graphics().getFrameCount());
}

static VALUE graphicsSetFrameCount(VALUE self, VALUE num)
{
  shState->graphics().setFrameCount(RB_FIX2INT(num));
  return rb_fix_new(shState->graphics().getFrameCount());
}

static VALUE graphicsGetBrightness(VALUE self)
{
  return rb_fix_new(shState->graphics().getBrightness());
}

static VALUE graphicsSetBrightness(VALUE self, VALUE num)
{
  shState->graphics().setBrightness(RB_FIX2INT(num));
  return rb_fix_new(shState->graphics().getBrightness());
}

static VALUE graphicsGetBlockFullscreen(VALUE self)
{
  return rb_iv_get(self, "@block_fullscreen");
}

static VALUE graphicsSetBlockFullscreen(VALUE self, VALUE boolean)
{
  rb_iv_set(self, "@block_fullscreen", boolean);
  shState->graphics().setBlockFullscreen(boolean == Qtrue ? true : false);
  return boolean;
}

static VALUE graphicsGetFullscreen(VALUE self)
{
  return shState->graphics().getFullscreen() ? Qtrue : Qfalse;
}

static VALUE graphicsSetFullscreen(VALUE self, VALUE boolean)
{
  if (rb_iv_get(self, "@block_fullscreen") == Qfalse) return Qfalse;
  shState->graphics().setFullscreen(boolean == Qtrue ? true : false);
  return shState->graphics().getFullscreen() ? Qtrue : Qfalse;
}

static VALUE graphicsGetShowCursor(VALUE self)
{
  return shState->graphics().getShowCursor() ? Qtrue : Qfalse;
}

static VALUE graphicsSetShowCursor(VALUE self, VALUE boolean)
{
  shState->graphics().setShowCursor(boolean == Qtrue ? true : false);
  return boolean;
}

void graphicsBindingInit()
{
  VALUE module = rb_define_module("Graphics");
  rb_iv_set(module, "@block_fullscreen", Qfalse);
  rb_define_module_function(module, "update", RUBY_METHOD_FUNC(graphicsUpdate), 0);
  rb_define_module_function(module, "freeze", RUBY_METHOD_FUNC(graphicsFreeze), 0);
  rb_define_module_function(module, "transition", RUBY_METHOD_FUNC(graphicsTransition), -1);
  rb_define_module_function(module, "frame_reset", RUBY_METHOD_FUNC(graphicsFrameReset), 0);
  rb_define_module_function(module, "__reset__", RUBY_METHOD_FUNC(graphicsReset), 0);
  rb_define_module_function(module, "frame_rate", RUBY_METHOD_FUNC(graphicsGetFrameRate), 0);
  rb_define_module_function(module, "frame_rate=", RUBY_METHOD_FUNC(graphicsSetFrameRate), 1);
  rb_define_module_function(module, "frame_count", RUBY_METHOD_FUNC(graphicsGetFrameCount), 0);
  rb_define_module_function(module, "frame_count=", RUBY_METHOD_FUNC(graphicsSetFrameCount), 1);
  rb_define_module_function(module, "width", RUBY_METHOD_FUNC(graphicsWidth), 0);
  rb_define_module_function(module, "height", RUBY_METHOD_FUNC(graphicsHeight), 0);
  rb_define_module_function(module, "dimensions", RUBY_METHOD_FUNC(graphicsDimensions), 0);
  rb_define_module_function(module, "wait", RUBY_METHOD_FUNC(graphicsWait), -1);
  rb_define_module_function(module, "fadeout", RUBY_METHOD_FUNC(graphicsFadeout), -1);
  rb_define_module_function(module, "fadein", RUBY_METHOD_FUNC(graphicsFadein), -1);
  rb_define_module_function(module, "snap_to_bitmap", RUBY_METHOD_FUNC(graphicsSnapToBitmap), 0);
  rb_define_module_function(module, "save_screenshot", RUBY_METHOD_FUNC(graphicsSaveScreenShot), 0);
  rb_define_module_function(module, "resize_screen", RUBY_METHOD_FUNC(graphicsResizeScreen), 2);
  rb_define_module_function(module, "brightness", RUBY_METHOD_FUNC(graphicsGetBrightness), 0);
  rb_define_module_function(module, "brightness=", RUBY_METHOD_FUNC(graphicsSetBrightness), 1);
  rb_define_module_function(module, "play_movie", RUBY_METHOD_FUNC(graphicsPlayMovie), 1);
  rb_define_module_function(module, "block_fullscreen", RUBY_METHOD_FUNC(graphicsGetBlockFullscreen), 0);
  rb_define_module_function(module, "block_fullscreen=", RUBY_METHOD_FUNC(graphicsSetBlockFullscreen), 1);
  rb_define_module_function(module, "fullscreen", RUBY_METHOD_FUNC(graphicsGetFullscreen), 0);
  rb_define_module_function(module, "fullscreen=", RUBY_METHOD_FUNC(graphicsSetFullscreen), 1);
  rb_define_module_function(module, "show_cursor", RUBY_METHOD_FUNC(graphicsGetShowCursor), 0);
  rb_define_module_function(module, "show_cursor=", RUBY_METHOD_FUNC(graphicsSetShowCursor), 1);
}

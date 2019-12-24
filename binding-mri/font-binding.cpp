/*
** font-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** Extended (C) 2019 Kyonides Arkanthes <kyonides@gmail.com>
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

#include "hcextras.h"
#include "font.h"
#include "binding-util.h"
#include "binding-types.h"
#include "exception.h"
#include "sharedstate.h"
#include <string.h>

static void collectStrings(VALUE obj, std::vector<std::string> &out)
{
  if (RB_TYPE_P(obj, RUBY_T_STRING)) {
    out.push_back(RSTRING_PTR(obj));
  } else if (RB_TYPE_P(obj, RUBY_T_ARRAY)) {
    for (long i = 0; i < RARRAY_LEN(obj); ++i) {
      VALUE str = rb_ary_entry(obj, i);
      // Non-string objects are tolerated (ignored)
      if (!RB_TYPE_P(str, RUBY_T_STRING)) continue;
      out.push_back(RSTRING_PTR(str));
    }
  }
}

DEF_TYPE(Font);

static VALUE font_does_exist(VALUE self, VALUE fname)
{
  if (!RB_TYPE_P(fname, RUBY_T_STRING)) return Qfalse;
  const char *name = rb_string_value_cstr(&fname);
  return Font::does_exist(name) ? Qtrue : Qfalse;
}

static VALUE font_initialize(int argc, VALUE* argv, VALUE self)
{
  VALUE namesObj = Qnil;
  int size = 0;
  rb_get_args(argc, argv, "|oi", &namesObj, &size RB_ARG_END);
  Font *f;
  if (RB_NIL_P(namesObj)) {
    namesObj = rb_iv_get(rb_obj_class(self), "default_name");
    f = new Font(0, size);
  } else {
    std::vector<std::string> names;
    collectStrings(namesObj, names);
    f = new Font(&names, size);
  }
  /* This is semantically wrong; the new Font object should take
   * a dup'ed object here in case of an array. Ditto for the setters.
   * However the same bug/behavior exists in all RM versions. */
  rb_iv_set(self, "name", namesObj);
  setPrivateData(self, f);
  // Wrap property objects
  f->initDynAttribs();
  wrapProperty(self, &f->get_color(), "color", ColorType);
  wrapProperty(self, &f->get_out_color(), "out_color", ColorType);
  wrapProperty(self, &f->get_shadow_color(), "shadow_color", ColorType);
  return self;
}

static VALUE font_initialize_copy(int argc, VALUE* argv, VALUE self)
{
  VALUE origObj;
  rb_get_args(argc, argv, "o", &origObj RB_ARG_END);
  if (!OBJ_INIT_COPY(self, origObj)) return self;
  Font *orig = getPrivateData<Font>(origObj);
  Font *f = new Font(*orig);
  setPrivateData(self, f);
  // Wrap property objects
  f->initDynAttribs();
  wrapProperty(self, &f->get_color(), "color", ColorType);
  wrapProperty(self, &f->get_out_color(), "out_color", ColorType);
  wrapProperty(self, &f->get_shadow_color(), "shadow_color", ColorType);
  return self;
}

static VALUE font_get_name(VALUE self)
{
  return rb_iv_get(self, "name");
}

template<class C>
static void checkDisposed(VALUE) {}

static VALUE font_get_color(VALUE self)
{
  checkDisposed<Font>(self);
  return rb_iv_get(self, "color");
}

static VALUE font_get_out_color(VALUE self)
{
  checkDisposed<Font>(self);
  return rb_iv_get(self, "out_color");
}

static VALUE font_get_shadow_color(VALUE self)
{
  checkDisposed<Font>(self);
  return rb_iv_get(self, "shadow_color");
}

static VALUE font_get_size(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  return RB_INT2FIX(f->get_size());
}

static VALUE font_get_outline_size(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  return RB_INT2FIX(f->get_outline_size());
}

static VALUE font_get_shadow_size(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  return RB_INT2FIX(f->get_shadow_size());
}

static VALUE font_get_no_squeeze(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_no_squeeze() ? Qtrue : Qfalse;
}

static VALUE font_get_bold(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_bold() ? Qtrue : Qfalse;
}

static VALUE font_get_italic(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_italic() ? Qtrue : Qfalse;
}

static VALUE font_get_shadow(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_shadow() ? Qtrue : Qfalse;
}

static VALUE font_get_outline(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_outline() ? Qtrue : Qfalse;
}

static VALUE font_get_underline(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_underline() ? Qtrue : Qfalse;
}

static VALUE font_get_strikethrough(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_strikethrough() ? Qtrue : Qfalse;
}

static VALUE font_get_shadow_mode(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  return f ? rb_iv_get(self, "shadow_mode") : Qnil;
}

static VALUE font_set_name(int argc, VALUE* argv, VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  rb_check_argc(argc, 1);
  std::vector<std::string> namesObj;
  collectStrings(argv[0], namesObj);
  f->set_name(namesObj);
  rb_iv_set(self, "name", argv[0]);
  return argv[0];
}

static VALUE font_set_color(VALUE self, VALUE color)
{
  Font *f = (Font*)RTYPEDDATA(self)->data;
  if (!f || RB_NIL_P(color)) return Qnil;
  Color *c = (Color*)RTYPEDDATA(color)->data;
  GUARD_EXC( f->set_color(*c); )
  return color;
}

static VALUE font_set_out_color(VALUE self, VALUE out)
{
  Font *f = (Font*)RTYPEDDATA(self)->data;
  if (!f || RB_NIL_P(out)) return Qnil;
  Color *c = (Color*)RTYPEDDATA(out)->data;
  GUARD_EXC( f->set_out_color(*c); )
  return out;
}

static VALUE font_set_shadow_color(VALUE self, VALUE color)
{
  Font *f = (Font*)RTYPEDDATA(self)->data;
  if (!f || RB_NIL_P(color)) return Qnil;
  Color *c = (Color*)RTYPEDDATA(color)->data;
  GUARD_EXC( f->set_shadow_color(*c); )
  return color;
}

static VALUE font_set_size(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  f->set_size(RB_FIX2INT(size));
  return RB_INT2FIX(f->get_size());
}

static VALUE font_set_outline_size(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  f->set_outline_size(RB_FIX2INT(size));
  return RB_INT2FIX(f->get_outline_size());
}

static VALUE font_set_shadow_size(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  f->set_shadow_size(RB_FIX2INT(size));
  return RB_INT2FIX(f->get_shadow_size());
}

static VALUE font_set_no_squeeze(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_no_squeeze(boolean == Qtrue);
  return f->get_no_squeeze() ? Qtrue : Qfalse;
}

static VALUE font_set_bold(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_bold(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_italic(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_italic(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_shadow(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_shadow(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_outline(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_outline(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_underline(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_underline(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_strikethrough(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_strikethrough(boolean == Qtrue);
  return boolean;
}

static VALUE font_set_shadow_mode(VALUE self, VALUE mode)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_shadow_mode(RB_FIX2INT(mode));
  return rb_iv_set(self, "shadow_mode", mode);
}

static VALUE font_get_default_size(VALUE self)
{
  return RB_INT2FIX(Font::get_default_size());
}

static VALUE font_get_default_bold(VALUE self)
{
  return Font::get_default_bold();
}

static VALUE font_get_default_italic(VALUE self)
{
  return Font::get_default_italic();
}

static VALUE font_get_default_outline(VALUE self)
{
  return Font::get_default_outline();
}

static VALUE font_get_default_shadow(VALUE self)
{
  return Font::get_default_shadow();
}

static VALUE font_get_default_underline(VALUE self)
{
  return Font::get_default_underline();
}

static VALUE font_get_default_strikethrough(VALUE self)
{
  return Font::get_default_strikethrough();
}

static VALUE font_get_default_color(VALUE self)
{
  return rb_iv_get(self, "default_color");
}

static VALUE font_get_default_out_color(VALUE self)
{
  return rb_iv_get(self, "default_out_color");
}

static VALUE font_get_default_shadow_color(VALUE self)
{
  return rb_iv_get(self, "default_shadow_color");
}

static VALUE font_get_default_name(VALUE self)
{
  return rb_iv_get(self, "default_name");
}

static VALUE font_set_default_size(VALUE self, VALUE new_size)
{
  Font::set_default_size(RB_NUM2INT(new_size));
  return RB_INT2NUM(Font::get_default_size());
}

static VALUE font_set_default_bold(VALUE self, VALUE boolean)
{
  Font::set_default_bold(boolean == Qtrue);
  return rb_iv_set(self, "default_bold", boolean);
}

static VALUE font_set_default_italic(VALUE self, VALUE boolean)
{
  Font::set_default_italic(boolean == Qtrue);
  return rb_iv_set(self, "default_italic", boolean);
}

static VALUE font_set_default_shadow(VALUE self, VALUE boolean)
{
  Font::set_default_shadow(boolean == Qtrue);
  return rb_iv_set(self, "default_shadow", boolean);
}

static VALUE font_set_default_outline(VALUE self, VALUE boolean)
{
  Font::set_default_outline(boolean == Qtrue);
  return rb_iv_set(self, "default_outline", boolean);
}

static VALUE font_set_default_underline(VALUE self, VALUE boolean)
{
  Font::set_default_underline(boolean == Qtrue);
  return rb_iv_set(self, "default_underline", boolean);
}

static VALUE font_set_default_strikethrough(VALUE self, VALUE boolean)
{
  Font::set_default_strikethrough(boolean == Qtrue);
  return rb_iv_set(self, "default_strikethrough", boolean);
}

static VALUE font_set_default_color(VALUE self, VALUE color)
{
  Color *c = getPrivateDataCheck<Color>(color, ColorType);
  if (!c) return Qnil;
  Font::set_default_color(*c);
  return color;
}

static VALUE font_set_default_out_color(VALUE self, VALUE color)
{
  Color *c = getPrivateDataCheck<Color>(color, ColorType);
  if (!c) return Qnil;
  Font::set_default_out_color(*c);
  return color;
}

static VALUE font_set_default_shadow_color(VALUE self, VALUE color)
{
  Color *c = getPrivateDataCheck<Color>(color, ColorType);
  if (!c) return Qnil;
  Font::set_default_shadow_color(*c);
  return color;
}

static VALUE font_set_default_name(int argc, VALUE* argv, VALUE self)
{
  rb_check_argc(argc, 1);
  std::vector<std::string> namesObj;
  collectStrings(argv[0], namesObj);
  Font::set_default_name(namesObj, shState->fontState());
  rb_iv_set(self, "default_name", argv[0]);
  return argv[0];
}

void fontBindingInit()
{
  VALUE klass = rb_define_class("Font", rb_cObject);
  rb_define_alloc_func(klass, classAllocate<&FontType>);
  Font::initDefaultDynAttribs();
  const std::vector<std::string> &defNames = Font::getInitialDefaultNames();
  VALUE defNamesObj;
  if (defNames.size() == 1) {
    defNamesObj = rb_str_new_cstr(defNames[0].c_str());
  } else {
    defNamesObj = rb_ary_new2(defNames.size());
    for (size_t i = 0; i < defNames.size(); ++i)
      rb_ary_push(defNamesObj, rb_str_new_cstr(defNames[i].c_str()));
  }
  rb_iv_set(klass, "default_name", defNamesObj);
  wrapProperty(klass, &Font::get_default_color(), "default_color", ColorType);
  wrapProperty(klass, &Font::get_default_out_color(), "default_out_color", ColorType);
  wrapProperty(klass, &Font::get_default_shadow_color(), "default_shadow_color", ColorType);
  rb_define_singleton_method(klass, "default_name", RMF(font_get_default_name), 0);
  rb_define_singleton_method(klass, "default_name=", RMF(font_set_default_name), -1);
  rb_define_singleton_method(klass, "default_size", RMF(font_get_default_size), 0);
  rb_define_singleton_method(klass, "default_size=", RMF(font_set_default_size), 1);
  rb_define_singleton_method(klass, "default_bold", RMF(font_get_default_bold), 0);
  rb_define_singleton_method(klass, "default_bold=", RMF(font_set_default_bold), 1);
  rb_define_singleton_method(klass, "default_italic", RMF(font_get_default_italic), 0);
  rb_define_singleton_method(klass, "default_italic=", RMF(font_set_default_italic), 1);
  rb_define_singleton_method(klass, "default_color", RMF(font_get_default_color), 0);
  rb_define_singleton_method(klass, "default_color=", RMF(font_set_default_color), 1);
  rb_define_singleton_method(klass, "default_outline", RMF(font_get_default_outline), 0);
  rb_define_singleton_method(klass, "default_outline=", RMF(font_set_default_outline), 1);
  rb_define_singleton_method(klass, "default_out_color", RMF(font_get_default_out_color), 0);
  rb_define_singleton_method(klass, "default_out_color=", RMF(font_set_default_out_color), 1);
  rb_define_singleton_method(klass, "default_outline_color", RMF(font_get_default_out_color), 0);
  rb_define_singleton_method(klass, "default_outline_color=", RMF(font_set_default_out_color), 1);
  rb_define_singleton_method(klass, "default_shadow", RMF(font_get_default_shadow), 0);
  rb_define_singleton_method(klass, "default_shadow=", RMF(font_set_default_shadow), 1);
  rb_define_singleton_method(klass, "default_shadow_color", RMF(font_get_default_shadow), 0);
  rb_define_singleton_method(klass, "default_shadow_color=", RMF(font_set_default_shadow), 1);
  rb_define_singleton_method(klass, "default_underline", RMF(font_get_default_underline), 0);
  rb_define_singleton_method(klass, "default_underline=", RMF(font_set_default_underline), 1);
  rb_define_singleton_method(klass, "default_strikethrough", RMF(font_get_default_strikethrough), 0);
  rb_define_singleton_method(klass, "default_strikethrough=", RMF(font_set_default_strikethrough), 1);
  rb_define_singleton_method(klass, "default_strikethru", RMF(font_get_default_strikethrough), 0);
  rb_define_singleton_method(klass, "default_strikethru=", RMF(font_set_default_strikethrough), 1);
  rb_define_singleton_method(klass, "exist?", RMF(font_does_exist), 1);
  rb_define_method(klass, "initialize",      RMF(font_initialize), -1);
  rb_define_method(klass, "initialize_copy", RMF(font_initialize_copy), -1);
  rb_define_method(klass, "name", RMF(font_get_name), 0);
  rb_define_method(klass, "name=", RMF(font_set_name), -1);
  rb_define_method(klass, "size", RMF(font_get_size), 0);
  rb_define_method(klass, "size=", RMF(font_set_size), 1);
  rb_define_method(klass, "outline_size", RMF(font_get_outline_size), 0);
  rb_define_method(klass, "outline_size=", RMF(font_set_outline_size), 1);
  rb_define_method(klass, "shadow_size", RMF(font_get_shadow_size), 0);
  rb_define_method(klass, "shadow_size=", RMF(font_set_shadow_size), 1);
  rb_define_method(klass, "no_squeeze", RMF(font_get_no_squeeze), 0);
  rb_define_method(klass, "no_squeeze=", RMF(font_set_no_squeeze), 1);
  rb_define_method(klass, "bold", RMF(font_get_bold), 0);
  rb_define_method(klass, "bold=", RMF(font_set_bold), 1);
  rb_define_method(klass, "italic", RMF(font_get_italic), 0);
  rb_define_method(klass, "italic=", RMF(font_set_italic), 1);
  rb_define_method(klass, "shadow", RMF(font_get_shadow), 0);
  rb_define_method(klass, "shadow=", RMF(font_set_shadow), 1);
  rb_define_method(klass, "outline", RMF(font_get_outline), 0);
  rb_define_method(klass, "outline=", RMF(font_set_outline), 1);
  rb_define_method(klass, "underline", RMF(font_get_underline), 0);
  rb_define_method(klass, "underline=", RMF(font_set_underline), 1);
  rb_define_method(klass, "strikethrough", RMF(font_get_strikethrough), 0);
  rb_define_method(klass, "strikethrough=", RMF(font_set_strikethrough), 1);
  rb_define_method(klass, "strikethru", RMF(font_get_strikethrough), 0);
  rb_define_method(klass, "strikethru=", RMF(font_set_strikethrough), 1);
  rb_define_method(klass, "color", RMF(font_get_color), 0);
  rb_define_method(klass, "color=", RMF(font_set_color), 1);
  rb_define_method(klass, "out_color", RMF(font_get_out_color), 0);
  rb_define_method(klass, "out_color=", RMF(font_set_out_color), 1);
  rb_define_method(klass, "outline_color", RMF(font_get_out_color), 0);
  rb_define_method(klass, "outline_color=", RMF(font_set_out_color), 1);
  rb_define_method(klass, "shadow_color", RMF(font_get_shadow_color), 0);
  rb_define_method(klass, "shadow_color=", RMF(font_set_shadow_color), 1);
  rb_define_method(klass, "shadow_mode", RMF(font_get_shadow_mode), 0);
  rb_define_method(klass, "shadow_mode=", RMF(font_set_shadow_mode), 1);
}

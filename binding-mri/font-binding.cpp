/*
** font-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
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

RB_METHOD(fontDoesExist)
{
  RB_UNUSED_PARAM;
  const char *name = 0;
  VALUE nameObj;
  rb_get_args(argc, argv, "o", &nameObj RB_ARG_END);
  if (RB_TYPE_P(nameObj, RUBY_T_STRING))
    name = rb_string_value_cstr(&nameObj);
  return Font::doesExist(name) ? Qtrue : Qfalse;
}

RB_METHOD(fontInitialize)
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
  return self;
}

RB_METHOD(fontInitializeCopy)
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
  return self;
}

static VALUE FontGetName(VALUE self)
{
  return rb_iv_get(self, "name");
}

static VALUE FontSetName(int argc, VALUE* argv, VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  rb_check_argc(argc, 1);
  std::vector<std::string> namesObj;
  collectStrings(argv[0], namesObj);
  f->setName(namesObj);
  rb_iv_set(self, "name", argv[0]);
  return argv[0];
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

static VALUE font_get_size(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  return RB_INT2FIX(f->get_size());
}

static VALUE font_set_size(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  f->set_size(RB_FIX2INT(size));
  return RB_INT2FIX(f->get_size());
}

static VALUE font_get_outline_size(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  return RB_INT2FIX(f->get_outline_size());
}

static VALUE font_set_outline_size(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return RB_INT2FIX(0);
  f->set_outline_size(RB_FIX2INT(size));
  return RB_INT2FIX(f->get_outline_size());
}

static VALUE font_get_no_squeeze(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_no_squeeze() ? Qtrue : Qfalse;
}

static VALUE font_set_no_squeeze(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_no_squeeze(boolean == Qtrue);
  return f->get_no_squeeze() ? Qtrue : Qfalse;
}

static VALUE FontGetBold(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->getBold() ? Qtrue : Qfalse;
}

static VALUE FontSetBold(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->setBold(boolean == Qtrue);
  return boolean;
}

static VALUE FontGetItalic(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->getItalic() ? Qtrue : Qfalse;
}

static VALUE FontSetItalic(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->setItalic(boolean == Qtrue);
  return boolean;
}

static VALUE FontGetShadow(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->getShadow() ? Qtrue : Qfalse;
}

static VALUE FontSetShadow(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->setShadow(boolean == Qtrue);
  return boolean;
}

static VALUE FontGetOutline(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->getOutline() ? Qtrue : Qfalse;
}

static VALUE FontSetOutline(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->setOutline(boolean == Qtrue);
  return boolean;
}

static VALUE font_get_underline(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_underline() ? Qtrue : Qfalse;
}

static VALUE font_set_underline(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_underline(boolean == Qtrue);
  return boolean;
}

static VALUE font_get_strikethrough(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  return f->get_strikethrough() ? Qtrue : Qfalse;
}

static VALUE font_set_strikethrough(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  if (!f) return Qnil;
  f->set_strikethrough(boolean == Qtrue);
  return boolean;
}

#define DEF_KLASS_PROP(Klass, type, PropName, param_t_s, value_fun) \
	RB_METHOD(Klass##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return value_fun(Klass::get##PropName()); \
	} \
	RB_METHOD(Klass##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		type value; \
		rb_get_args(argc, argv, param_t_s, &value RB_ARG_END); \
		Klass::set##PropName(value); \
		return value_fun(value); \
	}

static VALUE FontGetDefaultSize(VALUE self)
{
  return rb_fix_new(Font::getDefaultSize());
}

static VALUE FontSetDefaultSize(VALUE self, VALUE new_size)
{
  Font::setDefaultSize(RB_NUM2INT(new_size));
  return RB_INT2NUM(Font::getDefaultSize());
}

DEF_KLASS_PROP(Font, bool, DefaultBold,    "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultItalic,  "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultShadow,  "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultOutline, "b", rb_bool_new)

static VALUE font_get_default_underline(VALUE self)
{
  return rb_iv_get(self, "default_underline");
}

static VALUE font_get_default_strikethrough(VALUE self)
{
  return rb_iv_get(self, "default_strikethrough");
}

static VALUE FontGetDefaultOutColor(VALUE self)
{
  return rb_iv_get(self, "default_out_color");
}

static VALUE FontGetDefaultName(VALUE self)
{
  return rb_iv_get(self, "default_name");
}

static VALUE FontSetDefaultOutColor(VALUE self, VALUE color)
{
  Color *c = getPrivateDataCheck<Color>(color, ColorType);
  Font::setDefaultOutColor(*c);
  return color;
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

RB_METHOD(FontSetDefaultName)
{
  RB_UNUSED_PARAM;
  rb_check_argc(argc, 1);
  std::vector<std::string> namesObj;
  collectStrings(argv[0], namesObj);
  Font::setDefaultName(namesObj, shState->fontState());
  rb_iv_set(self, "default_name", argv[0]);
  return argv[0];
}

static VALUE FontGetDefaultColor(VALUE self)
{
  return rb_iv_get(self, "default_color");
}

static VALUE FontSetDefaultColor(VALUE self, VALUE color)
{
  Color *c = getPrivateDataCheck<Color>(color, ColorType);
  Font::setDefaultColor(*c);
  return color;
}

void fontBindingInit()
{
  VALUE klass = rb_define_class("Font", rb_cObject);
  rb_define_alloc_func(klass, classAllocate<&FontType>);
  Font::initDefaultDynAttribs();
  wrapProperty(klass, &Font::getDefaultColor(), "default_color", ColorType);
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
  wrapProperty(klass, &Font::getDefaultOutColor(), "default_out_color", ColorType);
  rb_define_singleton_method(klass, "default_name", RMF(FontGetDefaultName), 0);
  rb_define_singleton_method(klass, "default_name=", RMF(FontSetDefaultName), -1);
  rb_define_singleton_method(klass, "default_size", RMF(FontGetDefaultSize), 0);
  rb_define_singleton_method(klass, "default_size=", RMF(FontSetDefaultSize), 1);
  rb_define_singleton_method(klass, "default_bold", RMF(FontGetDefaultBold), 0);
  rb_define_singleton_method(klass, "default_bold=", RMF(FontSetDefaultBold), 1);
  rb_define_singleton_method(klass, "default_italic", RMF(FontGetDefaultItalic), 0);
  rb_define_singleton_method(klass, "default_italic=", RMF(FontSetDefaultItalic), 1);
  rb_define_singleton_method(klass, "default_color", RMF(FontGetDefaultColor), 0);
  rb_define_singleton_method(klass, "default_color=", RMF(FontSetDefaultColor), 1);
  rb_define_singleton_method(klass, "default_outline", RMF(FontGetDefaultOutline), 0);
  rb_define_singleton_method(klass, "default_outline=", RMF(FontSetDefaultOutline), 1);
  rb_define_singleton_method(klass, "default_out_color", RMF(FontGetDefaultOutColor), 0);
  rb_define_singleton_method(klass, "default_out_color=", RMF(FontSetDefaultOutColor), 1);
  rb_define_singleton_method(klass, "default_outline_color", RMF(FontGetDefaultOutColor), 0);
  rb_define_singleton_method(klass, "default_outline_color=", RMF(FontSetDefaultOutColor), 1);
  rb_define_singleton_method(klass, "default_shadow", RMF(FontGetDefaultShadow), 0);
  rb_define_singleton_method(klass, "default_shadow=", RMF(FontSetDefaultShadow), 1);
  rb_define_singleton_method(klass, "default_underline", RMF(font_get_default_underline), 0);
  rb_define_singleton_method(klass, "default_underline=", RMF(font_set_default_underline), 1);
  rb_define_singleton_method(klass, "default_strikethrough", RMF(font_get_default_strikethrough), 0);
  rb_define_singleton_method(klass, "default_strikethrough=", RMF(font_set_default_strikethrough), 1);
  rb_define_singleton_method(klass, "default_strikethru", RMF(font_get_default_strikethrough), 0);
  rb_define_singleton_method(klass, "default_strikethru=", RMF(font_set_default_strikethrough), 1);
  rb_define_class_method(klass, "exist?", fontDoesExist);
  _rb_define_method(klass, "initialize",      fontInitialize);
  _rb_define_method(klass, "initialize_copy", fontInitializeCopy);
  rb_define_method(klass, "name", RMF(FontGetName), 0);
  rb_define_method(klass, "name=", RMF(FontSetName), -1);
  rb_define_method(klass, "size", RMF(font_get_size), 0);
  rb_define_method(klass, "size=", RMF(font_set_size), 1);
  rb_define_method(klass, "outline_size", RMF(font_get_outline_size), 0);
  rb_define_method(klass, "outline_size=", RMF(font_set_outline_size), 1);
  rb_define_method(klass, "no_squeeze", RMF(font_get_no_squeeze), 0);
  rb_define_method(klass, "no_squeeze=", RMF(font_set_no_squeeze), 1);
  rb_define_method(klass, "bold", RMF(FontGetBold), 0);
  rb_define_method(klass, "bold=", RMF(FontSetBold), 1);
  rb_define_method(klass, "italic", RMF(FontGetItalic), 0);
  rb_define_method(klass, "italic=", RMF(FontSetItalic), 1);
  rb_define_method(klass, "shadow", RMF(FontGetShadow), 0);
  rb_define_method(klass, "shadow=", RMF(FontSetShadow), 1);
  rb_define_method(klass, "outline", RMF(FontGetOutline), 0);
  rb_define_method(klass, "outline=", RMF(FontSetOutline), 1);
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
}

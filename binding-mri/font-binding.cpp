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

#include "font.h"
#include "binding-util.h"
#include "binding-types.h"
#include "exception.h"
#include "sharedstate.h"
#include <string.h>

static void
collectStrings(VALUE obj, std::vector<std::string> &out)
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
  return rb_bool_new(Font::doesExist(name));
}

RB_METHOD(fontInitialize)
{
  VALUE namesObj = Qnil;
  int size = 0;
  rb_get_args(argc, argv, "|oi", &namesObj, &size RB_ARG_END);
  Font *f;
  if (NIL_P(namesObj)) {
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
  wrapProperty(self, &f->getColor(), "color", ColorType);
  wrapProperty(self, &f->getOutColor(), "out_color", ColorType);
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
  wrapProperty(self, &f->getColor(), "color", ColorType);
  wrapProperty(self, &f->getOutColor(), "out_color", ColorType);
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

DEF_PROP_OBJ_VAL(Font, Color, Color,    "color")
DEF_PROP_OBJ_VAL(Font, Color, OutColor, "out_color")

static VALUE FontGetSize(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  int value = 0;
  GUARD_EXC( value = f->getSize(); )
  return RB_INT2FIX(value);
}

static VALUE FontSetSize(VALUE self, VALUE size)
{
  Font *f = getPrivateData<Font>(self);
  int value = 0;
  GUARD_EXC( f->setSize(RB_FIX2INT(size)); )
  GUARD_EXC( value = f->getSize(); )
  return RB_INT2FIX(value);
}

static VALUE FontGetBold(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  bool value = 0;
  GUARD_EXC( value = f->getBold(); )
  return value == true ? Qtrue : Qfalse;
}

static VALUE FontSetBold(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  GUARD_EXC( f->setBold(boolean == Qtrue ? true : false); )
  return boolean;
}

static VALUE FontGetItalic(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  bool value = 0;
  GUARD_EXC( value = f->getItalic(); )
  return value == true ? Qtrue : Qfalse;
}

static VALUE FontSetItalic(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  GUARD_EXC( f->setItalic(boolean == Qtrue ? true : false); )
  return boolean;
}

static VALUE FontGetShadow(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  bool value = 0;
  GUARD_EXC( value = f->getShadow(); )
  return value == true ? Qtrue : Qfalse;
}

static VALUE FontSetShadow(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  GUARD_EXC( f->setShadow(boolean == Qtrue ? true : false); )
  return boolean;
}

static VALUE FontGetOutline(VALUE self)
{
  Font *f = getPrivateData<Font>(self);
  bool value = 0;
  GUARD_EXC( value = f->getOutline(); )
  return value == true ? Qtrue : Qfalse;
}

static VALUE FontSetOutline(VALUE self, VALUE boolean)
{
  Font *f = getPrivateData<Font>(self);
  GUARD_EXC( f->setOutline(boolean == Qtrue ? true : false); )
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
//DEF_KLASS_PROP(Font, int,  DefaultSize,    "i", rb_fix_new)
DEF_KLASS_PROP(Font, bool, DefaultBold,    "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultItalic,  "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultShadow,  "b", rb_bool_new)
DEF_KLASS_PROP(Font, bool, DefaultOutline, "b", rb_bool_new)

RB_METHOD(FontGetDefaultOutColor)
{
  RB_UNUSED_PARAM;
  return rb_iv_get(self, "default_out_color");
}

RB_METHOD(FontSetDefaultOutColor)
{
  RB_UNUSED_PARAM;
  VALUE colorObj;
  rb_get_args(argc, argv, "o", &colorObj RB_ARG_END);
  Color *c = getPrivateDataCheck<Color>(colorObj, ColorType);
  Font::setDefaultOutColor(*c);
  return colorObj;
}

static VALUE FontGetDefaultName(VALUE self)
{
  return rb_iv_get(self, "default_name");
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

RB_METHOD(FontGetDefaultColor)
{
  RB_UNUSED_PARAM;
  return rb_iv_get(self, "default_color");
}

RB_METHOD(FontSetDefaultColor)
{
  RB_UNUSED_PARAM;
  VALUE colorObj;
  rb_get_args(argc, argv, "o", &colorObj RB_ARG_END);
  Color *c = getPrivateDataCheck<Color>(colorObj, ColorType);
  Font::setDefaultColor(*c);
  return colorObj;
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
  rb_define_singleton_method(klass, "default_name", RUBY_METHOD_FUNC(FontGetDefaultName), 0);
  rb_define_singleton_method(klass, "default_name=", RUBY_METHOD_FUNC(FontSetDefaultName), -1);
  rb_define_singleton_method(klass, "default_size", RUBY_METHOD_FUNC(FontGetDefaultSize), 0);
  rb_define_singleton_method(klass, "default_size=", RUBY_METHOD_FUNC(FontSetDefaultSize), 1);
  rb_define_singleton_method(klass, "default_bold", RUBY_METHOD_FUNC(FontGetDefaultBold), -1);
  rb_define_singleton_method(klass, "default_bold=", RUBY_METHOD_FUNC(FontSetDefaultBold), -1);
  rb_define_singleton_method(klass, "default_italic", RUBY_METHOD_FUNC(FontGetDefaultItalic), -1);
  rb_define_singleton_method(klass, "default_italic=", RUBY_METHOD_FUNC(FontSetDefaultItalic), -1);
  rb_define_singleton_method(klass, "default_color", RUBY_METHOD_FUNC(FontGetDefaultColor), -1);
  rb_define_singleton_method(klass, "default_color=", RUBY_METHOD_FUNC(FontSetDefaultColor), -1);
  rb_define_singleton_method(klass, "default_outline", RUBY_METHOD_FUNC(FontGetDefaultOutline), -1);
  rb_define_singleton_method(klass, "default_outline=", RUBY_METHOD_FUNC(FontSetDefaultOutline), -1);
  rb_define_singleton_method(klass, "default_out_color", RUBY_METHOD_FUNC(FontGetDefaultOutColor), -1);
  rb_define_singleton_method(klass, "default_out_color=", RUBY_METHOD_FUNC(FontSetDefaultOutColor), -1);
  rb_define_singleton_method(klass, "default_shadow", RUBY_METHOD_FUNC(FontGetDefaultShadow), -1);
  rb_define_singleton_method(klass, "default_shadow=", RUBY_METHOD_FUNC(FontSetDefaultShadow), -1);
  rb_define_class_method(klass, "exist?", fontDoesExist);
  _rb_define_method(klass, "initialize",      fontInitialize);
  _rb_define_method(klass, "initialize_copy", fontInitializeCopy);
  rb_define_method(klass, "name", RUBY_METHOD_FUNC(FontGetName), 0);
  rb_define_method(klass, "name=", RUBY_METHOD_FUNC(FontSetName), -1);
  rb_define_method(klass, "size", RUBY_METHOD_FUNC(FontGetSize), 0);
  rb_define_method(klass, "size=", RUBY_METHOD_FUNC(FontSetSize), 1);
  rb_define_method(klass, "bold", RUBY_METHOD_FUNC(FontGetBold), 0);
  rb_define_method(klass, "bold=", RUBY_METHOD_FUNC(FontSetBold), 1);
  rb_define_method(klass, "italic", RUBY_METHOD_FUNC(FontGetItalic), 0);
  rb_define_method(klass, "italic=", RUBY_METHOD_FUNC(FontSetItalic), 1);
  rb_define_method(klass, "shadow", RUBY_METHOD_FUNC(FontGetShadow), 0);
  rb_define_method(klass, "shadow=", RUBY_METHOD_FUNC(FontSetShadow), 1);
  rb_define_method(klass, "outline", RUBY_METHOD_FUNC(FontGetOutline), 0);
  rb_define_method(klass, "outline=", RUBY_METHOD_FUNC(FontSetOutline), 1);
  INIT_PROP_BIND(Font, OutColor, "out_color");
  INIT_PROP_BIND(Font, Color, "color");
}

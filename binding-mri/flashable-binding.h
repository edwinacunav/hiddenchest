/*
** flashable-binding.h
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

#ifndef FLASHABLEBINDING_H
#define FLASHABLEBINDING_H

#include "flashable.h"
#include "binding-util.h"
#include "binding-types.h"

template<class C>
static VALUE flashableFlash(VALUE self, VALUE colorObj, VALUE rduration)
{
  Flashable *f = static_cast<C*>(RTYPEDDATA_DATA(self));
  int duration = RB_FIX2INT(rduration);
  Color *color;
  if (RB_NIL_P(colorObj)) {
    f->flash(0, duration);
  } else {
    color = getPrivateDataCheck<Color>(colorObj, ColorType);
    f->flash(&color->norm, duration);
  }
  return Qnil;
}

template<class C>
static VALUE flashableUpdate(VALUE self)
{
  Flashable *f = static_cast<C*>(RTYPEDDATA_DATA(self));
  f->update();
  return Qnil;
}

template<class C>
static void flashableBindingInit(VALUE klass)
{
  rb_define_method(klass, "flash", RUBY_METHOD_FUNC(flashableFlash<C>), 2);
  rb_define_method(klass, "update", RUBY_METHOD_FUNC(flashableUpdate<C>), 0);
}

#endif // FLASHABLEBINDING_H

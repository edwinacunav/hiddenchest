/*
** sceneelement-binding.h
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

#ifndef SCENEELEMENTBINDING_H
#define SCENEELEMENTBINDING_H

#include "scene.h"
#include "binding-util.h"

template<class C>
static VALUE sceneElementGetZ(VALUE self)
{
  SceneElement *se = static_cast<C*>(RTYPEDDATA_DATA(self));
  if (se == 0) return RB_INT2FIX(0);
  int value = 0;
  GUARD_EXC( value = se->getZ(); );
  return rb_fix_new(value);
}

template<class C>
static VALUE sceneElementSetZ(VALUE self, VALUE rz)
{
  SceneElement *se = static_cast<C*>(RTYPEDDATA_DATA(self));
  if (se == 0) return RB_INT2FIX(0);
  int z = RB_FIX2INT(rz);
  GUARD_EXC( se->setZ(z); );
  return rb_fix_new(se->getZ());
}

template<class C>
static VALUE sceneElementGetVisible(VALUE self)
{
  SceneElement *se = static_cast<C*>(RTYPEDDATA_DATA(self));
  if (se == 0) return Qfalse;
  bool value = false;
  GUARD_EXC( value = se->getVisible(); );
  return value ? Qtrue : Qfalse;
}

template<class C>
static VALUE sceneElementSetVisible(VALUE self, VALUE bln)
{
  SceneElement *se = static_cast<C*>(RTYPEDDATA_DATA(self));
  if (se == 0) return Qfalse;
  bool visible = bln == Qtrue ? true : false;
  GUARD_EXC( se->setVisible(visible); );
  return se->getVisible() ? Qtrue : Qfalse;
}

template<class C>
void sceneElementBindingInit(VALUE klass)
{
  rb_define_method(klass, "z", RUBY_METHOD_FUNC(sceneElementGetZ<C>), 0);
  rb_define_method(klass, "z=", RUBY_METHOD_FUNC(sceneElementSetZ<C>), 1);
  rb_define_method(klass, "visible", RUBY_METHOD_FUNC(sceneElementGetVisible<C>), 0);
  rb_define_method(klass, "visible=", RUBY_METHOD_FUNC(sceneElementSetVisible<C>), 1);
}

#endif // SCENEELEMENTBINDING_H

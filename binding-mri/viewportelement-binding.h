/*
** viewportelement-binding.h
**
** This file is part of HiddenChest and mkxp.
**
** mkxp Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** 2019 Extended by Kyonides Arkanthes
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
** along with HiddenChest.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VIEWPORTELEMENTBINDING_H
#define VIEWPORTELEMENTBINDING_H
// Included viewport= in RGSS1
#include "viewport.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "sceneelement-binding.h"
#include "disposable-binding.h"

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
static C * viewportElementInitialize(int argc, VALUE *v, VALUE self)
{ // Get parameters
  VALUE rbView = Qnil;
  Viewport *viewport = 0;
  rb_get_args(argc, v, "|o", &rbView RB_ARG_END);
  if (!RB_NIL_P(rbView)) {
    viewport = getPrivateDataCheck<Viewport>(rbView, ViewportType);
    if (rgssVer == 1) disposableAddChild(rbView, self);
  } // Construct object
  C *ve = new C(viewport);
  rb_iv_set(self, "viewport", rbView); // Set property objects
  return ve;
}

template<class C>
void viewportElementBindingInit(VALUE klass)
{
  sceneElementBindingInit<C>(klass);
  rb_define_method(klass, "viewport", RUBY_METHOD_FUNC(viewportElementGetViewport<C>), 0);
  rb_define_method(klass, "viewport=", RUBY_METHOD_FUNC(viewportElementSetViewport<C>), 1);
}

#endif // VIEWPORTELEMENTBINDING_H

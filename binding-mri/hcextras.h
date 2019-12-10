/*
** hcextras.h
**
** This file is part of HiddenChest.
**
** Copyright (C) 2019 Kyonides-Arkanthes <kyonides@gmail.com>
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

#ifndef HCEXTRAS_H
#define HCEXTRAS_H

#include <ruby.h>

static VALUE hc_sym(const char* str)
{
  return rb_id2sym(rb_intern(str));
}

static VALUE hc_sym2(VALUE str)
{
  return rb_id2sym(rb_intern_str(str));
}

void safe_mkdir(VALUE dir);

#define RMF(func) ((VALUE (*)(ANYARGS))(func))

#endif

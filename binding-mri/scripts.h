/*
** scripts.h
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

#ifndef SCRIPTS_H
#define SCRIPTS_H

static VALUE scripts_main_index_set(VALUE self, VALUE pos)
{
  return rb_iv_set(self, "@main_index", pos);
}

static VALUE find_main_script_index(VALUE self)
{
  VALUE ary, name, main_section = rb_iv_get(self, "@main_section_name");
  VALUE scripts = rb_gv_get("$RGSS_SCRIPTS");
  int pos = 89, max = RARRAY_LEN(scripts);
  for (int n = 0; n < max; n++) {
    ary = rb_ary_entry(scripts, n);
    name = rb_ary_entry(ary, 1);
    if (!rb_str_cmp(name, main_section)) {
      pos = n;
      break;
    }
  }
  if (pos >= max) pos = max - 1;
  return RB_INT2FIX(pos);
}

#endif

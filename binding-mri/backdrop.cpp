/*
** backdrop.cpp
**
** This file is part of HiddenChest
**
** Copyright (C) 2018-2019 Kyonides-Arkanthes
*/

#include <ruby.h>
#include "bitmap.h"
#include "graphics.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "disposable-binding.h"

static VALUE terms_critical_hit_get(VALUE self)
{
  rb_iv_get(self, "critical_hit");
}

static VALUE terms_critical_hit_set(VALUE self, VALUE str)
{
  rb_iv_set(self, "critical_hit", str);
}

static VALUE backdrop_keep_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snapToBitmap(); );
  VALUE bitmap = wrapObject(result, BitmapType);
  rb_iv_set(self, "@bitmap", bitmap);
  return bitmap;
}

static VALUE backdrop_blur_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snapToBitmap(); );
  result->blur();
  VALUE bitmap = wrapObject(result, BitmapType);
  rb_iv_set(self, "@bitmap", bitmap);
  return bitmap;
}

static VALUE backdrop_bitmap(VALUE self)
{
  return rb_iv_get(self, "@bitmap");
}

static VALUE backdrop_clear_bitmap(VALUE self)
{
  VALUE bitmap = rb_iv_get(self, "@bitmap");
  Bitmap *d = getPrivateData<Bitmap>(bitmap);
  if (!d || d->isDisposed()) rb_iv_set(self, "@bitmap", Qnil);
  if (rgssVer == 1)
    disposableDisposeChildren(bitmap);
  d->dispose();
  return rb_iv_set(self, "@bitmap", Qnil);
}

#define RMF(func) ((VALUE (*)(ANYARGS))(func))

void Init_TermsBackdrop()
{
  VALUE terms = rb_define_module("Terms");
  rb_iv_set(terms, "critical_hit", rb_str_new_cstr("CRITICAL"));
  rb_define_module_function(terms, "critical_hit", RMF(terms_critical_hit_get), 0);
  rb_define_module_function(terms, "critical_hit=", RMF(terms_critical_hit_set), 1);
  VALUE module = rb_define_module("Backdrop");
  rb_iv_set(module, "@bitmap", Qnil);
  rb_define_module_function(module, "keep_bitmap", RMF(backdrop_keep_bitmap), 0);
  rb_define_module_function(module, "blur_bitmap", RMF(backdrop_blur_bitmap), 0);
  rb_define_module_function(module, "bitmap", RMF(backdrop_bitmap), 0);
  rb_define_module_function(module, "clear_bitmap", RMF(backdrop_clear_bitmap), 0);
}
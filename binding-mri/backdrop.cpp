/*
** backdrop.cpp
**
** This file is part of HiddenChest
**
** Copyright (C) 2018 Kyonides-Arkanthes
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

static VALUE module_attr_accessor(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_attr_accessor must be called for modules");
  int m, count = argc*2;
  VALUE args[count], basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    m = n * 2;
    args[m] = argv[n];
    basic_str = rb_sym_to_s(argv[n]);
    rb_io_puts(1, &basic_str, rb_stdout);
    const char* func_name = StringValueCStr(basic_str);
    rb_define_attr(self, func_name, 1, 1);
    new_str = rb_str_plus(basic_str, rb_str_new_cstr("="));
    args[m+1] = rb_id2sym( rb_intern_str(new_str) );
    rb_io_puts(1, &args[m+1], rb_stdout);
  }
  VALUE meth = rb_obj_method(self, rb_id2sym(rb_intern("module_function")));
  rb_method_call(count, args, meth);
  return Qnil;
}

#define RMF(func) ((VALUE (*)(ANYARGS))(func))

void Init_TermsBackdrop()
{
  rb_define_method(rb_cModule, "module_attr_accessor", RMF(module_attr_accessor), -1);
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
/*
** backdrop.cpp
**
** This file is part of HiddenChest
**
** Copyright (C) 2018-2019 Kyonides-Arkanthes
*/

#include "hcextras.h"
#include "bitmap.h"
#include "graphics.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "disposable-binding.h"

static VALUE settings_image_format_get(VALUE self)
{
  return rb_iv_get(self, "@image_format");
}

static VALUE settings_image_format_set(VALUE self, VALUE fmt)
{
  int form;
  if (fmt == hc_sym("jpg") || fmt == RB_INT2FIX(0)) form = 0;
  if (fmt == hc_sym("png") || fmt == RB_INT2FIX(1)) form = 1;
  GUARD_EXC( shState->graphics().set_screenshot_format(form); );
  return rb_iv_set(self, "@image_format", fmt);
}

static VALUE settings_snapshot_dir_get(VALUE self)
{
  return rb_iv_get(self, "@snapshot_dir");
}

static VALUE settings_snapshot_dir_set(VALUE self, VALUE sdir)
{
  char* dir = StringValueCStr(sdir);
  GUARD_EXC( shState->graphics().set_screenshot_dir(dir); );
  return rb_iv_set(self, "@snapshot_dir", sdir);
}

static VALUE settings_snapshot_filename_get(VALUE self)
{
  return rb_iv_get(self, "@snapshot_filename");
}

static VALUE settings_snapshot_filename_set(VALUE self, VALUE name)
{
  char* fn = StringValueCStr(name);
  GUARD_EXC( shState->graphics().set_screenshot_fn(fn); );
  return rb_iv_set(self, "@snapshot_filename", name);
}

static VALUE settings_save_dir_get(VALUE self)
{
  return rb_iv_get(self, "@save_dir");
}

static VALUE settings_save_dir_set(VALUE self, VALUE dir)
{
  return rb_iv_set(self, "@save_dir", dir);
}

static VALUE settings_save_filename_get(VALUE self)
{
  return rb_iv_get(self, "@save_filename");
}

static VALUE settings_save_filename_set(VALUE self, VALUE dir)
{
  return rb_iv_set(self, "@save_filename", dir);
}

static VALUE settings_auto_create_dirs(VALUE self)
{
  safe_mkdir(rb_iv_get(self, "@snapshot_dir"));
  safe_mkdir(rb_iv_get(self, "@save_dir"));
  return Qtrue;
}

static VALUE terms_critical_hit_get(VALUE self)
{
  return rb_iv_get(self, "@critical_hit");
}

static VALUE terms_critical_hit_set(VALUE self, VALUE str)
{
  return rb_iv_set(self, "@critical_hit", str);
}

static VALUE backdrop_keep_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snapToBitmap(); );
  VALUE bitmap = wrapObject(result, BitmapType);
  return rb_iv_set(self, "@bitmap", bitmap);
}

static VALUE backdrop_gray_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snap_to_gray_bitmap(); );
  VALUE bitmap = wrapObject(result, BitmapType);
  return rb_iv_set(self, "@bitmap", bitmap);
}

static VALUE backdrop_sepia_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snap_to_sepia_bitmap(); );
  VALUE bitmap = wrapObject(result, BitmapType);
  return rb_iv_set(self, "@bitmap", bitmap);
}

static VALUE backdrop_color_bitmap(VALUE self, VALUE color)
{
  Bitmap *b = 0;
  if (color == hc_sym("red"))
    GUARD_EXC( b = shState->graphics().snap_to_color_bitmap(0); )
  else if (color == hc_sym("green"))
    GUARD_EXC( b = shState->graphics().snap_to_color_bitmap(1); )
  else if (color == hc_sym("blue"))
    GUARD_EXC( b = shState->graphics().snap_to_color_bitmap(2); )
  else if (color == hc_sym("yellow"))
    GUARD_EXC( b = shState->graphics().snap_to_color_bitmap(3); )
  else if (color == hc_sym("sepia"))
    GUARD_EXC( b = shState->graphics().snap_to_sepia_bitmap(); )
  else if (color == hc_sym("gray"))
    GUARD_EXC( b = shState->graphics().snap_to_gray_bitmap(); )
  VALUE bitmap = wrapObject(b, BitmapType);
  return rb_iv_set(self, "@bitmap", bitmap);
}

static VALUE backdrop_blur_bitmap(VALUE self)
{
  Bitmap *result = 0;
  GUARD_EXC( result = shState->graphics().snapToBitmap(); );
  result->blur();
  VALUE bitmap = wrapObject(result, BitmapType);
  return rb_iv_set(self, "@bitmap", bitmap);
}

static VALUE backdrop_bitmap(VALUE self)
{
  return rb_iv_get(self, "@bitmap");
}

static VALUE backdrop_bitmap_dup(VALUE self)
{
  VALUE img = rb_iv_get(self, "@bitmap");
  return rb_obj_dup(img);
}

static VALUE backdrop_clear_bitmap(VALUE self)
{
  VALUE bitmap = rb_iv_get(self, "@bitmap");
  Bitmap *d = getPrivateData<Bitmap>(bitmap);
  if (!d || d->isDisposed()) rb_iv_set(self, "@bitmap", Qnil);
  if (rgssVer == 1) disposableDisposeChildren(bitmap);
  d->dispose();
  return rb_iv_set(self, "@bitmap", Qnil);
}

void Init_terms_backdrop()
{
  VALUE set = rb_define_module("Settings");
  rb_iv_set(set, "@save_dir", rb_str_new_cstr("Saves"));
  rb_iv_set(set, "@save_filename", rb_str_new_cstr("Save"));
  settings_image_format_set(set, hc_sym("jpg"));
  settings_snapshot_dir_set(set, rb_str_new_cstr("Screenshots"));
  settings_snapshot_filename_set(set, rb_str_new_cstr("screenshot"));
  rb_define_module_function(set, "image_format", RMF(settings_image_format_get), 0);
  rb_define_module_function(set, "image_format=", RMF(settings_image_format_set), 1);
  rb_define_module_function(set, "snapshot_dir", RMF(settings_snapshot_dir_get), 0);
  rb_define_module_function(set, "snapshot_dir=", RMF(settings_snapshot_dir_set), 1);
  rb_define_module_function(set, "snapshot_filename", RMF(settings_snapshot_filename_get), 0);
  rb_define_module_function(set, "snapshot_filename=", RMF(settings_snapshot_filename_set), 1);
  rb_define_module_function(set, "save_dir", RMF(settings_save_dir_get), 0);
  rb_define_module_function(set, "save_dir=", RMF(settings_save_dir_set), 1);
  rb_define_module_function(set, "save_filename", RMF(settings_save_filename_get), 0);
  rb_define_module_function(set, "save_filename=", RMF(settings_save_filename_set), 1);
  rb_define_module_function(set, "auto_create_dirs", RMF(settings_auto_create_dirs), 0);
  VALUE terms = rb_define_module("Terms");
  rb_iv_set(terms, "@critical_hit", rb_str_new_cstr("CRITICAL"));
  rb_define_module_function(terms, "critical_hit", RMF(terms_critical_hit_get), 0);
  rb_define_module_function(terms, "critical_hit=", RMF(terms_critical_hit_set), 1);
  VALUE module = rb_define_module("Backdrop");
  rb_iv_set(module, "@bitmap", Qnil);
  rb_define_module_function(module, "keep_bitmap", RMF(backdrop_keep_bitmap), 0);
  rb_define_module_function(module, "gray_bitmap", RMF(backdrop_gray_bitmap), 0);
  rb_define_module_function(module, "sepia_bitmap", RMF(backdrop_sepia_bitmap), 0);
  rb_define_module_function(module, "color_bitmap", RMF(backdrop_color_bitmap), 1);
  rb_define_module_function(module, "blur_bitmap", RMF(backdrop_blur_bitmap), 0);
  rb_define_module_function(module, "bitmap", RMF(backdrop_bitmap), 0);
  rb_define_module_function(module, "bitmap_dup", RMF(backdrop_bitmap_dup), 0);
  rb_define_module_function(module, "clear_bitmap", RMF(backdrop_clear_bitmap), 0);
}

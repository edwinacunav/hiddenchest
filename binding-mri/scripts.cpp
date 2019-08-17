/*
** scripts.cpp
**
** This file is part of HiddenChest
**
** Copyright (C) 2018-2019 Kyonides-Arkanthes
*/

#include "hcsymbol.h"

#define CMF(func) ((int (*)(ANYARGS))(func))

static int get_keys(VALUE key, VALUE value, VALUE ary)
{
  rb_ary_push(ary, key);
  return 0;
}

static VALUE rb_hash_keys(VALUE hash)
{
  VALUE keys = rb_ary_new();
  rb_hash_foreach(hash, CMF(get_keys), keys);
  return keys;
}

static VALUE scripts_all(VALUE self)
{
  return rb_iv_get(self, "@pack");
}

static VALUE scripts_names(VALUE self)
{
  return rb_hash_keys(rb_iv_get(self, "@pack"));
}

static VALUE scripts_dependencies(VALUE self)
{
  VALUE pack, keys, result;
  pack = rb_iv_get(self, "@pack");
  keys = rb_hash_keys(pack);
  result = rb_ary_new();
  int max = RARRAY_LEN(keys);
  for (int i = 0; i < max; i++)
    rb_ary_push(result, rb_hash_aref(pack, rb_ary_entry(keys, i)));
  return result;
}

static VALUE scripts_is_included(VALUE self, VALUE name)
{
  VALUE keys = rb_hash_keys(rb_iv_get(self, "@pack"));
  return rb_ary_includes(keys, name);
}

static VALUE scripts_get(VALUE self, VALUE name)
{
  return rb_hash_aref(rb_iv_get(self, "@pack"), name);
}

static VALUE scripts_set(VALUE self, VALUE name, VALUE dependencies)
{
  return rb_hash_aset(rb_iv_get(self, "@pack"), name, dependencies);
}

static VALUE scripts_reset(VALUE self)
{
  VALUE start_scene, new_scene, this_scene;
  start_scene = rb_iv_get(self, "@start_scene");
  new_scene = rb_funcall(start_scene, rb_intern("new"), 0);
  this_scene = rb_gv_set("$scene", new_scene);
  return Qnil;
}

static VALUE scripts_scene_is_running(VALUE self)
{
  return rb_iv_get(self, "@run");
}

static VALUE scripts_scene_stop(VALUE self)
{
  rb_iv_set(self, "@run", Qnil);
  return rb_iv_set(self, "@scene", Qnil);
}

static VALUE scripts_scene_get(VALUE self)
{
  return rb_iv_get(self, "@scene");
}

static VALUE scripts_scene_set(VALUE self, VALUE name)
{
  return rb_iv_set(self, "@scene", name);
}

static VALUE scripts_start_scene_get(VALUE self)
{
  return rb_iv_get(self, "@start_scene");
}

static VALUE scripts_start_scene_set(VALUE self, VALUE name)
{
  return rb_iv_set(self, "@start_scene", name);
}

static VALUE module_attr_accessor(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_attr_accessor is exclusive for modules!");
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
    args[m + 1] = hc_str2sym(new_str);
  }
  VALUE meth = rb_obj_method(self, hc_symbol("module_function"));
  rb_method_call(count, args, meth);
  return Qnil;
}

void Init_Scripts()
{
  VALUE module = rb_define_module("Scripts");
  rb_iv_set(module, "@run", Qtrue);
  rb_iv_set(module, "@pack", rb_hash_new());
  rb_iv_set(module, "@start_scene", Qnil);
  rb_define_module_function(module, "all", RMF(scripts_all), 0);
  rb_define_module_function(module, "names", RMF(scripts_names), 0);
  rb_define_module_function(module, "dependencies", RMF(scripts_dependencies), 0);
  rb_define_module_function(module, "include?", RMF(scripts_is_included), 1);
  rb_define_module_function(module, "[]", RMF(scripts_get), 1);
  rb_define_module_function(module, "[]=", RMF(scripts_set), 2);
  rb_define_module_function(module, "reset", RMF(scripts_reset), 0);
  rb_define_module_function(module, "run?", RMF(scripts_scene_is_running), 0);
  rb_define_module_function(module, "stop", RMF(scripts_scene_stop), 0);
  rb_define_module_function(module, "start_scene", RMF(scripts_start_scene_get), 0);
  rb_define_module_function(module, "start_scene=", RMF(scripts_start_scene_set), 1);
  rb_define_module_function(module, "scene", RMF(scripts_scene_get), 0);
  rb_define_module_function(module, "scene=", RMF(scripts_scene_set), 1);
  rb_define_method(rb_cModule, "module_attr_accessor", RMF(module_attr_accessor), -1);
  rb_define_method(rb_cModule, "module_accessor", RMF(module_attr_accessor), -1);
}
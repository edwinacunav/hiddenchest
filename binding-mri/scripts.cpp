/*
** scripts.cpp
**
** This file is part of HiddenChest
**
** Copyright (C) 2018-2019 Kyonides-Arkanthes
*/

#include "hcsymbol.h"
#include "scripts.h"

#define CRMF(func) ((int (*)(ANYARGS))(func))

static int get_keys(VALUE key, VALUE value, VALUE ary)
{
  rb_ary_push(ary, key);
  return 0;
}

static VALUE rb_hash_keys(VALUE hash)
{
  VALUE keys = rb_ary_new();
  rb_hash_foreach(hash, CRMF(get_keys), keys);
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

static VALUE scripts_scene_get(VALUE self)
{
  return rb_iv_get(self, "@scene");
}

static VALUE scripts_scene_set(VALUE self, VALUE name)
{
  return rb_iv_set(self, "@scene", name);
}

static VALUE scripts_main_index_get(VALUE self)
{
  return rb_iv_get(self, "@main_index");
}

static VALUE scripts_main_section_name_get(VALUE self)
{
  return rb_iv_get(self, "@main_section_name");
}

static VALUE scripts_main_section_name_set(VALUE self, VALUE name)
{
  return rb_iv_set(self, "@main_section_name", name);
}

static VALUE module_reader(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_reader is exclusive for modules!");
  VALUE args[argc], basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    rb_define_attr(self, func_name, 1, 0);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(argc, argv, meth);
  return Qnil;
}

static VALUE module_writer(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_writer is exclusive for modules!");
  VALUE basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    new_str = rb_str_plus(basic_str, rb_str_new_cstr("="));
    argv[n] = hc_sym2(new_str);
    rb_define_attr(self, func_name, 0, 1);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(argc, argv, meth);
  return Qnil;
}

static VALUE module_accessor(int argc, VALUE* argv, VALUE self)
{
  if (!RB_TYPE_P(self, T_MODULE))
    rb_raise(rb_eTypeError, "module_accessor is exclusive for modules!");
  int m, count = argc*2;
  VALUE args[count], basic_str, new_str;
  for (int n = 0 ; n < argc ; n++) {
    m = n * 2;
    args[m] = argv[n];
    basic_str = rb_sym_to_s(argv[n]);
    const char* func_name = StringValueCStr(basic_str);
    rb_define_attr(self, func_name, 1, 1);
    new_str = rb_str_plus(basic_str, rb_str_new_cstr("="));
    args[m + 1] = hc_sym2(new_str);
  }
  VALUE meth = rb_obj_method(self, hc_sym("module_function"));
  rb_method_call(count, args, meth);
  return Qnil;
}//extern "C"

void Init_scripts()
{
  VALUE module = rb_define_module("Scripts");
  rb_iv_set(module, "@pack", rb_hash_new());
  rb_iv_set(module, "@main_section_name", rb_str_new_cstr("Main"));
  rb_iv_set(module, "@main_index", RB_INT2FIX(0));
  rb_define_module_function(module, "all", RMF(scripts_all), 0);
  rb_define_module_function(module, "names", RMF(scripts_names), 0);
  rb_define_module_function(module, "dependencies", RMF(scripts_dependencies), 0);
  rb_define_module_function(module, "include?", RMF(scripts_is_included), 1);
  rb_define_module_function(module, "[]", RMF(scripts_get), 1);
  rb_define_module_function(module, "[]=", RMF(scripts_set), 2);
  rb_define_module_function(module, "main_index", RMF(scripts_main_index_get), 0);
  rb_define_module_function(module, "main_index=", RMF(scripts_main_index_set), 1);
  rb_define_module_function(module, "main_section_name", RMF(scripts_main_section_name_get), 0);
  rb_define_module_function(module, "main_section_name=", RMF(scripts_main_section_name_set), 1);
  rb_define_module_function(module, "scene", RMF(scripts_scene_get), 0);
  rb_define_module_function(module, "scene=", RMF(scripts_scene_set), 1);
  rb_define_method(rb_cModule, "module_reader", RMF(module_reader), -1);
  rb_define_method(rb_cModule, "module_writer", RMF(module_writer), -1);
  rb_define_method(rb_cModule, "module_accessor", RMF(module_accessor), -1);
  rb_define_method(rb_cModule, "mod_reader", RMF(module_reader), -1);
  rb_define_method(rb_cModule, "mod_writer", RMF(module_writer), -1);
  rb_define_method(rb_cModule, "mod_accessor", RMF(module_accessor), -1);
}
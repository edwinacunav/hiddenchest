/*
** audio-binding.cpp
**
** This file is part of hiddenchest and mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** 2018 (C) Modified by Kyonides Arkanthes <kyonides@gmail.com>
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

#include "audio.h"
#include "sharedstate.h"
#include "exception.h"
#include "binding-util.h"
#include "filesystem.h"

static VALUE audio_bgmPlay(int argc, VALUE* argv, VALUE self)
{
  const char *filename;
  int volume = 100;
  int pitch = 100;
  double pos = 0.0;
  rb_iv_set(self, "@old_bgm_name", argv[0]);
  rb_get_args(argc, argv, "z|iif", &filename, &volume, &pitch, &pos RB_ARG_END);
  GUARD_EXC( shState->audio().bgmPlay(filename, volume, pitch, pos); )
  return Qnil;
}

static VALUE audio_bgmStop(VALUE self)
{
  shState->audio().bgmStop();
  return Qnil;
}

static VALUE audio_bgmPos(VALUE self)
{
  VALUE pos = rb_float_new(shState->audio().bgmPos());
  return rb_iv_set(self, "@bgm_pos", pos);
}

static VALUE audio_bgsPlay(int argc, VALUE* argv, VALUE self)
{
  const char *filename;
  int volume = 100;
  int pitch = 100;
  double pos = 0.0;
  rb_get_args(argc, argv, "z|iif", &filename, &volume, &pitch, &pos RB_ARG_END);
  GUARD_EXC( shState->audio().bgsPlay(filename, volume, pitch, pos); )
  return Qnil;
}

static VALUE audio_bgsStop(VALUE self)
{
  shState->audio().bgsStop();
  return Qnil;
}

static VALUE audio_bgm_volume_get(VALUE self)
{
  return rb_iv_get(self, "@bgm_volume");
}

static VALUE audio_bgs_volume_get(VALUE self)
{
  return rb_iv_get(self, "@bgs_volume");
}

static VALUE audio_se_volume_get(VALUE self)
{
  return rb_iv_get(self, "@se_volume");
}

static VALUE audio_me_volume_get(VALUE self)
{
  return rb_iv_get(self, "@me_volume");
}

static VALUE audio_bgm_volume_set(VALUE self, VALUE volume)
{
  return rb_iv_set(self, "@bgm_volume", volume);
}

static VALUE audio_bgs_volume_set(VALUE self, VALUE volume)
{
  return rb_iv_set(self, "@bgs_volume", volume);
}

static VALUE audio_se_volume_set(VALUE self, VALUE volume)
{
  return rb_iv_set(self, "@se_volume", volume);
}

static VALUE audio_me_volume_set(VALUE self, VALUE volume)
{
  return rb_iv_set(self, "@me_volume", volume);
}

static VALUE audio_bgsPos(VALUE self)
{
  VALUE pos = rb_float_new(shState->audio().bgsPos());
  return rb_iv_set(self, "@bgs_pos", pos);
}

static VALUE audio_old_bgm_pos_get(VALUE self)
{
  return rb_iv_get(self, "@old_bgm_pos");
}

static VALUE audio_old_bgs_pos_get(VALUE self)
{
  return rb_iv_get(self, "@old_bgs_pos");
}

static VALUE audio_old_bgm_name(VALUE self)
{
  return rb_iv_get(self, "@old_bgm_name");
}

static VALUE audio_old_bgs_name(VALUE self)
{
  return rb_iv_get(self, "@old_bgs_name");
}

static VALUE audio_save_bgm_data(VALUE self)
{
  rb_iv_set(self, "@old_bgm_name", audio_bgmPos(self));
  rb_iv_set(self, "@old_bgm_pos", audio_bgmPos(self));
  return true;
}

static VALUE audio_save_bgs_data(VALUE self)
{
  rb_iv_set(self, "@old_bgs_pos", audio_bgsPos(self));
  return true;
}

void raiseRbExc(const Exception &exc);

static VALUE audio_play_se(int argc, VALUE* argv, VALUE self)
{
  VALUE se = rb_iv_get(self, "@se"), name, volume, pitch;
  rb_scan_args(argc, argv, "12", &name, &volume, &pitch);
  if (RB_NIL_P(name) || RSTRING_LEN(name) == 0) return Qnil;
  name = rb_str_plus(rb_str_new_cstr("Audio/SE/"), name);
  const char *fn = StringValueCStr(name);
  int vol = RB_NIL_P(volume) ? RB_FIX2INT(rb_iv_get(self, "@se_volume")) :
            RB_FIX2INT(volume);
  int pit = RB_NIL_P(pitch) ? 100 : RB_FIX2INT(pitch);
  try {
    shState->audio().sePlay(fn, vol, pit);
  } catch (const Exception &e) { raiseRbExc(e); }
  return Qnil;
}

static VALUE audio_sePlay(int argc, VALUE* argv, VALUE self)
{
  const char *filename;
  int volume = 100;
  int pitch = 100;
  rb_get_args(argc, argv, "z|ii", &filename, &volume, &pitch RB_ARG_END);
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_seStop(VALUE self)
{
  shState->audio().seStop();
  return Qnil;
}

static VALUE audio_mePlay(int argc, VALUE* argv, VALUE self)
{
  const char *filename;
  int volume = 100;
  int pitch = 100;
  rb_get_args(argc, argv, "z|ii", &filename, &volume, &pitch RB_ARG_END);
  GUARD_EXC( shState->audio().mePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_meStop(VALUE self)
{
  shState->audio().meStop();
  return Qnil;
}

static VALUE audio_bgmFade(int argc, VALUE* argv, VALUE self)
{
  int time;
  rb_get_args(argc, argv, "i", &time RB_ARG_END);
  shState->audio().bgmFade(time);
  return Qnil;
}

static VALUE audio_bgsFade(int argc, VALUE* argv, VALUE self)
{
  int time;
  rb_get_args(argc, argv, "i", &time RB_ARG_END);
  shState->audio().bgsFade(time);
  return Qnil;
}

static VALUE audio_meFade(int argc, VALUE* argv, VALUE self)
{
  int time;
  rb_get_args(argc, argv, "i", &time RB_ARG_END);
  shState->audio().meFade(time);
  return Qnil;
}

static VALUE audioSetupMidi(VALUE self)
{
  shState->audio().setupMidi();
  return Qnil;
}

static VALUE audioReset(VALUE self)
{
  shState->audio().reset();
  return Qnil;
}

static VALUE audio_play_buzzer(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@buzzer");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_cancel(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@cancel");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_ok(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@ok");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_cursor(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@cursor");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_shop(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@shop");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_equip(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@equip");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_save(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@save");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_load(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@load");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_escape(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@escape");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_actor_ko(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@actor_ko");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE audio_play_enemy_ko(VALUE self)
{
  VALUE ose = rb_iv_get(self, "@se");
  VALUE se = rb_iv_get(self, "@enemy_ko");
  VALUE se_name = rb_str_plus(ose, rb_iv_get(se, "@name"));
  const char *filename = StringValueCStr(se_name);
  int volume = RB_FIX2INT(rb_iv_get(se, "@volume"));
  int pitch = RB_FIX2INT(rb_iv_get(se, "@pitch"));
  GUARD_EXC( shState->audio().sePlay(filename, volume, pitch); )
  return Qnil;
}

static VALUE rpg_audio_file_initialize(int argc, VALUE* argv, VALUE self)
{
  VALUE name, volume, pitch, pos;
  rb_scan_args(argc, argv, "04", &name, &volume, &pitch, &pos);
  if ( RB_NIL_P(name) ) name = rb_str_new_cstr("");
  if ( RB_NIL_P(volume) ) volume = RB_INT2FIX(100);
  if ( RB_NIL_P(pitch) ) pitch = RB_INT2FIX(100);
  if ( RB_NIL_P(pos) ) pos = rb_float_new(0.0);
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@volume", volume);
  rb_iv_set(self, "@pitch", pitch);
  rb_iv_set(self, "@pos", pos);
  return self;
}

void audio_setup_custom_se()
{
  VALUE mod = rb_const_get(rb_cObject, rb_intern("Audio"));
  VALUE filename = rb_str_new_cstr("Data/System.rxdata");
  VALUE ds = rb_funcall(rb_mKernel, rb_intern("load_data"), 1, filename);
  if ( RB_NIL_P(ds) ) return;
  rb_p(rb_str_new_cstr("Initializing extra Audio methods now..."));
  rb_iv_set(mod, "@buzzer", rb_iv_get(ds, "@buzzer_se"));
  rb_iv_set(mod, "@cancel", rb_iv_get(ds, "@cancel_se"));
  rb_iv_set(mod, "@cursor", rb_iv_get(ds, "@cursor_se"));
  rb_iv_set(mod, "@ok", rb_iv_get(ds, "@decision_se"));
  rb_iv_set(mod, "@shop", rb_iv_get(ds, "@shop_se"));
  rb_iv_set(mod, "@equip", rb_iv_get(ds, "@equip_se"));
  rb_iv_set(mod, "@save", rb_iv_get(ds, "@save_se"));
  rb_iv_set(mod, "@load", rb_iv_get(ds, "@load_se"));
  rb_iv_set(mod, "@escape", rb_iv_get(ds, "@escape_se"));
  rb_iv_set(mod, "@actor_ko", rb_iv_get(ds, "@actor_collapse_se"));
  rb_iv_set(mod, "@enemy_ko", rb_iv_get(ds, "@enemy_collapse_se"));
}

#define RMF(func) ((VALUE (*)(ANYARGS))(func))

void audioBindingInit()
{
  VALUE md = rb_define_module("Audio");
  rb_iv_set(md, "@bgm_pos", rb_float_new(0.0));
  rb_iv_set(md, "@bgs_pos", rb_float_new(0.0));
  rb_iv_set(md, "@old_bgm_pos", rb_float_new(0.0));
  rb_iv_set(md, "@old_bgs_pos", rb_float_new(0.0));
  rb_iv_set(md, "@old_bgm_name", rb_str_new_cstr(""));
  rb_iv_set(md, "@old_bgs_name", rb_str_new_cstr(""));
  rb_iv_set(md, "@se", rb_str_new_cstr("Audio/SE/"));
  rb_iv_set(md, "@bgm_volume", RB_INT2FIX(80));
  rb_iv_set(md, "@bgs_volume", RB_INT2FIX(80));
  rb_iv_set(md, "@se_volume", RB_INT2FIX(80));
  rb_iv_set(md, "@me_volume", RB_INT2FIX(80));
  rb_define_module_function(md, "bgm_play", RMF(audio_bgmPlay), -1);
  rb_define_module_function(md, "bgm_stop", RMF(audio_bgmStop), 0);
  rb_define_module_function(md, "bgm_fade", RMF(audio_bgmFade), -1);
  rb_define_module_function(md, "bgs_play", RMF(audio_bgsPlay), -1);
  rb_define_module_function(md, "bgs_stop", RMF(audio_bgsStop), 0);
  rb_define_module_function(md, "bgs_fade", RMF(audio_bgsFade), -1);
  rb_define_module_function(md, "me_play", RMF(audio_mePlay), -1);
  rb_define_module_function(md, "me_stop", RMF(audio_meStop), 0);
  rb_define_module_function(md, "me_fade", RMF(audio_meFade), -1);
  if (rgssVer == 1) {
    rb_define_module_function(md, "play_buzzer", RMF(audio_play_buzzer), 0);
    rb_define_module_function(md, "play_cancel", RMF(audio_play_cancel), 0);
    rb_define_module_function(md, "play_cursor", RMF(audio_play_cursor), 0);
    rb_define_module_function(md, "play_decision", RMF(audio_play_ok), 0);
    rb_define_module_function(md, "play_ok", RMF(audio_play_ok), 0);
    rb_define_module_function(md, "play_shop", RMF(audio_play_shop), 0);
    rb_define_module_function(md, "play_equip", RMF(audio_play_equip), 0);
    rb_define_module_function(md, "play_save", RMF(audio_play_save), 0);
    rb_define_module_function(md, "play_load", RMF(audio_play_load), 0);
    rb_define_module_function(md, "play_escape", RMF(audio_play_escape), 0);
    rb_define_module_function(md, "play_actor_ko", RMF(audio_play_actor_ko), 0);
    rb_define_module_function(md, "play_enemy_ko", RMF(audio_play_enemy_ko), 0);
    rb_define_module_function(md, "play_se", RMF(audio_play_se), -1);
    VALUE rpg = rb_define_module("RPG");
    VALUE file = rb_define_class_under(rpg, "AudioFile", rb_cObject);
    rb_define_method(file, "initialize", RMF(rpg_audio_file_initialize), -1);
    rb_define_attr(file, "name", 1, 1);
    rb_define_attr(file, "volume", 1, 1);
    rb_define_attr(file, "pitch", 1, 1);
    rb_define_attr(file, "pos", 1, 1);
  }
  rb_define_module_function(md, "bgm_volume", RMF(audio_bgm_volume_get), 0);
  rb_define_module_function(md, "bgs_volume", RMF(audio_bgs_volume_get), 0);
  rb_define_module_function(md, "se_volume", RMF(audio_se_volume_get), 0);
  rb_define_module_function(md, "me_volume", RMF(audio_me_volume_get), 0);
  rb_define_module_function(md, "bgm_volume=", RMF(audio_bgm_volume_set), 1);
  rb_define_module_function(md, "bgs_volume=", RMF(audio_bgs_volume_set), 1);
  rb_define_module_function(md, "se_volume=", RMF(audio_se_volume_set), 1);
  rb_define_module_function(md, "me_volume=", RMF(audio_me_volume_set), 1);
  rb_define_module_function(md, "bgm_pos", RMF(audio_bgmPos), 0);
  rb_define_module_function(md, "bgs_pos", RMF(audio_bgsPos), 0);
  rb_define_module_function(md, "old_bgm_pos", RMF(audio_old_bgm_pos_get), 0);
  rb_define_module_function(md, "old_bgs_pos", RMF(audio_old_bgs_pos_get), 0);
  rb_define_module_function(md, "old_bgm_name", RMF(audio_old_bgm_name), 0);
  rb_define_module_function(md, "old_bgs_name", RMF(audio_old_bgs_name), 0);
  rb_define_module_function(md, "save_bgm_data", RMF(audio_save_bgm_data), 0);
  rb_define_module_function(md, "save_bgs_data", RMF(audio_save_bgs_data), 0);
  if (rgssVer >= 3)
    rb_define_module_function(md, "setup_midi", RMF(audioSetupMidi), 0);
  rb_define_module_function(md, "se_play", RMF(audio_sePlay), -1);
  rb_define_module_function(md, "se_stop", RMF(audio_seStop), 0);
  rb_define_module_function(md, "__reset__", RMF(audioReset), 0);
}

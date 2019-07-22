/*
** sprite-binding.cpp
**
** This file is part of HiddenChest and mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** 2019 Extended by Kyonides Arkanthes <kyonides@gmail.com>
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

#include "sprite.h"
#include "sharedstate.h"
#include "disposable-binding.h"
#include "flashable-binding.h"
#include "sceneelement-binding.h"
#include "viewportelement-binding.h"
#include "binding-util.h"
#include "binding-types.h"

rb_data_type_t SpriteType = { "Sprite",
  { 0, freeInstance<Sprite>, 0, { 0, 0 } }, 0, 0, 0 };

extern "C"
{

static VALUE spriteInitialize(int argc, VALUE* argv, VALUE self)
{
  Sprite *s = viewportElementInitialize<Sprite>(argc, argv, self);
  setPrivateData(self, s);
  s->initDynAttribs();
  wrapProperty(self, &s->getSrcRect(), "src_rect", RectType);
  wrapProperty(self, &s->getColor(), "color", ColorType);
  wrapProperty(self, &s->getTone(), "tone", ToneType);
  return self;
}

static VALUE SpriteGetBitmap(VALUE self)
{
  return rb_iv_get(self, "bitmap");
}

static VALUE SpriteSetBitmap(VALUE self, VALUE bit)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  Bitmap *b;
  if ( RB_NIL_P(bit) )
    b = 0;
  else
    b = getPrivateDataCheck<Bitmap>(bit, BitmapType);
  GUARD_EXC( s->setBitmap(b); )
  return rb_iv_set(self, "bitmap", bit);
}

static VALUE SpriteGetSrcRect(VALUE self)
{
  checkDisposed<Sprite>(self);
  return rb_iv_get(self, "src_rect");
}

static VALUE SpriteSetSrcRect(VALUE self, VALUE rect)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  Rect *r;
  if ( RB_NIL_P(rect) )
    r = 0;
  else
    r = getPrivateDataCheck<Rect>(rect, RectType);
  GUARD_EXC( s->setSrcRect(*r); )
  return rb_iv_set(self, "src_rect", rect);
}

static VALUE SpriteGetColor(VALUE self)
{
  checkDisposed<Sprite>(self);
  return rb_iv_get(self, "color");
}

static VALUE SpriteSetColor(VALUE self, VALUE color)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  Color *c;
  if ( RB_NIL_P(color) )
    c = 0;
  else
    c = getPrivateDataCheck<Color>(color, ColorType);
  GUARD_EXC( s->setColor(*c); )
  return rb_iv_set(self, "color", color);
}

static VALUE SpriteGetTone(VALUE self)
{
  checkDisposed<Sprite>(self);
  return rb_iv_get(self, "tone");
}

static VALUE SpriteSetTone(VALUE self, VALUE tone)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  Tone *t;
  if ( RB_NIL_P(tone) )
    t = 0;
  else
    t = getPrivateDataCheck<Tone>(tone, ToneType);
  GUARD_EXC( s->setTone(*t); )
  return rb_iv_set(self, "tone", tone);
}

static VALUE SpriteGetX(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getX() );
}

static VALUE SpriteSetX(VALUE self, VALUE x)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setX( RB_FIX2INT(x) ); )
  return RB_INT2FIX( s->getX() );
}

static VALUE SpriteGetY(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getY() );
}

static VALUE SpriteSetY(VALUE self, VALUE y)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setY( RB_FIX2INT(y) ); )
  return RB_INT2FIX( s->getY() );
}

static VALUE SpriteGetOX(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getOX() );
}

static VALUE SpriteSetOX(VALUE self, VALUE ox)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOX( RB_FIX2INT(ox) ); )
  return RB_INT2FIX( s->getOX() );
}

static VALUE SpriteGetOY(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getOY() );
}

static VALUE SpriteSetOY(VALUE self, VALUE oy)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOY( RB_FIX2INT(oy) ); )
  return RB_INT2FIX( s->getOY() );
}

static VALUE SpriteGetBushDepth(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getBushDepth() );
}

static VALUE SpriteSetBushDepth(VALUE self, VALUE depth)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setBushDepth( RB_FIX2INT(depth) ); )
  return RB_INT2FIX( s->getBushDepth() );
}

static VALUE SpriteGetBushOpacity(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getBushOpacity() );
}

static VALUE SpriteSetBushOpacity(VALUE self, VALUE opacity)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setBushOpacity( RB_FIX2INT(opacity) ); )
  return RB_INT2FIX( s->getBushOpacity() );
}

static VALUE SpriteGetOpacity(VALUE self)
{
  checkDisposed<Sprite>(self);
  return rb_iv_get(self, "opacity");
}//Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));RB_INT2FIX( s->getOpacity() );

static VALUE SpriteSetOpacity(VALUE self, VALUE opacity)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOpacity( RB_FIX2INT(opacity) ); )
  return rb_iv_set(self, "opacity", RB_INT2FIX( s->getOpacity() ));
}

static VALUE SpriteGetBlendType(VALUE self)
{
  checkDisposed<Sprite>(self);
  return rb_iv_get(self, "blend_type");
}//Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  //return RB_INT2FIX( s->getBlendType() );
static VALUE SpriteSetBlendType(VALUE self, VALUE type)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setBlendType( RB_FIX2INT(type) ); )
  VALUE blend_kind = RB_INT2FIX( s->getBlendType() );
  return rb_iv_set(self, "blend_type", blend_kind);
}

static VALUE SpriteGetWaveAmp(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getWaveAmp() );
}

static VALUE SpriteSetWaveAmp(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setWaveAmp( RB_FIX2INT(number) ); )
  return RB_INT2FIX( s->getWaveAmp() );
}

static VALUE SpriteGetWaveLength(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getWaveLength() );
}

static VALUE SpriteSetWaveLength(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setWaveLength( RB_FIX2INT(number) ); )
  return RB_INT2FIX( s->getWaveLength() );
}

static VALUE SpriteGetWaveSpeed(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getWaveSpeed() );
}

static VALUE SpriteSetWaveSpeed(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setWaveSpeed( RB_FIX2INT(number) ); )
  return RB_INT2FIX( s->getWaveSpeed() );
}

static VALUE SpriteGetWavePhase(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return rb_float_new( s->getWavePhase() );
}

static VALUE SpriteSetWavePhase(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setWavePhase( NUM2DBL(number) ); )
  return rb_float_new( s->getWavePhase() );
}

static VALUE SpriteGetZoomX(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return rb_float_new( s->getZoomX() );
}

static VALUE SpriteSetZoomX(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setZoomX( NUM2DBL(number) ); )
  return rb_float_new( s->getZoomX() );
}

static VALUE SpriteGetZoomY(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  return rb_float_new( s->getZoomY() );
}

static VALUE SpriteSetZoomY(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setZoomY( NUM2DBL(number) ); )
  return rb_float_new( s->getZoomY() );
}

static VALUE SpriteGetAngle(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  return rb_float_new( s->getAngle() );
}

static VALUE SpriteSetAngle(VALUE self, VALUE number)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  GUARD_EXC( s->setAngle( NUM2DBL(number) ); )
  return rb_float_new( s->getAngle() );
}

static VALUE SpriteGetMirror(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  return s->getMirror() ? Qtrue : Qfalse;
}

static VALUE SpriteSetMirror(VALUE self, VALUE bln)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  GUARD_EXC( s->setMirror( bln == Qtrue ? true : false ); )
  return bln;
}

static VALUE SpriteGetMirrorY(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  return s->getMirrorY() ? Qtrue : Qfalse;
}

static VALUE SpriteSetMirrorY(VALUE self, VALUE bln)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  GUARD_EXC( s->setMirrorY( bln == Qtrue ? true : false ); )
  return bln;
}

static VALUE SpriteWidth(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getWidth() );
}

static VALUE SpriteHeight(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getHeight() );
}

static VALUE SpriteGetReduceSpeed(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getReduceSpeed() );
}

static VALUE SpriteSetReduceSpeed(VALUE self, VALUE rspeed)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return RB_INT2FIX(0);
  s->setReduceSpeed(RB_FIX2INT(rspeed));
  return rspeed;
}

static VALUE SpriteIncreaseWidth(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  s->increaseWidth();
  return Qtrue;
}

static VALUE SpriteIncreaseHeight(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qfalse;
  s->increaseHeight();
  return Qtrue;
}

static VALUE SpriteReduceWidth(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  s->reduceWidth();
  return Qtrue;
}

static VALUE SpriteReduceHeight(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  s->reduceHeight();
  return Qtrue;
}

static VALUE SpriteIsWidthIncreased(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  return s->isWidthIncreased() ? Qtrue : Qfalse;
}

static VALUE SpriteIsHeightIncreased(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  return s->isHeightIncreased() ? Qtrue : Qfalse;
}

static VALUE SpriteIsWidthReduced(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  return s->isWidthReduced() ? Qtrue : Qfalse;
}

static VALUE SpriteIsHeightReduced(VALUE self)
{
  Sprite *s = static_cast<Sprite*>(RTYPEDDATA_DATA(self));
  if (s == 0) return Qnil;
  return s->isHeightReduced() ? Qtrue : Qfalse;
}

}

template<rb_data_type_t *SpriteType>
static VALUE SpriteAllocate(VALUE klass)
{
  return rb_data_typed_object_wrap(klass, 0, SpriteType);
}

void spriteBindingInit() {
  VALUE RSprite = rb_define_class("Sprite", rb_cObject);
  rb_define_alloc_func(RSprite, SpriteAllocate<&SpriteType>);
  disposableBindingInit<Sprite>(RSprite);
  flashableBindingInit<Sprite>(RSprite);
  viewportElementBindingInit<Sprite>(RSprite);
  rb_define_method(RSprite, "initialize", RUBY_METHOD_FUNC(spriteInitialize), -1);
  rb_define_method(RSprite, "bitmap", RUBY_METHOD_FUNC(SpriteGetBitmap), 0);
  rb_define_method(RSprite, "bitmap=", RUBY_METHOD_FUNC(SpriteSetBitmap), 1);
  rb_define_method(RSprite, "src_rect", RUBY_METHOD_FUNC(SpriteGetSrcRect), 0);
  rb_define_method(RSprite, "src_rect=", RUBY_METHOD_FUNC(SpriteSetSrcRect), 1);
  rb_define_method(RSprite, "color", RUBY_METHOD_FUNC(SpriteGetColor), 0);
  rb_define_method(RSprite, "color=", RUBY_METHOD_FUNC(SpriteSetColor), 1);
  rb_define_method(RSprite, "tone", RUBY_METHOD_FUNC(SpriteGetTone), 0);
  rb_define_method(RSprite, "tone=", RUBY_METHOD_FUNC(SpriteSetTone), 1);
  rb_define_method(RSprite, "x", RUBY_METHOD_FUNC(SpriteGetX), 0);
  rb_define_method(RSprite, "x=", RUBY_METHOD_FUNC(SpriteSetX), 1);
  rb_define_method(RSprite, "y", RUBY_METHOD_FUNC(SpriteGetY), 0);
  rb_define_method(RSprite, "y=", RUBY_METHOD_FUNC(SpriteSetY), 1);
  rb_define_method(RSprite, "ox", RUBY_METHOD_FUNC(SpriteGetOX), 0);
  rb_define_method(RSprite, "ox=", RUBY_METHOD_FUNC(SpriteSetOX), 1);
  rb_define_method(RSprite, "oy", RUBY_METHOD_FUNC(SpriteGetOY), 0);
  rb_define_method(RSprite, "oy=", RUBY_METHOD_FUNC(SpriteSetOY), 1);
  rb_define_method(RSprite, "zoom_x", RUBY_METHOD_FUNC(SpriteGetZoomX), 0);
  rb_define_method(RSprite, "zoom_x=", RUBY_METHOD_FUNC(SpriteSetZoomX), 1);
  rb_define_method(RSprite, "zoom_y", RUBY_METHOD_FUNC(SpriteGetZoomY), 0);
  rb_define_method(RSprite, "zoom_y=", RUBY_METHOD_FUNC(SpriteSetZoomY), 1);
  rb_define_method(RSprite, "angle", RUBY_METHOD_FUNC(SpriteGetAngle), 0);
  rb_define_method(RSprite, "angle=", RUBY_METHOD_FUNC(SpriteSetAngle), 1);
  rb_define_method(RSprite, "mirror", RUBY_METHOD_FUNC(SpriteGetMirror), 0);
  rb_define_method(RSprite, "mirror=", RUBY_METHOD_FUNC(SpriteSetMirror), 1);
  rb_define_method(RSprite, "mirror_y", RUBY_METHOD_FUNC(SpriteGetMirrorY), 0);
  rb_define_method(RSprite, "mirror_y=", RUBY_METHOD_FUNC(SpriteSetMirrorY), 1);
  rb_define_method(RSprite, "flip", RUBY_METHOD_FUNC(SpriteGetMirror), 0);
  rb_define_method(RSprite, "flip=", RUBY_METHOD_FUNC(SpriteSetMirror), 1);
  rb_define_method(RSprite, "flip_y", RUBY_METHOD_FUNC(SpriteGetMirrorY), 0);
  rb_define_method(RSprite, "flip_y=", RUBY_METHOD_FUNC(SpriteSetMirrorY), 1);
  rb_define_method(RSprite, "bush_depth", RUBY_METHOD_FUNC(SpriteGetBushDepth), 0);
  rb_define_method(RSprite, "bush_depth=", RUBY_METHOD_FUNC(SpriteSetBushDepth), 1);
  rb_define_method(RSprite, "opacity", RUBY_METHOD_FUNC(SpriteGetOpacity), 0);
  rb_define_method(RSprite, "opacity=", RUBY_METHOD_FUNC(SpriteSetOpacity), 1);
  rb_define_method(RSprite, "blend_type", RUBY_METHOD_FUNC(SpriteGetBlendType), 0);
  rb_define_method(RSprite, "blend_type=", RUBY_METHOD_FUNC(SpriteSetBlendType), 1);
  rb_define_method(RSprite, "width", RUBY_METHOD_FUNC(SpriteWidth), 0);
  rb_define_method(RSprite, "height", RUBY_METHOD_FUNC(SpriteHeight), 0);
  rb_define_method(RSprite, "reduce_speed", RUBY_METHOD_FUNC(SpriteGetReduceSpeed), 0);
  rb_define_method(RSprite, "reduce_speed=", RUBY_METHOD_FUNC(SpriteSetReduceSpeed), 1);
  rb_define_method(RSprite, "increase_width!", RUBY_METHOD_FUNC(SpriteIncreaseWidth), 0);
  rb_define_method(RSprite, "increase_height!", RUBY_METHOD_FUNC(SpriteIncreaseHeight), 0);
  rb_define_method(RSprite, "increased_width?", RUBY_METHOD_FUNC(SpriteIsWidthIncreased), 0);
  rb_define_method(RSprite, "increased_height?", RUBY_METHOD_FUNC(SpriteIsHeightIncreased), 0);
  rb_define_method(RSprite, "reduce_width!", RUBY_METHOD_FUNC(SpriteReduceWidth), 0);
  rb_define_method(RSprite, "reduce_height!", RUBY_METHOD_FUNC(SpriteReduceHeight), 0);
  rb_define_method(RSprite, "reduced_width?", RUBY_METHOD_FUNC(SpriteIsWidthReduced), 0);
  rb_define_method(RSprite, "reduced_height?", RUBY_METHOD_FUNC(SpriteIsHeightReduced), 0);
  if (rgssVer >= 2) {
    rb_define_method(RSprite, "bush_opacity", RUBY_METHOD_FUNC(SpriteGetBushOpacity), 0);
    rb_define_method(RSprite, "bush_opacity=", RUBY_METHOD_FUNC(SpriteSetBushOpacity), 1);
  }
  rb_define_method(RSprite, "wave_amp", RUBY_METHOD_FUNC(SpriteGetWaveAmp), 0);
  rb_define_method(RSprite, "wave_amp=", RUBY_METHOD_FUNC(SpriteSetWaveAmp), 1);
  rb_define_method(RSprite, "wave_length", RUBY_METHOD_FUNC(SpriteGetWaveLength), 0);
  rb_define_method(RSprite, "wave_length=", RUBY_METHOD_FUNC(SpriteSetWaveLength), 1);
  rb_define_method(RSprite, "wave_speed", RUBY_METHOD_FUNC(SpriteGetWaveSpeed), 0);
  rb_define_method(RSprite, "wave_speed=", RUBY_METHOD_FUNC(SpriteSetWaveSpeed), 1);
  rb_define_method(RSprite, "wave_phase", RUBY_METHOD_FUNC(SpriteGetWavePhase), 0);
  rb_define_method(RSprite, "wave_phase=", RUBY_METHOD_FUNC(SpriteSetWavePhase), 1);
}

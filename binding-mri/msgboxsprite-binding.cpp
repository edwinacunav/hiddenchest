/*
** spritebox-binding.cpp
**
** This file is part of HiddenChest.
**
** (C) 2019 by Kyonides Arkanthes <kyonides@gmail.com>
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
** along with HiddenChest. If not, see <http://www.gnu.org/licenses/>.
*/

#include "disposable.h"
#include "hcsymbol.h"
#include "msgboxsprite.h"
#include "msgboxsprite-binding.h"
#include "debugwriter.h"

rb_data_type_t MsgBoxSpriteType = { "MsgBoxSprite",
  { 0, freeInstance<MsgBoxSprite>, 0, { 0, 0 } }, 0, 0, 0 };

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

static VALUE MsgBoxSpriteInitialize(int argc, VALUE* argv, VALUE self)
{
  MsgBoxSprite *s = viewportElementInitialize<MsgBoxSprite>(argc, argv, self);
  setPrivateData(self, s);
  s->initDynAttribs();
  wrapProperty(self, &s->getSrcRect(), "src_rect", RectType);
  wrapProperty(self, &s->getColor(), "color", ColorType);
  wrapProperty(self, &s->getTone(), "tone", ToneType);
  return self;
}

static VALUE MsgBoxSpriteGetBitmap(VALUE self)
{
  return rb_iv_get(self, "bitmap");
}

static VALUE MsgBoxSpriteGetContents(VALUE self)
{
  return rb_iv_get(self, "contents");
}

static VALUE MsgBoxSpriteGetBarBitmap(VALUE self)
{
  return rb_iv_get(self, "bar_bitmap");
}

static VALUE MsgBoxSpriteGetCloseIcon(VALUE self)
{
  return rb_iv_get(self, "close_icon");
}

static VALUE MsgBoxSpriteGetSrcRect(VALUE self)
{
  checkDisposed<MsgBoxSprite>(self);
  return rb_iv_get(self, "src_rect");
}

static VALUE MsgBoxSpriteSetBitmap(VALUE self, VALUE bit)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Bitmap *b;
  if ( RB_NIL_P(bit) )
    b = 0;
  else
    b = getPrivateDataCheck<Bitmap>(bit, BitmapType);
  try {
    s->setBitmap(b);
  } catch (Exception &e) { Debug() << "Could not create bitmap."; }
  return rb_iv_set(self, "bitmap", bit);
}

static VALUE MsgBoxSpriteSetContents(VALUE self, VALUE contents)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Bitmap *b;
  if ( RB_NIL_P(contents) )
    b = 0;
  else
    b = getPrivateDataCheck<Bitmap>(contents, BitmapType);
  try {
    s->setContents(b);
  } catch (Exception &e) { Debug() << "Could not create bitmap."; }
  return rb_iv_set(self, "contents", contents);
}

static VALUE MsgBoxSpriteSetBarBitmap(VALUE self, VALUE bit)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Bitmap *b;
  if ( RB_NIL_P(bit) )
    b = 0;
  else
    b = getPrivateDataCheck<Bitmap>(bit, BitmapType);
  GUARD_EXC( s->setBarBitmap(b); )
  return rb_iv_set(self, "bar_bitmap", bit);
}

static VALUE MsgBoxSpriteSetCloseIcon(VALUE self, VALUE bit)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Bitmap *b;
  if ( RB_NIL_P(bit) )
    b = 0;
  else
    b = getPrivateDataCheck<Bitmap>(bit, BitmapType);
  GUARD_EXC( s->setCloseIcon(b); )
  return rb_iv_set(self, "close_icon", bit);
}

static VALUE MsgBoxSpriteSetSrcRect(VALUE self, VALUE rect)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Rect *r;
  if ( RB_NIL_P(rect) )
    r = 0;
  else
    r = getPrivateDataCheck<Rect>(rect, RectType);
  GUARD_EXC( s->setSrcRect(*r); )
  return rb_iv_set(self, "src_rect", rect);
}

static VALUE MsgBoxSpriteGetColor(VALUE self)
{
  checkDisposed<MsgBoxSprite>(self);
  return rb_iv_get(self, "color");
}

static VALUE MsgBoxSpriteSetColor(VALUE self, VALUE color)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Color *c;
  if ( RB_NIL_P(color) )
    c = 0;
  else
    c = getPrivateDataCheck<Color>(color, ColorType);
  GUARD_EXC( s->setColor(*c); )
  return rb_iv_set(self, "color", color);
}

static VALUE MsgBoxSpriteGetTone(VALUE self)
{
  checkDisposed<MsgBoxSprite>(self);
  return rb_iv_get(self, "tone");
}

static VALUE MsgBoxSpriteSetTone(VALUE self, VALUE tone)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  Tone *t;
  if ( RB_NIL_P(tone) )
    t = 0;
  else
    t = getPrivateDataCheck<Tone>(tone, ToneType);
  GUARD_EXC( s->setTone(*t); )
  return tone;
}

static VALUE MsgBoxSpriteGetX(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getX() );
}

static VALUE MsgBoxSpriteSetX(VALUE self, VALUE x)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setX( RB_FIX2INT(x) ); )
  return RB_INT2FIX( s->getX() );
}

static VALUE MsgBoxSpriteGetY(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getY() );
}

static VALUE MsgBoxSpriteSetY(VALUE self, VALUE y)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setY( RB_FIX2INT(y) ); )
  return RB_INT2FIX( s->getY() );
}

static VALUE MsgBoxSpriteGetOX(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getOX() );
}

static VALUE MsgBoxSpriteSetOX(VALUE self, VALUE ox)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOX( RB_FIX2INT(ox) ); )
  return RB_INT2FIX( s->getOX() );
}

static VALUE MsgBoxSpriteGetOY(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return RB_INT2FIX( s->getOY() );
}

static VALUE MsgBoxSpriteSetOY(VALUE self, VALUE oy)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOY( RB_FIX2INT(oy) ); )
  return RB_INT2FIX( s->getOY() );
}

static VALUE MsgBoxSpriteGetOpacity(VALUE self)
{
  checkDisposed<MsgBoxSprite>(self);
  return rb_iv_get(self, "opacity");
}

static VALUE MsgBoxSpriteSetOpacity(VALUE self, VALUE opacity)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setOpacity( RB_FIX2INT(opacity) ); )
  return rb_iv_set(self, "opacity", RB_INT2FIX( s->getOpacity() ));
}

static VALUE MsgBoxSpriteGetBlendType(VALUE self)
{
  checkDisposed<MsgBoxSprite>(self);
  return rb_iv_get(self, "blend_type");
}

static VALUE MsgBoxSpriteSetBlendType(VALUE self, VALUE type)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setBlendType( RB_FIX2INT(type) ); )
  VALUE blend_kind = RB_INT2FIX( s->getBlendType() );
  return rb_iv_set(self, "blend_type", blend_kind);
}

static VALUE MsgBoxSpriteGetZoomX(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return rb_float_new( s->getZoomX() );
}

static VALUE MsgBoxSpriteSetZoomX(VALUE self, VALUE number)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setZoomX( NUM2DBL(number) ); )
  return rb_float_new( s->getZoomX() );
}

static VALUE MsgBoxSpriteGetZoomY(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  return rb_float_new( s->getZoomY() );
}

static VALUE MsgBoxSpriteSetZoomY(VALUE self, VALUE number)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  GUARD_EXC( s->setZoomY( NUM2DBL(number) ); )
  return rb_float_new( s->getZoomY() );
}

static VALUE MsgBoxSpriteGetAngle(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  return rb_float_new( s->getAngle() );
}

static VALUE MsgBoxSpriteSetAngle(VALUE self, VALUE number)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  GUARD_EXC( s->setAngle( NUM2DBL(number) ); )
  return rb_float_new( s->getAngle() );
}

static VALUE MsgBoxSpriteGetMirror(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  return s->getMirror() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteSetMirror(VALUE self, VALUE bln)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  GUARD_EXC( s->setMirror( bln == Qtrue ? true : false ); )
  return bln;
}

static VALUE MsgBoxSpriteGetMirrorY(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  return s->getMirrorY() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteSetMirrorY(VALUE self, VALUE bln)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  GUARD_EXC( s->setMirrorY( bln == Qtrue ? true : false ); )
  return bln;
}

static VALUE MsgBoxSpriteWidth(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getWidth() );
}

static VALUE MsgBoxSpriteHeight(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getHeight() );
}

static VALUE MsgBoxSpriteGetReduceSpeed(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  return RB_INT2FIX( s->getReduceSpeed() );
}

static VALUE MsgBoxSpriteSetReduceSpeed(VALUE self, VALUE rspeed)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return RB_INT2FIX(0);
  s->setReduceSpeed(RB_FIX2INT(rspeed));
  return rspeed;
}

static VALUE MsgBoxSpriteIncreaseWidth(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  s->increaseWidth();
  return Qtrue;
}

static VALUE MsgBoxSpriteIncreaseHeight(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  s->increaseHeight();
  return Qtrue;
}

static VALUE MsgBoxSpriteIncreaseWidthHeight(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qfalse;
  s->increaseWidthHeight();
  return Qtrue;
}

static VALUE MsgBoxSpriteReduceWidth(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  s->reduceWidth();
  return Qtrue;
}

static VALUE MsgBoxSpriteReduceHeight(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  s->reduceHeight();
  return Qtrue;
}

static VALUE MsgBoxSpriteReduceWidthHeight(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  s->reduceWidthHeight();
  return Qtrue;
}

static VALUE MsgBoxSpriteIsWidthIncreased(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isWidthIncreased() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsHeightIncreased(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isHeightIncreased() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsWidthReduced(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isWidthReduced() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsHeightReduced(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isHeightReduced() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsMouseInside(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isMouseInside() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsMouseAboveColorFound(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isMouseAboveColorFound() ? Qtrue : Qfalse;
}

static VALUE MsgBoxSpriteIsMouseAboveCloseIcon(VALUE self)
{
  MsgBoxSprite *s = static_cast<MsgBoxSprite*>(RTYPEDDATA_DATA(self));
  if (!s) return Qnil;
  return s->isMouseAboveCloseIcon() ? Qtrue : Qfalse;
}

template<rb_data_type_t *MsgBoxSpriteType>
static VALUE MsgBoxSpriteAllocate(VALUE klass)
{
  return rb_data_typed_object_wrap(klass, 0, MsgBoxSpriteType);
}

void MsgBoxSpriteBindingInit() {
  VALUE RSBox = rb_define_class("MsgBoxSprite", rb_cObject);
  rb_define_alloc_func(RSBox, MsgBoxSpriteAllocate<&MsgBoxSpriteType>);
  disposableBindingInit<MsgBoxSprite>(RSBox);
  viewportElementBindingInit<MsgBoxSprite>(RSBox);
  rb_define_method(RSBox, "initialize", RMF(MsgBoxSpriteInitialize), -1);
  rb_define_method(RSBox, "bitmap", RMF(MsgBoxSpriteGetBitmap), 0);
  rb_define_method(RSBox, "contents", RMF(MsgBoxSpriteGetContents), 0);
  rb_define_method(RSBox, "bar_bitmap", RMF(MsgBoxSpriteGetBarBitmap), 0);
  rb_define_method(RSBox, "close_icon", RMF(MsgBoxSpriteGetCloseIcon), 0);
  rb_define_method(RSBox, "bitmap=", RMF(MsgBoxSpriteSetBitmap), 1);
  rb_define_method(RSBox, "contents=", RMF(MsgBoxSpriteSetContents), 1);
  rb_define_method(RSBox, "bar_bitmap=", RMF(MsgBoxSpriteSetBarBitmap), 1);
  rb_define_method(RSBox, "close_icon=", RMF(MsgBoxSpriteSetCloseIcon), 1);
  rb_define_method(RSBox, "src_rect", RMF(MsgBoxSpriteGetSrcRect), 0);
  rb_define_method(RSBox, "src_rect=", RMF(MsgBoxSpriteSetSrcRect), 1);
  rb_define_method(RSBox, "color", RMF(MsgBoxSpriteGetColor), 0);
  rb_define_method(RSBox, "color=", RMF(MsgBoxSpriteSetColor), 1);
  rb_define_method(RSBox, "tone", RMF(MsgBoxSpriteGetTone), 0);
  rb_define_method(RSBox, "tone=", RMF(MsgBoxSpriteSetTone), 1);
  rb_define_method(RSBox, "x", RMF(MsgBoxSpriteGetX), 0);
  rb_define_method(RSBox, "x=", RMF(MsgBoxSpriteSetX), 1);
  rb_define_method(RSBox, "y", RMF(MsgBoxSpriteGetY), 0);
  rb_define_method(RSBox, "y=", RMF(MsgBoxSpriteSetY), 1);
  rb_define_method(RSBox, "ox", RMF(MsgBoxSpriteGetOX), 0);
  rb_define_method(RSBox, "ox=", RMF(MsgBoxSpriteSetOX), 1);
  rb_define_method(RSBox, "oy", RMF(MsgBoxSpriteGetOY), 0);
  rb_define_method(RSBox, "oy=", RMF(MsgBoxSpriteSetOY), 1);
  rb_define_method(RSBox, "zoom_x", RMF(MsgBoxSpriteGetZoomX), 0);
  rb_define_method(RSBox, "zoom_x=", RMF(MsgBoxSpriteSetZoomX), 1);
  rb_define_method(RSBox, "zoom_y", RMF(MsgBoxSpriteGetZoomY), 0);
  rb_define_method(RSBox, "zoom_y=", RMF(MsgBoxSpriteSetZoomY), 1);
  rb_define_method(RSBox, "angle", RMF(MsgBoxSpriteGetAngle), 0);
  rb_define_method(RSBox, "angle=", RMF(MsgBoxSpriteSetAngle), 1);
  rb_define_method(RSBox, "mirror", RMF(MsgBoxSpriteGetMirror), 0);
  rb_define_method(RSBox, "mirror=", RMF(MsgBoxSpriteSetMirror), 1);
  rb_define_method(RSBox, "mirror_y", RMF(MsgBoxSpriteGetMirrorY), 0);
  rb_define_method(RSBox, "mirror_y=", RMF(MsgBoxSpriteSetMirrorY), 1);
  rb_define_method(RSBox, "flip", RMF(MsgBoxSpriteGetMirror), 0);
  rb_define_method(RSBox, "flip=", RMF(MsgBoxSpriteSetMirror), 1);
  rb_define_method(RSBox, "flip_y", RMF(MsgBoxSpriteGetMirrorY), 0);
  rb_define_method(RSBox, "flip_y=", RMF(MsgBoxSpriteSetMirrorY), 1);
  rb_define_method(RSBox, "opacity", RMF(MsgBoxSpriteGetOpacity), 0);
  rb_define_method(RSBox, "opacity=", RMF(MsgBoxSpriteSetOpacity), 1);
  rb_define_method(RSBox, "blend_type", RMF(MsgBoxSpriteGetBlendType), 0);
  rb_define_method(RSBox, "blend_type=", RMF(MsgBoxSpriteSetBlendType), 1);
  rb_define_method(RSBox, "width", RMF(MsgBoxSpriteWidth), 0);
  rb_define_method(RSBox, "height", RMF(MsgBoxSpriteHeight), 0);
  rb_define_method(RSBox, "reduce_speed", RMF(MsgBoxSpriteGetReduceSpeed), 0);
  rb_define_method(RSBox, "reduce_speed=", RMF(MsgBoxSpriteSetReduceSpeed), 1);
  rb_define_method(RSBox, "increase_width!", RMF(MsgBoxSpriteIncreaseWidth), 0);
  rb_define_method(RSBox, "increase_height!", RMF(MsgBoxSpriteIncreaseHeight), 0);
  rb_define_method(RSBox, "increase_width_height!", RMF(MsgBoxSpriteIncreaseWidthHeight), 0);
  rb_define_method(RSBox, "increased_width?", RMF(MsgBoxSpriteIsWidthIncreased), 0);
  rb_define_method(RSBox, "increased_height?", RMF(MsgBoxSpriteIsHeightIncreased), 0);
  rb_define_method(RSBox, "reduce_width!", RMF(MsgBoxSpriteReduceWidth), 0);
  rb_define_method(RSBox, "reduce_height!", RMF(MsgBoxSpriteReduceHeight), 0);
  rb_define_method(RSBox, "reduce_width_height!", RMF(MsgBoxSpriteReduceWidthHeight), 0);
  rb_define_method(RSBox, "reduced_width?", RMF(MsgBoxSpriteIsWidthReduced), 0);
  rb_define_method(RSBox, "reduced_height?", RMF(MsgBoxSpriteIsHeightReduced), 0);
  rb_define_method(RSBox, "mouse_inside?", RMF(MsgBoxSpriteIsMouseInside), 0);
  rb_define_method(RSBox, "mouse_above?", RMF(MsgBoxSpriteIsMouseInside), 0);
  rb_define_method(RSBox, "mouse_above_color?", RMF(MsgBoxSpriteIsMouseAboveColorFound), 0);
  rb_define_method(RSBox, "mouse_above_close?", RMF(MsgBoxSpriteIsMouseAboveCloseIcon), 0);
}

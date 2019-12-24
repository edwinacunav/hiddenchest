/*
** sprite.h
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

#ifndef SPRITE_H
#define SPRITE_H

#include "scene.h"
#include "flashable.h"
#include "disposable.h"
#include "viewport.h"
#include "util.h"

class Bitmap;
struct Color;
struct Tone;
struct Rect;
struct SpritePrivate;

class Sprite : public ViewportElement, public Flashable, public Disposable
{
public:
  Sprite(Viewport *viewport = 0);
  ~Sprite();
  void update();
  DECL_ATTR( BushDepth,   int     )
  DECL_ATTR( WaveAmp,     int     )
  DECL_ATTR( WaveLength,  int     )
  DECL_ATTR( WaveSpeed,   int     )
  DECL_ATTR( WavePhase,   float   )
  int getX() const;
  int getY() const;
  int getOX() const;
  int getOY() const;
  int getWidth() const;
  int getHeight() const;
  float getZoomX() const;
  float getZoomY() const;
  float getAngle() const;
  bool getMirror() const;
  bool getMirrorY() const;
  void setX(int);
  void setY(int);
  void set_xy(int, int);
  void setOX(int);
  void setOY(int);
  void setZoomX(float);
  void setZoomY(float);
  void setAngle(float);
  void setMirror(bool);
  void setMirrorY(bool);
  Bitmap* getBitmap();
  void setBitmap(Bitmap*);
  void gray_out();
  void turn_sepia();
  void invert_colors();
  void pixelate();
  Rect& getSrcRect() const;
  void setSrcRect(Rect& rect);
  int getBushOpacity() const;
  void setBushOpacity(int opacity);
  int getOpacity() const;
  void setOpacity(int opacity);
  int getBlendType() const;
  void setBlendType(int type);
  Color& getColor() const;
  void setColor(Color& color);
  void setColor(double r, double g, double b, double a);
  Tone& getTone() const;
  void setTone(Tone& tone);
  int  getReduceSpeed();
  void setReduceSpeed(int);
  void increaseWidth();
  void increaseHeight();
  void increaseWidthHeight();
  void reduceWidth();
  void reduceHeight();
  void reduceWidthHeight();
  bool isWidthIncreased();
  bool isHeightIncreased();
  bool isWidthReduced();
  bool isHeightReduced();
  bool isMouseInside();
  bool isMouseAboveColorFound();
  void initDynAttribs();
  void onGeometryChange(const Scene::Geometry &);

private:
  SpritePrivate *p;
  void draw();
  void releaseResources();
  const char *klassName() const { return "sprite"; }
  ABOUT_TO_ACCESS_DISP
};

#endif // SPRITE_H

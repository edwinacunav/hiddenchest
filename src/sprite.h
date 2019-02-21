/*
** sprite.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
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
  int getWidth()  const;
  int getHeight() const;
  void update();
  DECL_ATTR( Bitmap,      Bitmap* )
  DECL_ATTR( X,           int     )
  DECL_ATTR( Y,           int     )
  DECL_ATTR( OX,          int     )
  DECL_ATTR( OY,          int     )
  DECL_ATTR( ZoomX,       float   )
  DECL_ATTR( ZoomY,       float   )
  DECL_ATTR( Angle,       float   )
  DECL_ATTR( Mirror,      bool    )
  DECL_ATTR( MirrorY,     bool    )
  DECL_ATTR( BushDepth,   int     )
  DECL_ATTR( WaveAmp,     int     )
  DECL_ATTR( WaveLength,  int     )
  DECL_ATTR( WaveSpeed,   int     )
  DECL_ATTR( WavePhase,   float   )

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

  void initDynAttribs();

private:
  SpritePrivate *p;
  void draw();
  void onGeometryChange(const Scene::Geometry &);
  void releaseResources();
  const char *klassName() const { return "sprite"; }
  ABOUT_TO_ACCESS_DISP
};

#endif // SPRITE_H
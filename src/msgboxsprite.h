/*
** spritebox.h
**
** This file is part of HiddenChest
** (C) 2019 Kyonides Arkanthes <kyonides@gmail.com>
*/

#ifndef MSGBOXSPRITE_H
#define MSGBOXSPRITE_H

#include "scene.h"
#include "flashable.h"
#include "disposable.h"
#include "viewport.h"
#include "util.h"
#include "shader.h"

class Bitmap;
struct Color;
struct Tone;
struct Rect;
struct MsgBoxSpritePrivate;

class MsgBoxSprite : public ViewportElement, public Disposable
{
public:
  MsgBoxSprite(Viewport *viewport = 0);
  ~MsgBoxSprite();
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
  Bitmap* getBitmap() const;
  Bitmap* getContents() const;
  Bitmap* getBarBitmap() const;
  Bitmap* getCloseIcon() const;
  void setX(int);
  void setY(int);
  void setOX(int);
  void setOY(int);
  void setZoomX(float);
  void setZoomY(float);
  void setAngle(float);
  void setMirror(bool);
  void setMirrorY(bool);
  void setBitmap(Bitmap*);
  void setContents(Bitmap*);
  void setBarBitmap(Bitmap*);
  void setCloseIcon(Bitmap*);
  Rect& getSrcRect() const;
  void setSrcRect(Rect& rect);
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
  bool isMouseAboveCloseIcon();
  void initDynAttribs();
  void onGeometryChange(const Scene::Geometry &);
  void update();

private:
  MsgBoxSpritePrivate *p;
  SpriteShader* makeShader(SpriteShader&);
  AlphaSpriteShader* makeAlphaShader(AlphaSpriteShader&);
  SimpleSpriteShader* makeSimpleShader(SimpleSpriteShader&);
  void draw();
  void releaseResources();
  const char *klassName() const { return "MsgBoxSprite"; }
  ABOUT_TO_ACCESS_DISP
};

#endif
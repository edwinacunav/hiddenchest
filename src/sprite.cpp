/*
** sprite.cpp
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
#include "bitmap.h"
#include "input.h"
#include "etc.h"
#include "etc-internal.h"
#include "util.h"
#include "gl-util.h"
#include "quad.h"
#include "transform.h"
#include "shader.h"
#include "glstate.h"
#include "quadarray.h"
#include <math.h>
#include <SDL_rect.h>
#include <sigc++/connection.h>

#define ROWH 6

struct SpritePrivate
{
  Bitmap *bitmap;
  Quad quad;
  Transform trans;
  Rect *srcRect;
  sigc::connection srcRectCon;
  bool mirrored;
  bool mirroredY;
  bool increaseWidth;
  bool increaseHeight;
  bool reduceWidth;
  bool reduceHeight;
  int reducedWidth;
  int reducedHeight;
  int reduceSpeed;
  int bushDepth;
  float efBushDepth;
  NormValue bushOpacity;
  NormValue opacity;
  BlendType blendType;
  IntRect sceneRect;
  Vec2i sceneOrig;
  bool isVisible;// Would this sprite be visible on the screen if drawn?
  Color *color;
  Tone *tone;
  struct
  {
    int amp;
    int length;
    int speed;
    float phase;
    // Wave effect is active (amp != 0)
    bool active;
    // qArray needs updating
    bool dirty;
    SimpleQuadArray qArray;
  } wave;
  EtcTemps tmp;
  sigc::connection prepareCon;
  SpritePrivate()
  : bitmap(0),
    srcRect(&tmp.rect),
    mirrored(false),
    mirroredY(false),
    bushDepth(0),
    efBushDepth(0),
    bushOpacity(128),
    opacity(255),
    blendType(BlendNormal),
    increaseWidth(false),
    increaseHeight(false),
    reduceWidth(false),
    reduceHeight(false),
    reducedWidth(0),
    reducedHeight(0),
    reduceSpeed(ROWH),
    isVisible(false),
    color(&tmp.color),
    tone(&tmp.tone)
  {
    sceneRect.x = sceneRect.y = 0;
    updateSrcRectCon();
    prepareCon = shState->prepareDraw.connect(sigc::mem_fun(this, &SpritePrivate::prepare));
    wave.amp = 0;
    wave.length = 180;
    wave.speed = 360;
    wave.phase = 0.0f;
    wave.dirty = false;
  }

  ~SpritePrivate()
  {
    srcRectCon.disconnect();
    prepareCon.disconnect();
  }

  void updateReduceWidth()
  {
    if (increaseWidth) {
      int w = bitmap->width();
      reducedWidth -= reduceSpeed;
      reducedWidth = clamp<int>(reducedWidth, 0, w);
      onSrcRectChange();
      increaseWidth = reducedWidth > 0;
      return;
    }
    if (reduceWidth) {
      int w = bitmap->width();
      reducedWidth += reduceSpeed;
      reducedWidth = clamp<int>(reducedWidth, 0, w);
      onSrcRectChange();
      reduceWidth = w > reducedWidth;
    }
  }

  void updateReduceHeight()
  {
    if (increaseHeight) {
      int h = bitmap->height();
      reducedHeight -= reduceSpeed;
      reducedHeight = clamp<int>(reducedHeight, 0, h);
      onSrcRectChange();
      increaseHeight = reducedHeight > 0;
      return;
    }
    if (reduceHeight) {
      int h = bitmap->height();
      reducedHeight += reduceSpeed;
      reducedHeight = clamp<int>(reducedHeight, 0, h);
      onSrcRectChange();
      reduceHeight = h > reducedHeight;
    }
  }

  void recomputeBushDepth()
  {
    if (nullOrDisposed(bitmap)) return;
    // Calculate effective (normalized) bush depth
    float texBushDepth = (bushDepth / trans.getScale().y) -
                         (srcRect->y + srcRect->height) +
                         bitmap->height();
    efBushDepth = 1.0f - texBushDepth / bitmap->height();
  }

  void onSrcRectChange()
  {
    FloatRect rect = srcRect->toFloatRect();
    Vec2i bmSize;
    if (!nullOrDisposed(bitmap))
      bmSize = Vec2i(bitmap->width() - reducedWidth, bitmap->height() - reducedHeight);
    // Clamp the rectangle so it doesn't reach outside the bitmap bounds
    rect.w = clamp<int>(rect.w, 0, bmSize.x-rect.x);
    rect.h = clamp<int>(rect.h, 0, bmSize.y-rect.y);
    quad.setTexRect(mirrored ? rect.hFlipped() : rect);
    quad.setTexRect(mirroredY ? rect.wFlipped() : rect);
    quad.setPosRect(FloatRect(0, 0, rect.w, rect.h));
    recomputeBushDepth();
    wave.dirty = true;
  }

  void updateSrcRectCon()
  { // Cut old connection and Create new one
    srcRectCon.disconnect();
    srcRectCon = srcRect->valueChanged.connect
                (sigc::mem_fun(this, &SpritePrivate::onSrcRectChange));
  }

  void updateVisibility()
  {
    isVisible = false;
    if (nullOrDisposed(bitmap)) return;
    if (!opacity) return;
    if (wave.active) {
      isVisible = true;
      return;// Don't do expensive wave bounding box calculations
    }
    // Compare sprite bounding box against the scene
    // If sprite is zoomed/rotated, just opt out for now for simplicity's sake
    const Vec2 &scale = trans.getScale();
    if (scale.x != 1 || scale.y != 1 || trans.getRotation() != 0) {
      isVisible = true;
      return;
    }
    IntRect self;
    self.setPos(trans.getPositionI() - (trans.getOriginI() + sceneOrig));
    self.w = bitmap->width();
    self.h = bitmap->height();
    isVisible = SDL_HasIntersection(&self, &sceneRect);
  }

  void emitWaveChunk(SVertex *&vert, float phase, int width, float zoomY, int chunkY, int chunkLength)
  {
    float wavePos = phase + (chunkY / (float) wave.length) * (float) (M_PI * 2);
    float chunkX = sin(wavePos) * wave.amp;
    FloatRect tex(0, chunkY / zoomY, width, chunkLength / zoomY);
    FloatRect pos = tex;
    pos.x = chunkX;
    Quad::setTexPosRect(vert, tex, pos);
    vert += 4;
  }

  void updateWave()
  {
    if (nullOrDisposed(bitmap)) return;
    if (wave.amp == 0) {
      wave.active = false;
      return;
    }
    wave.active = true;
    int width = srcRect->width;
    int height = srcRect->height;
    float zoomY = trans.getScale().y;
    if (wave.amp < -(width / 2)) {
      wave.qArray.resize(0);
      wave.qArray.commit();
      return;
    }
    // RMVX does this, and I have no *** clue why
    if (wave.amp < 0) {
      wave.qArray.resize(1);
      int x = -wave.amp;
      int w = width - x * 2;
      FloatRect tex(x, srcRect->y, w, srcRect->height);
      Quad::setTexPosRect(&wave.qArray.vertices[0], tex, tex);
      wave.qArray.commit();
      return;
    }
    // The length of the sprite as it appears on screen
    int visibleLength = height * zoomY;
    // First chunk length (aligned to 8 pixel boundary
    int firstLength = ((int) trans.getPosition().y) % 8;
    // Amount of full 8 pixel chunks in the middle
    int chunks = (visibleLength - firstLength) / 8;
    // Final chunk length
    int lastLength = (visibleLength - firstLength) % 8;
    wave.qArray.resize(!!firstLength + chunks + !!lastLength);
    SVertex *vert = &wave.qArray.vertices[0];
    float phase = (wave.phase * (float) M_PI) / 180.0f;
    if (firstLength > 0)
      emitWaveChunk(vert, phase, width, zoomY, 0, firstLength);
    for (int i = 0; i < chunks; ++i)
      emitWaveChunk(vert, phase, width, zoomY, firstLength + i * 8, 8);
    if (lastLength > 0)
      emitWaveChunk(vert, phase, width, zoomY, firstLength + chunks * 8, lastLength);
    wave.qArray.commit();
  }

  void prepare() {
    if (wave.dirty) {
      updateWave();
      wave.dirty = false;
    }
    updateVisibility();
  }
};

Sprite::Sprite(Viewport *viewport) : ViewportElement(viewport)
{
  p = new SpritePrivate;
  onGeometryChange(scene->getGeometry());
}

Sprite::~Sprite() { dispose(); }

Bitmap* Sprite::getBitmap()
{
  return p->bitmap;
}

int Sprite::getX() const
{
  return p->trans.getPosition().x;
}

int Sprite::getY() const
{
  return p->trans.getPosition().y;
}

int Sprite::getOX() const
{
  return p->trans.getOrigin().x;
}

int Sprite::getOY() const
{
  return p->trans.getOrigin().y;
}

float Sprite::getZoomX() const
{
  return p->trans.getScale().x;
}

float Sprite::getZoomY() const
{
  return p->trans.getScale().y;
}

float Sprite::getAngle() const
{
  return p->trans.getRotation();
}

bool Sprite::getMirror() const
{
  return p->mirrored;
}

bool Sprite::getMirrorY() const
{
  return p->mirroredY;
}

int Sprite::getBlendType() const
{
  return p->blendType;
}

int Sprite::getWidth() const
{
  return p->srcRect->width;
}

int Sprite::getHeight() const
{
  return p->srcRect->height;
}

DEF_ATTR_RD_SIMPLE(Sprite, BushDepth,  int,     p->bushDepth)
DEF_ATTR_RD_SIMPLE(Sprite, WaveAmp,    int,     p->wave.amp)
DEF_ATTR_RD_SIMPLE(Sprite, WaveLength, int,     p->wave.length)
DEF_ATTR_RD_SIMPLE(Sprite, WaveSpeed,  int,     p->wave.speed)
DEF_ATTR_RD_SIMPLE(Sprite, WavePhase,  float,   p->wave.phase)

int Sprite::getBushOpacity() const
{
  guardDisposed();
  return p->bushOpacity;
}

void Sprite::setBushOpacity(int opacity)
{
  guardDisposed();
  p->bushOpacity = opacity;
}

int Sprite::getOpacity() const
{
  guardDisposed();
  return p->opacity;
}

void Sprite::setOpacity(int opacity)
{
  guardDisposed();
  p->opacity = opacity;
}

Rect& Sprite::getSrcRect() const
{
  guardDisposed();
  return *p->srcRect;
}

void Sprite::setSrcRect(Rect &rect)
{
  guardDisposed();
  *p->srcRect = rect;
}

Color& Sprite::getColor() const
{
  guardDisposed();
  return *p->color;
}

void Sprite::setColor(Color &color)
{
  guardDisposed();
  *p->color = color;
}

void Sprite::setColor(double r, double g, double b, double a)
{
  guardDisposed();
  p->color->set(r, g, b, a);
}

Tone& Sprite::getTone() const
{
  guardDisposed();
  return *p->tone;
}

void Sprite::setTone(Tone &tone)
{
  guardDisposed();
  *p->tone = tone;
}

void Sprite::setBitmap(Bitmap *bitmap)
{
  guardDisposed();
  if (p->bitmap == bitmap) return;
  p->bitmap = bitmap;
  if (nullOrDisposed(bitmap)) return;
  bitmap->ensureNonMega();
  *p->srcRect = bitmap->rect();
  p->onSrcRectChange();
  p->quad.setPosRect(p->srcRect->toFloatRect());
  if (p->wave.active) p->wave.dirty = true;
}

void Sprite::gray_out()
{
  p->bitmap->gray_out();
}

void Sprite::turn_sepia()
{
  p->bitmap->turn_sepia();
}

void Sprite::invert_colors()
{
  p->bitmap->invert_colors();
}

void Sprite::pixelate()
{
  p->bitmap->pixelate();
}

void Sprite::setX(int nx)
{
  guardDisposed();
  if (p->trans.getPosition().x == nx) return;
  p->trans.setPosition(Vec2(nx, getY()));
}

void Sprite::setY(int ny)
{
  guardDisposed();
  if (p->trans.getPosition().y == ny) return;
  p->trans.setPosition(Vec2(getX(), ny));
  if (!p->wave.active) return;//rgssVer >= 2) {
  p->wave.dirty = true;
  setSpriteY(ny);
}

void Sprite::set_xy(int nx, int ny)
{
  guardDisposed();
  if (p->trans.getPosition().x != nx || p->trans.getPosition().y != ny)
    p->trans.setPosition(Vec2(nx, ny));
  if (!p->wave.active) return;
  p->wave.dirty = true;
  setSpriteY(ny);
}

void Sprite::setOX(int value)
{
  guardDisposed();
  if (p->trans.getOrigin().x == value) return;
  p->trans.setOrigin(Vec2(value, getOY()));
}

void Sprite::setOY(int value)
{
  guardDisposed();
  if (p->trans.getOrigin().y == value) return;
  p->trans.setOrigin(Vec2(getOX(), value));
}

void Sprite::setZoomX(float value)
{
  guardDisposed();
  if (p->trans.getScale().x == value) return;
  p->trans.setScale(Vec2(value, getZoomY()));
}

void Sprite::setZoomY(float value)
{
  guardDisposed();
  if (p->trans.getScale().y == value) return;
  p->trans.setScale(Vec2(getZoomX(), value));
  p->recomputeBushDepth();
  //if (rgssVer >= 2)
  p->wave.dirty = true;
}

void Sprite::setAngle(float value)
{
  guardDisposed();
  if (p->trans.getRotation() == value) return;
  p->trans.setRotation(value);
}

void Sprite::setMirror(bool mirrored)
{
  guardDisposed();
  if (p->mirrored == mirrored) return;
  p->mirrored = mirrored;
  p->onSrcRectChange();
}

void Sprite::setMirrorY(bool mirrored)
{
  guardDisposed();
  if (p->mirroredY == mirrored) return;
  p->mirroredY = mirrored;
  p->onSrcRectChange();
}

void Sprite::setBushDepth(int value)
{
  guardDisposed();
  if (p->bushDepth == value) return;
  p->bushDepth = value;
  p->recomputeBushDepth();
}

void Sprite::setBlendType(int type)
{
  guardDisposed();
  switch (type) {
  default :
  case BlendNormal :
    p->blendType = BlendNormal;
    return;
  case BlendAddition :
    p->blendType = BlendAddition;
    return;
  case BlendSubstraction :
    p->blendType = BlendSubstraction;
    return;
  }
}

int Sprite::getReduceSpeed()
{
  guardDisposed();
  return p->reduceSpeed;
}

void Sprite::setReduceSpeed(int speed)
{
  guardDisposed();
  p->reduceSpeed = speed;
}

void Sprite::increaseWidth()
{
  guardDisposed();
  p->reducedWidth = p->bitmap->width();
  p->increaseWidth = true;
  p->updateReduceWidth();
}

void Sprite::increaseHeight()
{
  guardDisposed();
  p->reducedHeight = p->bitmap->height();
  p->increaseHeight = true;
  p->updateReduceHeight();
}

void Sprite::increaseWidthHeight()
{
  guardDisposed();
  p->reducedWidth = p->bitmap->width();
  p->reducedHeight = p->bitmap->height();
  p->increaseWidth = true;
  p->increaseHeight = true;
  p->updateReduceWidth();
  p->updateReduceHeight();
}

void Sprite::reduceWidth()
{
  guardDisposed();
  p->reduceWidth = true;
}

void Sprite::reduceHeight()
{
  guardDisposed();
  p->reduceHeight = true;
}

void Sprite::reduceWidthHeight()
{
  guardDisposed();
  p->reduceWidth = true;
  p->reduceHeight = true;
}

bool Sprite::isWidthIncreased()
{
  guardDisposed();
  return p->reducedWidth == 0;
}

bool Sprite::isHeightIncreased()
{
  guardDisposed();
  return p->reducedHeight == 0;
}

bool Sprite::isWidthReduced()
{
  guardDisposed();
  return p->reducedWidth == p->bitmap->width();
}

bool Sprite::isHeightReduced()
{
  guardDisposed();
  return p->reducedHeight == p->bitmap->height();
}

bool Sprite::isMouseInside()
{
  guardDisposed();
  if (!p->isVisible) return false;
  int mx = shState->input().mouseX();
  int x = p->trans.getPosition().x;
  if (mx < x) return false;
  if (mx > x + p->srcRect->width) return false;
  int my = shState->input().mouseY();
  int y = p->trans.getPosition().y;
  if (my < y) return false;
  return my <= y + p->srcRect->height;
}

bool Sprite::isMouseAboveColorFound()
{
  guardDisposed();
  if (!p->isVisible) return false;
  int mx = shState->input().mouseX();
  int x = p->trans.getPosition().x;
  if (mx < x) return false;
  if (mx > x + p->srcRect->width) return false;
  int my = shState->input().mouseY();
  int y = p->trans.getPosition().y;
  if (my < y) return false;
  if (my > y + p->srcRect->height) return false;
  int ax = mx - x, ay = my - y;
  return !p->bitmap->is_alpha_pixel(ax, ay);
}

#define DEF_WAVE_SETTER(Name, name, type) \
void Sprite::setWave##Name(type value) \
{ \
  guardDisposed(); \
  if (p->wave.name == value) return; \
  p->wave.name = value; \
  p->wave.dirty = true; \
}

DEF_WAVE_SETTER(Amp,    amp,    int)
DEF_WAVE_SETTER(Length, length, int)
DEF_WAVE_SETTER(Speed,  speed,  int)
DEF_WAVE_SETTER(Phase,  phase,  float)

#undef DEF_WAVE_SETTER

void Sprite::initDynAttribs()
{
  p->srcRect = new Rect;
  p->color = new Color;
  p->tone = new Tone;
  p->updateSrcRectCon();
}
// Flashable
void Sprite::update()
{
  guardDisposed();
  Flashable::update();
  p->updateReduceWidth();
  p->updateReduceHeight();
  if (!p->wave.active) return;
  p->wave.phase += p->wave.speed / 180;
  p->wave.dirty = true;
}
// SceneElement
void Sprite::draw()
{
  if (!p->isVisible) return;
  if (emptyFlashFlag) return;
  ShaderBase *base;
  bool renderEffect = p->color->hasEffect() ||
                      p->tone->hasEffect()  ||
                      flashing              ||
                      p->bushDepth != 0;
  if (renderEffect) {
    SpriteShader &shader = shState->shaders().sprite;
    shader.bind();
    shader.applyViewportProj();
    shader.setSpriteMat(p->trans.getMatrix());
    shader.setTone(p->tone->norm);
    shader.setOpacity(p->opacity.norm);
    shader.setBushDepth(p->efBushDepth);
    shader.setBushOpacity(p->bushOpacity.norm);
    /* When both flashing and effective color are set,
     * the one with higher alpha will be blended */
    const Vec4 *blend = (flashing && flashColor.w > p->color->norm.w) ?
                             &flashColor : &p->color->norm;
    shader.setColor(*blend);
    base = &shader;
  } else if (p->opacity != 255) {
    AlphaSpriteShader &shader = shState->shaders().alphaSprite;
    shader.bind();
    shader.setSpriteMat(p->trans.getMatrix());
    shader.setAlpha(p->opacity.norm);
    shader.applyViewportProj();
    base = &shader;
  } else {
    SimpleSpriteShader &shader = shState->shaders().simpleSprite;
    shader.bind();
    shader.setSpriteMat(p->trans.getMatrix());
    shader.applyViewportProj();
    base = &shader;
  }
  glState.blendMode.pushSet(p->blendType);
  p->bitmap->bindTex(*base);
  if (p->wave.active)
    p->wave.qArray.draw();
  else
    p->quad.draw();
  glState.blendMode.pop();
}

void Sprite::onGeometryChange(const Scene::Geometry &geo)
{// Offset at which the sprite will be drawn relative to screen origin
  p->trans.setGlobalOffset(geo.offset());
  p->sceneRect.setSize(geo.rect.size());
  p->sceneOrig = geo.orig;
}

void Sprite::releaseResources() {
  unlink();
  delete p;
}

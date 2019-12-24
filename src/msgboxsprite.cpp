/*
** spritebox.cpp
**
** This file is part of HiddenChest
** (C) 2019 Kyonides Arkanthes <kyonides@gmail.com>
*/

#include "msgboxsprite.h"
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
#include "debugwriter.h"

#define ROWH 6

struct MsgBoxSpritePrivate
{
  Bitmap *bitmap;
  Bitmap *bar_bitmap;
  Bitmap *close_icon;
  Bitmap *contents;
  Quad quad;
  Quad bar_quad;
  Quad close_quad;
  Quad contents_quad;
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
  NormValue opacity;
  BlendType blendType;
  IntRect sceneRect;
  Vec2i sceneOrig;
  bool isVisible;// Would this sprite be visible on the screen if drawn?
  Color *color;
  Tone *tone;
  EtcTemps tmp;
  sigc::connection prepareCon;

  MsgBoxSpritePrivate()
  : bitmap(0),
    bar_bitmap(0),
    close_icon(0),
    contents(0),
    srcRect(&tmp.rect),
    mirrored(false),
    mirroredY(false),
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
    prepareCon = shState->prepareDraw.connect(sigc::mem_fun(this, &MsgBoxSpritePrivate::prepare));
  }

  ~MsgBoxSpritePrivate()
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

  void onSrcRectChange()
  {
    FloatRect temp_r, rect = srcRect->toFloatRect();
    Vec2i bmSize;
    if (!nullOrDisposed(bitmap))
      bmSize = Vec2i(bitmap->width() - reducedWidth, bitmap->height() - reducedHeight);
    // Clamp the rectangle so it doesn't reach outside the bitmap bounds
    rect.w = clamp<int>(rect.w, 0, bmSize.x-rect.x);
    rect.h = clamp<int>(rect.h, 0, bmSize.y-rect.y);
    temp_r = mirrored ? rect.hFlipped() : mirroredY ? rect.wFlipped() : rect;
    quad.setTexRect(temp_r);
    quad.setPosRect(FloatRect(0, 0, rect.w, rect.h));
    if (contents) {
      contents_quad.setTexRect(temp_r);
      contents_quad.setPosRect(FloatRect(4, 0, rect.w - 8, rect.h - 4));
    }
    if (bar_bitmap) {
      temp_r.h = 28;
      bar_quad.setTexRect(temp_r);
      bar_quad.setPosRect(FloatRect(0, 0, rect.w, 28));
    }
    if (close_icon) {
      temp_r.w = 24;
      temp_r.h = 24;
      close_quad.setTexRect(temp_r);
      close_quad.setPosRect(FloatRect(rect.w - 26, 2, 24, 24));
    }
  }

  void updateSrcRectCon()
  { // Cut old connection and Create new one
    srcRectCon.disconnect();
    srcRectCon = srcRect->valueChanged.connect
                (sigc::mem_fun(this, &MsgBoxSpritePrivate::onSrcRectChange));
  }

  void updateVisibility()
  {
    isVisible = false;
    if (nullOrDisposed(bitmap)) return;
    if (!opacity) return;
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

  void prepare() { updateVisibility(); }
};

MsgBoxSprite::MsgBoxSprite(Viewport *viewport) : ViewportElement(viewport)
{
  p = new MsgBoxSpritePrivate;
  onGeometryChange(scene->getGeometry());
}

MsgBoxSprite::~MsgBoxSprite() { dispose(); }

Bitmap* MsgBoxSprite::getBitmap() const
{
  return p->bitmap;
}

Bitmap* MsgBoxSprite::getBarBitmap() const
{
  return p->bar_bitmap;
}

Bitmap* MsgBoxSprite::getCloseIcon() const
{
  return p->close_icon;
}

Bitmap* MsgBoxSprite::getContents() const
{
  return p->contents;
}

int MsgBoxSprite::getX() const
{
  return p->trans.getPosition().x;
}

int MsgBoxSprite::getY() const
{
  return p->trans.getPosition().y;
}

int MsgBoxSprite::getOX() const
{
  return p->trans.getOrigin().x;
}

int MsgBoxSprite::getOY() const
{
  return p->trans.getOrigin().y;
}

float MsgBoxSprite::getZoomX() const
{
  return p->trans.getScale().x;
}

float MsgBoxSprite::getZoomY() const
{
  return p->trans.getScale().y;
}

float MsgBoxSprite::getAngle() const
{
  return p->trans.getRotation();
}

bool MsgBoxSprite::getMirror() const
{
  return p->mirrored;
}

bool MsgBoxSprite::getMirrorY() const
{
  return p->mirroredY;
}

int MsgBoxSprite::getBlendType() const
{
  return p->blendType;
}

int MsgBoxSprite::getWidth() const
{
  return p->srcRect->width;
}

int MsgBoxSprite::getHeight() const
{
  return p->srcRect->height;
}

int MsgBoxSprite::getOpacity() const
{
  guardDisposed();
  return p->opacity;
}

Rect& MsgBoxSprite::getSrcRect() const
{
  guardDisposed();
  return *p->srcRect;
}

Color& MsgBoxSprite::getColor() const
{
  guardDisposed();
  return *p->color;
}

Tone& MsgBoxSprite::getTone() const
{
  guardDisposed();
  return *p->tone;
}

void MsgBoxSprite::setOpacity(int opacity)
{
  guardDisposed();
  p->opacity = opacity;
}

void MsgBoxSprite::setSrcRect(Rect &rect)
{
  guardDisposed();
  *p->srcRect = rect;
}

void MsgBoxSprite::setColor(Color &color)
{
  guardDisposed();
  *p->color = color;
}

void MsgBoxSprite::setColor(double r, double g, double b, double a)
{
  guardDisposed();
  p->color->set(r, g, b, a);
}

void MsgBoxSprite::setTone(Tone &tone)
{
  guardDisposed();
  *p->tone = tone;
}

void MsgBoxSprite::setBitmap(Bitmap *bitmap)
{
  guardDisposed();
  if (p->bitmap == bitmap) return;
  p->bitmap = bitmap;
  if (nullOrDisposed(bitmap)) return;
  bitmap->ensureNonMega();
  *p->srcRect = bitmap->rect();
  p->onSrcRectChange();
  p->quad.setPosRect(p->srcRect->toFloatRect());
}

void MsgBoxSprite::setContents(Bitmap *bitmap)
{
  guardDisposed();
  if (p->bitmap == bitmap) return;
  p->contents = bitmap;
  if (nullOrDisposed(bitmap)) return;
  bitmap->ensureNonMega();
  p->onSrcRectChange();
}

void MsgBoxSprite::setBarBitmap(Bitmap *bitmap)
{
  guardDisposed();
  if (nullOrDisposed(bitmap)) return;
  p->bar_bitmap = bitmap;
  if (p->bar_bitmap == bitmap) return;
  bitmap->ensureNonMega();
  p->onSrcRectChange();
}

void MsgBoxSprite::setCloseIcon(Bitmap *bitmap)
{
  guardDisposed();
  if (nullOrDisposed(bitmap)) return;
  p->close_icon = bitmap;
  if (p->close_icon == bitmap) return;
  bitmap->ensureNonMega();
  p->onSrcRectChange();
}

void MsgBoxSprite::setX(int value)
{
  guardDisposed();
  if (p->trans.getPosition().x == value) return;
  p->trans.setPosition(Vec2(value, getY()));
}

void MsgBoxSprite::setY(int value)
{
  guardDisposed();
  if (p->trans.getPosition().y == value) return;
  p->trans.setPosition(Vec2(getX(), value));
}

void MsgBoxSprite::setOX(int value)
{
  guardDisposed();
  if (p->trans.getOrigin().x == value) return;
  p->trans.setOrigin(Vec2(value, getOY()));
}

void MsgBoxSprite::setOY(int value)
{
  guardDisposed();
  if (p->trans.getOrigin().y == value) return;
  p->trans.setOrigin(Vec2(getOX(), value));
}

void MsgBoxSprite::setZoomX(float value)
{
  guardDisposed();
  if (p->trans.getScale().x == value) return;
  p->trans.setScale(Vec2(value, getZoomY()));
}

void MsgBoxSprite::setZoomY(float value)
{
  guardDisposed();
  if (p->trans.getScale().y == value) return;
  p->trans.setScale(Vec2(getZoomX(), value));
}

void MsgBoxSprite::setAngle(float value)
{
  guardDisposed();
  if (p->trans.getRotation() == value) return;
  p->trans.setRotation(value);
}

void MsgBoxSprite::setMirror(bool mirrored)
{
  guardDisposed();
  if (p->mirrored == mirrored) return;
  p->mirrored = mirrored;
  p->onSrcRectChange();
}

void MsgBoxSprite::setMirrorY(bool mirrored)
{
  guardDisposed();
  if (p->mirroredY == mirrored) return;
  p->mirroredY = mirrored;
  p->onSrcRectChange();
}

void MsgBoxSprite::setBlendType(int type)
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

int MsgBoxSprite::getReduceSpeed()
{
  guardDisposed();
  return p->reduceSpeed;
}

void MsgBoxSprite::setReduceSpeed(int speed)
{
  guardDisposed();
  p->reduceSpeed = speed;
}

void MsgBoxSprite::increaseWidth()
{
  guardDisposed();
  p->reducedWidth = p->bitmap->width();
  p->increaseWidth = true;
  p->updateReduceWidth();
}

void MsgBoxSprite::increaseHeight()
{
  guardDisposed();
  p->reducedHeight = p->bitmap->height();
  p->increaseHeight = true;
  p->updateReduceHeight();
}

void MsgBoxSprite::increaseWidthHeight()
{
  guardDisposed();
  p->reducedWidth = p->bitmap->width();
  p->reducedHeight = p->bitmap->height();
  p->increaseWidth = true;
  p->increaseHeight = true;
  p->updateReduceWidth();
  p->updateReduceHeight();
}

void MsgBoxSprite::reduceWidth()
{
  guardDisposed();
  p->reduceWidth = true;
}

void MsgBoxSprite::reduceHeight()
{
  guardDisposed();
  p->reduceHeight = true;
}

void MsgBoxSprite::reduceWidthHeight()
{
  guardDisposed();
  p->reduceWidth = true;
  p->reduceHeight = true;
}

bool MsgBoxSprite::isWidthIncreased()
{
  guardDisposed();
  return p->reducedWidth == 0;
}

bool MsgBoxSprite::isHeightIncreased()
{
  guardDisposed();
  return p->reducedHeight == 0;
}

bool MsgBoxSprite::isWidthReduced()
{
  guardDisposed();
  return p->reducedWidth == p->bitmap->width();
}

bool MsgBoxSprite::isHeightReduced()
{
  guardDisposed();
  return p->reducedHeight == p->bitmap->height();
}

bool MsgBoxSprite::isMouseInside()
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

bool MsgBoxSprite::isMouseAboveColorFound()
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
  return !p->bitmap->is_alpha_pixel(mx - x, my - y);
}

bool MsgBoxSprite::isMouseAboveCloseIcon()
{
  guardDisposed();
  if (!p->isVisible) return false;
  int mx = shState->input().mouseX();
  int x = p->trans.getPosition().x + p->srcRect->width - 26;
  if (mx < x) return false;
  if (mx > x + 24) return false;
  int my = shState->input().mouseY();
  int y = p->trans.getPosition().y + 2;
  if (my < y) return false;
  if (my > y + 24) return false;
  return !p->bitmap->is_alpha_pixel(mx - x, my - y);
}

void MsgBoxSprite::initDynAttribs()
{
  p->srcRect = new Rect;
  p->color = new Color;
  p->tone = new Tone;
  p->updateSrcRectCon();
}

void MsgBoxSprite::update()
{
  guardDisposed();
  p->updateReduceWidth();
  p->updateReduceHeight();
}

SpriteShader* MsgBoxSprite::makeShader(SpriteShader &shader)
{
  shader.bind();
  shader.applyViewportProj();
  shader.setSpriteMat(p->trans.getMatrix());
  shader.setTone(p->tone->norm);
  shader.setOpacity(p->opacity.norm);
  const Vec4 *blend = &p->color->norm;
  shader.setColor(*blend);
  return &shader;
}

AlphaSpriteShader* MsgBoxSprite::makeAlphaShader(AlphaSpriteShader &shader)
{
  shader.bind();
  shader.setSpriteMat(p->trans.getMatrix());
  shader.setAlpha(p->opacity.norm);
  shader.applyViewportProj();
  return &shader;
}

SimpleSpriteShader* MsgBoxSprite::makeSimpleShader(SimpleSpriteShader &shader)
{
  shader.bind();
  shader.setSpriteMat(p->trans.getMatrix());
  shader.applyViewportProj();
  return &shader;
}

void MsgBoxSprite::draw()
{
  if (!p->isVisible) return;
  ShaderBase *bitmap_base;
  ShaderBase *c_base;
  ShaderBase *bar_base;
  ShaderBase *close_base;
  bool renderEffect = p->color->hasEffect() || p->tone->hasEffect();
  if (renderEffect) {//Debug() << "Render Effect";
    SpriteShader &shader1 = shState->shaders().sprite;
    SpriteShader &shader2 = shState->shaders().sprite;
    SpriteShader &shader3 = shState->shaders().sprite;
    SpriteShader &shader4 = shState->shaders().sprite;
    bitmap_base = makeShader(shader1);
    c_base = makeShader(shader2);
    bar_base = makeShader(shader3);
    close_base = makeShader(shader4);
  } else if (p->opacity != 255) {//Debug() << "Opacity Effect";
    AlphaSpriteShader &shader1 = shState->shaders().alphaSprite;
    AlphaSpriteShader &shader2 = shState->shaders().alphaSprite;
    AlphaSpriteShader &shader3 = shState->shaders().alphaSprite;
    AlphaSpriteShader &shader4 = shState->shaders().alphaSprite;
    bitmap_base = makeAlphaShader(shader1);
    c_base = makeAlphaShader(shader2);
    bar_base = makeAlphaShader(shader3);
    close_base = makeAlphaShader(shader4);
  } else {//Debug() << "No Effect";
    SimpleSpriteShader &shader1 = shState->shaders().simpleSprite;
    AlphaSpriteShader &shader2 = shState->shaders().alphaSprite;
    SimpleSpriteShader &shader3 = shState->shaders().simpleSprite;
    SimpleSpriteShader &shader4 = shState->shaders().simpleSprite;
    bitmap_base = makeSimpleShader(shader1);
    c_base = makeAlphaShader(shader2);
    bar_base = makeSimpleShader(shader3);
    close_base = makeSimpleShader(shader4);
  }
  glState.blendMode.pushSet(p->blendType);
  p->bitmap->bindTex(*bitmap_base);
  p->quad.draw();
  if (p->bar_bitmap) {
    p->bar_bitmap->bindTex(*bar_base);
    p->bar_quad.draw();
  }
  if (p->close_icon) {
    p->close_icon->bindTex(*close_base);
    p->close_quad.draw();
  }
  if (p->contents) {
    p->contents->bindTex(*c_base);
    p->contents_quad.draw();
  }
  glState.blendMode.pop();
}

void MsgBoxSprite::onGeometryChange(const Scene::Geometry &geo)
{// Offset at which the sprite will be drawn relative to screen origin
  p->trans.setGlobalOffset(geo.offset());
  p->sceneRect.setSize(geo.rect.size());
  p->sceneOrig = geo.orig;
}

void MsgBoxSprite::releaseResources() {
  unlink();
  delete p;
}

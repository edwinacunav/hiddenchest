/* --- Modified! ---
** graphics.cpp
**
** This file is part of mkxpplus and mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** 2018 Modified by Kyonides-Arkanthes
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

#include "graphics.h"
#include "util.h"
#include "gl-util.h"
#include "sharedstate.h"
#include "config.h"
#include "glstate.h"
#include "shader.h"
#include "scene.h"
#include "quad.h"
#include "eventthread.h"
#include "texpool.h"
#include "bitmap.h"
#include "etc-internal.h"
#include "disposable.h"
#include "intrulist.h"
#include "binding.h"
#include "debugwriter.h"
#include <SDL_video.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <algorithm>
#include <iostream>
// Increased Screen Resolution for RGSS1
#include "resolution.h"
#define DEF_FRAMERATE 60 //(rgssVer == 1 ?  40 :  60)

struct PingPong
{
  TEXFBO rt[2];
  uint8_t srcInd, dstInd;
  int screenW, screenH;

  PingPong(int screenW, int screenH)
  : srcInd(0), dstInd(1), screenW(screenW), screenH(screenH)
  {
    for (int i = 0; i < 2; ++i) {
      TEXFBO::init(rt[i]);
      TEXFBO::allocEmpty(rt[i], screenW, screenH);
      TEXFBO::linkFBO(rt[i]);
      gl.ClearColor(0, 0, 0, 1);
      FBO::clear();
    }
  }

  ~PingPong()
  {
    for (int i = 0; i < 2; ++i)
      TEXFBO::fini(rt[i]);
  }

  TEXFBO &backBuffer()
  {
    return rt[srcInd];
  }

  TEXFBO &frontBuffer()
  {
    return rt[dstInd];
  }
  // Better not call this during render cycles
  void resize(int width, int height)
  {
    screenW = width;
    screenH = height;
    for (int i = 0; i < 2; ++i)
      TEXFBO::allocEmpty(rt[i], width, height);
  }

  void startRender()
  {
    bind();
  }

  void swapRender()
  {
    std::swap(srcInd, dstInd);
    bind();
  }

  void clearBuffers()
  {
    glState.clearColor.pushSet(Vec4(0, 0, 0, 1));
    for (int i = 0; i < 2; ++i) {
      FBO::bind(rt[i].fbo);
      FBO::clear();
    }
    glState.clearColor.pop();
  }

private:
  void bind()
  {
    FBO::bind(rt[dstInd].fbo);
  }
};

class ScreenScene : public Scene
{
public:
  ScreenScene(int width, int height) : pp(width, height)
  {
    updateReso(width, height);
    brightEffect = false;
    brightnessQuad.setColor(Vec4());
  }

  void composite()
  {
    const int w = geometry.rect.w;
    const int h = geometry.rect.h;
    shState->prepareDraw();
    pp.startRender();
    glState.viewport.set(IntRect(0, 0, w, h));
    FBO::clear();
    Scene::composite();
    if (!brightEffect) return;
    SimpleColorShader &shader = shState->shaders().simpleColor;
    shader.bind();
    shader.applyViewportProj();
    shader.setTranslation(Vec2i());
    brightnessQuad.draw();
  }

  void apply_scissors()
  {
    composite();
    Scene::composite();
    const IntRect &viewpRect = glState.scissorBox.get();
    const IntRect &screenRect = geometry.rect;
    pp.swapRender();
    glState.scissorTest.pushSet(true);
    glState.scissorBox.pushSet(screenRect);
  }

  void apply_shader(ShaderBase &shader)
  {
    shader.applyViewportProj();
    shader.setTexSize(Vec2i(geometry.rect.w, geometry.rect.h));
    TEX::bind(pp.backBuffer().tex);
    glState.blend.pushSet(false);
    screenQuad.draw();
    glState.blend.pop();
    glState.scissorBox.pop();
    glState.scissorTest.pop();
  }

  void composite_gray()
  {
    apply_scissors();
    GrayShader &shader = shState->shaders().gray;
    shader.bind();
    shader.setGray(1.0);
    apply_shader(shader);
  }

  void composite_color(int c)
  {
    float r = 0.0, g = 0.0, b = 0.0;
    if (c == 0)
      r = 1.0;
    else if (c == 1)
      g = 1.0;
    else if (c == 2)
      b = 1.0;
    else {
      r = 0.85;
      g = 0.85;
    }
    apply_scissors();
    BasicColorShader &shader = shState->shaders().basic_color;
    shader.bind();
    shader.set_color(r, g, b);
    apply_shader(shader);
  }

  void composite_sepia()
  {
    apply_scissors();
    SepiaShader &shader = shState->shaders().sepia;
    shader.bind();
    apply_shader(shader);
  }

  void requestViewportRender(const Vec4 &c, const Vec4 &f, const Vec4 &t)
  {
    const IntRect &viewpRect = glState.scissorBox.get();
    const IntRect &screenRect = geometry.rect;
    const bool toneRGBEffect  = t.xyzNotNull();
    const bool toneGrayEffect = t.w != 0;
    const bool colorEffect    = c.w > 0;
    const bool flashEffect    = f.w > 0;
    if (toneGrayEffect) {
      pp.swapRender();
      if (!viewpRect.encloses(screenRect)) {
        /* Scissor test _does_ affect FBO blit operations,
         * and since we're inside the draw cycle, it will
         * be turned on, so turn it off temporarily */
        glState.scissorTest.pushSet(false);
        GLMeta::blitBegin(pp.frontBuffer());
        GLMeta::blitSource(pp.backBuffer());
        GLMeta::blitRectangle(geometry.rect, Vec2i());
        GLMeta::blitEnd();
        glState.scissorTest.pop();
      }
      GrayShader &shader = shState->shaders().gray;
      shader.bind();
      shader.setGray(t.w);
      shader.applyViewportProj();
      shader.setTexSize(screenRect.size());
      TEX::bind(pp.backBuffer().tex);
      glState.blend.pushSet(false);
      screenQuad.draw();
      glState.blend.pop();
    }
    if (!toneRGBEffect && !colorEffect && !flashEffect) return;
    FlatColorShader &shader = shState->shaders().flatColor;
    shader.bind();
    shader.applyViewportProj();
    if (toneRGBEffect) {
      /* First split up additive / substractive components */
      Vec4 add, sub;
      if (t.x > 0)
        add.x = t.x;
      if (t.y > 0)
        add.y = t.y;
      if (t.z > 0)
        add.z = t.z;
      if (t.x < 0)
        sub.x = -t.x;
      if (t.y < 0)
        sub.y = -t.y;
      if (t.z < 0)
        sub.z = -t.z;
      /* Then apply them using hardware blending */
      gl.BlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
      if (add.xyzNotNull()) {
        gl.BlendEquation(GL_FUNC_ADD);
        shader.setColor(add);
        screenQuad.draw();
      }
      if (sub.xyzNotNull()) {
        gl.BlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        shader.setColor(sub);
        screenQuad.draw();
      }
    }
    if (colorEffect || flashEffect) {
      gl.BlendEquation(GL_FUNC_ADD);
      gl.BlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    }
    if (colorEffect) {
      shader.setColor(c);
      screenQuad.draw();
    }
    if (flashEffect) {
      shader.setColor(f);
      screenQuad.draw();
    }
    glState.blendMode.refresh();
  }

  void setBrightness(float norm)
  {
    brightnessQuad.setColor(Vec4(0, 0, 0, 1.0f - norm));
    brightEffect = norm < 1.0f;
  }

  void updateReso(int width, int height)
  {
    geometry.rect.w = width;
    geometry.rect.h = height;
    screenQuad.setTexPosRect(geometry.rect, geometry.rect);
    brightnessQuad.setTexPosRect(geometry.rect, geometry.rect);
    notifyGeometryChange();
  }

  void setResolution(int width, int height)
  {
    pp.resize(width, height);
    updateReso(width, height);
  }

  PingPong &getPP()
  {
    return pp;
  }

private:
  PingPong pp;
  Quad screenQuad;
  Quad brightnessQuad;
  bool brightEffect;
};

/* Nanoseconds per second */
#define NS_PER_S 1000000000

struct FPSLimiter
{
  uint64_t lastTickCount;
  /* ticks per frame */
  int64_t tpf;
  /* Ticks per second */
  const uint64_t tickFreq;
  /* Ticks per milisecond */
  const uint64_t tickFreqMS;
  /* Ticks per nanosecond */
  const double tickFreqNS;
  bool disabled;
  /* Data for frame timing adjustment */
  struct
  {
    /* Last tick count */
    uint64_t last;
    /* How far behind/in front we are for ideal frame timing */
    int64_t idealDiff;
    bool resetFlag;
  } adj;

  FPSLimiter(uint16_t desiredFPS)
      : lastTickCount(SDL_GetPerformanceCounter()),
        tickFreq(SDL_GetPerformanceFrequency()),
        tickFreqMS(tickFreq / 1000),
        tickFreqNS((double) tickFreq / NS_PER_S),
        disabled(false)
  { // std::cout << "SDL Frequency " << SDL_GetPerformanceFrequency() << std::endl;
    setDesiredFPS(desiredFPS);
    adj.last = SDL_GetPerformanceCounter();
    adj.idealDiff = 0;
    adj.resetFlag = false;
  }

  void setDesiredFPS(uint16_t value)
  {
    tpf = tickFreq / value;
  }

  void delay()
  {
    if (disabled) return;
    int64_t tickDelta = SDL_GetPerformanceCounter() - lastTickCount;
    int64_t toDelay = tpf - tickDelta;
    /* Compensate for the last delta
     * to the ideal timestep */
    toDelay -= adj.idealDiff;
    if (toDelay < 0)
      toDelay = 0;
    delayTicks(toDelay);
    uint64_t now = lastTickCount = SDL_GetPerformanceCounter();
    int64_t diff = now - adj.last;
    adj.last = now;
    // Recalculate our temporal position relative to the ideal timestep
    adj.idealDiff = diff - tpf + adj.idealDiff;
    if (adj.resetFlag) {
      adj.idealDiff = 0;
      adj.resetFlag = false;
    }
  }

  void resetFrameAdjust()
  {
    adj.resetFlag = true;
  }
  /* If we're more than a full frame's worth
   * of ticks behind the ideal timestep,
   * there's no choice but to skip frame(s) to catch up */
  bool frameSkipRequired() const
  {
    if (disabled) return false;
    return adj.idealDiff > tpf;
  }

private:
  void delayTicks(uint64_t ticks)
  {
    SDL_Delay(ticks / tickFreqMS);
  }
};

struct GraphicsPrivate
{
  /* Screen resolution, ie. the resolution at which
   * RGSS renders at (settable with Graphics.resize_screen).
   * Can only be changed from within RGSS */
  Vec2i scRes;// Vec2i maxScRes;
  /* Screen size, to which the rendered frames are scaled up.
   * This can be smaller than the window size when fixed aspect
   * ratio is enforced */
  Vec2i scSize;
  /* Actual physical size of the game window */
  Vec2i winSize;
  /* Offset in the game window at which the scaled game screen
   * is blitted inside the game window */
  Vec2i scOffset;
  ScreenScene screen;
  RGSSThreadData *threadData;
  SDL_GLContext glCtx;
  int frameRate;
  int frameCount;
  int brightness;
  FPSLimiter fpsLimiter;
  bool block_fullscreen;
  bool block_ftwelve;
  bool block_fone;
  bool frozen;
  TEXFBO frozenScene;
  Quad screenQuad;
  /* Global list of all live Disposables
   * (disposed on reset) */
  IntruList<Disposable> dispList;

  GraphicsPrivate(RGSSThreadData *rtData)
  : scRes(START_WIDTH, START_HEIGHT),// scRes(WIDTH_MAX, HEIGHT_MAX),
    scSize(scRes),
    winSize(rtData->config.defScreenW, rtData->config.defScreenH),
    screen(scRes.x, scRes.y),
    threadData(rtData),
    glCtx(SDL_GL_GetCurrentContext()),
    frameRate(DEF_FRAMERATE),
    frameCount(0),
    brightness(255),
    fpsLimiter(frameRate),
    frozen(false),
    block_fullscreen(false),
    block_ftwelve(false),
    block_fone(false)
  {
    winSize.x = START_WIDTH;
    winSize.y = START_HEIGHT;
    recalculateScreenSize(rtData);
    updateScreenResoRatio(rtData);
    TEXFBO::init(frozenScene);
    TEXFBO::allocEmpty(frozenScene, scRes.x, scRes.y);
    TEXFBO::linkFBO(frozenScene);
    FloatRect screenRect(0, 0, scRes.x, scRes.y);
    screenQuad.setTexPosRect(screenRect, screenRect);
    fpsLimiter.resetFrameAdjust();
  }

  ~GraphicsPrivate()
  {
    TEXFBO::fini(frozenScene);
  }

  void updateScreenResoRatio(RGSSThreadData *rtData)
  {
    Vec2 &ratio = rtData->sizeResoRatio;
    ratio.x = (float) scRes.x / scSize.x;
    ratio.y = (float) scRes.y / scSize.y;
    rtData->screenOffset = scOffset;
  }
  /* Enforces fixed aspect ratio, if desired */
  void recalculateScreenSize(RGSSThreadData *rtData)
  {// Debug() << winSize.x << " " << winSize.y;
    scSize = winSize;
    if (!rtData->config.fixedAspectRatio) {
      scOffset = Vec2i(0, 0);
      return;
    }
    float resRatio = (float) scRes.x / scRes.y;
    float winRatio = (float) winSize.x / winSize.y;
    if (resRatio > winRatio)
      scSize.y = scSize.x / resRatio;
    else if (resRatio < winRatio)
      scSize.x = scSize.y * resRatio;
    scOffset.x = (winSize.x - scSize.x) / 2.f;
    scOffset.y = (winSize.y - scSize.y) / 2.f;
  }

  void checkResize()
  {
    if (threadData->windowSizeMsg.poll(winSize))
    { // some GL drivers change the viewport on window resize
      glState.viewport.refresh();
      recalculateScreenSize(threadData);
      updateScreenResoRatio(threadData);
      SDL_Rect screen = { scOffset.x, scOffset.y, scSize.x, scSize.y };
      threadData->ethread->notifyGameScreenChange(screen);
    }
  }

  void checkShutDownReset()
  {
    shState->checkShutdown();
    shState->checkReset();
  }

  void shutdown()
  {
    threadData->rqTermAck.set();
    shState->texPool().disable();
    scriptBinding->terminate();
  }

  void swapGLBuffer()
  {
    fpsLimiter.delay();
    SDL_GL_SwapWindow(threadData->window);
    ++frameCount;
    threadData->ethread->notifyFrame();
  }

  void set_buffer(TEXFBO &buffer)
  {
    GLMeta::blitBegin(buffer);
    GLMeta::blitSource(screen.getPP().frontBuffer());
    GLMeta::blitRectangle(IntRect(0, 0, scRes.x, scRes.y), Vec2i());
    GLMeta::blitEnd();
  }

  void compositeToBuffer(TEXFBO &buffer)
  {
    screen.composite();
    set_buffer(buffer);
  }

  void composite_to_gray_buffer(TEXFBO &buffer)
  {
    screen.composite_gray();
    set_buffer(buffer);
  }

  void composite_to_sepia_buffer(TEXFBO &buffer)
  {
    screen.composite_sepia();
    set_buffer(buffer);
  }

  void composite_to_color_buffer(TEXFBO &buffer, int c)
  {
    screen.composite_color(c);
    set_buffer(buffer);
  }

  /*void composite_to_oil_buffer(TEXFBO &buffer)
  {
    screen.composite_oil();
    set_buffer(buffer);
  }*/

  void metaBlitBufferFlippedScaled()
  {
    GLMeta::blitRectangle(IntRect(0, 0, scRes.x, scRes.y),
        IntRect(scOffset.x, scSize.y+scOffset.y, scSize.x, -scSize.y),
        threadData->config.smoothScaling);
  }

  void redrawScreen()
  {
    screen.composite();
    GLMeta::blitBeginScreen(winSize);
    GLMeta::blitSource(screen.getPP().frontBuffer());
    FBO::clear();
    metaBlitBufferFlippedScaled();
    GLMeta::blitEnd();
    swapGLBuffer();
  }

  void checkSyncLock()
  {
    if (!threadData->syncPoint.mainSyncLocked()) return;
    /* Releasing the GL context before sleeping and making it
     * current again on wakeup seems to avoid the context loss
     * when the app moves into the background on Android */
    SDL_GL_MakeCurrent(threadData->window, 0);
    threadData->syncPoint.waitMainSync();
    SDL_GL_MakeCurrent(threadData->window, glCtx);
    fpsLimiter.resetFrameAdjust();
  }
};

Graphics::Graphics(RGSSThreadData *data) :
  screenshot_format(0), screenshot_dir(""), screenshot_fn("")
{
  p = new GraphicsPrivate(data);
  if (data->config.syncToRefreshrate) {
    p->frameRate = data->refreshRate;
    p->fpsLimiter.disabled = true;
  } else if (data->config.fixedFramerate > 0) {
    p->fpsLimiter.setDesiredFPS(data->config.fixedFramerate);
  } else if (data->config.fixedFramerate < 0) {
    p->fpsLimiter.disabled = true;
  }
}

Graphics::~Graphics()
{
  delete p;
}

void Graphics::set_screenshot_format(int format)
{
  screenshot_format = format;
}

void Graphics::set_screenshot_dir(const std::string dir)
{
  screenshot_dir = dir;
}

void Graphics::set_screenshot_fn(const std::string fn)
{
  screenshot_fn = fn;
}

void Graphics::update()
{
  p->checkShutDownReset();
  p->checkSyncLock();
  if (p->frozen) return;
  if (p->fpsLimiter.frameSkipRequired()) {
    if (p->threadData->config.frameSkip) { // Skip frame
      p->fpsLimiter.delay();
      ++p->frameCount;
      p->threadData->ethread->notifyFrame();
      return;
    } else { // Just reset frame adjust counter
      p->fpsLimiter.resetFrameAdjust();
    }
  }// p->fpsLimiter.delay();//call_delay(); p->threadData->ethread->notifyFrame();
  p->checkResize();
  p->redrawScreen();
}

void Graphics::freeze()
{
  p->frozen = true;
  p->checkShutDownReset();
  p->checkResize();
  p->compositeToBuffer(p->frozenScene);
}

void Graphics::transition(int duration, const char *filename, int vague)
{
  p->checkSyncLock();
  if (!p->frozen) return;
  vague = clamp(vague, 1, 256);
  Bitmap *transMap = *filename ? new Bitmap(filename) : 0;
  setBrightness(255);
  // Capture new scene
  p->screen.composite();
  /* The PP frontbuffer will hold the current scene after the
   * composition step. Since the backbuffer is unused during
   * the transition, we can reuse it as the target buffer for
   * the final rendered image. */
  TEXFBO &currentScene = p->screen.getPP().frontBuffer();
  TEXFBO &transBuffer  = p->screen.getPP().backBuffer();
  // If no transition bitmap is provided, we can use a simplified shader
  TransShader &transShader = shState->shaders().trans;
  SimpleTransShader &simpleShader = shState->shaders().simpleTrans;
  if (transMap) {
    TransShader &shader = transShader;
    shader.bind();
    shader.applyViewportProj();
    shader.setFrozenScene(p->frozenScene.tex);
    shader.setCurrentScene(currentScene.tex);
    shader.setTransMap(transMap->getGLTypes().tex);
    shader.setVague(vague / 256.0f);
    shader.setTexSize(p->scRes);
  } else {
    SimpleTransShader &shader = simpleShader;
    shader.bind();
    shader.applyViewportProj();
    shader.setFrozenScene(p->frozenScene.tex);
    shader.setCurrentScene(currentScene.tex);
    shader.setTexSize(p->scRes);
  }
  glState.blend.pushSet(false);
  for (int i = 0; i < duration; ++i) {
    /* We need to clean up transMap properly before
     * a possible longjmp, so we manually test for
     * shutdown/reset here */
    if (p->threadData->rqTerm) {
      glState.blend.pop();
      delete transMap;
      p->shutdown();
      return;
    }
    if (p->threadData->rqReset) {
      glState.blend.pop();
      delete transMap;
      scriptBinding->reset();
      return;
    }
    p->checkSyncLock();
    const float prog = i * (1.0f / duration);
    if (transMap) {
      transShader.bind();
      transShader.setProg(prog);
    } else {
      simpleShader.bind();
      simpleShader.setProg(prog);
    }
    /* Draw the composed frame to a buffer first
     * (we need this because we're skipping PingPong) */
    FBO::bind(transBuffer.fbo);
    FBO::clear();
    p->screenQuad.draw();
    p->checkResize();
    /* Then blit it flipped and scaled to the screen */
    FBO::unbind();
    FBO::clear();
    GLMeta::blitBeginScreen(Vec2i(p->winSize));
    GLMeta::blitSource(transBuffer);
    p->metaBlitBufferFlippedScaled();
    GLMeta::blitEnd();
    p->swapGLBuffer();
  }
  glState.blend.pop();
  delete transMap;
  p->frozen = false;
}

void Graphics::frameReset()
{
  p->fpsLimiter.resetFrameAdjust();
}

static void guardDisposed() {}

int Graphics::getFrameRate() const
{
  return p->frameRate;
}

DEF_ATTR_SIMPLE(Graphics, FrameCount, int, p->frameCount)

void Graphics::setFrameRate(int value)
{
  p->frameRate = clamp(value, 10, 120);
  if (p->threadData->config.syncToRefreshrate) return;
  if (p->threadData->config.fixedFramerate > 0) return;
  p->fpsLimiter.setDesiredFPS(p->frameRate);
}

void Graphics::wait(int duration)
{
  for (int i = 0; i < duration; ++i) {
    p->checkShutDownReset();
    p->redrawScreen();
  }
}

void Graphics::fadeout(int duration)
{
  FBO::unbind();
  float curr = p->brightness;
  float diff = 255.0f - curr;
  for (int i = duration-1; i > -1; --i) {
    setBrightness(diff + (curr / duration) * i);
    if (p->frozen) {
      GLMeta::blitBeginScreen(p->scSize);
      GLMeta::blitSource(p->frozenScene);
      FBO::clear();
      p->metaBlitBufferFlippedScaled();
      GLMeta::blitEnd();
      p->swapGLBuffer();
    } else {
      update();
    }
  }
}

void Graphics::fadein(int duration)
{
  FBO::unbind();
  float curr = p->brightness;
  float diff = 255.0f - curr;
  for (int i = 1; i <= duration; ++i) {
    setBrightness(curr + (diff / duration) * i);
    if (p->frozen) {
      GLMeta::blitBeginScreen(p->scSize);
      GLMeta::blitSource(p->frozenScene);
      FBO::clear();
      p->metaBlitBufferFlippedScaled();
      GLMeta::blitEnd();
      p->swapGLBuffer();
    } else {
      update();
    }
  }
}

Bitmap *Graphics::snapToBitmap()
{
  Bitmap *bitmap = new Bitmap(width(), height());
  p->compositeToBuffer(bitmap->getGLTypes());
  bitmap->taintArea(IntRect(0, 0, width(), height()));
  return bitmap;
}

Bitmap *Graphics::snap_to_gray_bitmap()
{
  Bitmap *bitmap = new Bitmap(width(), height());
  p->composite_to_gray_buffer(bitmap->getGLTypes());
  bitmap->taintArea(IntRect(0, 0, width(), height()));
  return bitmap;
}

Bitmap *Graphics::snap_to_sepia_bitmap()
{
  Bitmap *bitmap = new Bitmap(width(), height());
  p->composite_to_sepia_buffer(bitmap->getGLTypes());
  bitmap->taintArea(IntRect(0, 0, width(), height()));
  return bitmap;
}

Bitmap *Graphics::snap_to_color_bitmap(int c)
{
  Bitmap *bitmap = new Bitmap(width(), height());
  p->composite_to_color_buffer(bitmap->getGLTypes(), c);
  bitmap->taintArea(IntRect(0, 0, width(), height()));
  return bitmap;
}

/*Bitmap *Graphics::snap_to_oil_bitmap()
{
  Bitmap *bitmap = new Bitmap(width(), height());
  p->composite_to_oil_buffer(bitmap->getGLTypes());
  bitmap->taintArea(IntRect(0, 0, width(), height()));
  return bitmap;
}*/

bool Graphics::save_screenshot()
{
  time_t rt = time(NULL);
  tm *tmp = localtime(&rt);
  char str[500];
  std::string format = screenshot_format == 0 ? "jpg" : "png";
  sprintf(str, "%s/%s_%d-%02d-%02d_%02dh%02dm%02ds.%s",
          screenshot_dir.c_str(), screenshot_fn.c_str(),
          tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday,
          tmp->tm_hour, tmp->tm_min, tmp->tm_sec, format.c_str());
  Bitmap *bmp = snapToBitmap();
  SDL_Surface *surf = bmp->surface();//Fast
  SDL_LockSurface(surf);
  bool failed = false;
  if (screenshot_format == 0) {
    if ( IMG_SaveJPG(surf, str, 95) != 0 ) {
      Debug() << "Freeing JPG surface after failure";
      failed = true;
    }
  } else {
    if ( IMG_SavePNG(surf, str) != 0 ) {
      Debug() << "Freeing PNG surface after failure";
      failed = true;
    }
  }
  SDL_FreeSurface(surf);
  delete bmp;
  return !failed;
}

int Graphics::width() const
{
  return p->scRes.x;
}

int Graphics::height() const
{
  return p->scRes.y;
}

void Graphics::resizeScreen(int width, int height)
{
  width = clamp(width, 1, WIDTH_MAX);
  height = clamp(height, 1, HEIGHT_MAX);
  Vec2i size(width, height);
  if (p->scRes == size) return;
  p->scRes = size;
  p->screen.setResolution(width, height);
  TEXFBO::allocEmpty(p->frozenScene, width, height);
  FloatRect screenRect(0, 0, width, height);
  p->screenQuad.setTexPosRect(screenRect, screenRect);
  shState->eThread().requestWindowResize(width, height);
}

void Graphics::playMovie(const char *filename)
{
  Debug() << "Graphics.playMovie(" << filename << ") not implemented";
}

DEF_ATTR_RD_SIMPLE(Graphics, Brightness, int, p->brightness)

void Graphics::setBrightness(int value)
{
  value = clamp(value, 0, 255);
  if (p->brightness == value) return;
  p->brightness = value;
  p->screen.setBrightness(value / 255.0);
}

void Graphics::reset()
{ /* Dispose all live Disposables */
  IntruListLink<Disposable> *iter;
  for (iter = p->dispList.begin();
       iter != p->dispList.end();
       iter = iter->next)
  {
    iter->data->dispose();
  }
  p->dispList.clear();
  /* Reset attributes (frame count not included) */
  p->fpsLimiter.resetFrameAdjust();
  p->frozen = false;
  p->screen.getPP().clearBuffers();
  setFrameRate(DEF_FRAMERATE);
  setBrightness(255);
}

bool Graphics::get_fullscreen() const
{
  return p->threadData->ethread->getFullscreen();
}

void Graphics::set_fullscreen(bool value)
{
  p->threadData->ethread->requestFullscreenMode(value);
}

bool Graphics::get_block_fullscreen() const
{
  return p->block_fullscreen;
}

void Graphics::set_block_fullscreen(bool value)
{
  p->block_fullscreen = value;
}

bool Graphics::get_block_ftwelve() const
{
  return p->block_ftwelve;
}

void Graphics::set_block_ftwelve(bool value)
{
  p->block_ftwelve = value;
}

bool Graphics::get_block_fone() const
{
  return p->block_fone;
}

void Graphics::set_block_fone(bool value)
{
  p->block_fone = value;
}

bool Graphics::get_show_cursor() const
{
  return p->threadData->ethread->getShowCursor();
}

void Graphics::set_show_cursor(bool value)
{
  p->threadData->ethread->requestShowCursor(value);
}

Scene *Graphics::getScreen() const
{
  return &p->screen;
}

void Graphics::repaintWait(const AtomicFlag &exitCond, bool checkReset)
{
  if (exitCond) return;
  // Repaint the screen with the last good frame we drew 
  TEXFBO &lastFrame = p->screen.getPP().frontBuffer();
  GLMeta::blitBeginScreen(p->winSize);
  GLMeta::blitSource(lastFrame);
  while (!exitCond) {
    shState->checkShutdown();
    if (checkReset)
      shState->checkReset();
    FBO::clear();
    p->metaBlitBufferFlippedScaled();
    SDL_GL_SwapWindow(p->threadData->window);
    p->fpsLimiter.delay();
    p->threadData->ethread->notifyFrame();
  }
  GLMeta::blitEnd();
}

void Graphics::addDisposable(Disposable *d)
{
  p->dispList.append(d->link);
}

void Graphics::remDisposable(Disposable *d)
{
  p->dispList.remove(d->link);
}

void Graphics::call_delay()
{
  p->fpsLimiter.delay();
}

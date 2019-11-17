/*
** graphics.h
**
** This file is part of HiddenChest and mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** Extended 2019 by Kyonides Arkanthers <kyonides@gmail.com>
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "util.h"

class Scene;
class Bitmap;
class Disposable;
struct RGSSThreadData;
struct GraphicsPrivate;
struct AtomicFlag;

class Graphics
{
public:
  void update();
  void freeze();
  void freeze_invert();
  void transition(int duration = 8,
                  const char *filename = "",
                  int vague = 40);
  void frameReset();
  int getFrameRate() const;
  int getFrameCount() const;
  int getBrightness() const;
  void setFrameRate(int);
  void setFrameCount(int);
  void setBrightness(int);
  void set_screenshot_format(int);
  void wait(int duration);
  void fadeout(int duration);
  void fadein(int duration);
  Bitmap *snapToBitmap();
  Bitmap *snap_to_gray_bitmap();
  Bitmap *snap_to_sepia_bitmap();
  Bitmap *snap_to_color_bitmap(int c);
  //Bitmap *snap_to_oil_bitmap();
  bool save_screenshot();
  int width() const;
  int height() const;
  void resizeScreen(int width, int height);
  void playMovie(const char *filename);
  void reset();
  /* Non-standard extension */
  bool get_fullscreen() const;
  bool get_block_fullscreen() const;
  bool get_block_ftwelve() const;
  bool get_block_fone() const;
  bool get_show_cursor() const;
  void set_fullscreen(bool value);
  void set_block_fullscreen(bool value);
  void set_block_ftwelve(bool value);
  void set_block_fone(bool value);
  void set_show_cursor(bool value);
  /* <internal> */
  Scene *getScreen() const;
  /* Repaint screen with static image until exitCond
   * is set. Observes reset flag on top of shutdown
   * if "checkReset" */
  void repaintWait(const AtomicFlag &exitCond, bool checkReset = true);
  void call_delay();

private:
  int screenshot_format;
  Graphics(RGSSThreadData *data);
  ~Graphics();
  void addDisposable(Disposable *);
  void remDisposable(Disposable *);
  friend struct SharedStatePrivate;
  friend class Disposable;
  GraphicsPrivate *p;
};

#endif // GRAPHICS_H

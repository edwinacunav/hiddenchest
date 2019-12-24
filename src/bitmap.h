/*
** bitmap.h
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

#ifndef BITMAP_H
#define BITMAP_H

#include "disposable.h"
#include "etc-internal.h"
#include "etc.h"
#include <sigc++/signal.h>

class Font;
class ShaderBase;
struct TEXFBO;
struct SDL_Surface;

struct BitmapPrivate;
// FIXME make this class use proper RGSS classes again
class Bitmap : public Disposable
{
public:
  Bitmap(const char *filename);
  Bitmap(int none);
  Bitmap(int width, int height);
  // Clone constructor
  Bitmap(const Bitmap &other);
  ~Bitmap();
  int width()  const;
  int height() const;
  IntRect rect() const;
  void blt(int x, int y, const Bitmap &source, IntRect rect, int opacity = 255);
  void stretchBlt(const IntRect &destRect,
                  const Bitmap &source, const IntRect &sourceRect,
                  int opacity = 255);
  void fillRect(int x, int y, int width, int height, const Vec4 &color);
  void fillRect(const IntRect &rect, const Vec4 &color);
  void gradientFillRect(int x, int y, int width, int height,
                        const Vec4 &color1, const Vec4 &color2,
                        bool vertical = false);
  void gradientFillRect(const IntRect &rect,
                        const Vec4 &color1, const Vec4 &color2,
                        bool vertical = false);
  void clearRect(int x, int y, int width, int height);
  void clearRect(const IntRect &rect);
  void blur();
  void radialBlur(int angle, int divisions);
  void clear();
  void makeSurface() const;
  bool is_alpha_pixel(int x, int y) const;
  Color getPixel(int x, int y) const;
  void setPixel(int x, int y, const Color &color);
  void invert_colors();
  void hueChange(int hue);
  void gray_out();
  void turn_sepia();
  void pixelate();
  enum TextAlign
  {
    Left = 0,
    Center = 1,
    Right = 2
  };
  void drawText(int x, int y, int width, int height,
                const char *str, int align = Left);
  void drawText(const IntRect &rect, const char *str, int align = Left);
  IntRect textSize(const char *str);
  int textWidth(const char *str);
  int textHeight(const char *str);
  DECL_ATTR(Font, Font&)
  // Sets initial reference without copying by value, use at construction
  void setInitFont(Font *value);
  // <internal>
  TEXFBO &getGLTypes();
  SDL_Surface *megaSurface() const;
  SDL_Surface *surface() const;
  void ensureNonMega() const;
  // Binds the backing texture and sets the correct texture size uniform in shader
  void bindTex(ShaderBase &shader);
  // Adds 'rect' to tainted area
  void taintArea(const IntRect &rect);
  sigc::signal<void> modified;

private:
  SDL_Surface* render_str(bool is_solid, const char *str, SDL_Color c);
  void apply_this_shader(ShaderBase &shader, bool enable, Vec4 vec);
  void releaseResources();
  const char *klassName() const { return "Bitmap"; }
  BitmapPrivate *p;
};

#endif // BITMAP_H

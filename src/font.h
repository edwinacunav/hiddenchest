/*
** font.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** Extended (C) 2019 Kyonides Arkanthes <kyonides@gmail.com>
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

#ifndef FONT_H
#define FONT_H

#include "etc.h"
#include "util.h"
#include <vector>
#include <string>

struct SDL_RWops;
struct _TTF_Font;
struct Config;
struct SharedFontStatePrivate;

class SharedFontState
{
public:
  SharedFontState(const Config &conf);
  ~SharedFontState();
  /* Called from FileSystem during font cache initialization
   * (when "Fonts/" is scanned for available assets).
   * 'ops' is an opened handle to a possible font file,
   * 'filename' is the corresponding path */
  void initFontSetCB(SDL_RWops &ops, const std::string &filename);
  _TTF_Font *getFont(std::string family, int size);
  bool fontPresent(std::string family) const;
  static _TTF_Font *openBundled(int size);

private:
  SharedFontStatePrivate *p;
};

struct FontPrivate;

class Font
{
public:
  static bool does_exist(const char *name);
  static Color& get_default_color();
  static Color& get_default_out_color();
  static Color& get_default_shadow_color();
  static int  get_default_size();
  static bool get_default_bold();
  static bool get_default_italic();
  static bool get_default_outline();
  static bool get_default_shadow();
  static bool get_default_underline();
  static bool get_default_strikethrough();
  static void set_default_name(const std::vector<std::string> &names,
                               const SharedFontState &sfs);
  static void set_default_size(int value);
  static void set_default_bold(bool value);
  static void set_default_italic(bool value);
  static void set_default_outline(bool value);
  static void set_default_shadow(bool value);
  static void set_default_underline(bool value);
  static void set_default_strikethrough(bool value);
  static void set_default_color(Color& value);
  static void set_default_out_color(Color& value);
  static void set_default_shadow_color(Color& value);
  static void set_default_color(double r, double g, double b, double a = 255.0);
  static void set_default_out_color(double r, double g, double b, double a = 255.0);
  static void set_default_shadow_color(double r, double g, double b, double a = 255.0);
  Font(const std::vector<std::string> *names = 0, int size = 0);
  /* Clone constructor */
  Font(const Font &other);
  ~Font();
  const Font &operator=(const Font &o);
  int  get_size() const;
  int  get_outline_size() const;
  int  get_shadow_size() const;
  int  get_shadow_mode() const;
  bool get_no_squeeze() const;
  bool get_bold() const;
  bool get_italic() const;
  bool get_outline() const;
  bool get_shadow() const;
  bool get_underline() const;
  bool get_strikethrough() const;
  Color& get_color();
  Color& get_out_color();
  Color& get_shadow_color();
  void set_name(const std::vector<std::string> &names);
  void set_size(int);
  void set_outline_size(int);
  void set_shadow_size(int);
  void set_shadow_mode(int);
  void set_no_squeeze(bool);
  void set_bold(bool);
  void set_italic(bool);
  void set_outline(bool);
  void set_shadow(bool);
  void set_underline(bool);
  void set_strikethrough(bool);
  void set_color(Color& value);
  void set_color(double r, double g, double b, double a = 255.0);
  void set_out_color(Color& value);
  void set_out_color(double r, double g, double b, double a = 255.0);
  void set_shadow_color(Color& value);
  void set_shadow_color(double r, double g, double b, double a = 255.0);
  /* There is no point in providing getters for these,
   * as the bindings will always return the stored native
   * string/array object anyway. It's impossible to mirror
   * in the C++ core.
   * The core object picks the first existing name from the
   * passed array and stores it internally (same for default). */
  void setName(const std::vector<std::string> &names);
  static void setDefaultName(const std::vector<std::string> &names,
                             const SharedFontState &sfs);

  static const std::vector<std::string> &getInitialDefaultNames();
  /* Assigns heap allocated objects to object properties;
   * using this in pure C++ will cause memory leaks
   * (ie. only to be used in GCed language bindings) */
  void initDynAttribs();
  static void initDefaultDynAttribs();
  static void initDefaults(const SharedFontState &sfs);
  // internal
  _TTF_Font *getSdlFont();

private:
  FontPrivate *p;
};

#endif // FONT_H

/* --- Modified! ---
** font.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
** (C) 2018-2019 Kyonides Arkanthes <kyonides@gmail.com>
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

#include "font.h"
#include "sharedstate.h"
#include "filesystem.h"
#include "exception.h"
#include "boost-hash.h"
#include "util.h"
#include "config.h"
#include <string>
#include <utility>
#include <SDL_ttf.h>

#define BUNDLED_FONT FreeSans

#define BUNDLED_FONT_DECL(FONT) \
	extern unsigned char assets_##FONT##_ttf[]; \
	extern unsigned int assets_##FONT##_ttf_len;

BUNDLED_FONT_DECL(FreeSans)

#define BUNDLED_FONT_D(f) assets_## f ##_ttf
#define BUNDLED_FONT_L(f) assets_## f ##_ttf_len
// Go *** yourself CPP
#define BNDL_F_D(f) BUNDLED_FONT_D(f)
#define BNDL_F_L(f) BUNDLED_FONT_L(f)

typedef std::pair<std::string, int> FontKey;

static SDL_RWops *openBundledFont()
{
  return SDL_RWFromConstMem(BNDL_F_D(BUNDLED_FONT), BNDL_F_L(BUNDLED_FONT));
}

struct FontSet
{ // 'Regular' style
  std::string regular;
  // Any other styles (used in case no 'Regular' exists)
  std::string other;
};

struct SharedFontStatePrivate
{
	/* Maps: font family name, To: substituted family name,
	 * as specified via configuration file / arguments */
	BoostHash<std::string, std::string> subs;

	/* Maps: font family name, To: set of physical
	 * font filenames located in "Fonts/" */
	BoostHash<std::string, FontSet> sets;

	/* Pool of already opened fonts; once opened, they are reused
	 * and never closed until the termination of the program */
	BoostHash<FontKey, TTF_Font*> pool;
};

SharedFontState::SharedFontState(const Config &conf)
{
  p = new SharedFontStatePrivate;
  /* Parse font substitutions */
  for (size_t i = 0; i < conf.fontSubs.size(); ++i) {
    const std::string &raw = conf.fontSubs[i];
    size_t sepPos = raw.find_first_of('>');
    if (sepPos == std::string::npos) continue;
    std::string from = raw.substr(0, sepPos);
    std::string to   = raw.substr(sepPos+1);
    p->subs.insert(from, to);
  }
}

SharedFontState::~SharedFontState()
{
  BoostHash<FontKey, TTF_Font*>::const_iterator iter;
  for (iter = p->pool.cbegin(); iter != p->pool.cend(); ++iter)
    TTF_CloseFont(iter->second);
  delete p;
}

void SharedFontState::initFontSetCB(SDL_RWops &ops,
                                    const std::string &filename)
{
  TTF_Font *font = TTF_OpenFontRW(&ops, 0, 0);
  if (!font) return;
  std::string family = TTF_FontFaceFamilyName(font);
  std::string style = TTF_FontFaceStyleName(font);
  TTF_CloseFont(font);
  FontSet &set = p->sets[family];
  if (style == "Regular")
    set.regular = filename;
  else
    set.other = filename;
}

_TTF_Font *SharedFontState::getFont(std::string family, int size)
{
	/* Check for substitutions */
	if (p->subs.contains(family))
		family = p->subs[family];
	/* Find out if the font asset exists */
	const FontSet &req = p->sets[family];
	if (req.regular.empty() && req.other.empty())
	{
		/* Doesn't exist; use built-in font */
		family = "";
	}
	FontKey key(family, size);
	TTF_Font *font = p->pool.value(key);
	if (font) return font;
	/* Not in pool; open new handle */
	SDL_RWops *ops;
	if (family.empty()) {/* Built-in font */
    ops = openBundledFont();
	} else {
// Use 'other' path as alternative in case we have no 'regular' styled font asset
		const char *path = !req.regular.empty()
		                 ? req.regular.c_str() : req.other.c_str();
		ops = SDL_AllocRW();
		shState->fileSystem().openReadRaw(*ops, path, true);
	}
	// FIXME 0.9 is guesswork at this point
//	float gamma = (96.0/45.0)*(5.0/14.0)*(size-5);
//	font = TTF_OpenFontRW(ops, 1, gamma /** .90*/);
	font = TTF_OpenFontRW(ops, 1, size* 0.90f);
	if (!font)
		throw Exception(Exception::SDLError, "%s", SDL_GetError());
	p->pool.insert(key, font);
	return font;
}

bool SharedFontState::fontPresent(std::string family) const
{ // Check for substitutions
  if (p->subs.contains(family)) family = p->subs[family];
  const FontSet &set = p->sets[family];
  return !(set.regular.empty() && set.other.empty());
}

_TTF_Font *SharedFontState::openBundled(int size)
{
  SDL_RWops *ops = openBundledFont();
  return TTF_OpenFontRW(ops, 1, size);
}

void pickExistingFontName(const std::vector<std::string> &names,
                          std::string &out, const SharedFontState &sfs)
{ /* Note: In RMXP, a names array with no existing entry
   * results in no text being drawn at all (same for "" and []);
   * we can't replicate this in mkxp due to the default substitute. */
  for (size_t i = 0; i < names.size(); ++i) {
    if (sfs.fontPresent(names[i])) {
      out = names[i];
      return;
    }
  }
  out = "";
}

struct FontPrivate
{
  std::string name;
  int size;
  int outline_size;
  bool bold;
  bool italic;
  bool outline;
  bool shadow;
  bool underline;
  bool strikethrough;
  bool no_squeeze;
  Color *color;
  Color *outColor;
  Color colorTmp;
  Color outColorTmp;
  static std::string defaultName;
  static int defaultSize;
  static bool defaultBold;
  static bool defaultItalic;
  static bool defaultOutline;
  static bool defaultShadow;
  static bool default_underline;
  static bool default_strikethrough;
  static Color *defaultColor;
  static Color *defaultOutColor;
  static Color defaultColorTmp;
  static Color defaultOutColorTmp;
  static std::vector<std::string> initialDefaultNames;
  // The actual font is opened as late as possible
  // (when it is queried by a Bitmap), prior it is set to null
  TTF_Font *sdlFont;

  FontPrivate(int size)
  : size(size),
    outline_size(1),
    no_squeeze(false),
    bold(defaultBold),
    italic(defaultItalic),
    outline(defaultOutline),
    shadow(defaultShadow),
    underline(default_underline),
    strikethrough(default_strikethrough),
    color(&colorTmp),
    outColor(&outColorTmp),
    colorTmp(*defaultColor),
    outColorTmp(*defaultOutColor),
    sdlFont(0)
  {}

  FontPrivate(const FontPrivate &other)
  : name(other.name),
    size(other.size),
    outline_size(other.outline_size),
    no_squeeze(other.no_squeeze),
    bold(other.bold),
    italic(other.italic),
    outline(other.outline),
    shadow(other.shadow),
    underline(other.underline),
    strikethrough(other.strikethrough),
    color(&colorTmp),
    outColor(&outColorTmp),
    colorTmp(*other.color),
    outColorTmp(*other.outColor),
    sdlFont(other.sdlFont)
  {}

  void operator=(const FontPrivate &o)
  {
    name          =  o.name;
    size          =  o.size;
    outline_size  =  o.outline_size;
    no_squeeze    =  o.no_squeeze;
    bold          =  o.bold;
    italic        =  o.italic;
    outline       =  o.outline;
    shadow        =  o.shadow;
    underline     =  o.underline;
    strikethrough =  o.strikethrough;
    *color        = *o.color;
    *outColor     = *o.outColor;
    sdlFont = 0;
  }
};

std::string FontPrivate::defaultName     = "Arial";
int         FontPrivate::defaultSize     = 22;
bool        FontPrivate::defaultBold     = false;
bool        FontPrivate::defaultItalic   = false;
bool        FontPrivate::defaultOutline  = false; /* Inited at runtime */
bool        FontPrivate::defaultShadow   = false; /* Inited at runtime */
bool        FontPrivate::default_underline     = false;
bool        FontPrivate::default_strikethrough = false;
Color      *FontPrivate::defaultColor    = &FontPrivate::defaultColorTmp;
Color      *FontPrivate::defaultOutColor = &FontPrivate::defaultOutColorTmp;
Color FontPrivate::defaultColorTmp(255, 255, 255, 255);
Color FontPrivate::defaultOutColorTmp(0, 0, 0, 128);
std::vector<std::string> FontPrivate::initialDefaultNames;

bool Font::doesExist(const char *name)
{
  if (!name) return false;
  return shState->fontState().fontPresent(name);
}

Font::Font(const std::vector<std::string> *names, int size)
{
  p = new FontPrivate(size ? size : FontPrivate::defaultSize);
  if (names)
    setName(*names);
  else
    p->name = FontPrivate::defaultName;
}

Font::Font(const Font &other)
{
  p = new FontPrivate(*other.p);
}

Font::~Font()
{
  delete p;
}

const Font &Font::operator=(const Font &o)
{
  *p = *o.p;
  return o;
}

void Font::setName(const std::vector<std::string> &names)
{
  pickExistingFontName(names, p->name, shState->fontState());
  p->sdlFont = 0;
}

static void guardDisposed() {}

int Font::get_size() const
{
  guardDisposed();
  return p->size;
}

int Font::get_outline_size() const
{
  guardDisposed();
  return p->outline_size;
}

bool Font::get_no_squeeze() const
{
  guardDisposed();
  return p->no_squeeze;
}

bool Font::get_underline() const
{
  guardDisposed();
  return p->underline;
}

bool Font::get_strikethrough() const
{
  guardDisposed();
  return p->strikethrough;
}

bool Font::get_default_underline()
{
  guardDisposed();
  return FontPrivate::default_underline;
}

bool Font::get_default_strikethrough()
{
  guardDisposed();
  return FontPrivate::default_strikethrough;
}

Color& Font::get_color() const
{
  guardDisposed();
  return *p->color;
}

Color& Font::get_out_color() const
{
  guardDisposed();
  return *p->outColor;
}

DEF_ATTR_SIMPLE(Font, Bold,    bool, p->bold)
DEF_ATTR_SIMPLE(Font, Italic,  bool, p->italic)
DEF_ATTR_SIMPLE(Font, Shadow,  bool, p->shadow)
DEF_ATTR_SIMPLE(Font, Outline, bool, p->outline)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultSize,        int, FontPrivate::defaultSize)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultBold,       bool, FontPrivate::defaultBold)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultItalic,     bool, FontPrivate::defaultItalic)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultShadow,     bool, FontPrivate::defaultShadow)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultOutline,    bool, FontPrivate::defaultOutline)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultColor,    Color&, *FontPrivate::defaultColor)
DEF_ATTR_SIMPLE_STATIC(Font, DefaultOutColor, Color&, *FontPrivate::defaultOutColor)

void Font::set_size(int value)
{
  if (p->size == value) return;
  p->size = value < 6 ? 6 : value;
  p->sdlFont = 0;
}

void Font::set_outline_size(int value)
{
  if (p->outline_size == value) return;
  p->outline_size = (value < 1 || value > 8)? 1 : value;
  p->sdlFont = 0;
}

void Font::set_no_squeeze(bool value)
{
  if (p->no_squeeze == value) return;
  p->no_squeeze = value;
  p->sdlFont = 0;
}

void Font::set_underline(bool value)
{
  guardDisposed();
  p->underline = value;
}

void Font::set_strikethrough(bool value)
{
  guardDisposed();
  p->strikethrough = value;
}

void Font::set_default_underline(bool value)
{
  guardDisposed();
  FontPrivate::default_underline = value;
}

void Font::set_default_strikethrough(bool value)
{
  guardDisposed();
  FontPrivate::default_strikethrough = value;
}

void Font::set_color(Color& value)
{
  guardDisposed();
  *p->color = value;
}

void Font::set_color(double r, double g, double b, double a)
{
  guardDisposed();
  p->color->set(r, g, b, a);
}

void Font::set_out_color(Color& value)
{
  guardDisposed();
  *p->outColor = value;
}

void Font::set_out_color(double r, double g, double b, double a)
{
  guardDisposed();
  p->outColor->set(r, g, b, a);
}

void Font::setDefaultName(const std::vector<std::string> &names,
                          const SharedFontState &sfs)
{
  pickExistingFontName(names, FontPrivate::defaultName, sfs);
}

const std::vector<std::string> &Font::getInitialDefaultNames()
{
  return FontPrivate::initialDefaultNames;
}

void Font::initDynAttribs()
{
  p->color = new Color(p->colorTmp);
  p->outColor = new Color(p->outColorTmp);;
}

void Font::initDefaultDynAttribs()
{
  FontPrivate::defaultColor = new Color(FontPrivate::defaultColorTmp);
  FontPrivate::defaultOutColor = new Color(FontPrivate::defaultOutColorTmp);
}

void Font::initDefaults(const SharedFontState &sfs)
{
  std::vector<std::string> &names = FontPrivate::initialDefaultNames;
  switch (rgssVer) {
  case 1 :
    // FIXME: Japanese version has "MS PGothic" instead
    names.push_back("Arial");
    break;
  case 2 :
    names.push_back("UmePlus Gothic");
    names.push_back("MS Gothic");
    names.push_back("Courier New");
    break;
  default:
  case 3 :
    names.push_back("VL Gothic");
  }
  setDefaultName(names, sfs);
  FontPrivate::defaultOutline = true;
  FontPrivate::defaultShadow  = false;
}

_TTF_Font *Font::getSdlFont()
{
  if (!p->sdlFont)
    p->sdlFont = shState->fontState().getFont(p->name.c_str(), p->size);
  int style = TTF_STYLE_NORMAL;
  if (p->bold)   style |= TTF_STYLE_BOLD;
  if (p->italic) style |= TTF_STYLE_ITALIC;
  if (p->underline) style |= TTF_STYLE_UNDERLINE;
  if (p->strikethrough) style |= TTF_STYLE_STRIKETHROUGH;
  TTF_SetFontStyle(p->sdlFont, style);
  return p->sdlFont;
}

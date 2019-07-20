# HiddenChest

HiddenChest is a project based upon mkxp developed by Ancurio. It should let you play RGSS based games on a 800 * 608 resolution or higher by default, but you might change it before compiling it if deemed necessary.

After you have finished compiling the binary executable, you should also open the MapCustomResFixes0 (for RMXP or RMVX games) text file with a text editor like Notepad++ or change its file extension to txt if you plan to use Windows Notepad application. On Linux distributions you can use your favorite text editor, for KDE GUI based systems Kate or KWrite should be fine. Once you have opened it copy its contents and paste them in the script editor below Scene_Debug and before Main scripts.

The `Sprite#mirror_y` alias `Sprite#flip_y` method has been added!

Except for the several scripting tools, the window openness options, the increased window resolution, the Y axis flip features, many additional keys you can press, and a bug fix, retains much of the original mkxp project.

mkxp Documentation is still valid so do not forget to check it out!

## About mkxp

[mkxp](https://github.com/Ancurio/mkxp) is a project that seeks to provide a fully open source implementation of the Ruby Game Scripting System (RGSS) interface used in the popular game creation software "RPG Maker XP", "RPG Maker VX" and "RPG Maker VX Ace" (trademark by Enterbrain, Inc.), with focus on Linux. The goal is to be able to run games created with the above software natively without changing a single file.

It is licensed under the GNU General Public License v2+.

## 640px Wide Prebuilt binaries
[**Linux (32bit/64bit)**](http://ancurio.bplaced.net/mkxp/generic/)
[**OSX**](https://app.box.com/mkxpmacbuilds) by Ali
[**Windows (mingw-w64 32bit)**](http://ancurio.bplaced.net/mkxp/mingw32/)

If you are planning to sport 640 pixel wide games, you should go for mkxp instead.

## Bindings
Bindings provide the glue code for an interpreted language environment to run game scripts in. Currently there are three bindings:

### MRI
Website: https://www.ruby-lang.org/en/

Matz's Ruby Interpreter, also called CRuby for it has a C based API, is the most widely deployed version of Ruby. If you're interested in running games created with RPG Maker XP, this is the one you should go for. MRI 1.8 is what was used in RPG Maker XP, however, this binding is written against 2.6 (the current stable version). For games utilizing only the default scripts provided by Enterbrain, this binding works quite well so far.

Note that there are language and syntax differences between 1.8 and 2.6, so some user created scripts may not work correctly. Version 2.6 will not forgive you if you use the old syntax like in case statements where people used to place a colon after a when condition, i.e. when 2 : call_battle. The colon should be replace with a then keyword to enforce compatibility with later versions of Ruby.

For a list of differences, see:
http://stackoverflow.com/questions/21574/what-is-the-difference-between-ruby-1-8-and-ruby-1-9

This binding supports RGSS1, RGSS2 and RGSS3.

### mruby (Lightweight Ruby)
Website: https://github.com/mruby/mruby

mruby is a new endeavor by Matz and others to create a more lightweight, spec-adhering, embeddable Ruby implementation. You can think of it as a Ruby version of Lua.

Due to heavy differences between mruby and MRI as well as lacking modules, running RPG Maker games with this binding will most likely not work correctly. It is provided as experimental code. You can eg. write your own Ruby scripts and run them.

Some extensions to the standard classes/modules are provided, taking the RPG Maker XP helpfile as a quasi "reference". These include Marshal, File, FileTest and Time.

This binding only supports RGSS1.

**Important:** If you decide to use [mattn's oniguruma regexp gem](https://github.com/mattn/mruby-onig-regexp), don't forget to add `-lonig` to the linker flags to avoid ugly symbol overlaps with libc.

It has been reported that mruby's Array does not support instance variables, and puts method displays its arguments as an array without any new line character.

### null
This binding only exists for testing purposes and does nothing (the engine quits immediately). It can be used to eg. run a minimal RGSS game loop directly in C++.

## Dependencies / Building

* Boost.Unordered (headers only)
* Boost.Program_options
* libsigc++ 2.0
* PhysFS (latest hg)
* OpenAL
* SDL2*
* SDL2_image
* SDL2_ttf
* [Ancurio's SDL_sound fork](https://github.com/Ancurio/SDL_sound)
* vorbisfile
* pixman
* zlib (only Ruby bindings)
* OpenGL header (alternatively GLES2 with `DEFINES+=GLES2_HEADER`)
* libiconv (on Windows, optional with INI_ENCODING)
* libguess (optional with INI_ENCODING)

(* For the F1 menu to work correctly under Linux/X11, you need latest hg + [this patch](https://bugzilla.libsdl.org/show_bug.cgi?id=2745))

HiddenChest and mkxp may employ Qt's qmake build system, so you'll need to install that beforehand. Alternatively, you can build with cmake successfully and even cross-compile them.


qmake will use pkg-config to locate the respective include/library paths. If you installed any dependencies into non-standard prefixes, make sure to adjust your `PKG_CONFIG_PATH` variable accordingly.

### CMake

You can compile it with CMake by running my custom shell script do.sh on your terminal by typing the following in the project's root directory:

./compile.sh

You might need to use chmod +x do.sh first to make it an executable script.

It will create a build directory where it will run CMake and later execute make to compile the HiddenChest binary. If succeeded it will move the binary executable file to the root directory for you.

Take in consideration that you need Ancurio's SDL_sound fork installed on your system. Your OS package manager, i.e. pacman or apt-get, might offer you to install a default version of SDL_sound. DO NOT INSTALL IT! Both HiddenChest and mkxp will fail for sure if you do not follow my advice!

Search for `set(MRIVERSION` in the CMakeLists.txt file to set a different version of Ruby. Default version is 2.6 now.

### Boost

The exception is boost, which is weird in that it still hasn't managed to pull off pkg-config support (seriously?). *If you installed boost in a non-standard prefix*, you will need to pass its include path via `BOOST_I` and library path via `BOOST_L`, either as direct arguments to qmake (`qmake BOOST_I="/usr/include" ...`) or via environment variables. You can specify a library suffix (eg. "-mt") via `BOOST_LIB_SUFFIX` if needed.

### MIDI Support

Midi support is enabled by default and requires fluidsynth to be present at runtime (not needed for building); if HiddenChest can't find it at runtime, midi playback is disabled. It looks for `libfluidsynth.so.1` on Linux, `libfluidsynth.dylib.1` on OSX and `fluidsynth.dll` on Windows, so make sure to have one of these in your link path. If you still need fluidsynth to be hard linked at buildtime, use `CONFIG+=SHARED_FLUID`. When building fluidsynth yourself, you can disable almost all options (audio drivers etc.) as they are not used. Note that upstream fluidsynth has support for sharing soundfont data between synthesizers (HiddenChest uses multiple synths), so if your memory usage is very high, you might want to try compiling fluidsynth from git master.

By default, HiddenChest switches into the directory where its binary is contained and then starts reading the configuration and resolving relative paths. In case this is undesired (eg. when the binary is to be installed to a system global, read-only location), it can be turned off by adding `DEFINES+=WORKDIR_CURRENT` to qmake's arguments.

To auto detect the encoding of the game title in `Game.ini` and auto convert it to UTF-8, build with `CONFIG+=INI_ENCODING`. Requires iconv implementation and libguess. If the encoding is wrongly detected, you can set the "titleLanguage" hint in hiddenchest.conf.

**MRI-Binding**: pkg-config will look for `ruby-2.6.pc`, but you can override the version with `MRIVERSION=2.2` ('2.2' being an example). This is the default binding, so no arguments to qmake needed (`BINDING=MRI` to be explicit).

**MRuby-Binding**: place the "mruby" folder into the project folder and build it first. Add `BINDING=MRUBY` to qmake's arguments.

**Null-Binding**: Add `BINDING=NULL` to qmake's arguments.

### Supported image/audio formats

These depend on the SDL auxiliary libraries. For maximum RGSS compliance, build SDL2_image with png/jpg support, and SDL_sound with oggvorbis/wav/mp3 support.

To run HiddenChest, you should have a graphics card capable of at least **OpenGL (ES) 2.0** with an up-to-date driver installed.

## Dependency kit

To facilitate hacking, I have assembled a package containing all dependencies to compile HiddenChest on a bare-bones Ubuntu 12.04 64bit installation. Compatibility with other distributions has not been tested. You can download it [here](https://www.dropbox.com/s/mtp44ur367m2zts/mkxp-depkit.tar.xz). Read the "README" for instructions.

## Configuration

HiddenChest reads configuration data from the file "hiddenchest.conf". The format is ini-style. Do *not* use quotes around file paths (spaces won't break). Lines starting with '#' are comments. See 'hiddenchest.conf.sample' for a list of accepted entries.

All option entries can alternatively be specified as command line options. Any options that are not arrays (eg. RTP paths) specified as command line options will override entries in hiddenchest.conf. Note that you will have to wrap values containing spaces in quotes (unlike in hiddenchest.conf).

The syntax is: `--<option>=<value>`

Example: `./hiddenchest --gameFolder="my game" --vsync=true --fixedFramerate=60`

## Midi music

HiddenChest doesn't come with a soundfont by default, so you will have to supply it yourself (set its path in the config). Playback has been tested and should work reasonably well with all RTP assets.

You can use this public domain soundfont: [GMGSx.sf2](https://www.dropbox.com/s/qxdvoxxcexsvn43/GMGSx.sf2?dl=0)

## Fonts

In the RMXP version of RGSS, fonts are loaded directly from system specific search paths (meaning they must be installed to be available to games). Because this whole thing is a giant platform-dependent headache, I decided to implement the behavior Enterbrain thankfully added in VX Ace: loading fonts will automatically search a folder called "Fonts", which obeys the default searchpath behavior (ie. it can be located directly in the game folder, or an RTP).

If a requested font is not found, no error is generated. Instead, a built-in font is used (currently "Abyssinica SIL").

## What doesn't work (yet)

* Movie playback
* wma audio files
* The Win32API Ruby class (for obvious reasons)
* Creating Bitmaps with sizes greater than the OpenGL texture size limit (probably around 8192 on some cards)

\* There is an exception to this, called *mega surface*. When a Bitmap bigger than the texture limit is created from a file, it is not stored in VRAM, but regular RAM. Its sole purpose is to be used as a tileset bitmap. Any other operation to it (besides blitting to a regular Bitmap) will result in an error.

## New Features alias Non-standard RGSS extensions

To alleviate possible porting of heavily Win32API reliant scripts, we have added certain functionality that you won't find in the RGSS spec. Currently this amounts to the following:

* Expand RGSS1 tilesets to fill the enlarged screen and adapts the Window_Message settings to the increased screen resolutions.
* Implementation of window openness feature in RMXP games without affecting VX and VX Ace ones. It works in a different fashion in VX Ace like using a 0 through 100 value range while VX Ace requires a maximum value of 255.
* There are 4 modes available to assign to the `self.open_mode` option, i.e.
    - nil or false or true - nothing ever happens
    - :top - the window stretches from the top to the bottom
    - :center - the window opens like a scroll in both directions
    - :bottom - the window seems to grow taller
        - Keep in mind that the `Window#open` command will be ignored if a boolean value is set as the current `Window#open_mode` value.
        - You can set `Window#open_mode=` value before or after the window has called its superclass to initialize or create that window.
        - Only the last assignment to `Window#open_mode=` will be taken in consideration by the engine.
* `Window#set_xy(newx, newy)` lets you assign both Carthesian coordinates at the same time.
* Assign a viewport to any RGSS1 window by using `Window#viewport = some_viewport`
* The `Input.press?` family of functions accepts three additional button constants: `::MOUSELEFT`, `::MOUSEMIDDLE` and `::MOUSERIGHT` for the respective mouse buttons.
* They now support additional keys like Return or Enter or LeftShift or RightAlt or NumPadDivide * or KeyH or KeyM or N1 through N0.
* The `Input` module has two additional functions, `#mouse_x` and `#mouse_y` to query the mouse pointer position relative to the game screen.
* The `Graphics` module has three additional properties: `fullscreen` represents the current fullscreen mode (`true` = fullscreen, `false` = windowed), `show_cursor` hides the system cursor inside the game window when `false` and `block_fullscreen` (`true` or `false`) will prevent the player from entering fullscreen mode or not even if they change the configuration file settings.
* Graphics module also lets you take snapshots by calling the save_screenshot method.
* The Backdrop module lets you create a temporary snapshot of a previous map to use it in any scene class at will. Use `keep_bitmap` or `blur_bitmap` to create the bitmap you will need in your (custom) scene. Later you can assign its bitmap to an instance variable of your choice by calling its `bitmap` method. After freezing the scene, call `clear_bitmap` to dispose it properly.
* Use the `module_attr_accessor` or `module_accessor` method to create module methods, getters and setters all in one! Example: module_attr_accessor :meow will create the self.meow and self.meow=(value) methods in a single step.
* The Scripts module allows you to store a Ruby string or symbol as a script ID via Scripts << :script_name. Once it has been stored there, you can call its methods, i.e. Scripts.all or Scripts.include?(:script_name) to access the Scripts IDs Array and confirm if it has been included respectively.
* Font class now lets you use its `underline` and `strikethrough` or `strikethru` options the same way you used `bold` or `italic`.
* `RPG::Weather.sprite_max = Number`. where Number is a positive integer number, lets you define the upper limit of the weather sprites like rain or storm or snow effects. Currently it is set at 400 sprites.

## List of Bug Fixes for HiddenChest

* Fixed error where the windowskin would disappear for all existing windows in the same scene if just one of them was ever disposed.

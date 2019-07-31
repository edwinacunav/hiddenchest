/*
** input.cpp
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

#include "input.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "keybindings.h"
#include "exception.h"
#include "util.h"
#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <vector>
#include <string.h>
#include <assert.h>

#define BUTTON_CODE_COUNT 24

struct ButtonState
{
  bool pressed;
  bool triggered;
  bool repeated;
  ButtonState()
          : pressed(false),
            triggered(false),
            repeated(false)
  {}
};

struct KbBindingData
{
  SDL_Scancode source;
  Input::ButtonCode target;
};

struct Binding
{
  Binding(Input::ButtonCode target = Input::None)
          : target(target)
  {}
  virtual bool sourceActive() const = 0;
  virtual bool sourceRepeatable() const = 0;

  Input::ButtonCode target;
};

/* Keyboard binding */
struct KbBinding : public Binding
{
	KbBinding() {}
	KbBinding(const KbBindingData &data)
		: Binding(data.target),
		  source(data.source)
	{}

  bool sourceActive() const
  { /* Special case aliases */
    /*if (source == SDL_SCANCODE_LSHIFT)
      return EventThread::keyStates[source]
             || EventThread::keyStates[SDL_SCANCODE_RSHIFT];*/
    if (source == SDL_SCANCODE_RETURN)
      return EventThread::keyStates[source]
             || EventThread::keyStates[SDL_SCANCODE_KP_ENTER];
    return EventThread::keyStates[source];
  }

  bool sourceRepeatable() const
  {
    return (source >= SDL_SCANCODE_A     && source <= SDL_SCANCODE_0)    ||
           (source >= SDL_SCANCODE_RIGHT && source <= SDL_SCANCODE_UP)   ||
           (source >= SDL_SCANCODE_F1    && source <= SDL_SCANCODE_F12);
  }
  SDL_Scancode source;
};

/* Joystick button binding */
struct JsButtonBinding : public Binding
{
  JsButtonBinding() {}

  bool sourceActive() const
  {
    return EventThread::joyState.buttons[source];
  }

  bool sourceRepeatable() const
  {
    return true;
  }

  uint8_t source;
};

/* Joystick axis binding */
struct JsAxisBinding : public Binding
{
  JsAxisBinding() {}
  JsAxisBinding(uint8_t source,
                AxisDir dir,
                Input::ButtonCode target)
      : Binding(target),
        source(source),
        dir(dir)
  {}

  bool sourceActive() const
  {
    int val = EventThread::joyState.axes[source];
    return (dir == Negative) ? val < -JAXIS_THRESHOLD : val > JAXIS_THRESHOLD;
  }

  bool sourceRepeatable() const
  {
    return true;
  }

  uint8_t source;
  AxisDir dir;
};

/* Joystick hat binding */
struct JsHatBinding : public Binding
{
  JsHatBinding() {}
  JsHatBinding(uint8_t source,
                uint8_t pos,
                Input::ButtonCode target)
      : Binding(target),
        source(source),
        pos(pos)
  {}

  bool sourceActive() const
  { // For a diagonal input accept it as an input for both the axes
    return (pos & EventThread::joyState.hats[source]) != 0;
  }

  bool sourceRepeatable() const
  {
    return true;
  }

  uint8_t source;
  uint8_t pos;
};

// Mouse button binding
struct MsBinding : public Binding
{
  MsBinding() {}
  MsBinding(int buttonIndex, Input::ButtonCode target)
      : Binding(target), index(buttonIndex)
  {}

  bool sourceActive() const
  {
    return EventThread::mouseState.buttons[index];
  }

  bool sourceRepeatable() const
  {
    return false;
  }

  int index;
};

// Not rebindable
static const KbBindingData staticKbBindings[] =
{
  //{ SDL_SCANCODE_LSHIFT,       Input::Shift },
  //{ SDL_SCANCODE_RSHIFT,       Input::Shift },
  //{ SDL_SCANCODE_LCTRL,        Input::Ctrl  },
  //{ SDL_SCANCODE_RCTRL,        Input::Ctrl  },
  { SDL_SCANCODE_A,              Input::KeyA },
  { SDL_SCANCODE_B,              Input::KeyB },
  { SDL_SCANCODE_C,              Input::KeyC },
  { SDL_SCANCODE_D,              Input::KeyD },
  { SDL_SCANCODE_E,              Input::KeyE },
  { SDL_SCANCODE_F,              Input::KeyF },
  { SDL_SCANCODE_G,              Input::KeyG },
  { SDL_SCANCODE_H,              Input::KeyH },
  { SDL_SCANCODE_I,              Input::KeyI },
  { SDL_SCANCODE_J,              Input::KeyJ },
  { SDL_SCANCODE_K,              Input::KeyK },
  { SDL_SCANCODE_L,              Input::KeyL },
  { SDL_SCANCODE_M,              Input::KeyM },
  { SDL_SCANCODE_N,              Input::KeyN },
  { SDL_SCANCODE_O,              Input::KeyO },
  { SDL_SCANCODE_P,              Input::KeyP },
  { SDL_SCANCODE_Q,              Input::KeyQ },
  { SDL_SCANCODE_R,              Input::KeyR },
  { SDL_SCANCODE_S,              Input::KeyS },
  { SDL_SCANCODE_T,              Input::KeyT },
  { SDL_SCANCODE_U,              Input::KeyU },
  { SDL_SCANCODE_V,              Input::KeyV },
  { SDL_SCANCODE_W,              Input::KeyW },
  { SDL_SCANCODE_X,              Input::KeyX },
  { SDL_SCANCODE_Y,              Input::KeyY },
  { SDL_SCANCODE_Z,              Input::KeyZ },
  { SDL_SCANCODE_F3,             Input::F3   },
  { SDL_SCANCODE_F4,             Input::F4   },
  { SDL_SCANCODE_F5,             Input::F5   },
  { SDL_SCANCODE_F6,             Input::F6   },
  { SDL_SCANCODE_F7,             Input::F7   },
  { SDL_SCANCODE_F8,             Input::F8   },
  { SDL_SCANCODE_F9,             Input::F9   },
  { SDL_SCANCODE_F10,            Input::F10  },
  { SDL_SCANCODE_F11,            Input::F11  },
  { SDL_SCANCODE_F12,            Input::F12  },
  { SDL_SCANCODE_1,              Input::N1   },
  { SDL_SCANCODE_2,              Input::N2   },
  { SDL_SCANCODE_3,              Input::N3   },
  { SDL_SCANCODE_4,              Input::N4   },
  { SDL_SCANCODE_5,              Input::N5   },
  { SDL_SCANCODE_6,              Input::N6   },
  { SDL_SCANCODE_7,              Input::N7   },
  { SDL_SCANCODE_8,              Input::N8   },
  { SDL_SCANCODE_9,              Input::N9   },
  { SDL_SCANCODE_0,              Input::N0   },
  { SDL_SCANCODE_RETURN,         Input::Return         },
  { SDL_SCANCODE_ESCAPE,         Input::Escape         },
  { SDL_SCANCODE_BACKSPACE,      Input::Backspace      },
  { SDL_SCANCODE_SPACE,          Input::Space          },
  { SDL_SCANCODE_TAB,            Input::Tab            },
  { SDL_SCANCODE_MINUS,          Input::Minus          },
  { SDL_SCANCODE_EQUALS,         Input::Equals         },
  { SDL_SCANCODE_LEFTBRACKET,    Input::LeftBracket    },
  { SDL_SCANCODE_RIGHTBRACKET,   Input::RightBracket   },
  { SDL_SCANCODE_BACKSLASH,      Input::BackSlash      },
  { SDL_SCANCODE_SEMICOLON,      Input::Semicolon      },
  { SDL_SCANCODE_APOSTROPHE,     Input::Apostrophe     },
  { SDL_SCANCODE_GRAVE,          Input::Grave          },
  { SDL_SCANCODE_COMMA,          Input::Comma          },
  { SDL_SCANCODE_PERIOD,         Input::Period         },
  { SDL_SCANCODE_SLASH,          Input::Slash          },
  { SDL_SCANCODE_CAPSLOCK,       Input::CapsLock       },
  { SDL_SCANCODE_SCROLLLOCK,     Input::ScrollLock     },
  { SDL_SCANCODE_PAUSE,          Input::Pause          },
  { SDL_SCANCODE_INSERT,         Input::Insert         },
  { SDL_SCANCODE_HOME,           Input::Home           },
  { SDL_SCANCODE_PAGEUP,         Input::PageUp         },
  { SDL_SCANCODE_DELETE,         Input::Delete         },
  { SDL_SCANCODE_END,            Input::End            },
  { SDL_SCANCODE_PAGEDOWN,       Input::PageDown       },
  { SDL_SCANCODE_KP_DIVIDE,      Input::NumPadDivide   },
  { SDL_SCANCODE_KP_MULTIPLY,    Input::NumPadMultiply },
  { SDL_SCANCODE_KP_MINUS,       Input::NumPadMinus    },
  { SDL_SCANCODE_KP_PLUS,        Input::NumPadPlus     },
  { SDL_SCANCODE_KP_ENTER,       Input::Enter          },
  { SDL_SCANCODE_KP_1,           Input::NumPad1        },
  { SDL_SCANCODE_KP_2,           Input::NumPad2        },
  { SDL_SCANCODE_KP_3,           Input::NumPad3        },
  { SDL_SCANCODE_KP_4,           Input::NumPad4        },
  { SDL_SCANCODE_KP_5,           Input::NumPad5        },
  { SDL_SCANCODE_KP_6,           Input::NumPad6        },
  { SDL_SCANCODE_KP_7,           Input::NumPad7        },
  { SDL_SCANCODE_KP_8,           Input::NumPad8        },
  { SDL_SCANCODE_KP_9,           Input::NumPad9        },
  { SDL_SCANCODE_KP_0,           Input::NumPad0        },
  { SDL_SCANCODE_KP_PERIOD,      Input::NumPadDot      },
  { SDL_SCANCODE_NONUSBACKSLASH, Input::LessOrGreater  },
  { SDL_SCANCODE_APPLICATION,    Input::APP            },
  { SDL_SCANCODE_KP_EQUALS,      Input::NumPadEquals   },
  { SDL_SCANCODE_LCTRL,          Input::LeftCtrl       },
  { SDL_SCANCODE_LSHIFT,         Input::LeftShift      },
  { SDL_SCANCODE_LALT,           Input::LeftAlt        },
  { SDL_SCANCODE_LGUI,           Input::LeftMeta       },
  { SDL_SCANCODE_RCTRL,          Input::RightCtrl      },
  { SDL_SCANCODE_RSHIFT,         Input::RightShift     },
  { SDL_SCANCODE_RALT,           Input::RightAlt       },
  { SDL_SCANCODE_RGUI,           Input::RightMeta      },
  { SDL_SCANCODE_WWW,            Input::Web            },
  { SDL_SCANCODE_MAIL,           Input::Mail           },
  { SDL_SCANCODE_CALCULATOR,     Input::Calculator     },
  { SDL_SCANCODE_COMPUTER,       Input::Computer       },
  { SDL_SCANCODE_APP1,           Input::APP1           },
  { SDL_SCANCODE_APP2,           Input::APP2           }
};

static elementsN(staticKbBindings);

/* Maps ButtonCode enum values to indices
 * in the button state array */
static const int mapToIndex[] =
{
	0, 0,
	1, 0, 2, 0, 3, 0, 4, 0,
	0,
	5, 6, 7, 8, 9, 10, 11, 12,
	0, 0,
	13, 14, 15,
	0,
	16, 17, 18, 19, 20,
	0, 0, 0, 0, 0, 0, 0, 0,
	21, 22//, 42, 43, 60, 61, 62, 63, 64, 65, 66, 67, 68, // 23, *...* 42, 43...
};

static elementsN(mapToIndex);

static const Input::ButtonCode dirs[] =
{ Input::Down, Input::Left, Input::Right, Input::Up };

static const int dirFlags[] =
{
  1 << Input::Down,
  1 << Input::Left,
  1 << Input::Right,
  1 << Input::Up
};

/* Dir4 is always zero on these combinations */
static const int deadDirFlags[] =
{
  dirFlags[0] | dirFlags[3],
  dirFlags[1] | dirFlags[2]
};

static const Input::ButtonCode otherDirs[4][3] =
{
  { Input::Left, Input::Right, Input::Up    }, /* Down  */
  { Input::Down, Input::Up,    Input::Right }, /* Left  */
  { Input::Down, Input::Up,    Input::Left  }, /* Right */
  { Input::Left, Input::Right, Input::Up    }  /* Up    */
};

struct InputPrivate
{
  std::vector<KbBinding> kbStatBindings;
  std::vector<KbBinding> kbBindings;
  std::vector<JsAxisBinding> jsABindings;
  std::vector<JsHatBinding> jsHBindings;
  std::vector<JsButtonBinding> jsBBindings;
  std::vector<MsBinding> msBindings;
  /* Collective binding array */
  std::vector<Binding*> bindings;
  ButtonState stateArray[300 * 2]; // stateArray[BUTTON_CODE_COUNT*2];
  ButtonState *states;
  ButtonState *statesOld;
  Input::ButtonCode repeating;
  unsigned int repeatCount;

  struct
  {
    int active;
    Input::ButtonCode previous;
  } dir4Data;

  struct
  {
    int active;
  } dir8Data;

  InputPrivate(const RGSSThreadData &rtData)
  {
    initStaticKbBindings();
    initMsBindings();
    /* Main thread should have these posted by now */
    checkBindingChange(rtData);
    states    = stateArray;
    statesOld = stateArray + 300; // + BUTTON_CODE_COUNT;
    /* Clear buffers */
    clearBuffer();
    swapBuffers();
    clearBuffer();
    repeating = Input::None;
    repeatCount = 0;
    dir4Data.active = 0;
    dir4Data.previous = Input::None;
    dir8Data.active = 0;
  }

  inline ButtonState &getStateCheck(int code)
  {
    if (code < 0) return states[0];
    return states[code];
  }
          /* int index;
          //if (code < 0 || (size_t) code > mapToIndexN-1) // index = 0;
          //else // index = mapToIndex[code];
          return states[index];  }*/

  inline ButtonState &getState(Input::ButtonCode code)
  {
    if (code < 0) return states[0];
    return states[code]; // states[mapToIndex[code]];
  }

  inline ButtonState &getOldState(Input::ButtonCode code)
  {
    if (code < 0) return states[0];
    return statesOld[code]; // statesOld[mapToIndex[code]];
  }

  void swapBuffers()
  {
    ButtonState *tmp = states;
    states = statesOld;
    statesOld = tmp;
  }

  void clearBuffer()
  { //  const size_t size = sizeof(ButtonState) * BUTTON_CODE_COUNT;
    memset(states, 0, 300 * 2);// size);
  }

  void checkBindingChange(const RGSSThreadData &rtData)
  {
    BDescVec d;
    if (!rtData.bindingUpdateMsg.poll(d)) return;
    applyBindingDesc(d);
  }

  template<class B>
  void appendBindings(std::vector<B> &bind)
  {
    for (size_t i = 0; i < bind.size(); ++i)
    bindings.push_back(&bind[i]);
  }

  void applyBindingDesc(const BDescVec &d)
  {
    kbBindings.clear();
    jsABindings.clear();
    jsHBindings.clear();
    jsBBindings.clear();
    for (size_t i = 0; i < d.size(); ++i) {
      const BindingDesc &desc = d[i];
      const SourceDesc &src = desc.src;
      if (desc.target == Input::None) continue;
      switch (desc.src.type)
      {
      case Invalid :
        break;
      case Key :
      {
        KbBinding bind;
        bind.source = src.d.scan;
        bind.target = desc.target;
        kbBindings.push_back(bind);
        break;
      }
      case JAxis :
      {
        JsAxisBinding bind;
        bind.source = src.d.ja.axis;
        bind.dir = src.d.ja.dir;
        bind.target = desc.target;
        jsABindings.push_back(bind);
        break;
      }
      case JHat :
      {
        JsHatBinding bind;
        bind.source = src.d.jh.hat;
        bind.pos = src.d.jh.pos;
        bind.target = desc.target;
        jsHBindings.push_back(bind);
        break;
      }
      case JButton :
      {
        JsButtonBinding bind;
        bind.source = src.d.jb;
        bind.target = desc.target;
        jsBBindings.push_back(bind);
        break;
      }
      default :
        assert(!"unreachable");
      }
    }
    bindings.clear();
    appendBindings(kbStatBindings);
    appendBindings(msBindings);
    appendBindings(kbBindings);
    appendBindings(jsABindings);
    appendBindings(jsHBindings);
    appendBindings(jsBBindings);
  }

  void initStaticKbBindings()
  {
    kbStatBindings.clear();
    for (size_t i = 0; i < staticKbBindingsN; ++i)
    kbStatBindings.push_back(KbBinding(staticKbBindings[i]));
  }

  void initMsBindings()
  {
    msBindings.resize(3);
    size_t i = 0;
    msBindings[i++] = MsBinding(SDL_BUTTON_LEFT,   Input::MouseLeft);
    msBindings[i++] = MsBinding(SDL_BUTTON_MIDDLE, Input::MouseMiddle);
    msBindings[i++] = MsBinding(SDL_BUTTON_RIGHT,  Input::MouseRight);
  }

  void pollBindings(Input::ButtonCode &repeatCand)
  {
    for (size_t i = 0; i < bindings.size(); ++i)
      pollBindingPriv(*bindings[i], repeatCand);
    updateDir4();
    updateDir8();
  }

  void pollBindingPriv(const Binding &b, Input::ButtonCode &repeatCand)
  {
    if (!b.sourceActive()) return;
    if (b.target == Input::None) return;
    ButtonState &state = getState(b.target);
    ButtonState &oldState = getOldState(b.target);
    state.pressed = true;
    /* Must have been released before to trigger */
    if (!oldState.pressed)
      state.triggered = true;
    /* Unbound keys don't create/break repeat */
    //if (repeatCand != Input::None) return;
    if (repeating != b.target && !oldState.pressed) {
      if (b.sourceRepeatable())
        repeatCand = b.target;
      else /* Unrepeatable keys still break current repeat */
        repeating = Input::None;
    }
  }

  void updateDir4()
  {
    int dirFlag = 0;
    for (size_t i = 0; i < 4; ++i)
      dirFlag |= (getState(dirs[i]).pressed ? dirFlags[i] : 0);
    if (dirFlag == deadDirFlags[0] || dirFlag == deadDirFlags[1])
    {
      dir4Data.active = Input::None;
      return;
    }
    if (dir4Data.previous != Input::None)
    { /* Check if prev still pressed */
      if (getState(dir4Data.previous).pressed)
      {
        for (size_t i = 0; i < 3; ++i)
        {
          Input::ButtonCode other = otherDirs[(dir4Data.previous/2)-1][i];
          if (!getState(other).pressed) continue;
          dir4Data.active = other;
          return;
        }
      }
    }
    for (size_t i = 0; i < 4; ++i)
    {
      if (!getState(dirs[i]).pressed) continue;
      dir4Data.active = dirs[i];
      dir4Data.previous = dirs[i];
      return;
    }
    dir4Data.active   = Input::None;
    dir4Data.previous = Input::None;
  }

  void updateDir8()
  {
    static const int combos[4][4] =
    {
      { 2, 1, 3, 0 },
      { 1, 4, 0, 7 },
      { 3, 0, 6, 9 },
      { 0, 7, 9, 8 }
    };
    dir8Data.active = 0;
    for (size_t i = 0; i < 4; ++i)
    {
      Input::ButtonCode one = dirs[i];
      if (!getState(one).pressed) continue;
      for (int j = 0; j < 3; ++j)
      {
        Input::ButtonCode other = otherDirs[i][j];
        if (!getState(other).pressed) continue;
        dir8Data.active = combos[(one/2)-1][(other/2)-1];
        return;
      }
      dir8Data.active = one;
      return;
    }
  }
};


Input::Input(const RGSSThreadData &rtData)
{
  p = new InputPrivate(rtData);
}

void Input::update()
{
  shState->checkShutdown();
  p->checkBindingChange(shState->rtData());
  p->swapBuffers();
  p->clearBuffer();
  ButtonCode repeatCand = None;
  // Poll all bindings
  p->pollBindings(repeatCand);
  // Check for new repeating key
  if (repeatCand != None && repeatCand != p->repeating) {
    p->repeating = repeatCand;
    p->repeatCount = 0;
    p->getState(repeatCand).repeated = true;
    return;
  }
  // Check if repeating key is still pressed
  if (p->getState(p->repeating).pressed) {
    p->repeatCount++;
    bool repeated;
    if (rgssVer >= 2)
      repeated = p->repeatCount >= 23 && ((p->repeatCount+1) % 6) == 0;
    else
      repeated = p->repeatCount >= 15 && ((p->repeatCount+1) % 4) == 0;
    p->getState(p->repeating).repeated |= repeated;
    return;
  }
  p->repeating = None;
}

bool Input::isLeftClick()
{
  bool trig = p->getStateCheck(MouseLeft).pressed ? true : false;
  p->getStateCheck(MouseMiddle).pressed = false;
  p->getStateCheck(MouseRight).pressed = false;
  p->getStateCheck(MouseLeft).pressed = false;
  bool state = p->getStateCheck(MouseLeft).pressed ? true : false;
  return (trig && !state);
}

bool Input::isMiddleClick()
{
  return isPressed(MouseMiddle);
}

bool Input::isRightClick()
{
  return isPressed(MouseRight);
}

bool Input::isPressed(int button)
{
  if (button == Shift)
    return p->getStateCheck(LeftShift).pressed || p->getStateCheck(RightShift).pressed;
  if (button == Ctrl)
    return p->getStateCheck(LeftCtrl).pressed || p->getStateCheck(RightCtrl).pressed;
  if (button == Alt)
    return p->getStateCheck(LeftAlt).pressed || p->getStateCheck(RightAlt).pressed;
  if (button == MouseLeft || button == MouseRight) {
    bool trig = p->getStateCheck(button).pressed ? true : false;
    p->getStateCheck(button).pressed = false;
    return trig;
  }
  return p->getStateCheck(button).pressed;
}

bool Input::isTriggered(int button)
{
  if (button == Shift)
    return p->getStateCheck(LeftShift).triggered || p->getStateCheck(RightShift).triggered;
  if (button == Ctrl)
    return p->getStateCheck(LeftCtrl).triggered || p->getStateCheck(RightCtrl).triggered;
  if (button == Alt)
    return p->getStateCheck(LeftAlt).triggered || p->getStateCheck(RightAlt).triggered;
  if (button == MouseLeft) {
    bool trig = p->getStateCheck(MouseLeft).triggered ? true : false;
    p->getStateCheck(MouseLeft).triggered = false;
    return trig;
  } else if (button == MouseRight) {
    bool trig = p->getStateCheck(MouseRight).triggered ? true : false;
    p->getStateCheck(MouseRight).triggered = false;
    return trig;
  }
  return p->getStateCheck(button).triggered;
}

bool Input::isRepeated(int button)
{
  if (button == Shift)
    return p->getStateCheck(LeftShift).repeated || p->getStateCheck(RightShift).repeated;
  if (button == Ctrl)
    return p->getStateCheck(LeftCtrl).repeated || p->getStateCheck(RightCtrl).repeated;
  if (button == Alt)
    return p->getStateCheck(LeftAlt).repeated || p->getStateCheck(RightAlt).repeated;
  return p->getStateCheck(button).repeated;
}

int Input::dir4Value()
{
  return p->dir4Data.active;
}

int Input::dir8Value()
{
  return p->dir8Data.active;
}

int Input::mouseX()
{
  RGSSThreadData &rtData = shState->rtData();
  return (EventThread::mouseState.x - rtData.screenOffset.x) * rtData.sizeResoRatio.x;
}

int Input::mouseY()
{
  RGSSThreadData &rtData = shState->rtData();
  return (EventThread::mouseState.y - rtData.screenOffset.y) * rtData.sizeResoRatio.y;
}

Input::~Input()
{
  delete p;
}

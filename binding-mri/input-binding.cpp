/*
** input-binding.cpp
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
#include "exception.h"
#include "binding-util.h"
#include "util.h"

RB_METHOD(inputUpdate)
{
  RB_UNUSED_PARAM;
  shState->input().update();
  return Qnil;
}

static int getButtonArg(int argc, VALUE *argv)
{
  int num;
  rb_check_argc(argc, 1);
  if (FIXNUM_P(argv[0])) {
    num = FIX2INT(argv[0]);
  } else if (SYMBOL_P(argv[0]) && rgssVer >= 3) {
    VALUE symHash = getRbData()->buttoncodeHash;
    num = FIX2INT(rb_hash_lookup2(symHash, argv[0], INT2FIX(Input::None)));
  } else {
    // FIXME: RMXP allows only few more types that
    // don't make sense (symbols in pre 3, floats)
    num = 0;
  }
  return num;
}

RB_METHOD(inputPress)
{
  RB_UNUSED_PARAM;
  int num = getButtonArg(argc, argv);
  return rb_bool_new(shState->input().isPressed(num));
}

RB_METHOD(inputTrigger)
{
  RB_UNUSED_PARAM;
  int num = getButtonArg(argc, argv);
  return rb_bool_new(shState->input().isTriggered(num));
}

RB_METHOD(inputRepeat)
{
  RB_UNUSED_PARAM;
  int num = getButtonArg(argc, argv);
  return rb_bool_new(shState->input().isRepeated(num));
}

RB_METHOD(inputDir4)
{
  RB_UNUSED_PARAM;
  return rb_fix_new(shState->input().dir4Value());
}

RB_METHOD(inputDir8)
{
  RB_UNUSED_PARAM;
  return rb_fix_new(shState->input().dir8Value());
}

/* Non-standard extensions */
RB_METHOD(inputMouseX)
{
  RB_UNUSED_PARAM;
  return rb_fix_new(shState->input().mouseX());
}

RB_METHOD(inputMouseY)
{
  RB_UNUSED_PARAM;
  return rb_fix_new(shState->input().mouseY());
}

struct
{
  const char *str;
  Input::ButtonCode val;
}
static buttonCodes[] =
{
  { "DOWN",           Input::Down  },
  { "LEFT",           Input::Left  },
  { "RIGHT",          Input::Right },
  { "UP",             Input::Up    },
  { "A",              Input::A     },
  { "B",              Input::B     },
  { "C",              Input::C     },
  { "X",              Input::X     },
  { "Y",              Input::Y     },
  { "Z",              Input::Z     },
  { "L",              Input::L     },
  { "R",              Input::R     },
  { "SHIFT",          Input::Shift },
  { "CTRL",           Input::Ctrl  },
  { "ALT",            Input::Alt   },
  { "KeyA",           Input::KeyA  },
  { "KeyB",           Input::KeyB  },
  { "KeyC",           Input::KeyC  },
  { "KeyD",           Input::KeyD  },
  { "KeyE",           Input::KeyE  },
  { "KeyF",           Input::KeyF  },
  { "KeyG",           Input::KeyG  },
  { "KeyH",           Input::KeyH  },
  { "KeyI",           Input::KeyI  },
  { "KeyJ",           Input::KeyJ  },
  { "KeyK",           Input::KeyK  },
  { "KeyL",           Input::KeyL  },
  { "KeyM",           Input::KeyM  },
  { "KeyN",           Input::KeyN  },
  { "KeyO",           Input::KeyO  },
  { "KeyP",           Input::KeyP  },
  { "KeyQ",           Input::KeyQ  },
  { "KeyR",           Input::KeyR  },
  { "KeyS",           Input::KeyS  },
  { "KeyT",           Input::KeyT  },
  { "KeyU",           Input::KeyU  },
  { "KeyV",           Input::KeyV  },
  { "KeyW",           Input::KeyW  },
  { "KeyX",           Input::KeyX  },
  { "KeyY",           Input::KeyY  },
  { "KeyZ",           Input::KeyZ  },
  { "N1",             Input::N1    },
  { "N2",             Input::N2    },
  { "N3",             Input::N3    },
  { "N4",             Input::N4    },
  { "N5",             Input::N5    },
  { "N6",             Input::N6    },
  { "N7",             Input::N7    },
  { "N8",             Input::N8    },
  { "N9",             Input::N9    },
  { "N0",             Input::N0    },
  { "Return",         Input::Return         },
  { "Escape",         Input::Escape         },
  { "Backspace",      Input::Backspace      },
  { "Space",          Input::Space          },
  { "Minus",          Input::Minus          },
  { "Equals",         Input::Equals         },
  { "LeftBracket",    Input::LeftBracket    },
  { "RightBracket",   Input::RightBracket   },
  { "BackSlash",      Input::BackSlash      },
  { "Semicolon",      Input::Semicolon      },
  { "Apostrophe",     Input::Apostrophe     },
  { "Grave",          Input::Grave          },
  { "Comma",          Input::Comma          },
  { "Period",         Input::Period         },
  { "Slash",          Input::Slash          },
  { "Tab",            Input::Tab            },
  { "F3",             Input::F3             },
  { "F4",             Input::F4             },
  { "F5",             Input::F5             },
  { "F6",             Input::F6             },
  { "F7",             Input::F7             },
  { "F8",             Input::F8             },
  { "F9",             Input::F9             },
  { "F10",            Input::F10            },
  { "F11",            Input::F11            },
  { "F12",            Input::F12            },
  { "ScrollLock",     Input::ScrollLock     },
  { "CapsLock",       Input::CapsLock       },
  { "Pause",          Input::Pause          },
  { "Insert",         Input::Insert         },
  { "Home",           Input::Home           },
  { "PageUp",         Input::PageUp         },
  { "Delete",         Input::Delete         },
  { "End",            Input::End            },
  { "PageDown",       Input::PageDown       },
  { "NumPadDivide",   Input::NumPadDivide   },
  { "NumPadMultiply", Input::NumPadMultiply },
  { "NumPadMinus",    Input::NumPadMinus    },
  { "NumPadPlus",     Input::NumPadPlus     },
  { "Enter",          Input::Enter          },
  { "NumPad1",        Input::NumPad1        },
  { "NumPad2",        Input::NumPad2        },
  { "NumPad3",        Input::NumPad3        },
  { "NumPad4",        Input::NumPad4        },
  { "NumPad5",        Input::NumPad5        },
  { "NumPad6",        Input::NumPad6        },
  { "NumPad7",        Input::NumPad7        },
  { "NumPad8",        Input::NumPad8        },
  { "NumPad9",        Input::NumPad9        },
  { "NumPad0",        Input::NumPad0        },
  { "NumPadDot",      Input::NumPadDot      },
  { "LessOrGreater",  Input::LessOrGreater  },
  { "Input::APP",     Input::APP            },
  { "NumPadEquals",   Input::NumPadEquals   },
  { "LeftCtrl",       Input::LeftCtrl       },
  { "LeftShift",      Input::LeftShift      },
  { "LeftAlt",        Input::LeftAlt        },
  { "LeftMeta",       Input::LeftMeta       },
  { "RightCtrl",      Input::RightCtrl      },
  { "RightShift",     Input::RightShift     },
  { "RightAlt",       Input::RightAlt       },
  { "RightMeta",      Input::RightMeta      },
  { "Web",            Input::Web            },
  { "Mail",           Input::Mail           },
  { "Calculator",     Input::Calculator     },
  { "Computer",       Input::Computer       },
  { "APP1",           Input::APP1           },
  { "APP2",           Input::APP2           },
  { "MOUSELEFT",      Input::MouseLeft      },
  { "MOUSEMIDDLE",    Input::MouseMiddle    },
  { "MOUSERIGHT",     Input::MouseRight     },
  { "MouseLeft",      Input::MouseLeft      },
  { "MouseMiddle",    Input::MouseMiddle    },
  { "MouseRight",     Input::MouseRight     }
};

static elementsN(buttonCodes);

void inputBindingInit()
{
  VALUE module = rb_define_module("Input");
  _rb_define_module_function(module, "update", inputUpdate);
  _rb_define_module_function(module, "press?", inputPress);
  _rb_define_module_function(module, "trigger?", inputTrigger);
  _rb_define_module_function(module, "repeat?", inputRepeat);
  _rb_define_module_function(module, "dir4", inputDir4);
  _rb_define_module_function(module, "dir8", inputDir8);
  _rb_define_module_function(module, "mouse_x", inputMouseX);
  _rb_define_module_function(module, "mouse_y", inputMouseY);
  if (rgssVer >= 3) {
    VALUE symHash = rb_hash_new();
    for (size_t i = 0; i < buttonCodesN; ++i) {
      ID sym = rb_intern(buttonCodes[i].str);
      VALUE val = INT2FIX(buttonCodes[i].val);
      /* In RGSS3 all Input::XYZ constants are equal to :XYZ symbols,
       * to be compatible with the previous convention */
      rb_const_set(module, sym, ID2SYM(sym));
      rb_hash_aset(symHash, ID2SYM(sym), val);
    }
    rb_iv_set(module, "buttoncodes", symHash);
    getRbData()->buttoncodeHash = symHash;
  } else {
    for (size_t i = 0; i < buttonCodesN; ++i) {
      ID sym = rb_intern(buttonCodes[i].str);
      VALUE val = INT2FIX(buttonCodes[i].val);
      rb_const_set(module, sym, val);
    }
  }
}
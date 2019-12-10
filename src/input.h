/*
** input.h
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
** along with HiddenChest. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INPUT_H
#define INPUT_H

struct InputPrivate;
struct RGSSThreadData;

class Input
{
public:
  enum ButtonCode
  {
    None = 0,
    Down = 2, Left = 4, Right = 6, Up = 8,
    A = 11, B = 12, C = 13, X = 14, Y = 15, Z = 16, L = 17, R = 18,
    Shift = 21, Ctrl = 22, Alt = 23,
    // Non-standard extensions
    F3 = 20, F4 = 24, F5 = 25, F6 = 26, F7 = 27, F8 = 28, F9 = 29,
    F10 = 30, F11 = 31, F12 = 32,
    KeyA = 33, KeyB = 34, KeyC = 35, KeyD = 36, KeyE = 37, KeyF = 38,
    KeyG = 39, KeyH = 40, KeyI = 41, KeyJ = 42, KeyK = 43, KeyL = 44,
    KeyM = 45, KeyN = 46, KeyO = 47, KeyP = 48, KeyQ = 49, KeyR = 50,
    KeyS = 51, KeyT = 52, KeyU = 53, KeyV = 54, KeyW = 55, KeyX = 56,
    KeyY = 57, KeyZ = 58,
    N1 = 59, N2 = 60, N3 = 61, N4 = 62, N5 = 63,
    N6 = 64, N7 = 65, N8 = 66, N9 = 67, N0 = 68,
    Return = 69, Escape = 70, Backspace = 71, Tab = 72, Space = 73,
    Minus = 74, Equals = 75, LeftBracket = 76, RightBracket = 77,
    BackSlash = 78, Semicolon = 79, Apostrophe = 80, Grave = 81,
    Comma = 82, Period = 83, Slash = 84, CapsLock = 85, PrintScreen = 86,
    ScrollLock = 87, Pause = 88, Insert = 89, Home = 90, PageUp = 91,
    Delete = 92, End = 93, PageDown = 94,
    NumPadDivide = 95, NumPadMultiply = 96, NumPadMinus = 97,
    NumPadPlus = 98, Enter = 99, NumPad1 = 100, NumPad2 = 101,
    NumPad3 = 102, NumPad4 = 103, NumPad5 = 104, NumPad6 = 105,
    NumPad7 = 106, NumPad8 = 107, NumPad9 = 108, NumPad0 = 109, NumPadDot = 110,
    LessOrGreater = 111, APP = 112, NumPadEquals = 113,
    LeftCtrl = 114, LeftShift = 115, LeftAlt = 116,
    RightCtrl = 117, RightShift = 118, RightAlt = 119,
    LeftMeta = 120, RightMeta = 121,
    Web = 124, Mail = 125, Calculator = 126, Computer = 127,
    APP1 = 128, APP2 = 129,
    MouseLeft = 130, MouseMiddle = 131, MouseRight = 132,
  };
  void update();
  bool is_left_click();
  bool is_middle_click();
  bool is_right_click();
  bool isPressed(int button);
  bool isTriggered(int button);
  bool isRepeated(int button);
  int dir4Value();
  int dir8Value();
  bool is_dir4();
  bool is_dir8();
  /* Non-standard extensions */
  int mouseX();
  int mouseY();
  bool is_any_char();
  char* string();
  void enableMode(bool state);

private:
  Input(const RGSSThreadData &rtData);
  ~Input();
  friend struct SharedStatePrivate;
  InputPrivate *p;
};

#endif // INPUT_H

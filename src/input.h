/*
** input.h
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
    //Shift = 21, Ctrl = 22, Alt = 23,//F5 = 25, F6 = 26, F7 = 27, F8 = 28, F9 = 29,
    KeyA = 4, KeyB = 5, KeyC = 6, KeyD = 7, KeyE = 8, KeyF = 9, KeyG = 10, KeyH = 11,
    KeyI = 12, KeyJ = 13, KeyK = 14, KeyL = 15, KeyM = 16, KeyN = 17, KeyO = 18,
    KeyP = 19, KeyQ = 20, KeyR = 21, KeyS = 22, KeyT = 23, KeyU = 24, KeyV = 25,
    KeyW = 26, KeyX = 27, KeyY = 28, KeyZ = 29, N1 = 30, N2 = 31, N3 = 32, N4 = 33,
    N5 = 34, N6 = 35, N7 = 36, N8 = 37, N9 = 38, N0 = 39,
    Return = 40, Escape = 41, Backspace = 42, Tab = 43, Space = 44, Minus = 45,
    Equals = 46, LeftBracket = 47, RightBracket = 48, BackSlash = 49, Semicolon = 51,
    Apostrophe = 52, Grave = 53, Comma = 54, Period = 55, Slash = 56, CapsLock = 57,
    F3 = 60, F4 = 61, F5 = 62, F6 = 63, F7 = 64, F8 = 65, F9 = 66, F10 = 67, F11 = 68, F12 = 69,
    ScrollLock = 71, Pause = 72, Insert = 73, Home = 74, PageUp = 75, Delete = 76,
    End = 77, PageDown = 78, NumPadDivide = 84, NumPadMultiply = 85, NumPadMinus = 86,
    NumPadPlus = 87, Enter = 88, NumPad1 = 89, NumPad2 = 90, NumPad3 = 91, NumPad4 = 92,
    NumPad5 = 93, NumPad6 = 94, NumPad7 = 95, NumPad8 = 96, NumPad9 = 97, NumPad0 = 98,
    NumPadDot = 99, LessOrGreater = 100, APP = 101, NumPadEquals = 103,
    LeftCtrl = 224, LeftShift = 225, LeftAlt = 226, LeftMeta = 227, RightCtrl = 228,
    RightShift = 229, RightAlt = 230, RightMeta = 231, Web = 264, Mail = 265,
    Calculator = 266, Computer = 267, APP1 = 283, APP2 = 284,
    Shift = 285, Ctrl = 286, Alt = 287,
    // Non-standard extensions
    MouseLeft = 297, MouseMiddle = 298, MouseRight = 299 // 38 39 40
  };
  void update();
  bool isPressed(int button);
  bool isTriggered(int button);
  bool isRepeated(int button);
  int dir4Value();
  int dir8Value();
  /* Non-standard extensions */
  int mouseX();
  int mouseY();

private:
  Input(const RGSSThreadData &rtData);
  ~Input();
  friend struct SharedStatePrivate;
  InputPrivate *p;
};

#endif // INPUT_H

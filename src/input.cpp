/* -- Bad Code --
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
#include "eventthread.h" //include "keybindings.h"
#include "exception.h"
#include "util.h"
#include <SDL_mouse.h>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#define FRAMERATE 60
#define BUTTONS_SIZE 500

typedef std::vector<int> vec_int;

struct SomeButton
{// public:
  bool pressed = false, triggered = false, repeated = false;
  int kind = 1, scan_code = 0, input_code = 0;
  vec_int buttons;
  SomeButton() {}
  void set_codes(int this_scan_code, int this_input_code)
  {
    scan_code = this_scan_code;
    input_code = this_input_code;
  }

  void set_code(int this_input_code)
  {
    scan_code = this_input_code;
    input_code = this_input_code;
  }

  void reset_states()
  {
    pressed = false;
    triggered = false;
    repeated = false;
  }

  bool get_state()
  {
    if (kind == 0) return EventThread::mouseState.buttons[input_code];
    if (kind == 1) return EventThread::keyStates[input_code];
    return EventThread::joyState.buttons[input_code];
  }
};

struct InputPrivate
{
  std::vector<SomeButton> buttons;
  std::vector<SomeButton> old_buttons;
  int extra_btns[15];
  SomeButton *dir4_pressed, *dir8_pressed, *repeater;
  SomeButton *button, *old_button, *joy_button;
  bool repeating = false;
  int repeat_count = 0, dir4_val = 0, dir8_val = 0;
  struct
  {
    int dir4[8] = { 81, 80, 79, 82, 90, 92, 94, 98 };
    int dir8[1] = { 81 };
  } btns;

  InputPrivate(const RGSSThreadData &rtData)
  {
    int basic_buttons[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 50, 51, 52, 53, 54, 55,
      56, 57, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
      77, 78, 79, 80, 81, 82, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
      97, 98, 99, 100, 101, 102, 103,224, 225, 226, 227, 228, 229, 230, 231, 264,
      265, 266, 267, 295, 296, 297, 298 };
    buttons.resize(BUTTONS_SIZE);
    old_buttons.resize(BUTTONS_SIZE);
    for (int i = 0 ; i < BUTTONS_SIZE ; i++) {
      buttons[i] = SomeButton();
      old_buttons[i] = SomeButton();
      if (i < 4) {
        buttons[i].kind = 0;
        old_buttons[i].kind = 0;
      } else if (i > 309 && i < 316) {
        buttons[i].kind = 2;
        old_buttons[i].kind = 2;
      }
      if (std::find(basic_buttons, std::end(basic_buttons), i) != std::end(basic_buttons)) {
        buttons[i].set_code(i);
        old_buttons[i].set_code(i);
      }
    }
    std::cout << "Print Screen - Kind: " << buttons[70].kind << std::endl;
    buttons[1].set_codes(SDL_BUTTON_LEFT,   Input::MouseLeft);
    buttons[2].set_codes(SDL_BUTTON_MIDDLE, Input::MouseMiddle);
    buttons[3].set_codes(SDL_BUTTON_RIGHT,  Input::MouseRight);
    old_buttons[1].set_codes(SDL_BUTTON_LEFT,   Input::MouseLeft);
    old_buttons[2].set_codes(SDL_BUTTON_MIDDLE, Input::MouseMiddle);
    old_buttons[3].set_codes(SDL_BUTTON_RIGHT,  Input::MouseRight);
    int pair[][3] = {
      { 81, 90, 0 }, { 80, 92, 0 }, { 79, 94, 0 }, { 82, 96, 0 }, { 311, 311, 0 },
      { Input::KeyX, Input::Escape, Input::Backspace },
      { Input::KeyC, Input::Return, Input::Enter }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
      { Input::KeyQ, Input::PageUp, 0 }, { Input::KeyW, Input::PageDown, 0 },
      { Input::LeftShift, Input::RightShift }, { Input::LeftCtrl, Input::RightCtrl },
      { Input::LeftAlt, Input::RightAlt }
    };
    for (int i = 295 ; i < 310 ; i++) {
      int pos = i - 295;
      extra_btns[pos] = i;
      buttons[i].buttons.resize(sizeof(pair[pos])/sizeof(*pair[pos]));
      buttons[i].buttons[0] = pair[pos][0];
      buttons[i].buttons[1] = pair[pos][1];
      buttons[i].buttons[2] = pair[pos][2];
    }
    for (int i = 0 ; i < 16 ; i++)
      buttons[310 + i].set_code(i);
    reset_repeater();
    checkBindingChange(rtData);
  }

  void reset_repeater() { repeater = &buttons[0]; }

  void checkBindingChange(const RGSSThreadData &rtData)
  {
    BDescVec d;
    if (!rtData.bindingUpdateMsg.poll(d)) return;
    applyBindingDesc(d);
  }

  void applyBindingDesc(const BDescVec &d)
  {
    for (int i = 0 ; i < BUTTONS_SIZE ; i++) {
      buttons[i].reset_states();
      old_buttons[i].reset_states();
    }
    repeat_count = 0;
    dir4_pressed = &buttons[0];
    dir4_val = 0;
    dir8_pressed = &buttons[0];
    dir8_val = 0;
  }

  void record_last_poll()
  {
    for (int i = 0; i < BUTTONS_SIZE; i++) {
      button = &buttons[i];
      old_button = &old_buttons[i];
      old_button->pressed = button->pressed;
      old_button->triggered = button->triggered;
      old_button->repeated = button->repeated;
    }
  }

  void poll_bindings()
  {
    for (int i = 0; i < BUTTONS_SIZE ; i++) {
      button = &buttons[i];
      old_button = &old_buttons[i];
      if (button->input_code == 0) continue;
      if (!button->get_state()) {
        button->reset_states();
        if (button->input_code == repeater->input_code) {
          repeat_count = 0;
          reset_repeater();
        }
        continue;
      }
      button->pressed = true;
      if (!old_button->pressed && !old_button->triggered) {
        repeater = &buttons[i];
        button->triggered = true;
        button->repeated = true;
        repeat_count = 0;
        continue;
      }
      if (old_button->triggered) button->triggered = false;
      if (!button->get_state()) return;
      repeat_count++;
      button->repeated = (repeat_count > 17 && (repeat_count + 1) % 12 == 0);
    }
  }

  void updateDir4()
  {
    for (int i = 0 ; i < 8 ; i++) {
      int code = btns.dir4[i];
      if (!buttons[code].pressed) {
        buttons[code].pressed = false;
        continue;
      }
      dir4_val = i % 4 * 2 + 2;
      dir4_pressed = &buttons[295 + i];
      dir4_pressed->pressed = true;
      return;
    }
    dir4_pressed = &buttons[0];
    dir4_val = 0;
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
  p->record_last_poll();
  p->poll_bindings();
  p->updateDir4();
  return;
}

bool Input::isPressed(int button)
{
  if (button < 295) return p->buttons[button].pressed;
  vec_int ary = p->buttons[button].buttons;
  for (int i = 0 ; i < ary.size() ; i++)
    if (p->buttons[ary[i]].pressed) return true;
  return false;
}

bool Input::isTriggered(int button)
{
  if (button < 295) return p->buttons[button].triggered;
  vec_int ary = p->buttons[button].buttons;
  for (int i = 0 ; i < ary.size() ; i++)
    if (p->buttons[ary[i]].triggered) return true;
  return false;
}

bool Input::isRepeated(int button)
{
  if (button < 295) return p->buttons[button].repeated;
  vec_int ary = p->buttons[button].buttons;
  for (int i = 0 ; i < ary.size() ; i++)
    if (p->buttons[ary[i]].repeated) return true;
  return false;
}

int Input::dir4_value()
{
  return p->dir4_val;
}

int Input::dir8_value()
{
  return p->dir8_val;
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

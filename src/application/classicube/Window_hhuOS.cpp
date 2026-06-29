/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <stddef.h>
#include <stdint.h>

#include "ClassiCube/src/Core.h"
#include "ClassiCube/src/Bitmap.h"
#include "ClassiCube/src/Errors.h"
#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Input.h"
#include "ClassiCube/src/Platform.h"
#include "ClassiCube/src/Window.h"
#include "ClassiCube/src/_WindowBase.h"

#include <util/graphic/Ansi.h>
#include <util/graphic/Colors.h>
#include <util/graphic/BufferedLinearFrameBuffer.h>
#include <util/io/file/File.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/io/key/KeyDecoder.h>
#include <util/io/key/MouseDecoder.h>
#include <util/io/key/layout/DeLayout.h>
#include <util/io/stream/FileInputStream.h>
#include <util/math/Math.h>
#include <util/base/Address.h>
#include <util/base/Panic.h>
#include <util/base/String.h>
#include <util/io/key/KeyEvent.h>

/// The frame buffer that is shown on the screen.
Util::Graphic::LinearFrameBuffer lfb(Util::Io::File("/device/lfb"));
/// Double buffer that ClassiCube renders in. It is flushed to `lfb` each frame.
Util::Graphic::BufferedLinearFrameBuffer *bufferedLfb = nullptr;

/// Keyboard layout to use for keyboard input.
Util::Io::DeLayout layout;
/// Decoder used to decode keyboard scancodes into key events.
Util::Io::KeyDecoder keyDecoder(layout);

/// Input stream for mouse data.
Util::Io::FileInputStream mouseInputStream("/device/mouse");
/// Buffer to read mouse events from `mouseInputStream` (each event consists of exactly four bytes).
uint8_t mouseValues[4]{};
/// Current index into `mouseValues`
size_t mouseValueIndex = 0;

/// Whether the mouse cursor is currently visible or not.
bool cursorVisible = true;
/// If enabled, raw mouse coordinates are forwarded to ClassiCube, otherwise they are clipped to screen boundaries.
/// Typically, ClassiCube enables raw mouse input for 3D-scenes and disables it for 2D-scenes (i.e., menus).
bool rawMouseInput = false;
/// Current x-coordinate of the mouse cursor.
int32_t mouseX = 0;
/// Current y-coordinate of the mouse cursor.
int32_t mouseY = 0;

/// Read all available key events and forward them to ClassiCube.
void pollKeyboard() {
	int16_t scancode = Util::System::in.read();
	while (scancode >= 0) {
		if (keyDecoder.parseScancode(scancode)) {
			auto key = keyDecoder.getKeyEvent();

			Input_SetNonRepeatable(CCKEY_RSHIFT, key.getShift());
			Input_SetNonRepeatable(CCKEY_LCTRL, key.getCtrlLeft());
			Input_SetNonRepeatable(CCKEY_LALT, key.getAltLeft());
			Input_SetNonRepeatable(CCKEY_SCROLLLOCK, key.getScrollLock());
			Input_SetNonRepeatable(CCKEY_NUMLOCK, key.getNumLock());
			Input_SetNonRepeatable(CCKEY_CAPSLOCK, key.getCapsLock());

			InputButtons ccCode;
			switch (key.getScancode()) {
				case Util::Io::KeyEvent::ESC:
					ccCode = CCKEY_ESCAPE;
					break;
				case Util::Io::KeyEvent::F1:
					ccCode = CCKEY_F1;
					break;
				case Util::Io::KeyEvent::F2:
					ccCode = CCKEY_F2;
					break;
				case Util::Io::KeyEvent::F3:
					ccCode = CCKEY_F3;
					break;
				case Util::Io::KeyEvent::F4:
					ccCode = CCKEY_F4;
					break;
				case Util::Io::KeyEvent::F5:
					ccCode = CCKEY_F5;
					break;
				case Util::Io::KeyEvent::F6:
					ccCode = CCKEY_F6;
					break;
				case Util::Io::KeyEvent::F7:
					ccCode = CCKEY_F7;
					break;
				case Util::Io::KeyEvent::F8:
					ccCode = CCKEY_F8;
					break;
				case Util::Io::KeyEvent::F9:
					ccCode = CCKEY_F9;
					break;
				case Util::Io::KeyEvent::F10:
					ccCode = CCKEY_F10;
					break;
				case Util::Io::KeyEvent::F11:
					ccCode = CCKEY_F11;
					break;
				case Util::Io::KeyEvent::F12:
					ccCode = CCKEY_F12;
					break;
				case Util::Io::KeyEvent::SPACE:
					ccCode = CCKEY_SPACE;
					break;
				case Util::Io::KeyEvent::ENTER:
					ccCode = CCKEY_ENTER;
					break;
				case Util::Io::KeyEvent::BACKSPACE:
					ccCode = CCKEY_BACKSPACE;
					break;
				case Util::Io::KeyEvent::UP:
					ccCode = CCKEY_UP;
					break;
				case Util::Io::KeyEvent::DOWN:
					ccCode = CCKEY_DOWN;
					break;
				case Util::Io::KeyEvent::LEFT:
					ccCode = CCKEY_LEFT;
					break;
				case Util::Io::KeyEvent::RIGHT:
					ccCode = CCKEY_RIGHT;
					break;
				case Util::Io::KeyEvent::ZERO:
					ccCode = CCKEY_0;
					break;
				case Util::Io::KeyEvent::ONE:
					ccCode = CCKEY_1;
					break;
				case Util::Io::KeyEvent::TWO:
					ccCode = CCKEY_2;
					break;
				case Util::Io::KeyEvent::THREE:
					ccCode = CCKEY_3;
					break;
				case Util::Io::KeyEvent::FOUR:
					ccCode = CCKEY_4;
					break;
				case Util::Io::KeyEvent::FIVE:
					ccCode = CCKEY_5;
					break;
				case Util::Io::KeyEvent::SIX:
					ccCode = CCKEY_6;
					break;
				case Util::Io::KeyEvent::SEVEN:
					ccCode = CCKEY_7;
					break;
				case Util::Io::KeyEvent::EIGHT:
					ccCode = CCKEY_8;
					break;
				case Util::Io::KeyEvent::NINE:
					ccCode = CCKEY_9;
					break;
				case Util::Io::KeyEvent::A:
					ccCode = CCKEY_A;
					break;
				case Util::Io::KeyEvent::B:
					ccCode = CCKEY_B;
					break;
				case Util::Io::KeyEvent::C:
					ccCode = CCKEY_C;
					break;
				case Util::Io::KeyEvent::D:
					ccCode = CCKEY_D;
					break;
				case Util::Io::KeyEvent::E:
					ccCode = CCKEY_E;
					break;
				case Util::Io::KeyEvent::F:
					ccCode = CCKEY_F;
					break;
				case Util::Io::KeyEvent::G:
					ccCode = CCKEY_G;
					break;
				case Util::Io::KeyEvent::H:
					ccCode = CCKEY_H;
					break;
				case Util::Io::KeyEvent::I:
					ccCode = CCKEY_I;
					break;
				case Util::Io::KeyEvent::J:
					ccCode = CCKEY_J;
					break;
				case Util::Io::KeyEvent::K:
					ccCode = CCKEY_K;
					break;
				case Util::Io::KeyEvent::L:
					ccCode = CCKEY_L;
					break;
				case Util::Io::KeyEvent::M:
					ccCode = CCKEY_M;
					break;
				case Util::Io::KeyEvent::N:
					ccCode = CCKEY_N;
					break;
				case Util::Io::KeyEvent::O:
					ccCode = CCKEY_O;
					break;
				case Util::Io::KeyEvent::P:
					ccCode = CCKEY_P;
					break;
				case Util::Io::KeyEvent::Q:
					ccCode = CCKEY_Q;
					break;
				case Util::Io::KeyEvent::R:
					ccCode = CCKEY_R;
					break;
				case Util::Io::KeyEvent::S:
					ccCode = CCKEY_S;
					break;
				case Util::Io::KeyEvent::T:
					ccCode = CCKEY_T;
					break;
				case Util::Io::KeyEvent::U:
					ccCode = CCKEY_U;
					break;
				case Util::Io::KeyEvent::V:
					ccCode = CCKEY_V;
					break;
				case Util::Io::KeyEvent::W:
					ccCode = CCKEY_W;
					break;
				case Util::Io::KeyEvent::X:
					ccCode = CCKEY_X;
					break;
				case Util::Io::KeyEvent::Y:
					ccCode = CCKEY_Y;
					break;
				case Util::Io::KeyEvent::Z:
					ccCode = CCKEY_Z;
					break;
				default:
					scancode = Util::System::in.read();
					continue;
			}

			if (key.isPressed()) {
				Input_SetPressed(ccCode);
				Event_RaiseInt(&InputEvents.Press, key.getAscii());
			} else {
				Input_SetReleased(ccCode);
			}
		}

		scancode = Util::System::in.read();
	}
}

/// Read all available mouse events and update the cursor position accordingly.
void pollMouse() {
	int16_t value = mouseInputStream.read();

	while (value >= 0) {
		mouseValues[mouseValueIndex++] = value;
		if (mouseValueIndex == 4) {
			const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);

			if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
				mouseX += mouseUpdate.xMovement;
				mouseY -= mouseUpdate.yMovement;

				if (!rawMouseInput) {
					if (mouseX < 0) {
						mouseX = 0;
					}
					if (mouseX >= DisplayInfo.Width) {
						mouseX = DisplayInfo.Width - 1;
					}
					if (mouseY < 0) {
						mouseY = 0;
					}
					if (mouseY >= DisplayInfo.Height) {
						mouseY = DisplayInfo.Height - 1;
					}

					Pointer_SetPosition(0, mouseX, mouseY);
					Event_RaiseVoid(&WindowEvents.RedrawNeeded);
				}
			}

			mouseValueIndex = 0;

			Input_SetNonRepeatable(CCMOUSE_L, mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON);
			Input_SetNonRepeatable(CCMOUSE_R, mouseUpdate.buttons & Util::Io::MouseDecoder::RIGHT_BUTTON);
			Input_SetNonRepeatable(CCMOUSE_M, mouseUpdate.buttons & Util::Io::MouseDecoder::MIDDLE_BUTTON);
		}

		value = mouseInputStream.read();
	}
}

/*##################################################################################################################*
*-----------------------------------------------ClassiCube Functions------------------------------------------------*
*###################################################################################################################*/

static void Cursor_DoSetVisible(const cc_bool visible) {
	cursorVisible = visible;
}

static void Cursor_GetRawPos(int *x, int *y) {
	*x = mouseX;
	*y = mouseY;
}

void Cursor_SetPosition(const int x, const int y) {
	mouseX = x;
	mouseY = y;
}

void Window_PreInit() {
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
	mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

	Util::Graphic::Ansi::prepareGraphicalApplication(true);
}

void Window_Init() {
	DisplayInfo.Width = lfb.getResolutionX();
	DisplayInfo.Height = lfb.getResolutionY();
	DisplayInfo.Depth = lfb.getColorDepth();
	DisplayInfo.ScaleX = 1.0;
	DisplayInfo.ScaleY = 1.0;
}

void Window_Free() {}

void Window_Create2D(int width, int height) {
	// 2D windows are always rendered at full display resolution
	Window_Main.Width    = lfb.getResolutionX();
	Window_Main.Height   = lfb.getResolutionY();
	Window_Main.Focused  = true;
	Window_Main.Exists   = true;
	Window_Main.UIScaleX = DEFAULT_UI_SCALE_X;
	Window_Main.UIScaleY = DEFAULT_UI_SCALE_Y;
}

void Window_Create3D(const int width, const int height) {
	// 3D windows are rendered at desired resolution and upscaled (resolution is capped to display resolution)
	Window_Main.Width    = width > lfb.getResolutionX() ? lfb.getResolutionX() : width;
	Window_Main.Height   = height > lfb.getResolutionY() ? lfb.getResolutionY() : height;
	Window_Main.Focused  = true;
	Window_Main.Exists   = true;
	Window_Main.UIScaleX = DEFAULT_UI_SCALE_X;
	Window_Main.UIScaleY = DEFAULT_UI_SCALE_Y;

	const auto scaleX = static_cast<float>(Window_Main.Width) / static_cast<float>(lfb.getResolutionX());
	const auto scaleY = static_cast<float>(Window_Main.Height) / static_cast<float>(lfb.getResolutionY());
	DisplayInfo.Width = Window_Main.Width;
	DisplayInfo.Height = Window_Main.Height;
	DisplayInfo.ScaleX = Util::Math::max(scaleX, 0.5f);
	DisplayInfo.ScaleY = Util::Math::max(scaleY, 0.5f);
}

void Window_Destroy() {}

void Window_SetTitle(const cc_string*) {}

void Clipboard_GetText(cc_string*) {}

void Clipboard_SetText(const cc_string*) {}

int Window_GetWindowState() {
	return WINDOW_STATE_NORMAL;
}

cc_result Window_EnterFullscreen() {
	return 0;
}

cc_result Window_ExitFullscreen() {
	return 0;
}

int Window_IsObscured() {
	return 0;
}

void Window_Show() {}

void Window_SetSize(int, int) {}

void Window_RequestClose() {
	Process_Exit(0);
}

void Window_ProcessEvents(float) {
	pollKeyboard();

	if (!rawMouseInput) {
		pollMouse();
	}
}

void Gamepads_PreInit() {}

void Gamepads_Init() {}

void Gamepads_Process(float) {}

static void ShowDialogCore(const char *title, const char *msg) {
	Platform_LogConst(title);
	Platform_LogConst(msg);
}

cc_result Window_OpenFileDialog(const OpenFileDialogArgs*) {
	return ERR_NOT_SUPPORTED;
}

cc_result Window_SaveFileDialog(const SaveFileDialogArgs*) {
	return ERR_NOT_SUPPORTED;
}

void Window_AllocFramebuffer(Bitmap *bmp, const int width, const int height) {
	lfb.clear();
	bufferedLfb = new Util::Graphic::BufferedLinearFrameBuffer(lfb, width, height);

	bmp->scan0 = reinterpret_cast<BitmapCol*>(bufferedLfb->getBuffer().get());
	bmp->width = width;
	bmp->height = height;
}

void Window_DrawFramebuffer(Rect2D, Bitmap*) {
	const auto sizeX = static_cast<uint16_t>(25 * DisplayInfo.ScaleX);
	const auto sizeY = static_cast<uint16_t>(25 * DisplayInfo.ScaleY);

	// Draw cursor on top of screen (if it is visible)
	if (cursorVisible) {
		bufferedLfb->drawLine(mouseX, mouseY, mouseX + sizeX, mouseY + sizeY / 2,
			Util::Graphic::Colors::RED);
		bufferedLfb->drawLine(mouseX, mouseY, mouseX + sizeX / 2, mouseY + sizeY,
			Util::Graphic::Colors::RED);
		bufferedLfb->drawLine(mouseX + sizeX, mouseY + sizeY / 2, mouseX + sizeX / 2, mouseY + sizeY,
			Util::Graphic::Colors::RED);
	}

	bufferedLfb->flush();
}

void Window_FreeFramebuffer(Bitmap *bmp) {
	if (reinterpret_cast<uint32_t>(bmp->scan0) != bufferedLfb->getBuffer().get()) {
		Util::Panic::fire(Util::Panic::INVALID_ARGUMENT,
			"Trying to free another framebuffer than the current one!");
	}

	delete bufferedLfb;
}

void OnscreenKeyboard_Open(OpenKeyboardArgs*) {}

void OnscreenKeyboard_SetText(const cc_string*) {}

void OnscreenKeyboard_Close() {}

void Window_EnableRawMouse() {
	rawMouseInput = true;
	DefaultEnableRawMouse();
}

void Window_UpdateRawMouse() {
	pollMouse();
	DefaultUpdateRawMouse();
}

void Window_DisableRawMouse() {
	rawMouseInput = false;
	DefaultDisableRawMouse();
}
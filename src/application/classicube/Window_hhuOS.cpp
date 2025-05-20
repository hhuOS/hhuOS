#include <stdint.h>

#include "application/classicube/ClassiCube/src/Core.h"
#include "application/classicube/ClassiCube/src/_WindowBase.h"
#include "application/classicube/ClassiCube/src/Bitmap.h"
#include "application/classicube/ClassiCube/src/Errors.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/math/Math.h"
#include "application/classicube/ClassiCube/src/Event.h"
#include "application/classicube/ClassiCube/src/Input.h"
#include "application/classicube/ClassiCube/src/Platform.h"
#include "application/classicube/ClassiCube/src/Window.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Panic.h"
#include "lib/util/base/String.h"
#include "lib/util/io/key/Key.h"

auto lfbFile = Util::Io::File("/device/lfb");
auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
Util::Graphic::BufferedLinearFrameBuffer *bufferedLfb = nullptr;

auto keyboardInputStream = Util::Io::FileInputStream(1);
auto keyDecoder = Util::Io::KeyDecoder(new Util::Io::DeLayout());

auto mouseInputStream = Util::Io::FileInputStream("/device/mouse");
uint8_t mouseValues[4]{};
uint32_t mouseValueIndex = 0;

bool cursorVisible = true;
bool rawMouseInput = false;
int32_t mouseX = 0;
int32_t mouseY = 0;

void pollKeyboard() {
	int16_t scancode = keyboardInputStream.read();
	while (scancode >= 0) {
		if (keyDecoder.parseScancode(scancode)) {
			auto key = keyDecoder.getCurrentKey();

			Input_SetNonRepeatable(CCKEY_RSHIFT, key.getShift());
			Input_SetNonRepeatable(CCKEY_LCTRL, key.getCtrlLeft());
			Input_SetNonRepeatable(CCKEY_LALT, key.getAltLeft());
			Input_SetNonRepeatable(CCKEY_SCROLLLOCK, key.getScrollLock());
			Input_SetNonRepeatable(CCKEY_NUMLOCK, key.getNumLock());
			Input_SetNonRepeatable(CCKEY_CAPSLOCK, key.getCapsLock());

			InputButtons ccCode;
			switch (key.getScancode()) {
				case Util::Io::Key::ESC:
					ccCode = CCKEY_ESCAPE;
					break;
				case Util::Io::Key::F1:
					ccCode = CCKEY_F1;
					break;
				case Util::Io::Key::F2:
					ccCode = CCKEY_F2;
					break;
				case Util::Io::Key::F3:
					ccCode = CCKEY_F3;
					break;
				case Util::Io::Key::F4:
					ccCode = CCKEY_F4;
					break;
				case Util::Io::Key::F5:
					ccCode = CCKEY_F5;
					break;
				case Util::Io::Key::F6:
					ccCode = CCKEY_F6;
					break;
				case Util::Io::Key::F7:
					ccCode = CCKEY_F7;
					break;
				case Util::Io::Key::F8:
					ccCode = CCKEY_F8;
					break;
				case Util::Io::Key::F9:
					ccCode = CCKEY_F9;
					break;
				case Util::Io::Key::F10:
					ccCode = CCKEY_F10;
					break;
				case Util::Io::Key::F11:
					ccCode = CCKEY_F11;
					break;
				case Util::Io::Key::F12:
					ccCode = CCKEY_F12;
					break;
				case Util::Io::Key::SPACE:
					ccCode = CCKEY_SPACE;
					break;
				case Util::Io::Key::ENTER:
					ccCode = CCKEY_ENTER;
					break;
				case Util::Io::Key::BACKSPACE:
					ccCode = CCKEY_BACKSPACE;
					break;
				case Util::Io::Key::UP:
					ccCode = CCKEY_UP;
					break;
				case Util::Io::Key::DOWN:
					ccCode = CCKEY_UP;
					break;
				case Util::Io::Key::LEFT:
					ccCode = CCKEY_UP;
					break;
				case Util::Io::Key::RIGHT:
					ccCode = CCKEY_UP;
					break;
				case Util::Io::Key::ZERO:
					ccCode = CCKEY_0;
					break;
				case Util::Io::Key::ONE:
					ccCode = CCKEY_1;
					break;
				case Util::Io::Key::TWO:
					ccCode = CCKEY_2;
					break;
				case Util::Io::Key::THREE:
					ccCode = CCKEY_3;
					break;
				case Util::Io::Key::FOUR:
					ccCode = CCKEY_4;
					break;
				case Util::Io::Key::FIVE:
					ccCode = CCKEY_5;
					break;
				case Util::Io::Key::SIX:
					ccCode = CCKEY_6;
					break;
				case Util::Io::Key::SEVEN:
					ccCode = CCKEY_7;
					break;
				case Util::Io::Key::EIGHT:
					ccCode = CCKEY_8;
					break;
				case Util::Io::Key::NINE:
					ccCode = CCKEY_9;
					break;
				case Util::Io::Key::A:
					ccCode = CCKEY_A;
					break;
				case Util::Io::Key::B:
					ccCode = CCKEY_B;
					break;
				case Util::Io::Key::C:
					ccCode = CCKEY_C;
					break;
				case Util::Io::Key::D:
					ccCode = CCKEY_D;
					break;
				case Util::Io::Key::E:
					ccCode = CCKEY_E;
					break;
				case Util::Io::Key::F:
					ccCode = CCKEY_F;
					break;
				case Util::Io::Key::G:
					ccCode = CCKEY_G;
					break;
				case Util::Io::Key::H:
					ccCode = CCKEY_H;
					break;
				case Util::Io::Key::I:
					ccCode = CCKEY_I;
					break;
				case Util::Io::Key::J:
					ccCode = CCKEY_J;
					break;
				case Util::Io::Key::K:
					ccCode = CCKEY_K;
					break;
				case Util::Io::Key::L:
					ccCode = CCKEY_L;
					break;
				case Util::Io::Key::M:
					ccCode = CCKEY_M;
					break;
				case Util::Io::Key::N:
					ccCode = CCKEY_N;
					break;
				case Util::Io::Key::O:
					ccCode = CCKEY_O;
					break;
				case Util::Io::Key::P:
					ccCode = CCKEY_P;
					break;
				case Util::Io::Key::Q:
					ccCode = CCKEY_Q;
					break;
				case Util::Io::Key::R:
					ccCode = CCKEY_R;
					break;
				case Util::Io::Key::S:
					ccCode = CCKEY_S;
					break;
				case Util::Io::Key::T:
					ccCode = CCKEY_T;
					break;
				case Util::Io::Key::U:
					ccCode = CCKEY_U;
					break;
				case Util::Io::Key::V:
					ccCode = CCKEY_V;
					break;
				case Util::Io::Key::W:
					ccCode = CCKEY_W;
					break;
				case Util::Io::Key::X:
					ccCode = CCKEY_X;
					break;
				case Util::Io::Key::Y:
					ccCode = CCKEY_Y;
					break;
				case Util::Io::Key::Z:
					ccCode = CCKEY_Z;
					break;
				default:
					scancode = keyboardInputStream.read();
					continue;
			}

			if (key.isPressed()) {
				Input_SetPressed(ccCode);
				Event_RaiseInt(&InputEvents.Press, key.getAscii());
			} else {
				Input_SetReleased(ccCode);
			}
		}

		scancode = keyboardInputStream.read();
	}
}

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
					if (mouseX < 0) mouseX = 0;
					if (mouseX >= DisplayInfo.Width) mouseX = DisplayInfo.Width - 1;
					if (mouseY < 0) mouseY = 0;
					if (mouseY >= DisplayInfo.Height) mouseY = DisplayInfo.Height - 1;

					Pointer_SetPosition(0, mouseX, mouseY);
					Event_RaiseVoid(&WindowEvents.RedrawNeeded);
				}
			}

			Util::Address(mouseValues).setRange(0, 4);
			mouseValueIndex = 0;

			Input_SetNonRepeatable(CCMOUSE_L, mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON);
			Input_SetNonRepeatable(CCMOUSE_R, mouseUpdate.buttons & Util::Io::Mouse::RIGHT_BUTTON);
			Input_SetNonRepeatable(CCMOUSE_M, mouseUpdate.buttons & Util::Io::Mouse::MIDDLE_BUTTON);
		}

		value = mouseInputStream.read();
	}
}

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
	if (!keyboardInputStream.setAccessMode(Util::Io::File::NON_BLOCKING)) {
		Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Failed to set keyboard input stream to non-blocking mode!");
	}
	if (!mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING)) {
		Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Failed to set mouse input stream to non-blocking mode!");
	}

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

void Window_Create2D([[maybe_unused]] int width, [[maybe_unused]] int height) {
	// 2D windows are always rendered at full display resolution
	Window_Main.Width    = lfb.getResolutionX();
	Window_Main.Height   = lfb.getResolutionY();
	Window_Main.Focused  = true;
	Window_Main.Exists   = true;
	Window_Main.UIScaleX = DEFAULT_UI_SCALE_X;
	Window_Main.UIScaleY = DEFAULT_UI_SCALE_Y;
}

void Window_Create3D([[maybe_unused]] const int width, [[maybe_unused]] const int height) {
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

void Window_SetTitle([[maybe_unused]] const cc_string *title) {}

void Clipboard_GetText([[maybe_unused]] cc_string *value) {}

void Clipboard_SetText([[maybe_unused]] const cc_string *value) {}

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

void Window_SetSize([[maybe_unused]] int width, [[maybe_unused]] int height) {}

void Window_RequestClose() {
	Process_Exit(0);
}

void Window_ProcessEvents([[maybe_unused]] float delta) {
	pollKeyboard();

	if (!rawMouseInput) {
		pollMouse();
	}
}

void Gamepads_Init() {}

void Gamepads_Process([[maybe_unused]] float delta) {}

static void ShowDialogCore(const char* title, const char* msg) {
	Platform_LogConst(title);
	Platform_LogConst(msg);
}

cc_result Window_OpenFileDialog([[maybe_unused]] const OpenFileDialogArgs *args) {
	return ERR_NOT_SUPPORTED;
}

cc_result Window_SaveFileDialog([[maybe_unused]] const SaveFileDialogArgs *args) {
	return ERR_NOT_SUPPORTED;
}

void Window_AllocFramebuffer(Bitmap *bmp, const int width, const int height) {
	bufferedLfb = new Util::Graphic::BufferedLinearFrameBuffer(lfb, width, height);

	bmp->scan0 = reinterpret_cast<BitmapCol*>(bufferedLfb->getBuffer().get());
	bmp->width = width;
	bmp->height = height;
}

void Window_DrawFramebuffer([[maybe_unused]] Rect2D r, [[maybe_unused]] Bitmap *bmp) {
	if (cursorVisible) {
		const auto sizeX = static_cast<uint16_t>(25 * DisplayInfo.ScaleX);
		const auto sizeY = static_cast<uint16_t>(25 * DisplayInfo.ScaleY);

		bufferedLfb->drawLine(mouseX, mouseY, mouseX + sizeX, mouseY + sizeY / 2, Util::Graphic::Colors::RED);
		bufferedLfb->drawLine(mouseX, mouseY, mouseX + sizeX / 2, mouseY + sizeY, Util::Graphic::Colors::RED);
		bufferedLfb->drawLine(mouseX + sizeX, mouseY + sizeY / 2, mouseX + sizeX / 2, mouseY + sizeY, Util::Graphic::Colors::RED);
	}

	bufferedLfb->flush();
}

void Window_FreeFramebuffer([[maybe_unused]] Bitmap *bmp) {
	if (reinterpret_cast<uint32_t>(bmp->scan0) != bufferedLfb->getBuffer().get()) {
		Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Trying to free another framebuffer than the current one!");
	}

	delete bufferedLfb;
}

void OnscreenKeyboard_Open([[maybe_unused]] OpenKeyboardArgs *args) {}

void OnscreenKeyboard_SetText([[maybe_unused]] const cc_string *text) {}

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
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>

#define USE_WEAPON_SET_1 1;
#define USE_WEAPON_SET_2 2;

bool RightButtonDown = false;
bool SystemPaused = false;
short CountSystemButton = 0;

int WeaponStateKey = USE_WEAPON_SET_1;
char PauseSystemKey = VK_MULTIPLY;
char WeaponSwitchKey = 0x58;//'X'
char WeaponSetOne = VK_OEM_MINUS;
char WeaponSetTwo = VK_OEM_PLUS;
char ActiveWeapon = WeaponSetOne;

HHOOK KeyboardHook;
HHOOK MouseHook;

INPUT Input;

void InitInput(void) {
	Input.type = INPUT_KEYBOARD;
	Input.ki.wScan = 0;
	Input.ki.time = 0;
	Input.ki.dwExtraInfo = 0;
}

void SendWeaponInput(DWORD EventType) {
	Input.ki.wVk = ActiveWeapon;
	Input.ki.dwFlags = EventType;
	SendInput(1, &Input, sizeof(INPUT));
}

std::wstring get_utf16(const std::string &str, int codepage)
{
	if (str.empty()) return std::wstring();
	int sz = MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), 0, 0);
	std::wstring res(sz, 0);
	MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), &res[0], sz);
	return res;
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {

	if (wParam == WM_RBUTTONDOWN && !SystemPaused)
	{
		RightButtonDown = true;
		SendWeaponInput(0);
	}
	else if (wParam == WM_RBUTTONUP && RightButtonDown) {
		RightButtonDown = false;
		SendWeaponInput(KEYEVENTF_KEYUP);
	}

	return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {

	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

	if(!RightButtonDown) {
		if (wParam == WM_KEYDOWN)
		{
			if (p->vkCode == PauseSystemKey) {
				SystemPaused = !SystemPaused;
			}
			else if (!SystemPaused && p->vkCode == WeaponSwitchKey) {
				if (ActiveWeapon == WeaponSetTwo) {
					ActiveWeapon = WeaponSetOne;
				}
				else {
					ActiveWeapon = WeaponSetTwo;
				}
			}
		}
	}

	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}

void StayAlive() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd) {
	FreeConsole();
	InitInput();
	MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, 0);
	KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hInstance, 0);
	StayAlive();
	UnhookWindowsHookEx(KeyboardHook);
	UnhookWindowsHookEx(MouseHook);
	return 0;
}

#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"

class Input
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize(WinApp* winApp);

	void Update();

	bool PushKey(BYTE keyNumber);

	//BYTE key[256] = {};

	bool Triggerkey(BYTE keyNumber);

private:
	WinApp* winApp = nullptr;

	ComPtr<IDirectInputDevice8> keyboard;

	ComPtr<IDirectInput8> directInput;
	BYTE key[256] = {};

	BYTE keyPre[256] = {};
};
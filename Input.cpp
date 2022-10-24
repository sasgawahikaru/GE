#include "Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}
void Input::Update()
{
	// キーボード情報の取得開始
	keyboard->Acquire();
	//BYTE key[256] = {};
	
	memcpy(keyPre, key, sizeof(key));

	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::Triggerkey(BYTE keyNumber)
{
	if (key[keyNumber] && !keyPre[keyNumber])
	{
		return true;
	}

	return false;
}

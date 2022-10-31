#include<windows.h>
#pragma once
class WinApp
{
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static const int window_width = 1280;

	static const int window_height = 720;

	void Initialize();
	void Update();
	void Finalize();

	HWND GetHwnd()const { return hwnd; }
	HINSTANCE GetHInstance()const { return w.hInstance; }

	bool ProcessMessage();

private:
	HWND hwnd = nullptr;
	WNDCLASSEX w{};
};


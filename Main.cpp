#include "stdafx.h"
#include "./Systems/Window.h"
#include <crtdbg.h>

//new 오버라이딩
#ifdef _DEBUG
#define new new(_CLIENT_BLOCK,__FILE__,__LINE__)
#endif

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR  param, int command)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 7506;
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	D3D::SetDesc(desc);

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SAFE_DELETE(window);

	return wParam;
}
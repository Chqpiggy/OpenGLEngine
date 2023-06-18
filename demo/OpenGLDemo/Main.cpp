#include <windows.h>
#include "RenderInterface.h"
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:    // 窗口销毁消息
	{
		PostQuitMessage(0);   //  发送退出消息
	}
	break;
	}
	// 其他的消息调用缺省的消息处理程序
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int main()
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Example", NULL };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"OpenGLDemo", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);
	bool bDone = false;
	RI_Init(hwnd);
	while (true)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (bDone)
		{
			break;
		}
	}
	return 0;
}
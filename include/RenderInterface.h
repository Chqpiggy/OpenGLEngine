#pragma once
#include <Windows.h>
#if defined(DLL_EXPORTS)
#define RI_API __declspec(dllexport)
#else
#define RI_API __declspec(dllimport)
#endif
//使用C风格接口函数

RI_API int __stdcall RI_Init(HWND pWnd);//初始化操作
#pragma once
#include <Windows.h>
#if defined(DLL_EXPORTS)
#define RI_API __declspec(dllexport)
#else
#define RI_API __declspec(dllimport)
#endif
//ʹ��C���ӿں���

RI_API int __stdcall RI_Init(HWND pWnd);//��ʼ������
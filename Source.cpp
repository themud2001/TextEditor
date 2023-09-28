#include <Windows.h>
#include <tchar.h>
#include "resource.h";

LRESULT CALLBACK WindProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

const TCHAR CLASS_NAME[] = _T("Text Editor");
const TCHAR WINDOW_NAME[] = _T("Text Editor");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);

	if (FAILED(RegisterClassEx(&wc)))
	{
		MessageBox(NULL, _T("Error in window registration!"), NULL, MB_OK);
		return 0;
	}

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (FAILED(hWnd))
	{
		MessageBox(NULL, _T("Error in window creation!"), NULL, MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg = {};

	while (GetMessage(&msg, hWnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case ID_HELP_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTDIALOG), hWnd, AboutDlgProc);
			break;
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps = {};

			HDC hdc = BeginPaint(hWnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hWnd, IDCANCEL);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}
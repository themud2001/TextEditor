#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "resource.h";

LRESULT CALLBACK WindProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
void OpenFile(HWND);
BOOL LoadText(HWND, LPTSTR);

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
	case WM_CREATE:
		{
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

			HWND hEditBox = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				_T("EDIT"),
				_T(""),
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				0,
				0,
				100,
				100,
				hWnd,
				(HMENU)IDC_MAIN_EDITBOX,
				GetModuleHandle(NULL),
				NULL
			);

			if (FAILED(hEditBox))
			{
				MessageBox(NULL, _T("Error in edit box creation!"), NULL, 0);
			}

			SendMessage(hEditBox, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

			TBBUTTON buttons[3];
			TBADDBITMAP bitmap;

			HWND hToolBar = CreateWindowEx(
				0,
				TOOLBARCLASSNAME,
				NULL,
				WS_CHILD | WS_VISIBLE,
				0,
				0,
				0,
				0,
				hWnd,
				(HMENU)IDC_MAIN_TOOLBAR,
				GetModuleHandle(NULL),
				NULL
			);

			if (FAILED(hToolBar))
			{
				MessageBox(NULL, _T("Error in toolbar creation!"), NULL, 0);
			}

			SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

			bitmap.hInst = HINST_COMMCTRL;
			bitmap.nID = IDB_STD_SMALL_COLOR;

			SendMessage(hToolBar, TB_ADDBITMAP, 0, (WPARAM)&bitmap);
			ZeroMemory(buttons, sizeof(buttons));

			buttons[0].iBitmap = STD_FILENEW;
			buttons[0].fsState = TBSTATE_ENABLED;
			buttons[0].fsStyle = TBSTYLE_BUTTON;
			buttons[0].idCommand = ID_FILE_NEW;

			buttons[1].iBitmap = STD_FILEOPEN;
			buttons[1].fsState = TBSTATE_ENABLED;
			buttons[1].fsStyle = TBSTYLE_BUTTON;
			buttons[1].idCommand = ID_FILE_OPEN;

			buttons[2].iBitmap = STD_FILESAVE;
			buttons[2].fsState = TBSTATE_ENABLED;
			buttons[2].fsStyle = TBSTYLE_BUTTON;
			buttons[2].idCommand = ID_FILE_SAVEAS;

			SendMessage(hToolBar, TB_ADDBUTTONS, sizeof(buttons) / sizeof(TBBUTTON), (LPARAM)&buttons);

			int statusBarWidths[] = { 150, -1 };

			HWND hStatusBar = CreateWindowEx(
				0,
				STATUSCLASSNAME,
				NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
				0,
				0,
				0,
				0,
				hWnd,
				(HMENU)IDC_MAIN_STATUSBAR,
				GetModuleHandle(NULL),
				NULL
			);

			if (FAILED(hStatusBar))
			{
				MessageBox(NULL, _T("Error in status bar creation!"), NULL, 0);
			}

			SendMessage(hStatusBar, SB_SETPARTS, sizeof(statusBarWidths) / sizeof(int), (LPARAM)statusBarWidths);
		}
		break;
		
	case WM_SIZE:
		{
			HWND hToolBar = GetDlgItem(hWnd, IDC_MAIN_TOOLBAR);
			RECT toolBarRect;
			int toolBarHeight;

			HWND hStatusBar = GetDlgItem(hWnd, IDC_MAIN_STATUSBAR);
			RECT statusBarRect;
			int statusBarHeight;

			HWND hEditBox = GetDlgItem(hWnd, IDC_MAIN_EDITBOX);
			RECT clientRect;
			int editBoxHeight;

			SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);
			SendMessage(hStatusBar, WM_SIZE, 0, 0);

			GetWindowRect(hToolBar, &toolBarRect);
			toolBarHeight = toolBarRect.bottom - toolBarRect.top;

			GetWindowRect(hStatusBar, &statusBarRect);
			statusBarHeight = statusBarRect.bottom - statusBarRect.top;

			GetClientRect(hWnd, &clientRect);
			editBoxHeight = clientRect.bottom - toolBarHeight - statusBarHeight;

			SetWindowPos(hEditBox, NULL, 0, toolBarHeight, clientRect.right, editBoxHeight, SWP_NOZORDER);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
			SetDlgItemText(hWnd, IDC_MAIN_EDITBOX, _T(""));
			SendDlgItemMessage(hWnd, IDC_MAIN_STATUSBAR, SB_SETTEXT, 0, (LPARAM)_T("New file"));
			break;

		case ID_FILE_OPEN:
			OpenFile(hWnd);
			break;

		case ID_FILE_SAVEAS:

			break;

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
		case ID_DIALOG_OK:
			EndDialog(hWnd, ID_DIALOG_OK);
			break;

		case ID_DIALOG_CANCEL:
			EndDialog(hWnd, ID_DIALOG_CANCEL);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void OpenFile(HWND hWnd)
{
	OPENFILENAME ofn = {};
	TCHAR fileName[MAX_PATH] = _T("");

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
	ofn.lpstrDefExt = _T("txt");

	if (SUCCEEDED(GetOpenFileName(&ofn)))
	{
		HWND hEditBox = GetDlgItem(hWnd, IDC_MAIN_EDITBOX);

		if (LoadText(hEditBox, fileName))
		{
			SendDlgItemMessage(hWnd, IDC_MAIN_STATUSBAR, SB_SETTEXT, 0, (LPARAM)_T("Opened"));
			SendDlgItemMessage(hWnd, IDC_MAIN_STATUSBAR, SB_SETTEXT, 1, (LPARAM)fileName);
		}
	}
}

BOOL LoadText(HWND hEditBox, LPTSTR fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL success = FALSE;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD fileSize = GetFileSize(hFile, NULL);

		if (fileSize != INVALID_FILE_SIZE)
		{
			LPTSTR fileText = (LPTSTR)GlobalAlloc(GPTR, fileSize + 1);

			if (fileText != NULL)
			{
				DWORD read;

				if (ReadFile(hFile, fileText, fileSize, &read, NULL))
				{
					fileText[fileSize] = 0;

					if (SUCCEEDED(SetWindowText(hEditBox, fileText)))
					{
						success = TRUE;
					}
				}
			}

			GlobalFree(fileText);
		}

		CloseHandle(hFile);
	}

	return success;
}
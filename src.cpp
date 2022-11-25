#include <windows.h>
#include "filter.h"

inline HWND GetTopOwner(HWND hwnd) {
	HWND ownerwnd = hwnd;
	while (GetWindow(ownerwnd, GW_OWNER) != NULL) {
		ownerwnd = GetWindow(ownerwnd, GW_OWNER);
	}
	return ownerwnd;
}


FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_DISP_FILTER,
	0,0,
	const_cast<char*>("ボタンショートカット追加"),
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	func_init,
	NULL,
	NULL,
	func_WndProc,
};



EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &filter;
}

#define COMMAND_FILE_BUTTON1 (WM_APP + 1)
#define COMMAND_FILE_BUTTON2 (WM_APP + 2)
#define COMMAND_FILE_BUTTON3 (WM_APP + 3)
#define COMMAND_FILE_BUTTON4 (WM_APP + 4)
BOOL func_init(FILTER* fp) {
	fp->exfunc->add_menu_item(fp, const_cast<char*>("参照ファイル1個目"), fp->hwnd, COMMAND_FILE_BUTTON1, NULL, NULL);
	fp->exfunc->add_menu_item(fp, const_cast<char*>("参照ファイル2個目"), fp->hwnd, COMMAND_FILE_BUTTON2, NULL, NULL);
	fp->exfunc->add_menu_item(fp, const_cast<char*>("参照ファイル3個目"), fp->hwnd, COMMAND_FILE_BUTTON3, NULL, NULL);
	fp->exfunc->add_menu_item(fp, const_cast<char*>("参照ファイル4個目"), fp->hwnd, COMMAND_FILE_BUTTON4, NULL, NULL);

	return TRUE;
}

void click_file_button(HWND fphwnd,char* str,int n) {
	HWND aviutl_hwnd = GetTopOwner(fphwnd);
	HWND eedialog_hwnd = FindWindowA("ExtendedFilterClass", NULL);
	if (!IsWindowVisible(eedialog_hwnd)) {
		return;
	}
	if (aviutl_hwnd != GetTopOwner(eedialog_hwnd)) { // AviUtl多重起動の場合、最初に拾ったのが別だったら面倒なので停止する
		return;
	}
	HWND pre_button = NULL;
	for (int i = -1; i < n; i++) {
		pre_button = FindWindowExA(eedialog_hwnd, pre_button, "Button", str);
		
		if (pre_button == NULL) {
			return;
		}
		
		if (!IsWindowEnabled(pre_button) || !IsWindowVisible(pre_button)) {
			i--;
		}
	}
	PostMessageA(pre_button, BM_CLICK, 0, 0);
}

BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	switch (message) {
	case WM_FILTER_CHANGE_WINDOW:
		if (fp->exfunc->is_filter_window_disp(fp)) {
			SetWindowPos(fp->hwnd, 0, 0, 0, 0, 0, (SWP_NOSIZE | SWP_HIDEWINDOW));
		}
		break;

	case WM_FILTER_COMMAND:
		if (COMMAND_FILE_BUTTON1 <= wparam && wparam <= COMMAND_FILE_BUTTON4) {
			click_file_button(fp->hwnd, const_cast<char*>("参照ファイル"), (int)wparam - COMMAND_FILE_BUTTON1);
		}
		break;
	}

	return FALSE;
}



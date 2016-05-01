#include "stdafx.h"
#include "WndUtil.h"

//! 右上のｘマークの閉じるボタンの有効化または無効化
void CWndUtil::SetCloseButtonEnable(
	HWND hwnd, //!< [in] 対象ウィンドウのハンドル
	BOOL bEnable //!< [in] TRUE:有効化、FALSE:無効化
) {
	if (bEnable) {
		::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	} else {
		::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
}

//! 最小化ボタンの有効化または無効化
void CWndUtil::SetMinimizeButtonEnable(
	HWND hwnd, //!< [in] 対象ウィンドウのハンドル
	BOOL bEnable //!< [in] TRUE:有効化、FALSE:無効化
) {
	if (bEnable) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MINIMIZEBOX);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
	}
}

//! 最大化ボタンの有効化または無効化
void CWndUtil::SetMaximizeButtonEnable(
	HWND hwnd, //!< [in] 対象ウィンドウのハンドル
	BOOL bEnable //!< [in] TRUE:有効化、FALSE:無効化
) {
	if (bEnable) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MAXIMIZEBOX);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
	}
}

//! タイトルバー左端のアイコンの表示設定
void CWndUtil::SetShowIcon(
	HWND hwnd, //!< [in] 対象ウィンドウのハンドル
	BOOL bVisible //!< [in] TRUE:表示、FALSE:非表示
) {
	if (bVisible) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_SYSMENU);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | ~WS_SYSMENU);
	}
}


//! クリップボードへテキストを貼り付ける
BOOL CWndUtil::SetClipboardText(HWND hWnd, LPCTSTR pszText) {
	HGLOBAL hGlobal;
	int nLen = (int)_tcslen(pszText);

	hGlobal = ::GlobalAlloc(GHND, nLen + 1);
	if (hGlobal == NULL)
		return false;

	void* pMem = ::GlobalLock(hGlobal);
	memcpy(pMem, pszText, nLen + 1);
	::GlobalUnlock(hGlobal);

	if (::OpenClipboard(hWnd) == 0) {
		GlobalFree(hGlobal);
		return false;
	}
	::EmptyClipboard();
	::SetClipboardData(CF_TEXT, hGlobal);
	::CloseClipboard();

	return true;
}

//! クリップボードからテキストを取得する
CString CWndUtil::GetClipboardText(HWND hWnd) {
	if (!::IsClipboardFormatAvailable(CF_TEXT)) {
		return _T("");
	}

	OpenClipboard(hWnd);
	HGLOBAL hGlobal = (HGLOBAL)::GetClipboardData(CF_TEXT);
	if (hGlobal == NULL) {
		::CloseClipboard();
		return _T("");
	}

	CString text = (LPCSTR)::GlobalLock(hGlobal);
	text.Replace("\r\n", "\n");

	::GlobalUnlock(hGlobal);
	::CloseClipboard();

	return text;
}

#include "stdafx.h"
#include "WndUtil.h"

//! �E��̂��}�[�N�̕���{�^���̗L�����܂��͖�����
void CWndUtil::SetCloseButtonEnable(
	HWND hwnd, //!< [in] �ΏۃE�B���h�E�̃n���h��
	BOOL bEnable //!< [in] TRUE:�L�����AFALSE:������
) {
	if (bEnable) {
		::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	} else {
		::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
}

//! �ŏ����{�^���̗L�����܂��͖�����
void CWndUtil::SetMinimizeButtonEnable(
	HWND hwnd, //!< [in] �ΏۃE�B���h�E�̃n���h��
	BOOL bEnable //!< [in] TRUE:�L�����AFALSE:������
) {
	if (bEnable) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MINIMIZEBOX);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
	}
}

//! �ő剻�{�^���̗L�����܂��͖�����
void CWndUtil::SetMaximizeButtonEnable(
	HWND hwnd, //!< [in] �ΏۃE�B���h�E�̃n���h��
	BOOL bEnable //!< [in] TRUE:�L�����AFALSE:������
) {
	if (bEnable) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MAXIMIZEBOX);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
	}
}

//! �^�C�g���o�[���[�̃A�C�R���̕\���ݒ�
void CWndUtil::SetShowIcon(
	HWND hwnd, //!< [in] �ΏۃE�B���h�E�̃n���h��
	BOOL bVisible //!< [in] TRUE:�\���AFALSE:��\��
) {
	if (bVisible) {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_SYSMENU);
	} else {
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | ~WS_SYSMENU);
	}
}


//! �N���b�v�{�[�h�փe�L�X�g��\��t����
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

//! �N���b�v�{�[�h����e�L�X�g���擾����
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

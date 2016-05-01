#include "stdafx.h"
#include "WinApiException.h"
#include "Error.h"

//! コンストラクタ、メッセージを指定して初期化する
WinApiException::WinApiException(DWORD nErrorCode, LPCTSTR pszMessage) {
	m_nErrorCode = nErrorCode;
	m_sMessage = Error::MessageFromWinApiErrorCode(nErrorCode) + _T("\n") + pszMessage;
}

WinApiException::~WinApiException() {
}

BOOL WinApiException::GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext) const {
	_tcscpy_s(lpszError, nMaxError, m_sMessage);
	return TRUE;
}

BOOL WinApiException::GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext) {
	_tcscpy_s(lpszError, nMaxError, m_sMessage);
	return TRUE;
}

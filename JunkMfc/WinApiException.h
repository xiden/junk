#pragma once
#ifndef __WIN32APIEXCEPTION_H__
#define __WIN32APIEXCEPTION_H__
#include <afx.h>

//! Win32APIの例外クラス、エラーコードと表示用のメッセージを持つ
class WinApiException :
	public CException {
public:
	WinApiException(DWORD nErrorCode, LPCTSTR pszMessage); //!< コンストラクタ、メッセージを指定して初期化する
	virtual ~WinApiException();

	virtual BOOL GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext = NULL) const;
	virtual BOOL GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext = NULL);

	//! エラーコード取得
	DWORD GetErrorCode() {
		return m_nErrorCode;
	}

protected:
	DWORD m_nErrorCode; //!< エラーコード
	CString m_sMessage; //!< メッセージ
};

#endif

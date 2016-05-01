#pragma once
#ifndef __WIN32APIEXCEPTION_H__
#define __WIN32APIEXCEPTION_H__
#include <afx.h>

//! Win32API�̗�O�N���X�A�G���[�R�[�h�ƕ\���p�̃��b�Z�[�W������
class WinApiException :
	public CException {
public:
	WinApiException(DWORD nErrorCode, LPCTSTR pszMessage); //!< �R���X�g���N�^�A���b�Z�[�W���w�肵�ď���������
	virtual ~WinApiException();

	virtual BOOL GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext = NULL) const;
	virtual BOOL GetErrorMessage(LPTSTR lpszError, _In_ UINT nMaxError, PUINT pnHelpContext = NULL);

	//! �G���[�R�[�h�擾
	DWORD GetErrorCode() {
		return m_nErrorCode;
	}

protected:
	DWORD m_nErrorCode; //!< �G���[�R�[�h
	CString m_sMessage; //!< ���b�Z�[�W
};

#endif

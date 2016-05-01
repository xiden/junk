#include "StdAfx.h"
#include "Error.h"

//! �G���[�R�[�h�𕶎��񉻂���
CString Error::MessageFromWinApiErrorCode(
	DWORD errorCode //!< [in] Win32�G���[�R�[�h
) {
	char* buf = NULL;
	CString s;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0,
		NULL);
	if (buf != NULL) {
		s = buf;
		LocalFree(buf);
	}
	return s;
}

//! �Ō�ɔ��������G���[�̃G���[�R�[�h�𕶎��񉻂���
CString Error::MessageFromWinApiLastError() {
	return MessageFromWinApiErrorCode(::GetLastError());
}

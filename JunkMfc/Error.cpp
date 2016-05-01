#include "StdAfx.h"
#include "Error.h"

//! エラーコードを文字列化する
CString Error::MessageFromWinApiErrorCode(
	DWORD errorCode //!< [in] Win32エラーコード
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

//! 最後に発生したエラーのエラーコードを文字列化する
CString Error::MessageFromWinApiLastError() {
	return MessageFromWinApiErrorCode(::GetLastError());
}

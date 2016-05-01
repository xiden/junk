#pragma once
#ifndef __ERROR_H_
#define __ERROR_H_

#include <afxstr.h>

//! エラー処理ヘルパクラス
class Error {
public:
	//! エラーコードを文字列化する
	static CString MessageFromWinApiErrorCode(DWORD errorCode);
	//! 最後に発生したエラーのエラーコードを文字列化する
	static CString MessageFromWinApiLastError();
};

#endif

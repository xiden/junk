#include "stdafx.h"
#include "DirUtil.h"
#include <afx.h>
#include "Error.h"
#include "WinApiException.h"


//! 指定されたディレクトリが存在するか調べる
bool DirUtil::Exists(
	LPCTSTR pszDir //!< [in] ディレクトリパス名
) {
	DWORD a;
	a = ::GetFileAttributes(pszDir);
	if (a == DWORD(-1))
		return FALSE;
	return a & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

//! 指定されたディレクトリを作成する、失敗したら例外が発生する
void DirUtil::Create(
	LPCTSTR pszDir //!< [in] ディレクトリパス名
) {
	if (!::CreateDirectory(pszDir, NULL)) {
		throw new WinApiException(::GetLastError(), pszDir);
	}
}

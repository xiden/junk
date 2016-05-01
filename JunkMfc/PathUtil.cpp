#include "stdafx.h"
#include "PathUtil.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


//! EXEファイルがあるディレクトリパス名の取得
CString PathUtil::GetExeDir() {
	TCHAR buf[MAX_PATH];
	::GetModuleFileName(NULL, buf, MAX_PATH);
	::PathRemoveFileSpec(buf);
	return buf;
}

//! ファイル名を連結する
CString PathUtil::Combine(
	LPCTSTR pszDir, //!< [in] ディレクトリパス名
	LPCTSTR pszFile //!< [in] ファイル名
) {
	TCHAR buf[MAX_PATH];
	::PathCombine(buf, pszDir, pszFile);
	return buf;
}

//! 親ディレクトリ名を連結する
CString PathUtil::GetParent(LPCTSTR pszFilePath) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFilePath);
	::PathRemoveFileSpec(buf);
	return buf;
}

//! ファイルパス名からファイル名を取得する
CString PathUtil::GetFileName(LPCTSTR pszFilePath) {
	return ::PathFindFileName(pszFilePath);
}

//! ファイルパス名から拡張子取り除いたファイル名部分を取得する
CString PathUtil::GetFileTitle(LPCTSTR pszFilePath) {
	return RemoveExt(::PathFindFileName(pszFilePath));
}

//! 拡張子を取り除く
CString PathUtil::RemoveExt(LPCTSTR pszFile) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFile);
	::PathRemoveExtension(buf);
	return buf;
}

//! 拡張子を変更する
CString PathUtil::RenameExt(LPCTSTR pszFile, LPCTSTR pszNewExt) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFile);
	::PathRenameExtension(buf, pszNewExt);
	return buf;
}

#pragma once
#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

#include <afxstr.h>
#include <Windows.h>

//! パス名関係のヘルパ処理をまとめたクラス
class PathUtil {
public:
	static CString GetExeDir(); //!< EXEファイルがあるディレクトリパス名の取得
	static CString Combine(LPCTSTR pszDir, LPCTSTR pszFile); //!< ファイル名を連結する
	static CString GetParent(LPCTSTR pszFilePath); //!< 親ディレクトリ名を連結する
	static CString GetFileName(LPCTSTR pszFilePath); //!< ファイルパス名からファイル名を取得する
	static CString GetFileTitle(LPCTSTR pszFilePath); //!< ファイルパス名から拡張子取り除いたファイル名部分を取得する
	static CString RemoveExt(LPCTSTR pszFile); //!< 拡張子を取り除く
	static CString RenameExt(LPCTSTR pszFile, LPCTSTR pszNewExt); //!< 拡張子を変更する
};

#endif

#pragma once
#ifndef __DIRUTIL_H__
#define __DIRUTIL_H__

#include <afxstr.h>
#include <Windows.h>

//! ディレクトリ関係のヘルパ処理をまとめたクラス
class DirUtil {
public:
	static bool Exists(LPCTSTR pszDir); //!< 指定されたディレクトリが存在するか調べる
	static void Create(LPCTSTR pszDir); //!< 指定されたディレクトリを作成する、失敗したら例外が発生する
};

#endif

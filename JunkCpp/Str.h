#pragma once
#ifndef __JUNK_STR_H__
#define __JUNK_STR_H__

#include "JunkConfig.h"
#include <stdarg.h>
#include <string>

_JUNK_BEGIN

//! 文字列関係ヘルパ
struct Str {
	static void FV(std::string& s, const char* pszFmt, va_list args); //!< 指定の文字列へ書式化した文字を代入する
	static void AFV(std::string& s, const char* pszFmt, va_list args); //!< 指定の文字列へ書式化した文字を追加する
	static void F(std::string& s, const char* pszFmt, ...); //!< 指定の文字列へ書式化した文字を代入する
	static void AF(std::string& s, const char* pszFmt, ...); //!< 指定の文字列へ書式化した文字を追加する
	static std::string FV(const char* pszFmt, va_list args); //!< 書式化した文字を取得する
	static std::string F(const char* pszFmt, ...); //!< 書式化した文字を取得する
	static void Replace(std::string& s, const char* pszBefore, const char* pszAfter); //!< 文字列を置き換える
};

_JUNK_END

#endif

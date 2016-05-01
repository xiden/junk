#pragma once
#ifndef __JUNK_FILEPATH_H__
#define __JUNK_FILEPATH_H__

#include "JunkConfig.h"
#include <string>

_JUNK_BEGIN

//! ファイルパス操作
class FilePath {
public:
	static ibool Combine(std::string& result, const char* pszPath, const char* pszFile); //!< 指定されたパス名とファイル名を連結する
	static ibool StripPath(std::string& result, const char* pszPath); //!< パス名を取り除きファイル名部分のみ取得する
};

_JUNK_END

#endif

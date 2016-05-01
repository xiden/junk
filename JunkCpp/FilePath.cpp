#include "FilePath.h"

#include <string.h>

#if defined __GNUC__
#else

#include <Windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#endif



_JUNK_BEGIN

//! 指定されたパス名とファイル名を連結する
intptr_t FilePath::Combine(
	std::string& result, //!< [out] 連結されたパスが返る
	const char* pszPath, //!< [in] パス名
	const char* pszFile //!< [in] ファイル名
) {
#if defined __GNUC__
	// もしファイル名がフルパスならファイル名をそのまま返す
	if(pszFile[0] == '/') {
		result = pszFile;
		return true;
	}

	// パス名で初期化
	result = pszPath;

	// パス名の最後が '/' じゃなかったら追加する
	if(!result.empty() && result[result.size() - 1] != '/')
		result += "/";

	// ファイル名を連結
	result += pszFile;

	return true;
#else
	char buf[MAX_PATH];
	::PathCombineA(buf, pszPath, pszFile);
	result = buf;
	return true;
#endif
}

//! パス名を取り除きファイル名部分のみ取得する
ibool FilePath::StripPath(
	std::string& result, //!< [out] ファイル名部分が返る
	const char* pszPath //!< [in] パス名
) {
#if defined __GNUC__
	// もしファイル名がフルパスならファイル名をそのまま返す
	const char* p = strrchr(pszPath, '/');
	result.resize(0);
	if(p != NULL) {
		p++;
		result.insert(result.begin(), p, p + strlen(p));
	}
	return true;
#else
	result = pszPath;
	if (result.empty())
		return true;
	::PathStripPathA(&result[0]);
	result.resize(strlen(result.c_str()));
	return true;
#endif
}

_JUNK_END

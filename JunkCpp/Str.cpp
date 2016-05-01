#include "Str.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

_JUNK_BEGIN

//! 指定の文字列へ書式化した文字を代入する
void Str::FV(
	std::string& s, //!< [out] 出力先文字列
	const char* pszFmt, //!< [in] 書式
	va_list args //!< [in] 引数列
) {
#if defined __GNUC__
	s.resize(vsnprintf(&s[0], 0, pszFmt, args));
	vsnprintf(&s[0], s.size() + 1, pszFmt, args);
#else
	intptr_t n;
	s.resize(32);
	do {
	START:
		n = vsnprintf_s(&s[0], s.size(), s.size(), pszFmt, args); //!< GCC と VC++ で挙動が違うので注意
		if (n == -1 || (intptr_t)s.size() <= n) {
			s.resize(s.size() * 2);
			goto START;
		}
	} while((intptr_t)s.size() < n);
	s.resize(n);
#endif
}

//! 指定の文字列へ書式化した文字を追加する
void Str::AFV(
	std::string& s, //!< [in,out] 出力先文字列、ここに文字列が追加される
	const char* pszFmt, //!< [in] 書式
	va_list args //!< [in] 引数列
) {
#if defined __GNUC__
	intptr_t start = (intptr_t)s.length();
	s.resize(start + vsnprintf(&s[start], 0, pszFmt, args));
	vsnprintf(&s[start], s.size() - start + 1, pszFmt, args);
#else
	intptr_t n;
	intptr_t start = (intptr_t)s.length();
	s.resize(start + 32);
	do {
	START:
		n = vsnprintf_s(&s[start], s.size() - start, s.size() - start, pszFmt, args); //!< GCC と VC++ で挙動が違うので注意
		if (n == -1 || (intptr_t)s.size() - start <= n) {
			s.resize(s.size() * 2);
			goto START;
		}
	} while ((intptr_t)s.size() - start < n);
	s.resize(start + n);
#endif
}

//! 指定の文字列へ書式化した文字を代入する
void Str::F(
	std::string& s, //!< [out] 出力先文字列
	const char* pszFmt, //!< [in] 書式
	...
) {
	va_list args;
	va_start(args, pszFmt);
	FV(s, pszFmt, args);
	va_end(args);
}

//! 指定の文字列へ書式化した文字を追加する
void Str::AF(
	std::string& s, //!< [in,out] 出力先文字列、ここに文字列が追加される
	const char* pszFmt, //!< [in] 書式
	...
) {
	va_list args;
	va_start(args, pszFmt);
	AFV(s, pszFmt, args);
	va_end(args);
}

//! 書式化した文字を取得する
std::string Str::FV(
	const char* pszFmt, //!< [in] 書式
	va_list args
) {
	std::string s;
	FV(s, pszFmt, args);
	return s;
}

//! 書式化した文字を取得する
std::string Str::F(
	const char* pszFmt, //!< [in] 書式
	...
) {
	va_list args;
	va_start(args, pszFmt);
	std::string s;
	FV(s, pszFmt, args);
	va_end(args);
	return s;
}

//! 文字列を置き換える
void Str::Replace(
	std::string& s, //!< [in, out] 置き換え対象文字列
	const char* pszBefore, //!< [in] 置き換え前文字列
	const char* pszAfter //!< [in] 置き換え後文字列
) {
	if (s.empty())
		return;
	size_t blen = strlen(pszBefore);
	if (blen == 0)
		return;
	size_t alen = strlen(pszAfter);
	std::string::size_type pos = s.size() - 1;
	while ((pos = s.rfind(pszBefore, pos, blen)) != std::string::npos) {
		s.replace(pos, blen, pszAfter, alen);
		pos--;
	}
}

_JUNK_END

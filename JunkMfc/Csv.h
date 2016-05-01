#pragma once
#ifndef __CSV_H__
#define __CSV_H__

#include <afxstr.h>
#include <vector>
#include "Cvt.h"

//! CSV処理ヘルパクラス
//! @remarks マルチバイト、ワイド文字両対応
class Csv {
public:
	//! 指定区切り文字でフィールドを切り分けて取得する
	static void Split(
		const CStringA& text, //!< [in] 切り出したい文字列
		char separator, //!< [in] 区切り文字
		char bundler, //!< [in] 括り文字
		std::vector<CStringA>& fields //!< [out] 分割後のフィールド配列が返る
	);

	//! 指定区切り文字でフィールドを切り分けて取得する
	static void Split(
		const CStringW& text, //!< [in] 切り出したい文字列
		wchar_t separator, //!< [in] 区切り文字
		wchar_t bundler, //!< [in] 括り文字
		std::vector<CStringW>& fields //!< [out] 分割後のフィールド配列が返る
	);


	//! 指定区切り文字でフィールドを連結して取得する
	static void Combine(
		const std::vector<CStringA>& fields, //!< [in] 結果の分割された文字列が追加される配列
		wchar_t separator, //!< [in] 区切り文字
		wchar_t bundler, //!< [in] 括り文字
		CStringA& text //!< [out] 連結後の文字列
	);

	//! 指定区切り文字でフィールドを連結して取得する
	static void Combine(
		const std::vector<CStringW>& fields, //!< [in] 結果の分割された文字列が追加される配列
		wchar_t separator, //!< [in] 区切り文字
		wchar_t bundler, //!< [in] 括り文字
		CStringW& text //!< [out] 連結後の文字列
	);

	//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
	static CString GetField(
		const std::vector<CStringW>& fields, //!< [in] 配列
		int iIndex, //!< 配列内のインデックス
		LPCTSTR pszDef = NULL //!< デフォルト値
	);

	//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
	static CString GetField(
		const std::vector<CStringA>& fields, //!< [in] 配列
		int iIndex, //!< 配列内のインデックス
		LPCTSTR pszDef = NULL //!< デフォルト値
	);

	//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
	template<class T>
	static T GetField(
		const std::vector<CStringW>& fields, //!< [in] 配列
		int iIndex, //!< 配列内のインデックス
		T def = T() //!< デフォルト値
	) {
		CString s = GetField(fields, iIndex);
		if (s.IsEmpty())
			return def;
		return FromStr<T>(s);
	}

	//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
	template<class T>
	static T GetField(
		const std::vector<CStringA>& fields, //!< [in] 配列
		int iIndex, //!< 配列内のインデックス
		T def = T() //!< デフォルト値
	) {
		CString s = GetField(fields, iIndex);
		if (s.IsEmpty())
			return def;
		return FromStr<T>(s);
	}
};

#endif

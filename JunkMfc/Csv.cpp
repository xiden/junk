#include "stdafx.h"
#include "Csv.h"

//! 指定区切り文字でフィールドを切り分けて取得する
void Csv::Split(
	const CStringA& text, //!< [in] 切り出したい文字列
	char separator, //!< [in] 区切り文字
	char bundler, //!< [in] 括り文字
	std::vector<CStringA>& fields //!< [out] 分割後のフィールド配列が返る
) {
	fields.resize(0);

	if(text.IsEmpty())
		return;

	// マルチバイトのままだと１文字が複数バイトになり得るので処理が難しい
	// ワイド文字に変換する
	CStringW textW(text);

	const wchar_t* p = textW; // 作業用ポインタ
	const wchar_t* pFieldStart; // フィールドの開始位置
	const wchar_t* pFieldEnd; // フィールドの終端位置+1(区切り文字または終端の位置)
	const wchar_t* pTextEnd = p + textW.GetLength(); // 文字列長
	std::vector<wchar_t> field; // フィールド文字配列

	// 文字列からフィールドを取り出していく
	while (p <= pTextEnd) {
		// フィールドの先頭を探す
		while (p < pTextEnd && (*p == L' ')) {
			p++;
		}

		// ダブルクォーテーションで括られているか調べる
		wchar_t sp2 = separator;
		if (bundler != 0 && p < pTextEnd && p[0] == bundler) {
			sp2 = bundler;
			p++;
		}

		// この時点でフィールドの先頭が確定
		pFieldStart = p;

		// 区切り文字を探しフィールドの終端を確定する
		field.resize(0);
		pFieldEnd = pFieldStart;
		while(pFieldEnd < pTextEnd) {
			int move = 1;
			if (pFieldEnd[0] == sp2) {
				if (separator != sp2 && pFieldEnd[1] == sp2)
					move = 2; // ダブルクォーテーションが２つ並んでいるならそれは終端ではない
				else
					break;
			}
			field.push_back(*pFieldEnd);
			pFieldEnd += move;
		}

		// フィールド配列にフィールドを追加
		if (field.empty())
			fields.push_back("");
		else
			fields.push_back(CStringA(&field[0], field.size()));

		// 区切り文字がオーバーロードされていた場合には元の区切り文字まで進める
		if (separator != sp2) {
			while(pFieldEnd < pTextEnd) {
				if(*pFieldEnd == separator)
					break;
				pFieldEnd++;
			}
		}

		// フィールド終端が文字列の終端に一致しているなら全て切り出し完了
		if (pFieldEnd == pTextEnd)
			break;

		// 次フィールドの検索開始位置へ移動
		p = pFieldEnd + 1;
	}
}

//! 指定区切り文字でフィールドを切り分けて取得する
void Csv::Split(
	const CStringW& text, //!< [in] 切り出したい文字列
	wchar_t separator, //!< [in] 区切り文字
	wchar_t bundler, //!< [in] 括り文字
	std::vector<CStringW>& fields //!< [out] 分割後のフィールド配列が返る
) {
	fields.resize(0);

	if(text.IsEmpty())
		return;

	const wchar_t* p = text; // 作業用ポインタ
	const wchar_t* pFieldStart; // フィールドの開始位置
	const wchar_t* pFieldEnd; // フィールドの終端位置+1(区切り文字または終端の位置)
	const wchar_t* pTextEnd = p + text.GetLength(); // 文字列長
	std::vector<wchar_t> field; // フィールド文字配列

	// 文字列からフィールドを取り出していく
	while (p <= pTextEnd) {
		// フィールドの先頭を探す
		while (p < pTextEnd && (*p == L' ')) {
			p++;
		}

		// ダブルクォーテーションで括られているか調べる
		wchar_t sp2 = separator;
		if (bundler != 0 && p < pTextEnd && p[0] == bundler) {
			sp2 = bundler;
			p++;
		}

		// この時点でフィールドの先頭が確定
		pFieldStart = p;

		// 区切り文字を探しフィールドの終端を確定する
		field.resize(0);
		pFieldEnd = pFieldStart;
		while(pFieldEnd < pTextEnd) {
			int move = 1;
			if (pFieldEnd[0] == sp2) {
				if (separator != sp2 && pFieldEnd[1] == sp2)
					move = 2; // ダブルクォーテーションが２つ並んでいるならそれは終端ではない
				else
					break;
			}
			field.push_back(*pFieldEnd);
			pFieldEnd += move;
		}

		// フィールド配列にフィールドを追加
		if (field.empty())
			fields.push_back(L"");
		else
			fields.push_back(CStringW(&field[0], field.size()));

		// 区切り文字がオーバーロードされていた場合には元の区切り文字まで進める
		if (separator != sp2) {
			while(pFieldEnd < pTextEnd) {
				if(*pFieldEnd == separator)
					break;
				pFieldEnd++;
			}
		}

		// フィールド終端が文字列の終端に一致しているなら全て切り出し完了
		if (pFieldEnd == pTextEnd)
			break;

		// 次フィールドの検索開始位置へ移動
		p = pFieldEnd + 1;
	}
}

//! 指定区切り文字でフィールドを連結して取得する
void Csv::Combine(
	const std::vector<CStringA>& fields, //!< [in] 結果の分割された文字列が追加される配列
	wchar_t separator, //!< [in] 区切り文字
	wchar_t bundler, //!< [in] 括り文字
	CStringA& text //!< [out] 連結後の文字列
) {
	char spchars[3], rep1[2], rep2[3];
	CStringA s2;

	if (bundler == 0) {
		spchars[0] = (char)separator;
		spchars[1] = 0;
	} else {
		spchars[0] = (char)separator;
		spchars[1] = (char)bundler;
		spchars[2] = 0;

		rep1[0] = (char)bundler;
		rep1[1] = 0;
		rep2[0] = (char)bundler;
		rep2[1] = (char)bundler;
		rep2[2] = 0;
	}

	text.Empty();
	for (int i = 0; i < (int)fields.size(); i++) {
		if (i != 0)
			text += separator;

		const CStringA& s = fields[i];
		if (!s.IsEmpty()) {
			if (bundler == 0 || s.FindOneOf(spchars) < 0) {
				// 括り文字指定が無いまたはフィールド内に区切り文字が含まれていない場合には括らない
				text += s;
			} else {
				// 括り文字指定があり且つフィールド内に区切り文字が含まれている場合には括る
				// その際括り文字を２連に置き換える
				s2 = s;
				s2.Replace(rep1, rep2);
				text += bundler;
				text += s2;
				text += bundler;
			}
		}
	}
}

//! 指定区切り文字でフィールドを連結して取得する
void Csv::Combine(
	const std::vector<CStringW>& fields, //!< [in] 結果の分割された文字列が追加される配列
	wchar_t separator, //!< [in] 区切り文字
	wchar_t bundler, //!< [in] 括り文字
	CStringW& text //!< [out] 連結後の文字列
) {
	wchar_t spchars[3], rep1[2], rep2[3];
	CStringW s2;

	if (bundler == 0) {
		spchars[0] = separator;
		spchars[1] = 0;
	} else {
		spchars[0] = separator;
		spchars[1] = bundler;
		spchars[2] = 0;

		rep1[0] = bundler;
		rep1[1] = 0;
		rep2[0] = bundler;
		rep2[1] = bundler;
		rep2[2] = 0;
	}

	text.Empty();
	for (int i = 0; i < (int)fields.size(); i++) {
		if (i != 0)
			text += separator;

		const CStringW& s = fields[i];
		if (!s.IsEmpty()) {
			if (bundler == 0 || s.FindOneOf(spchars) < 0) {
				// 括り文字指定が無いまたはフィールド内に区切り文字が含まれていない場合には括らない
				text += s;
			} else {
				// 括り文字指定があり且つフィールド内に区切り文字が含まれている場合には括る
				// その際括り文字を２連に置き換える
				s2 = s;
				s2.Replace(rep1, rep2);
				text += bundler;
				text += s2;
				text += bundler;
			}
		}
	}
}


//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
CString Csv::GetField(
	const std::vector<CStringW>& fields, //!< [in] 配列
	int iIndex, //!< 配列内のインデックス
	LPCTSTR pszDef //!< デフォルト値
	) {
	if (iIndex < 0 || (int)fields.size() <= iIndex) {
		if (pszDef != NULL)
			return pszDef;
		else
			return CString();
	}
	return CString(fields[iIndex]);
}

//! 配列から指定フィールドを取得する、配列サイズが足りなかったらデフォルト値が返る
CString Csv::GetField(
	const std::vector<CStringA>& fields, //!< [in] 配列
	int iIndex, //!< 配列内のインデックス
	LPCTSTR pszDef //!< デフォルト値
) {
	if (iIndex < 0 || (int)fields.size() <= iIndex) {
		if (pszDef != NULL)
			return pszDef;
		else
			return CString();
	}
	return CString(fields[iIndex]);
}

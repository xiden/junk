
#include "stdafx.h"
#include "IniFile.h"
#include "NumberFormat.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//------------------------------------------------------------------------------
#define INITIAL_BUF_SIZE 16384 // 初期バッファサイズ
#define DIGITS 15 // デフォルトの有効桁数

//------------------------------------------------------------------------------
// 機能 : コンストラクタ
//
CIniFile::CIniFile()
	: m_delimiter(',')
	, m_nDigits(DIGITS)
	, m_fncStrToInt(NULL)
	, m_fncStrToDouble(NULL)
	, m_fncIntToStr(NULL)
	, m_fncDoubleToStr(NULL)
{
}

//------------------------------------------------------------------------------
// 機能 : コピーコンストラクタ
//
CIniFile::CIniFile(
					const CIniFile& c	// [in] コピー元
					)
	: m_strFile(c.m_strFile)
	, m_strSection(c.m_strSection)
	, m_delimiter(',')
	, m_nDigits(DIGITS)
	, m_fncStrToInt(c.m_fncStrToInt)
	, m_fncStrToDouble(c.m_fncStrToDouble)
	, m_fncIntToStr(c.m_fncIntToStr)
	, m_fncDoubleToStr(c.m_fncDoubleToStr)
{
	m_Map.Copy(c.m_Map);
}

//------------------------------------------------------------------------------
// 機能 : コンストラクタ
//
CIniFile::CIniFile(
					const CString& sFile // [in] 処理ファイルパス
					)
	: m_strFile(sFile)
	, m_delimiter(',')
	, m_nDigits(DIGITS)
	, m_fncStrToInt(NULL)
	, m_fncStrToDouble(NULL)
	, m_fncIntToStr(NULL)
	, m_fncDoubleToStr(NULL)
{
}

//------------------------------------------------------------------------------
// 機能 : コンストラクタ
//
CIniFile::CIniFile(
					const CString& sFile,	// [in] 処理ファイルパス
					const CString& sSection	// [in] セクション
					)
	: m_strFile(sFile)
	, m_strSection(sSection)
	, m_delimiter(',')
	, m_nDigits(DIGITS)
	, m_fncStrToInt(NULL)
	, m_fncStrToDouble(NULL)
	, m_fncIntToStr(NULL)
	, m_fncDoubleToStr(NULL)
{
}

//------------------------------------------------------------------------------
// 機能 : 代入オペレータ
//
CIniFile& CIniFile::operator=(
					const CIniFile& c		// [in] コピー元
					)
{
	m_strFile = c.m_strFile;
	m_strSection = c.m_strSection;
	m_delimiter = c.m_delimiter;
	m_nDigits = c.m_nDigits;
	m_fncStrToInt = c.m_fncStrToInt;
	m_fncStrToDouble = c.m_fncStrToDouble;
	m_fncIntToStr = c.m_fncIntToStr;
	m_fncDoubleToStr = c.m_fncDoubleToStr;
	m_Map.Copy(c.m_Map);
	return *this;
}

//------------------------------------------------------------------------------
// 機能 : 指定のセクション、キーに値が存在するか？
//			セクション、キー自体がない時もある
//
// 返り値 : 存在する=TRUE 存在しない=FALSE
//
BOOL CIniFile::ValueExists(
					LPCSTR pszSection,	// [in] セクション
					LPCSTR pszKey		// [in] キー
					)
{
	char buf[2];
	return GetPrivateProfileString(pszSection, pszKey, "", buf, 2) != 0;
}

//------------------------------------------------------------------------------
// 機能 : 指定のセクション、キーに値が存在するか？
//			セクション、キー自体がない時もある
//
// 返り値 : 存在する=TRUE 存在しない=FALSE
//
BOOL CIniFile::ValueExists(
					LPCSTR pszKey		// [in] キー
					)
{
	return ValueExists(m_strSection, pszKey);
}

//------------------------------------------------------------------------------
// 機能 : 指定のセクションが存在するか？
//
// 返り値 : 存在する=TRUE 存在しない=FALSE
//
BOOL CIniFile::SectionExists(
					LPCSTR pszSection	// [in] セクション
					)
{
	char buf[3];
	return GetPrivateProfileSection(pszSection, buf, 3) != 0;
//	return GetPrivateProfileString(pszSection, NULL, "", buf, 3) != 0; ←これだとCSV形式のセクションの存在を調べられない
}

//------------------------------------------------------------------------------
// 機能 : フラッシュ。
//
void CIniFile::Flush()
{
	SetPrivateProfileString(NULL, NULL, NULL);
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーから文字列として取得。
//
// 返り値 : 取得できた：TRUE、できなかった：FALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] キー
					CString* pVal,		// [out] 取得文字列
					LPCSTR pszDef		// [in] デフォルト
					)
{
	*pVal = GetString(pszKey, pszDef);
	return TRUE;
}

// 機能 : 指定のパス、セクション、キーから実数値として取得。
//
// 返り値 : 取得できた：TRUE、できなかった：FALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] キー
					double* pVal,		// [out] 取得実数値
					double Def			// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToDouble(s);
	return TRUE;
}

// 機能 : 指定のパス、セクション、キーから整数値として取得。
//
// 返り値 : 取得できた：TRUE、できなかった：FALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] キー
					int* pVal,			// [out] 取得整数値
					int Def				// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToInt(s);
	return TRUE;
}

// 機能 : 指定のパス、セクション、キーから整数値として取得。
//
// 返り値 : 取得できた：TRUE、できなかった：FALSE
//
// 機能説明 : １ベースの正数として取得.
///		ファイル上で１ベースで記述されている値を読み込み０ベースへ変換する。
BOOL CIniFile::GetValueOrg1(
					LPCSTR pszKey,		// [in] キー
					int* pVal,			// [out] 取得整数値
					int Def				// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToInt(s) - 1;
	return TRUE;
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーに文字列として書きこむ。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszVal		// [in] 書きこむ文字列
					)
{
	return SetPrivateProfileString(m_strSection, pszKey, InvConvertByMap(pszVal));
}

// 機能 : 指定のパス、セクション、キーに実数値として書きこむ。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] キー
					double Val			// [in] 書きこむ実数値
					)
{
	return SetValue(pszKey, ToStr(Val));
}

// 機能 : 指定のパス、セクション、キーに整数値として書きこむ。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] キー
					int Val				// [in] 整数値
					)
{
	return SetValue(pszKey, ToStr(Val));
}

// 機能 : 指定のパス、セクション、キーに整数値として書きこむ。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
// 機能説明 : １ベースの整数として書き込む.
///				０ベースの値をファイル上の１ベース表現に変換して書き込む。
//
BOOL CIniFile::SetValueOrg1(
					LPCSTR pszKey,		// [in] キー
					int Val				// [in] 整数値
					)
{
	return SetValue(pszKey, ToStr(Val + 1));
}

//------------------------------------------------------------------------------
// 機能 : 文字列の初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には何もしない。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszVal		// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, pszVal);
}

// 機能 : 実数の初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には何もしない。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					double Val			// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, Val);
}

// 機能 : 整数初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には何もしない。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					int Val				// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, Val);
}

// 機能 : 文字列の初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					CString* pVar,		// [out] 既存の値が返る
					LPCSTR pszVal		// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
	{
		return GetValue(pszKey, pVar);
	} else
	{
		*pVar = pszVal;
		return SetValue(pszKey, pszVal);
	}
}

// 機能 : 実数の初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					double* pVar,		// [out] 既存の値が返る
					double Val			// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
	{
		return GetValue(pszKey, pVar);
	} else
	{
		*pVar = Val;
		return SetValue(pszKey, Val);
	}
}

// 機能 : 整数の初期データを書き込む
//
// 返り値 : 書き込んだ=TRUE  既存のデータが存在した=FALSE
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] キー
					int* pVar,			// [out] 既存の値が返る
					int Val				// [in] 書き込む値
					)
{
	if(ValueExists(m_strSection, pszKey))
	{
		return GetValue(pszKey, pVar);
	} else
	{
		*pVar = Val;
		return SetValue(pszKey, Val);
	}
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで文字列として読み書き
//
// 返り値 : 読み込みの場合 読み込み文字数
// 			書き込みの場合 成功=TRUE  失敗=FALSE
//
// 機能説明 : bWrite によって、読み or 書き
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=書き込み FALSE=読み込み
					LPCSTR pszKey,		// [in] キー
					CString* pVal,		// [in / out] 読み書きされる文字列
					LPCSTR pszDef		// [in] 読み込み時、デフォルト
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, pszDef);
}

// 機能 : 指定のパス、セクション、キーで実数値として読み書き
//
// 返り値 : 読み込みの場合 読み込み文字数
// 			書き込みの場合 成功=TRUE  失敗=FALSE
//
// 機能説明 : bWrite によって、読み or 書き
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=書き込み FALSE=読み込み
					LPCSTR pszKey,		// [in] キー
					double* pVal,		// [in / out] 読み書きされる実数値
					double Def			// [in] 読み込み時、デフォルト
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, Def);
}

// 機能 : 指定のパス、セクション、キーで整数値として読み書き
//
// 返り値 : 読み込みの場合 読み込み文字数
// 			書き込みの場合 成功=TRUE  失敗=FALSE
//
// 機能説明 : bWrite によって、読み or 書き
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=書き込み FALSE=読み込み
					LPCSTR pszKey,		// [in] キー
					int* pVal,			// [in / out] 読み書きされる整数値
					int Def				// [in] 読み込み時、デフォルト
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, Def);
}

// 機能 : 指定のパス、セクション、キーで整数値として読み書き
//
// 返り値 : 読み込みの場合 読み込み文字数
// 			書き込みの場合 成功=TRUE  失敗=FALSE
//
// 機能説明 : bWrite によって、読み or 書き
//
// 備考 :	１ベース整数として読み込み＆書き込み.
//           プログラム内では０ベース、ファイル上では１ベースで表現されている場合に使用する。
BOOL CIniFile::ExchangeValueOrg1(
					BOOL bWrite,		// [in] TRUE=書き込み FALSE=読み込み
					LPCSTR pszKey,		// [in] キー
					int* pVal,			// [in / out] 読み書きされる整数値
					int Def				// [in] 読み込み時、デフォルト
					)
{
	if(bWrite) return SetValueOrg1(pszKey, *pVal);
	else       return GetValueOrg1(pszKey, pVal, Def);
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで文字列配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,        // [in] キー
						  CArray<CString>& val, // [out] 読み込まれた文字列配列が返る
						  LPCSTR pszDef         // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// 機能 : 指定のパス、セクション、キーで実数値配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,       // [in] キー
						  CArray<double>& val, // [out] 読み込まれた実数配列が返る
						  LPCSTR pszDef        // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// 機能 : 指定のパス、セクション、キーで整数値配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,    // [in] キー
						  CArray<int>& val, // [out] 読み込まれた実数配列が返る
						  LPCSTR pszDef     // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// 機能 : 指定のパス、セクション、キーで整数値配列として読み込み、INIファイル上では1ベースで読み込まれた値は0ベースとなる
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArrayOrg1(
						  LPCSTR pszKey,    // [in] キー
						  CArray<int>& val, // [out] 読み込まれた実数配列が返る
						  LPCSTR pszDef     // [in] デフォルト値文字列
						  )
{
	return GetArrayFromStringOrg1(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// 機能 : 指定のパス、セクション、キーで文字列配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] キー
						  CString* pVal, // [out] 読み込まれた文字列配列が返る
						  int nVal,      // [in] pVal の最大要素数
						  LPCSTR pszDef  // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// 機能 : 指定のパス、セクション、キーで実数値配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] キー
						  double* pVal,  // [out] 読み込まれた実数配列が返る
						  int nVal,      // [in] pVal の最大要素数
						  LPCSTR pszDef  // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// 機能 : 指定のパス、セクション、キーで整数値配列として読み込み
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] キー
						  int* pVal,     // [out] 読み込まれた整数配列が返る
						  int nVal,      // [in] pVal の最大要素数
						  LPCSTR pszDef  // [in] デフォルト値文字列
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// 機能 : 指定のパス、セクション、キーで整数値配列として読み込み、INIファイル上では1ベースで読み込まれた値は0ベースとなる
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//
DWORD CIniFile::GetArrayOrg1(
						  LPCSTR pszKey, // [in] キー
						  int* pVal,     // [out] 読み込まれた整数配列が返る
						  int nVal,      // [in] pVal の最大要素数
						  LPCSTR pszDef  // [in] デフォルト値文字列
						  )
{
	return GetArrayFromStringOrg1(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで文字列配列として書き込み
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,             // [in] キー
						  const CArray<CString>& val // [in] 書きこむ文字列配列
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで実数値配列として書き込み
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,            // [in] キー
						  const CArray<double>& val	// [in] 書きこむ実数値配列
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで整数値配列として書き込み
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,         // [in] キー
						  const CArray<int>& val // [in] 書きこむ整数値配列
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで整数値配列として書き込み、メモリ上は0ベースINIファイル上では1ベースとなる
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayOrg1(
						  LPCSTR pszKey,         // [in] キー
						  const CArray<int>& val // [in] 書きこむ整数値配列
						  )
{
	CString str;
	int n = SetArrayToStringOrg1(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで文字列配列として書き込み
//
// 返り値 : 書き込みした要素数
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,       // [in] キー
					const CString* pVal, // [in] 書きこむ文字列配列
					int nVal             // [in] 配列数
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで実数値配列として書き込み
//
// 返り値 : 書き込みした要素数
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,		// [in] キー
					const double* pVal,	// [in] 書きこむ実数値配列
					int nVal			// [in] 配列数
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで整数値配列として書き込み
//
// 返り値 : 書き込みした要素数
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,		// [in] キー
					const int* pVal,	// [in] 書きこむ整数値配列
					int nVal			// [in] 配列数
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// 機能 : 指定のパス、セクション、キーで整数値配列として書き込み、メモリ上は0ベースINIファイル上では1ベースとなる
//
// 返り値 : 書き込みした要素数
//
DWORD CIniFile::SetArrayOrg1(
					LPCSTR pszKey,		// [in] キー
					const int* pVal,	// [in] 書きこむ整数値配列
					int nVal			// [in] 配列数
					)
{
	CString str;
	int n = SetArrayToStringOrg1(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

//------------------------------------------------------------------------------
// 機能 : 文字列配列の初期データを書き込む
//
// 返り値 : 書き込み／読み込みしたデータ数
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,        // [in] キー
						   CArray<CString>& val, // [in, out] 書き込む値配列、既存値がある場合には既存値が返る
						   LPCSTR pszDef         // [in] デフォルト値文字列
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

// 機能 : 実数値配列の初期データを書き込む
//
// 返り値 : 書き込み／読み込みしたデータ数
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,       // [in] キー
						   CArray<double>& val, // [in, out] 書き込む値配列、既存値がある場合には既存値が返る
						   LPCSTR pszDef        // [in] デフォルト値文字列
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

// 機能 : 整数値配列の初期データを書き込む
//
// 返り値 : 書き込み／読み込みしたデータ数
//
// 機能説明 : 既存のデータが存在しない場合に書き込み、存在する場合には既存データを返す。
//            値を初期化する時に使用する。
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,    // [in] キー
						   CArray<int>& val, // [in, out] 書き込む値配列、既存値がある場合には既存値が返る
						   LPCSTR pszDef     // [in] デフォルト値文字列
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

//------------------------------------------------------------------------------
// 機能 : 文字配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,          // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,        // [in] キー
							   CArray<CString>& val, // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       LPCSTR pszDef         // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// 機能 : 実数配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,         // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	    // [in] キー
							   CArray<double>& val, // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       LPCSTR pszDef        // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// 機能 : 整数配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,      // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	 // [in] キー
							   CArray<int>& val, // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       LPCSTR pszDef     // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// 機能 : 整数配列の書き込み／読み込み、INIファイル上では1ベース、メモリ上では0ベースとなる
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArrayOrg1(
							   BOOL bWrite,      // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	 // [in] キー
							   CArray<int>& val, // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       LPCSTR pszDef     // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArrayOrg1(pszKey, val);
	else
		return GetArrayOrg1(pszKey, val, pszDef);
}

// 機能 : 文字配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,   // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey, // [in] キー
							   CString* pVal, // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       int nVal,      // [in] pVal の最大要素数
						       LPCSTR pszDef  // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// 機能 : 実数配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,        // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	   // [in] キー
							   double* pVal,       // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       int nVal,           // [in] pVal の最大要素数
						       LPCSTR pszDef       // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// 機能 : 整数配列の書き込み／読み込み
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,     // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	// [in] キー
							   int* pVal,       // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       int nVal,        // [in] pVal の最大要素数
						       LPCSTR pszDef    // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// 機能 : 整数配列の書き込み／読み込み、INIファイル上では1ベース、メモリ上では0ベースとなる
//
// 返り値 : 書き込み／読み込みしたデータ数
//
DWORD CIniFile::ExchangeArrayOrg1(
							   BOOL bWrite,     // [in] 書き込み時に TRUE 、読み込み時には FALSE を指定する
							   LPCSTR pszKey,	// [in] キー
							   int* pVal,       // [in, out] 書き込みデータ、又は読み込まれたデータが返る
						       int nVal,        // [in] pVal の最大要素数
						       LPCSTR pszDef    // [in] デフォルト値文字列
							   )
{
	if(bWrite)
		return SetArrayOrg1(pszKey, pVal, nVal);
	else
		return GetArrayOrg1(pszKey, pVal, nVal, pszDef);
}

//------------------------------------------------------------------------------
// 機能 : 指定のキーを削除します。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::DeleteValue(
					LPCSTR pszSection,	// [in] セクション
					LPCSTR pszKey		// [in] キー
					)
{
	return SetPrivateProfileString(pszSection, pszKey, NULL);
}
// 機能 : 指定のキーを削除します。
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::DeleteValue(
					LPCSTR pszKey		// [in] キー
					)
{
	return SetPrivateProfileString(m_strSection, pszKey, NULL);
}

// 機能 : 指定されたセクションを削除します
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::DeleteSection(
							  LPCSTR pszSection // [in] セクション
							  )
{
	return SetPrivateProfileString(pszSection, NULL, NULL);
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで文字列として読み込み
//
// 返り値 : 読み込み文字列
//
CString CIniFile::GetString(
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszDef		// [in] デフォルト
					)
{
	return ConvertByMap(GetPrivateProfileString(m_strSection, pszKey, pszDef));
}

// 機能 : 指定のパス、セクション、キーで実数値として読み込み
//
// 返り値 : 読み込んだ実数値
//
double CIniFile::GetDouble(
					LPCSTR pszKey,		// [in] キー
					double Def			// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToDouble(s);
}

// 機能 : 指定のパス、セクション、キーで整数値として読み込み
//
// 返り値 : 読み込んだ整数値
//
int CIniFile::GetInt(
					LPCSTR pszKey,		// [in] キー
					int Def				// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToInt(s);
}

// 機能 : 指定のパス、セクション、キーから整数値として取得。
//
// 返り値 : 読み込んだ整数値
//
// 機能説明 : １ベースの正数として取得.
//		ファイル上で１ベースで記述されている値を読み込み０ベースへ変換する。
int CIniFile::GetIntOrg1(
					LPCSTR pszKey,		// [in] キー
					int Def				// [in] デフォルト
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToInt(s) - 1;
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで初期化された文字列として書きこむ
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::FormatValue(
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszFormat,	// [in] フォーマット
					 ...)
{
	va_list args;
	va_start(args, pszFormat);
	BOOL r = FormatValueV(pszKey, pszFormat, args);
	va_end(args);
	return r;
}

// 機能 : 指定のパス、セクション、キーで初期化された文字列として書きこむ
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::FormatValueV(
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszFormat,	// [in] フォーマット
					va_list args		// [in] フォーマットパラメータ
					)
{
	CString s;
	s.FormatV(pszFormat, args);
	return SetValue(pszKey, s);
}

//------------------------------------------------------------------------------
// 機能 : 指定されたセクション内の全データを取得する
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
// 備考 : 取得されるデータは "識別子=値" が１データとして読み込まれ、
//			データ間には '\0' が挿入されています。'\0' が連続して二つある部分が終端です。
//
//			取得されるデータの例）"A=123'\0'B=456'\0'\0'"
//
BOOL CIniFile::GetSectionData(
							   LPCSTR pszSection, // [in] セクション名
							   CString& data      // [out] セクション内の全データ文字列が返る
							   )
{
	if(!SectionExists(pszSection))
		return FALSE;

	DWORD nSize = INITIAL_BUF_SIZE;
	std::string s; // 一時バッファ
	while(1)
	{
		s.resize(0); // 一端サイズ０にしないとバッファサイズ拡張した際に既存データがコピーされるので遅くなる
		s.resize(nSize);
		DWORD result = GetPrivateProfileSection(pszSection, &s[0], nSize);
		if(result != nSize - 2)
		{
			//	バッファに格納
			data.SetString(s.c_str(), result);
			break;
		}
		nSize *= 2;
	}

	return TRUE;
}

// 機能 : 指定されたセクション内の全データを文字列で取得する
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::GetSectionDataAsText(
									LPCSTR pszSection, // [in] セクション名
									CString& data      // [out] セクション内の全データ文字列が返る
									 )
{
	if(!GetSectionData(pszSection, data))
		return FALSE;

	//	'\0' を '\n' に置き換える
	LPSTR p = (LPSTR)(LPCSTR)data;
	LPSTR pEnd = p + data.GetLength();
	for(; p < pEnd; p++)
		if(*p == '\0')
			*p = '\n';

	return TRUE;
}

// 機能 : 指定されたセクションへ指定されたデータを書き込む
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
// 備考 : セクション内の全文字列が置き換わります。
//		最後の文字が '\n' では無い場合セクションにデータが追加されてしまいます。(WritePrivateProfileSection の動作上)
//		確実にセクション内の全データを設定したい場合にはまずセクションを消してから設定してください。
//
BOOL CIniFile::SetSectionData(
							   LPCSTR pszSection,  // [in] セクション名
							   const CString& data // [in] セクションに書き込む文字列
							   )
{
	int n = data.GetLength();

	//	文字列の最後に '\0' が二つ無い場合は '\0' を追加する
	if(n != 0 && data[n - 1] != '\0')
	{
		CString t((LPCSTR)data, n + 1);
		return SetPrivateProfileSection(pszSection, t);
	}
	else
	{
		return SetPrivateProfileSection(pszSection, data);
	}
}

//------------------------------------------------------------------------------
// 機能 : 文字列から整数への変換コールバック関数設定
//
void CIniFile::SetStrToIntFunc(
								StrToInt fnc // [in] 関数ポインタ
								)
{
	m_fncStrToInt = fnc;
}

// 機能 : 文字列から実数への変換コールバック関数設定
//
void CIniFile::SetStrToDoubleFunc(
								   StrToDouble fnc // [in] 関数ポインタ
								   )
{
	m_fncStrToDouble = fnc;
}

// 機能 : 整数から文字列への変換コールバック関数設定
//
void CIniFile::SetIntToStrFunc(
								IntToStr fnc // [in] 関数ポインタ
								)
{
	m_fncIntToStr = fnc;
}

// 機能 : 実数から文字列への変換コールバック関数設定
//
void CIniFile::SetDoubleToStrFunc(
								   DoubleToStr fnc // [in] 関数ポインタ
								   )
{
	m_fncDoubleToStr = fnc;
}

// 機能 : 文字列から整数への変換コールバック関数取得
//
CIniFile::StrToInt CIniFile::GetStrToIntFunc()
{
	return m_fncStrToInt;
}

// 機能 : 文字列から実数への変換コールバック関数取得
//
CIniFile::StrToDouble CIniFile::GetStrToDoubleFunc()
{
	return m_fncStrToDouble;
}

// 機能 : 正数から文字列への変換コールバック関数取得
//
CIniFile::IntToStr CIniFile::GetIntToStrFunc()
{
	return m_fncIntToStr;
}

// 機能 : 実数から文字列への変換コールバック関数取得
//
CIniFile::DoubleToStr CIniFile::GetDoubleToStrFunc()
{
	return m_fncDoubleToStr;
}

//------------------------------------------------------------------------------
// 機能 : 指定のパス、セクション、キーで文字列として読み込む
//
// 返り値 : 読み込み文字数
//
DWORD CIniFile::GetPrivateProfileString(
					LPCSTR pszSection,	// [in] セクション
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszDef,		// [in] デフォルト
					LPSTR pszRetBuf,	// [out] 読み込み文字列が格納
					DWORD nSize			// [in] 文字列格納エリアサイズ
					)
{
	DWORD n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, pszRetBuf, nSize, m_strFile);
	return n;
}

// 機能 : 指定のパス、セクション、キーで文字列として読み込む
//
// 返り値 : 読み込み文字列
//
CString CIniFile::GetPrivateProfileString(
					LPCSTR pszSection, // [in] セクション
					LPCSTR pszKey,     // [in] キー
					LPCSTR pszDef      // [in] デフォルト
					)
{
	//	まず固定サイズの一時バッファへロード
	char buf[INITIAL_BUF_SIZE];
	DWORD n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, buf, INITIAL_BUF_SIZE, m_strFile);
	if(n != INITIAL_BUF_SIZE - 1)
		return CString(buf, n); // 文字列全体が読み込めたならリターン

	//	バッファサイズが足りなければバッファサイズ増やしながらロード
	DWORD nSize = INITIAL_BUF_SIZE * 2;
	std::string s; // 一時バッファ
	while(1)
	{
		s.resize(0); // 一端サイズ０にしないとバッファサイズ拡張した際に既存データがコピーされるので遅くなる
		s.resize(nSize);
		n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, &s[0], nSize, m_strFile);
		if(n != nSize - 1)
			return CString(s.c_str(), n);
		nSize *= 2;
	}
}

// 機能 : 指定のパス、セクション、キーで文字列として書きこむ
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
BOOL CIniFile::SetPrivateProfileString(
					LPCSTR pszSection,	// [in] セクション
					LPCSTR pszKey,		// [in] キー
					LPCSTR pszVal		// [in] 文字列
					)
{
	BOOL b = ::WritePrivateProfileString(pszSection, pszKey, pszVal, m_strFile);
	return b;
}

// 機能 : 指定されたセクション内の全データを文字列で取得する
//
// 返り値 : 読み込まれた文字数
//
DWORD CIniFile::GetPrivateProfileSection(
	LPCSTR pszSection, // [in] セクション
	LPSTR pszRetBuf,   // [out] このバッファに読み込まれた文字列が格納される
	DWORD nSize        // [in] pszRetBuf バッファサイズ
	)
{
	return ::GetPrivateProfileSection(pszSection, pszRetBuf, nSize, m_strFile);
}

// 機能 : 指定されたセクションへ指定されたデータを書き込む
//
// 返り値 : 成功=TRUE  失敗=FALSE
//
// 備考 : セクション内の全文字列が置き換わります。
//
BOOL CIniFile::SetPrivateProfileSection(
	LPCTSTR pszSection, // [in] セクション
	LPCTSTR pszData     // [in] 設定するデータ
	)
{
	return ::WritePrivateProfileSection(pszSection, pszData, m_strFile);
}

// 機能 : 指定された文字列を変換マップで変換する
// 
// 返り値 : 変換後の文字列
//
CString CIniFile::ConvertByMap(
	CString sStr // [in] 変換する文字列
)
{
	for(int i = 0, n = (int)m_Map.GetCount(); i < n; i++)
	{
		if(m_Map[i].sKey == sStr)
			return m_Map[i].sVal;
	}
	return sStr;
}

// 機能 : 指定された文字列を変換マップで逆変換する
// 
// 返り値 : 変換後の文字列
//
CString CIniFile::InvConvertByMap(
	CString sStr // [in] 変換する文字列
)
{
	for(int i = 0, n = (int)m_Map.GetCount(); i < n; i++)
	{
		if(m_Map[i].sVal == sStr)
			return m_Map[i].sKey;
	}
	return sStr;
}

//------------------------------------------------------------------------------
// 機能 : INIファイル内CSV形式の文字列をカンマで分割する
//
// 備考 : ダブルクォーテーション(")で括られている中のカンマ(,)は区切り文字列と判断されない。
//		Win32API の GetPrivateProfileString は一番外側のダブルクォーテーションを勝手に外してしまう。
//			例) "abc" → abc
//			例) "a","b","c" → a","b","c
//
//		完全ではないが一応このようなパターンで正しく動作するようになっている。
//			a","b","c→ a と b と c に分解
//
void CIniFile::CsvSplit(
			  LPCSTR pszText,         // [in] 分割する文字列
			  CArray<CString>& fields // [out] 分割された文字列が返る
			  )
{
	int i, start, end;
	bool bQuoteStart, bQuoteEnd;
	int len = (int)strlen(pszText);
	int term = len + 1;
	int delim = m_delimiter;

	int iField;

	//	文字列からフィールドを取り出していく
	i = 0;
	iField = 0;
	fields.SetSize(0);

	if(pszText[0] == '\0')
		return; // 文字列が空ならフィールド数0

	while (i < term)
	{
		//	フィールドの先頭を探す
		while (i < len && (pszText[i] == ' ' || pszText[i] == '\t'))
		{
			i++;
		}

		//	ダブルクォーテーションで括られているか調べる
		if (i < len && pszText[i] == '"')
		{
			bQuoteStart = true;
			bQuoteEnd = false;
			i++;
		}
		else
		{
			bQuoteStart = false;
			bQuoteEnd = false;
		}

		start = end = i;

		//	フィールドの終端を探す
		for (; i < term; i++)
		{
			if (i == len)
			{
				end = i;
				i++;
				break;
			}

			if (pszText[i] == '"')
			{
				end = i;
				bQuoteEnd = true;

				//	" の後に , が来たら文字の終端だと判断
				//	(角度の秒(")が出てくると対処できないが、INIファイルの仕様上どうしようもない)
				if (i + 1 < len && pszText[i + 1] == delim)
				{
					i += 2;
					break;
				}
			}
			else if (pszText[i] == delim)
			{
				if (bQuoteStart)
				{
					//	" で括られている場合は , は区切り記号とはみなさない
					if (bQuoteEnd)
					{
						i++;
						break;
					}
				}
				else
				{
					end = i;
					i++;
					break;
				}
			}
		}

		//	フィールド配列にフィールドを追加
		fields.SetAtGrow(iField, CString(pszText + start, end - start));
		iField++;
	}
}

// 機能 : INIファイル内CSV形式の文字列をカンマで分割する
//
// 備考 : ダブルクォーテーション(")で括られている中のカンマ(,)は区切り文字列と判断されない。
//		Win32API の GetPrivateProfileString は一番外側のダブルクォーテーションを勝手に外してしまう。
//			例) "abc" → abc
//			例) "a","b","c" → a","b","c
//
//		完全ではないが一応このようなパターンで正しく動作するようになっている。
//			a","b","c→ a と b と c に分解
//
// 返り値 : 分割によって作成されたフィールド数
//
DWORD CIniFile::CsvSplit(
			  LPCSTR pszText,   // [in] 分割する文字列
			  CString* pFields, // [out] 分割された文字列が返る
			  int nFieldsMax    // [in] 最大フィールド数
			  )
{
	if(nFieldsMax <= 0)
		return 0;
	if(pszText[0] == '\0')
		return 0; // 文字列が空ならフィールド数0

	int i, start, end;
	bool bQuoteStart, bQuoteEnd;
	int len = (int)strlen(pszText);
	int term = len + 1;
	int delim = m_delimiter;

	int iField;

	//	文字列からフィールドを取り出していく
	i = 0;
	iField = 0;

	while (i < term)
	{
		//	フィールドの先頭を探す
		while (i < len && (pszText[i] == ' ' || pszText[i] == '\t'))
		{
			i++;
		}

		//	ダブルクォーテーションで括られているか調べる
		if (i < len && pszText[i] == '"')
		{
			bQuoteStart = true;
			bQuoteEnd = false;
			i++;
		}
		else
		{
			bQuoteStart = false;
			bQuoteEnd = false;
		}

		start = end = i;

		//	フィールドの終端を探す
		for (; i < term; i++)
		{
			if (i == len)
			{
				end = i;
				i++;
				break;
			}

			if (pszText[i] == '"')
			{
				end = i;
				bQuoteEnd = true;

				//	" の後に , が来たら文字の終端だと判断
				//	(角度の秒(")が出てくると対処できないが、INIファイルの仕様上どうしようもない)
				if (i + 1 < len && pszText[i + 1] == delim)
				{
					i += 2;
					break;
				}
			}
			else if (pszText[i] == delim)
			{
				if (bQuoteStart)
				{
					//	" で括られている場合は , は区切り記号とはみなさない
					if (bQuoteEnd)
					{
						i++;
						break;
					}
				}
				else
				{
					end = i;
					i++;
					break;
				}
			}
		}

		//	フィールド配列にフィールドを追加
		pFields[iField] = CString(pszText + start, end - start);
		iField++;
		if(nFieldsMax <= iField)
			break;
	}

	return iField;
}

//------------------------------------------------------------------------------
// 機能 : 指定されたCSV形式文字列から文字列配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//			変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString,    // [in] CSV形式の文字列
									CArray<CString>& val // [out] フィールド毎に分割されたデータが返る
									)
{
	CsvSplit(pszString, val);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
		val[i] = ConvertByMap(val[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から実数値配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString,   // [in] CSV形式の文字列
									CArray<double>& val	// [out] フィールド毎に分割されたデータが返る
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToDouble(strs[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から整数値配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV形式の文字列
									CArray<int>& val  // [out] フィールド毎に分割されたデータが返る
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToInt(strs[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から整数値配列を取得する、文字列では1ベース表現 int 型値では0ベースとなる
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromStringOrg1(
									LPCSTR pszString, // [in] CSV形式の文字列
									CArray<int>& val  // [out] フィールド毎に分割されたデータが返る
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToInt(strs[i]) - 1;
	return n;
}

// 機能 : 指定されたCSV形式文字列から文字列配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : KEY=a,b,c,d  の値の値を読み込むと "a" , "b" , "c" , "d" の配列が返ります。
//			変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV形式の文字列
									CString* pVal,    // [out] フィールド毎に分割されたデータが返る
									int nVal          // [in] pVal の最大要素数
									)
{
	int n = CsvSplit(pszString, pVal, nVal);
	for(int i = 0; i < n; i++)
		pVal[i] = ConvertByMap(pVal[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から実数値配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV形式の文字列
									double* pVal,     // [out] フィールド毎に分割されたデータが返る
									int nVal          // [in] pVal の最大要素数
									)
{
	if(nVal <= 0)
		return 0;
	CArray<CString> strs;
	strs.SetSize(nVal);
	int n = GetArrayFromString(pszString, &strs[0], nVal);
	if(nVal < n)
		n = nVal;
	for(int i = 0; i < n; i++)
		pVal[i] = ToDouble(strs[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から整数値配列を取得する
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV形式の文字列
									int* pVal,        // [out] フィールド毎に分割されたデータが返る
									int nVal          // [in] pVal の最大要素数
									)
{
	if(nVal <= 0)
		return 0;
	CArray<CString> strs;
	strs.SetSize(nVal);
	int n = GetArrayFromString(pszString, &strs[0], nVal);
	if(nVal < n)
		n = nVal;
	for(int i = 0; i < n; i++)
		pVal[i] = ToInt(strs[i]);
	return n;
}

// 機能 : 指定されたCSV形式文字列から整数値配列を取得する、文字列では1ベース表現 int 型値では0ベースとなる
//
// 返り値 : 読み込みデータ数
//
// 備考 : 変換マップが適用されます。
//
DWORD CIniFile::GetArrayFromStringOrg1(
									LPCSTR pszString, // [in] CSV形式の文字列
									int* pVal,        // [out] フィールド毎に分割されたデータが返る
									int nVal          // [in] pVal の最大要素数
									)
{
	if(nVal <= 0)
		return 0;
	CArray<CString> strs;
	strs.SetSize(nVal);
	int n = GetArrayFromString(pszString, &strs[0], nVal);
	if(nVal < n)
		n = nVal;
	for(int i = 0; i < n; i++)
		pVal[i] = ToInt(strs[i]) - 1;
	return n;
}

//------------------------------------------------------------------------------
// 機能 : 指定された文字列配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,          // [out] CSV形式の文字列が返る
								  const CArray<CString>& val // [in] 文字列配列
								  )
{
	sString.SetString("", 0);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
	{
		sString += InvConvertByMap(val[i]);
		if(i != n - 1)
			sString += char(m_delimiter);
	}
	return n;
}

// 機能 : 指定された実数値配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,         // [out] CSV形式の文字列が返る
								  const CArray<double>& val	// [in] 実数値配列
								  )
{
	sString.SetString("", 0);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
	{
		sString += InvConvertByMap(ToStr(val[i]));
		if(i != n - 1)
			sString += char(m_delimiter);
	}
	return n;
}

// 機能 : 指定された整数値配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,      // [out] CSV形式の文字列が返る
								  const CArray<int>& val // [in] 実数値配列
								  )
{
	sString.SetString("", 0);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
	{
		sString += InvConvertByMap(ToStr(val[i]));
		if(i != n - 1)
			sString += char(m_delimiter);
	}
	return n;
}

// 機能 : 指定された整数値配列をCSV形式文字列に変換する、int 型値では0ベース文字列では1ベース表現となる
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToStringOrg1(
								  CString& sString,      // [out] CSV形式の文字列が返る
								  const CArray<int>& val // [in] 実数値配列
								  )
{
	sString.SetString("", 0);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
	{
		sString += InvConvertByMap(ToStr(val[i] + 1));
		if(i != n - 1)
			sString += char(m_delimiter);
	}
	return n;
}

// 機能 : 指定された文字列配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,    // [out] CSV形式の文字列が返る
								  const CString* pVal, // [in] 文字列配列
								  int nVal             // [in] 配列の要素数
								  )
{
	sString.SetString("", 0);
	for(int i = 0; i < nVal; i++)
	{
		sString += InvConvertByMap(pVal[i]);
		if(i != nVal - 1)
			sString += char(m_delimiter);
	}
	return nVal;
}

// 機能 : 指定された実数値配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,   // [out] CSV形式の文字列が返る
								  const double* pVal, // [in] 実数値配列
								  int nVal			  // [in] 配列の要素数
								  )
{
	sString.SetString("", 0);
	for(int i = 0; i < nVal; i++)
	{
		sString += InvConvertByMap(ToStr(pVal[i]));
		if(i != nVal - 1)
			sString += char(m_delimiter);
	}
	return nVal;
}

// 機能 : 指定された整数値配列をCSV形式文字列に変換する
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToString(
								  CString& sString, // [out] CSV形式の文字列が返る
								  const int* pVal,	// [in] 整数値配列
								  int nVal			// [in] 配列の要素数
								  )
{
	sString.SetString("", 0);
	for(int i = 0; i < nVal; i++)
	{
		sString += InvConvertByMap(ToStr(pVal[i]));
		if(i != nVal - 1)
			sString += char(m_delimiter);
	}
	return nVal;
}

// 機能 : 指定された整数値配列をCSV形式文字列に変換する、int 型値では0ベース文字列では1ベース表現となる
//
// 返り値 : 書き込んだ要素数
//
DWORD CIniFile::SetArrayToStringOrg1(
								  CString& sString, // [out] CSV形式の文字列が返る
								  const int* pVal,	// [in] 整数値配列
								  int nVal			// [in] 配列の要素数
								  )
{
	sString.SetString("", 0);
	for(int i = 0; i < nVal; i++)
	{
		sString += InvConvertByMap(ToStr(pVal[i] + 1));
		if(i != nVal - 1)
			sString += char(m_delimiter);
	}
	return nVal;
}

//------------------------------------------------------------------------------
// 機能 : GetSectionData で取得したデータから１行取得する
//
// 返り値 : TRUE=成功 FALSE=失敗
//
BOOL CIniFile::GetLineFromSectionData(
									   CString& text,         // [out] １行分の文字列が返る
									   LPCSTR& pszSectionData // [in,out] データポインタ、１行取得されると次の行の先頭を指すポインタが返る
									   )
{
	LPCSTR p = pszSectionData;
	if(*p == '\0')
		return FALSE;

	LPCSTR e;
	for(e = p; *e != '\0'; e++);
	e++;

	text.SetString(p, (int)(e - p));
	pszSectionData = e;

	return TRUE;
}

//------------------------------------------------------------------------------
// 機能 : 文字列を整数値に変換する
//
// 返り値 : 変換した整数値
//
int CIniFile::ToInt(
					LPCSTR pszStr			// [in] 文字列
					)
{
	if(m_fncStrToInt != NULL)
		return m_fncStrToInt(pszStr);
	else
		return strtol(pszStr, NULL, 0);
}

// 機能 : 文字列を実数値に変換する
//
// 返り値 : 変換した実数値
//
double CIniFile::ToDouble(
					LPCSTR pszStr			// [in] 文字列
					)
{
	if(m_fncStrToDouble != NULL)
		return m_fncStrToDouble(pszStr);
	else
		return atof(pszStr);
}

// 機能 : 整数値を文字列に変換する
//
// 返り値 : 変換した文字列
//
CString CIniFile::ToStr(
					int val				// [in] 整数値
					)
{
	if(m_fncIntToStr != NULL)
	{
		return m_fncIntToStr(val);
	}
	else
	{
		char buf[64];
		_snprintf_s(buf, 64, "%d", val);
		return buf;
	}
}

// 機能 : 実数値を文字列に変換する
//
// 返り値 : 変換した文字列
//
CString CIniFile::ToStr(
					double val				// [in] 実数値
					)
{
	if(m_fncDoubleToStr != NULL)
	{
		return m_fncDoubleToStr(val);
	}
	else
	{
		return NumberFormat::FromEffectiveDigit(m_nDigits).ToStr(val);
	}
}

// 機能 : INI ファイルへ実数値を書き込む際の有効桁数の設定
// 
void CIniFile::SetDigits(
		int nVal // [in] 設定する値
	)
{
	m_nDigits = nVal;
}

// 機能 : INI ファイルへ実数値を書き込む際の有効桁数の取得
// 
// 返り値 : INI ファイルへ実数値を書き込む際の有効桁数
// 
int CIniFile::GetDigits() const
{
	return m_nDigits;
}

// 機能 : マップ追加
// 
// 備考 : INIから値読みこみ時にキーに一致するものが合ったら指定された値に変換するためのマップを追加する。
//
void CIniFile::AddMap(
	const CString& sValInIni, // [in] INI上での値
	const CString& sValOnMem // [in] メモリ読み込み時の値
)
{
	MapItem item;
	item.sKey = sValInIni;
	item.sVal = sValOnMem;
	m_Map.Add(item);
}

// 機能 : マップクリア
// 
void CIniFile::ClearMap()
{
	m_Map.RemoveAll();
}

//==============================================================================
//		INIファイル内容の読みこみと保存の基本クラス(RFC2004 ソースの互換性のために追加 2009.03.16 T.Ishikawa)
BOOL CIniBase::Load(LPCSTR pszFile)
{
	if(pszFile != NULL)
		SetFileName(pszFile);
	return Exchange(FALSE);
}

BOOL CIniBase::Save(LPCSTR pszFile)
{
	if(pszFile != NULL)
		SetFileName(pszFile);
	return Exchange(TRUE);
}

void CIniBase::SetSwMap()
{
	AddMap("OFF", "0");
	AddMap("ON", "1");
}

void CIniBase::SetTrueFalseMap()
{
	AddMap("FALSE", "0");
	AddMap("TRUE", "1");
}

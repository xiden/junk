#pragma once
#include <afx.h>
#include <stdarg.h>
#include <afxtempl.h>
#include <vector>
#include "ArrayHelper.h"

#define INI_SET_VALUE(section, key, val) \
{ \
	section.key = val; \
	SetSection(#section); \
	SetValue(#key, section.key); \
}
#define INI_INIT_VALUE(section, key, val) \
{ \
	SetSection(#section); \
	InitValue(#key, &section.key, val); \
}

#define INI_LOAD_VALUE(section, key) \
{ \
	SetSection(#section); \
	GetValue(#key, &section.key); \
}
#define INI_SAVE_VALUE(section, key) \
{ \
	SetSection(#section); \
	SetValue(#key, section.key); \
}
#define INI_EXCHANGE_VALUE(section, key) \
{ \
	SetSection(#section); \
	ExchangeValue(bWrite, #key, &section.key); \
}
#define INI_EXCHANGE_VALUE_DEF(section, key, def) \
{ \
	SetSection(#section); \
	ExchangeValue(bWrite, #key, &section.key, def); \
}
#define INI_EXCHANGE_CURVAL(strct, key) \
{ \
	ExchangeValue(bWrite, #key, &strct.key); \
}
#define INI_EXCHANGE_CURVAL_DEF(strct, key, def) \
{ \
	ExchangeValue(bWrite, #key, &strct.key, def); \
}

class CIniFile // INI ファイルアクセスクラス
{
public:
	typedef int (*StrToInt)(LPCSTR pszStr);
	typedef double (*StrToDouble)(LPCSTR pszStr);
	typedef CString (*IntToStr)(int val);
	typedef CString (*DoubleToStr)(double val);

	CIniFile();
	CIniFile(const CIniFile& c);
	CIniFile(const CString& sFile);
	CIniFile(const CString& sFile, const CString& sSection);
	CIniFile& operator=(const CIniFile& c);

	CString GetFileName() const { return m_strFile; }
	BOOL SetFileName(const CString& sFile) { m_strFile = sFile; return TRUE; }

	CString GetSection() const { return m_strSection; }
	BOOL SetSection(const CString& sSection) { m_strSection = sSection; return TRUE; }

	int GetDelimiter() const { return m_delimiter; }
	BOOL SetDelimiter(int dlmt) { m_delimiter = dlmt; return TRUE; }

	BOOL ValueExists(LPCSTR pszSection, LPCSTR pszKey);
	BOOL ValueExists(LPCSTR pszKey);
	BOOL SectionExists(LPCSTR pszSection);

	void Flush();

	BOOL GetValue(LPCSTR pszKey, CString* pVal, LPCSTR pszDef = "");
	BOOL GetValue(LPCSTR pszKey, double* pVal, double Def = 0.0);
	BOOL GetValue(LPCSTR pszKey, int* pVal, int Def = 0);
	BOOL GetValueOrg1(LPCSTR pszKey, int* pVal, int Def = 0);

	BOOL SetValue(LPCSTR pszKey, LPCSTR pszVal);
	BOOL SetValue(LPCSTR pszKey, double Val);
	BOOL SetValue(LPCSTR pszKey, int Val);
	BOOL SetValue(LPCSTR pszKey, const RECT& Val);
	BOOL SetValueOrg1(LPCSTR pszKey, int Val);

	BOOL InitValue(LPCSTR pszKey, LPCSTR pszVal);
	BOOL InitValue(LPCSTR pszKey, double Val);
	BOOL InitValue(LPCSTR pszKey, int Val);

	BOOL InitValue(LPCSTR pszKey, CString* pVar, LPCSTR pszVal);
	BOOL InitValue(LPCSTR pszKey, double* pVar, double Val);
	BOOL InitValue(LPCSTR pszKey, int* pVar, int Val);

	BOOL ExchangeValue(BOOL bWrite, LPCSTR pszKey, CString* pVal, LPCSTR pszDef = "");
	BOOL ExchangeValue(BOOL bWrite, LPCSTR pszKey, double* pVal, double Def = 0.0);
	BOOL ExchangeValue(BOOL bWrite, LPCSTR pszKey, int* pVal, int Def = 0);
	BOOL ExchangeValueOrg1(BOOL bWrite, LPCSTR pszKey, int* pVal, int Def = 0);

	DWORD GetArray(LPCSTR pszKey, CArray<CString>& val, LPCSTR pszDef = "");
	DWORD GetArray(LPCSTR pszKey, CArray<double>& val, LPCSTR pszDef = "");
	DWORD GetArray(LPCSTR pszKey, CArray<int>& val, LPCSTR pszDef = "");
	DWORD GetArrayOrg1(LPCSTR pszKey, CArray<int>& val, LPCSTR pszDef = "");
	DWORD GetArray(LPCSTR pszKey, CString* pVal, int nVal, LPCSTR pszDef = "");
	DWORD GetArray(LPCSTR pszKey, double* pVal, int nVal, LPCSTR pszDef = "");
	DWORD GetArray(LPCSTR pszKey, int* pVal, int nVal, LPCSTR pszDef = "");
	DWORD GetArrayOrg1(LPCSTR pszKey, int* pVal, int nVal, LPCSTR pszDef = "");

	DWORD GetArray(LPCSTR pszKey, std::vector<CString>& val, LPCSTR pszDef = "") {
		CArray<CString> a;
		DWORD result = GetArray(pszKey, a, pszDef);
		CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD GetArray(LPCSTR pszKey, std::vector<double>& val, LPCSTR pszDef = "") {
		CArray<double> a;
		DWORD result = GetArray(pszKey, a, pszDef);
		CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD GetArray(LPCSTR pszKey, std::vector<int>& val, LPCSTR pszDef = "") {
		CArray<int> a;
		DWORD result = GetArray(pszKey, a, pszDef);
		CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD GetArrayOrg1(LPCSTR pszKey, std::vector<int>& val, LPCSTR pszDef = "") {
		CArray<int> a;
		DWORD result = GetArrayOrg1(pszKey, a, pszDef);
		CArrayHelper::Copy(val, a);
		return result;
	}


	DWORD SetArray(LPCSTR pszKey, const CArray<CString>& val);
	DWORD SetArray(LPCSTR pszKey, const CArray<double>& val);
	DWORD SetArray(LPCSTR pszKey, const CArray<int>& val);
	DWORD SetArrayOrg1(LPCSTR pszKey, const CArray<int>& val);
	DWORD SetArray(LPCSTR pszKey, const CString* pVal, int nVal);
	DWORD SetArray(LPCSTR pszKey, const double* pVal, int nVal);
	DWORD SetArray(LPCSTR pszKey, const int* pVal, int nVal);
	DWORD SetArrayOrg1(LPCSTR pszKey, const int* pVal, int nVal);

	DWORD SetArray(LPCSTR pszKey, const std::vector<CString>& val) {
		CArray<CString> a;
		CArrayHelper::Copy(a, val);
		return SetArray(pszKey, a);
	}
	DWORD SetArray(LPCSTR pszKey, const std::vector<double>& val) {
		CArray<double> a;
		CArrayHelper::Copy(a, val);
		return SetArray(pszKey, a);
	}
	DWORD SetArray(LPCSTR pszKey, const std::vector<int>& val) {
		CArray<int> a;
		CArrayHelper::Copy(a, val);
		return SetArray(pszKey, a);
	}
	DWORD SetArrayOrg1(LPCSTR pszKey, const std::vector<int>& val) {
		CArray<int> a;
		CArrayHelper::Copy(a, val);
		return SetArrayOrg1(pszKey, a);
	}

	DWORD InitArray(LPCSTR pszKey, CArray<CString>& val, LPCSTR pszDef = "");
	DWORD InitArray(LPCSTR pszKey, CArray<double>& val, LPCSTR pszDef = "");
	DWORD InitArray(LPCSTR pszKey, CArray<int>& val, LPCSTR pszDef = "");

	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, CArray<CString>& val, LPCSTR pszDef = "");
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, CArray<double>& val, LPCSTR pszDef = "");
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, CArray<int>& val, LPCSTR pszDef = "");
	DWORD ExchangeArrayOrg1(BOOL bWrite, LPCSTR pszKey, CArray<int>& val, LPCSTR pszDef = "");
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, CString* pVal, int nVal, LPCSTR pszDef = "");
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, double* pVal, int nVal, LPCSTR pszDef = "");
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, int* pVal, int nVal, LPCSTR pszDef = "");
	DWORD ExchangeArrayOrg1(BOOL bWrite, LPCSTR pszKey, int* pVal, int nVal, LPCSTR pszDef = "");

	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, std::vector<CString>& val, LPCSTR pszDef = "") {
		CArray<CString> a;
		if(bWrite) CArrayHelper::Copy(a, val);
		DWORD result = ExchangeArray(bWrite, pszKey, a, pszDef);
		if(!bWrite) CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, std::vector<double>& val, LPCSTR pszDef = "") {
		CArray<double> a;
		if(bWrite) CArrayHelper::Copy(a, val);
		DWORD result = ExchangeArray(bWrite, pszKey, a, pszDef);
		if(!bWrite) CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD ExchangeArray(BOOL bWrite, LPCSTR pszKey, std::vector<int>& val, LPCSTR pszDef = "") {
		CArray<int> a;
		if(bWrite) CArrayHelper::Copy(a, val);
		DWORD result = ExchangeArray(bWrite, pszKey, a, pszDef);
		if(!bWrite) CArrayHelper::Copy(val, a);
		return result;
	}
	DWORD ExchangeArrayOrg1(BOOL bWrite, LPCSTR pszKey, std::vector<int>& val, LPCSTR pszDef = "") {
		CArray<int> a;
		if(bWrite) CArrayHelper::Copy(a, val);
		DWORD result = ExchangeArrayOrg1(bWrite, pszKey, a, pszDef);
		if(!bWrite) CArrayHelper::Copy(val, a);
		return result;
	}

	BOOL DeleteValue(LPCSTR pszSection, LPCSTR pszKey);
	BOOL DeleteValue(LPCSTR pszKey);
	BOOL DeleteSection(LPCSTR pszSection);

	CString GetString(LPCSTR pszKey, LPCSTR pszDef = "");
	double GetDouble(LPCSTR pszKey, double Def = 0.0);
	int GetInt(LPCSTR pszKey, int Def = 0);
	int GetIntOrg1(LPCSTR pszKey, int Def = 0);

	BOOL FormatValue(LPCSTR pszKey, LPCSTR pszFormat, ...);
	BOOL FormatValueV(LPCSTR pszKey, LPCSTR pszFormat, va_list args);

	BOOL GetSectionData(LPCSTR pszSection, CString& data);
	BOOL GetSectionDataAsText(LPCSTR pszSection, CString& text);
	BOOL SetSectionData(LPCSTR pszSection, const CString& data);

	void SetStrToIntFunc(StrToInt fnc);
	void SetStrToDoubleFunc(StrToDouble fnc);
	void SetIntToStrFunc(IntToStr fnc);
	void SetDoubleToStrFunc(DoubleToStr fnc);

	StrToInt GetStrToIntFunc();
	StrToDouble GetStrToDoubleFunc();
	IntToStr GetIntToStrFunc();
	DoubleToStr GetDoubleToStrFunc();

	void CsvSplit(LPCSTR pszText, CArray<CString>& fields);
	DWORD CsvSplit(LPCSTR pszText, CString* pFields, int nFieldsMax);

	DWORD GetArrayFromString(LPCSTR pszString, CArray<CString>& val);
	DWORD GetArrayFromString(LPCSTR pszString, CArray<double>& val);
	DWORD GetArrayFromString(LPCSTR pszString, CArray<int>& val);
	DWORD GetArrayFromStringOrg1(LPCSTR pszString, CArray<int>& val);
	DWORD GetArrayFromString(LPCSTR pszString, CString* pVal, int nVal);
	DWORD GetArrayFromString(LPCSTR pszString, double* pVal, int nVal);
	DWORD GetArrayFromString(LPCSTR pszString, int* pVal, int nVal);
	DWORD GetArrayFromStringOrg1(LPCSTR pszString, int* pVal, int nVal);

	DWORD SetArrayToString(CString& sString, const CArray<CString>& val);
	DWORD SetArrayToString(CString& sString, const CArray<double>& val);
	DWORD SetArrayToString(CString& sString, const CArray<int>& val);
	DWORD SetArrayToStringOrg1(CString& sString, const CArray<int>& val);
	DWORD SetArrayToString(CString& sString, const CString* pVal, int nVal);
	DWORD SetArrayToString(CString& sString, const double* pVal, int nVal);
	DWORD SetArrayToString(CString& sString, const int* pVal, int nVal);
	DWORD SetArrayToStringOrg1(CString& sString, const int* pVal, int nVal);

	CString ArrayToString(const CArray<CString>& val) { CString s; SetArrayToString(s, val); return s; }
	CString ArrayToString(const CArray<double>& val) { CString s; SetArrayToString(s, val); return s; }
	CString ArrayToString(const CArray<int>& val) { CString s; SetArrayToString(s, val); return s; }
	CString ArrayToStringOrg1(const CArray<int>& val) { CString s; SetArrayToStringOrg1(s, val); return s; }
	CString ArrayToString(const CString* pVal, int nVal) { CString s; SetArrayToString(s, pVal, nVal); return s; }
	CString ArrayToString(const double* pVal, int nVal) { CString s; SetArrayToString(s, pVal, nVal); return s; }
	CString ArrayToString(const int* pVal, int nVal) { CString s; SetArrayToString(s, pVal, nVal); return s; }
	CString ArrayToStringOrg1(const int* pVal, int nVal) { CString s; SetArrayToStringOrg1(s, pVal, nVal); return s; }

	CString ArrayToString(const std::vector<CString>& val) {
		CArray<CString> a;
		CArrayHelper::Copy(a, val);
		return ArrayToString(a);
	}
	CString ArrayToString(const std::vector<double>& val) {
		CArray<double> a;
		CArrayHelper::Copy(a, val);
		return ArrayToString(a);
	}
	CString ArrayToString(const std::vector<int>& val) {
		CArray<int> a;
		CArrayHelper::Copy(a, val);
		return ArrayToString(a);
	}
	CString ArrayToStringOrg1(const std::vector<int>& val) {
		CArray<int> a;
		CArrayHelper::Copy(a, val);
		return ArrayToStringOrg1(a);
	}

	virtual int ToInt(LPCSTR pszStr);
	virtual double ToDouble(LPCSTR pszStr);
	virtual CString ToStr(int val);
	virtual CString ToStr(double val);

	void SetDigits(int nVal); // INI ファイルへ実数値を書き込む際の有効桁数の設定
	int GetDigits() const; // INI ファイルへ実数値を書き込む際の有効桁数の取得

	void AddMap(const CString& sValInIni, const CString& sValOnMem); // マップ追加
	void ClearMap(); // マップクリア

	static BOOL GetLineFromSectionData(CString& text, LPCSTR& pszSectionData);

	template<class T>
	static DWORD CoverDefault(T* pVal, int nVal, const T* pDefVal, int nDefVal) // GetArray、ExchangeArray で取得された配列にデフォルト値を被せる
	{
		for(int i = nVal; i < nDefVal; i++)
			pVal[i] = pDefVal[i];
		if(nVal < nDefVal)
			return nDefVal;
		else
			return nVal;
	}

	template<class T>
	static void CoverDefault(CArray<T>& val, const CArray<T>& defVal) // GetArray、ExchangeArray で取得された配列にデフォルト値を被せる
	{
		int n1 = (int)val.GetCount();
		int n2 = (int)defVal.GetCount();
		if(n1 < n2)
			val.SetSize(n2);
		for(int i = n1; i < n2; i++)
			val[i] = defVal[i];
	}

protected:
	virtual DWORD GetPrivateProfileString(LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszDef, LPSTR pszRetBuf, DWORD nSize);
	virtual CString GetPrivateProfileString(LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszDef);
	virtual BOOL SetPrivateProfileString(LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszVal);
	virtual DWORD GetPrivateProfileSection(LPCSTR pszSection, LPSTR pszRetBuf, DWORD nSize);
	virtual BOOL SetPrivateProfileSection(LPCTSTR pszSection, LPCTSTR pszData);
	virtual CString ConvertByMap(CString sStr); // 指定された文字列を変換マップで変換する
	virtual CString InvConvertByMap(CString sStr); // 指定された文字列を変換マップで逆変換する

protected:
	struct MapItem // 変換アイテム
	{
		CString sKey; // INI上での値
		CString sVal; // メモリ読み込み時の値
	};

	CString m_strFile;    // INI ファイル名。絶対パス
	CString m_strSection; // アクセスするセクション名
	int     m_delimiter;  // 配列としてアクセス時のデリミタ
	int     m_nDigits;    // INI ファイルへ実数値を書き込む際の有効桁数

	StrToInt    m_fncStrToInt;    // 文字から整数への変換関数のポインタ
	StrToDouble m_fncStrToDouble; // 文字から実数への変換関数のポインタ
	IntToStr    m_fncIntToStr;    // 整数から文字への変換関数のポインタ
	DoubleToStr m_fncDoubleToStr; // 実数から文字への変換関数のポインタ

	CArray<MapItem> m_Map; // 変換マップ
};

class CIniBase : public CIniFile // INIファイル内容の読みこみと保存の基本クラス(RFC2004 ソースの互換性のために追加 2009.03.16 T.Ishikawa)
{
public:
	virtual ~CIniBase() {}
	virtual BOOL Load(LPCSTR pszFile = NULL);
	virtual BOOL Save(LPCSTR pszFile = NULL);
	virtual BOOL Exchange(BOOL bWrite) = 0;

	virtual void SetSwMap();
	virtual void SetTrueFalseMap();
};

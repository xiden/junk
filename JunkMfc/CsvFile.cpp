#include "StdAfx.h"
#include "CsvFile.h"

//!< コンストラクタ
CsvFile::CsvFile(void)
{
	m_Separator = ',';
	m_Bundler = '"';
}

//! コンストラクタ、ファイルオープンして初期化する
CsvFile::CsvFile(
	LPCTSTR lpszFileName,
	UINT nOpenFlags
) : CStdioFile(lpszFileName, nOpenFlags) {
	m_Separator = ',';
	m_Bundler = '"';
}

//! ファイルから１行を読み込む
BOOL CsvFile::ReadRow(
	std::vector<CStringA>& fields //!< [out] 読み込まれたフィールド配列が返る
) {
	if(!ReadString(m_sRow))
		return FALSE;
	m_sRowA = m_sRow;
	Csv::Split(m_sRowA, (char)m_Separator, (char)m_Bundler, fields);
	return true;
}

//! ファイルから１行を読み込む
BOOL CsvFile::ReadRow(
	std::vector<CStringW>& fields //!< [out] 読み込まれたフィールド配列が返る
) {
	if(!ReadString(m_sRow))
		return FALSE;
	m_sRowW = m_sRow;
	Csv::Split(m_sRowW, m_Separator, m_Bundler, fields);
	return true;
}

//! ファイルへ１行を書き込む
void CsvFile::WriteRow(
	const std::vector<CStringA>& fields //!< [in] 分割されている文字列
) {
	Csv::Combine(fields, (char)m_Separator, (char)m_Bundler, m_sRowA);
	m_sRowA += '\n';
	m_sRow = m_sRowA;
	WriteString(m_sRow);
}

//! ファイルへ１行を書き込む
void CsvFile::WriteRow(
	const std::vector<CStringW>& fields //!< [in] 分割されている文字列
) {
	Csv::Combine(fields, m_Separator, m_Bundler, m_sRowW);
	m_sRowW += L'\n';
	m_sRow = m_sRowW;
	WriteString(m_sRow);
}

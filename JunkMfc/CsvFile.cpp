#include "StdAfx.h"
#include "CsvFile.h"

//!< �R���X�g���N�^
CsvFile::CsvFile(void)
{
	m_Separator = ',';
	m_Bundler = '"';
}

//! �R���X�g���N�^�A�t�@�C���I�[�v�����ď���������
CsvFile::CsvFile(
	LPCTSTR lpszFileName,
	UINT nOpenFlags
) : CStdioFile(lpszFileName, nOpenFlags) {
	m_Separator = ',';
	m_Bundler = '"';
}

//! �t�@�C������P�s��ǂݍ���
BOOL CsvFile::ReadRow(
	std::vector<CStringA>& fields //!< [out] �ǂݍ��܂ꂽ�t�B�[���h�z�񂪕Ԃ�
) {
	if(!ReadString(m_sRow))
		return FALSE;
	m_sRowA = m_sRow;
	Csv::Split(m_sRowA, (char)m_Separator, (char)m_Bundler, fields);
	return true;
}

//! �t�@�C������P�s��ǂݍ���
BOOL CsvFile::ReadRow(
	std::vector<CStringW>& fields //!< [out] �ǂݍ��܂ꂽ�t�B�[���h�z�񂪕Ԃ�
) {
	if(!ReadString(m_sRow))
		return FALSE;
	m_sRowW = m_sRow;
	Csv::Split(m_sRowW, m_Separator, m_Bundler, fields);
	return true;
}

//! �t�@�C���ւP�s����������
void CsvFile::WriteRow(
	const std::vector<CStringA>& fields //!< [in] ��������Ă��镶����
) {
	Csv::Combine(fields, (char)m_Separator, (char)m_Bundler, m_sRowA);
	m_sRowA += '\n';
	m_sRow = m_sRowA;
	WriteString(m_sRow);
}

//! �t�@�C���ւP�s����������
void CsvFile::WriteRow(
	const std::vector<CStringW>& fields //!< [in] ��������Ă��镶����
) {
	Csv::Combine(fields, m_Separator, m_Bundler, m_sRowW);
	m_sRowW += L'\n';
	m_sRow = m_sRowW;
	WriteString(m_sRow);
}

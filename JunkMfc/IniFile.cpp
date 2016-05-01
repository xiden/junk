
#include "stdafx.h"
#include "IniFile.h"
#include "NumberFormat.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//------------------------------------------------------------------------------
#define INITIAL_BUF_SIZE 16384 // �����o�b�t�@�T�C�Y
#define DIGITS 15 // �f�t�H���g�̗L������

//------------------------------------------------------------------------------
// �@�\ : �R���X�g���N�^
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
// �@�\ : �R�s�[�R���X�g���N�^
//
CIniFile::CIniFile(
					const CIniFile& c	// [in] �R�s�[��
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
// �@�\ : �R���X�g���N�^
//
CIniFile::CIniFile(
					const CString& sFile // [in] �����t�@�C���p�X
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
// �@�\ : �R���X�g���N�^
//
CIniFile::CIniFile(
					const CString& sFile,	// [in] �����t�@�C���p�X
					const CString& sSection	// [in] �Z�N�V����
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
// �@�\ : ����I�y���[�^
//
CIniFile& CIniFile::operator=(
					const CIniFile& c		// [in] �R�s�[��
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
// �@�\ : �w��̃Z�N�V�����A�L�[�ɒl�����݂��邩�H
//			�Z�N�V�����A�L�[���̂��Ȃ���������
//
// �Ԃ�l : ���݂���=TRUE ���݂��Ȃ�=FALSE
//
BOOL CIniFile::ValueExists(
					LPCSTR pszSection,	// [in] �Z�N�V����
					LPCSTR pszKey		// [in] �L�[
					)
{
	char buf[2];
	return GetPrivateProfileString(pszSection, pszKey, "", buf, 2) != 0;
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃Z�N�V�����A�L�[�ɒl�����݂��邩�H
//			�Z�N�V�����A�L�[���̂��Ȃ���������
//
// �Ԃ�l : ���݂���=TRUE ���݂��Ȃ�=FALSE
//
BOOL CIniFile::ValueExists(
					LPCSTR pszKey		// [in] �L�[
					)
{
	return ValueExists(m_strSection, pszKey);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃Z�N�V���������݂��邩�H
//
// �Ԃ�l : ���݂���=TRUE ���݂��Ȃ�=FALSE
//
BOOL CIniFile::SectionExists(
					LPCSTR pszSection	// [in] �Z�N�V����
					)
{
	char buf[3];
	return GetPrivateProfileSection(pszSection, buf, 3) != 0;
//	return GetPrivateProfileString(pszSection, NULL, "", buf, 3) != 0; �����ꂾ��CSV�`���̃Z�N�V�����̑��݂𒲂ׂ��Ȃ�
}

//------------------------------------------------------------------------------
// �@�\ : �t���b�V���B
//
void CIniFile::Flush()
{
	SetPrivateProfileString(NULL, NULL, NULL);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[���當����Ƃ��Ď擾�B
//
// �Ԃ�l : �擾�ł����FTRUE�A�ł��Ȃ������FFALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] �L�[
					CString* pVal,		// [out] �擾������
					LPCSTR pszDef		// [in] �f�t�H���g
					)
{
	*pVal = GetString(pszKey, pszDef);
	return TRUE;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[��������l�Ƃ��Ď擾�B
//
// �Ԃ�l : �擾�ł����FTRUE�A�ł��Ȃ������FFALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] �L�[
					double* pVal,		// [out] �擾�����l
					double Def			// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToDouble(s);
	return TRUE;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[���琮���l�Ƃ��Ď擾�B
//
// �Ԃ�l : �擾�ł����FTRUE�A�ł��Ȃ������FFALSE
//
BOOL CIniFile::GetValue(
					LPCSTR pszKey,		// [in] �L�[
					int* pVal,			// [out] �擾�����l
					int Def				// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToInt(s);
	return TRUE;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[���琮���l�Ƃ��Ď擾�B
//
// �Ԃ�l : �擾�ł����FTRUE�A�ł��Ȃ������FFALSE
//
// �@�\���� : �P�x�[�X�̐����Ƃ��Ď擾.
///		�t�@�C����łP�x�[�X�ŋL�q����Ă���l��ǂݍ��݂O�x�[�X�֕ϊ�����B
BOOL CIniFile::GetValueOrg1(
					LPCSTR pszKey,		// [in] �L�[
					int* pVal,			// [out] �擾�����l
					int Def				// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	*pVal = s.IsEmpty() ? Def : ToInt(s) - 1;
	return TRUE;
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ɕ�����Ƃ��ď������ށB
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszVal		// [in] �������ޕ�����
					)
{
	return SetPrivateProfileString(m_strSection, pszKey, InvConvertByMap(pszVal));
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ɏ����l�Ƃ��ď������ށB
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] �L�[
					double Val			// [in] �������ގ����l
					)
{
	return SetValue(pszKey, ToStr(Val));
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ɐ����l�Ƃ��ď������ށB
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::SetValue(
					LPCSTR pszKey,		// [in] �L�[
					int Val				// [in] �����l
					)
{
	return SetValue(pszKey, ToStr(Val));
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ɐ����l�Ƃ��ď������ށB
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
// �@�\���� : �P�x�[�X�̐����Ƃ��ď�������.
///				�O�x�[�X�̒l���t�@�C����̂P�x�[�X�\���ɕϊ����ď������ށB
//
BOOL CIniFile::SetValueOrg1(
					LPCSTR pszKey,		// [in] �L�[
					int Val				// [in] �����l
					)
{
	return SetValue(pszKey, ToStr(Val + 1));
}

//------------------------------------------------------------------------------
// �@�\ : ������̏����f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͉������Ȃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszVal		// [in] �������ޒl
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, pszVal);
}

// �@�\ : �����̏����f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͉������Ȃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					double Val			// [in] �������ޒl
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, Val);
}

// �@�\ : ���������f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͉������Ȃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					int Val				// [in] �������ޒl
					)
{
	if(ValueExists(m_strSection, pszKey))
		return FALSE;
	return SetValue(pszKey, Val);
}

// �@�\ : ������̏����f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					CString* pVar,		// [out] �����̒l���Ԃ�
					LPCSTR pszVal		// [in] �������ޒl
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

// �@�\ : �����̏����f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					double* pVar,		// [out] �����̒l���Ԃ�
					double Val			// [in] �������ޒl
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

// �@�\ : �����̏����f�[�^����������
//
// �Ԃ�l : ��������=TRUE  �����̃f�[�^�����݂���=FALSE
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
BOOL CIniFile::InitValue(
					LPCSTR pszKey,		// [in] �L�[
					int* pVar,			// [out] �����̒l���Ԃ�
					int Val				// [in] �������ޒl
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
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����Ƃ��ēǂݏ���
//
// �Ԃ�l : �ǂݍ��݂̏ꍇ �ǂݍ��ݕ�����
// 			�������݂̏ꍇ ����=TRUE  ���s=FALSE
//
// �@�\���� : bWrite �ɂ���āA�ǂ� or ����
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=�������� FALSE=�ǂݍ���
					LPCSTR pszKey,		// [in] �L�[
					CString* pVal,		// [in / out] �ǂݏ�������镶����
					LPCSTR pszDef		// [in] �ǂݍ��ݎ��A�f�t�H���g
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, pszDef);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�Ƃ��ēǂݏ���
//
// �Ԃ�l : �ǂݍ��݂̏ꍇ �ǂݍ��ݕ�����
// 			�������݂̏ꍇ ����=TRUE  ���s=FALSE
//
// �@�\���� : bWrite �ɂ���āA�ǂ� or ����
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=�������� FALSE=�ǂݍ���
					LPCSTR pszKey,		// [in] �L�[
					double* pVal,		// [in / out] �ǂݏ������������l
					double Def			// [in] �ǂݍ��ݎ��A�f�t�H���g
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, Def);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�Ƃ��ēǂݏ���
//
// �Ԃ�l : �ǂݍ��݂̏ꍇ �ǂݍ��ݕ�����
// 			�������݂̏ꍇ ����=TRUE  ���s=FALSE
//
// �@�\���� : bWrite �ɂ���āA�ǂ� or ����
//
BOOL CIniFile::ExchangeValue(
					BOOL bWrite,		// [in] TRUE=�������� FALSE=�ǂݍ���
					LPCSTR pszKey,		// [in] �L�[
					int* pVal,			// [in / out] �ǂݏ�������鐮���l
					int Def				// [in] �ǂݍ��ݎ��A�f�t�H���g
					)
{
	if(bWrite) return SetValue(pszKey, *pVal);
	else         return GetValue(pszKey, pVal, Def);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�Ƃ��ēǂݏ���
//
// �Ԃ�l : �ǂݍ��݂̏ꍇ �ǂݍ��ݕ�����
// 			�������݂̏ꍇ ����=TRUE  ���s=FALSE
//
// �@�\���� : bWrite �ɂ���āA�ǂ� or ����
//
// ���l :	�P�x�[�X�����Ƃ��ēǂݍ��݁���������.
//           �v���O�������ł͂O�x�[�X�A�t�@�C����ł͂P�x�[�X�ŕ\������Ă���ꍇ�Ɏg�p����B
BOOL CIniFile::ExchangeValueOrg1(
					BOOL bWrite,		// [in] TRUE=�������� FALSE=�ǂݍ���
					LPCSTR pszKey,		// [in] �L�[
					int* pVal,			// [in / out] �ǂݏ�������鐮���l
					int Def				// [in] �ǂݍ��ݎ��A�f�t�H���g
					)
{
	if(bWrite) return SetValueOrg1(pszKey, *pVal);
	else       return GetValueOrg1(pszKey, pVal, Def);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,        // [in] �L�[
						  CArray<CString>& val, // [out] �ǂݍ��܂ꂽ������z�񂪕Ԃ�
						  LPCSTR pszDef         // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,       // [in] �L�[
						  CArray<double>& val, // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  LPCSTR pszDef        // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey,    // [in] �L�[
						  CArray<int>& val, // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  LPCSTR pszDef     // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ēǂݍ��݁AINI�t�@�C����ł�1�x�[�X�œǂݍ��܂ꂽ�l��0�x�[�X�ƂȂ�
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArrayOrg1(
						  LPCSTR pszKey,    // [in] �L�[
						  CArray<int>& val, // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  LPCSTR pszDef     // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromStringOrg1(GetPrivateProfileString(m_strSection, pszKey, pszDef), val);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] �L�[
						  CString* pVal, // [out] �ǂݍ��܂ꂽ������z�񂪕Ԃ�
						  int nVal,      // [in] pVal �̍ő�v�f��
						  LPCSTR pszDef  // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] �L�[
						  double* pVal,  // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  int nVal,      // [in] pVal �̍ő�v�f��
						  LPCSTR pszDef  // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArray(
						  LPCSTR pszKey, // [in] �L�[
						  int* pVal,     // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  int nVal,      // [in] pVal �̍ő�v�f��
						  LPCSTR pszDef  // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromString(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ēǂݍ��݁AINI�t�@�C����ł�1�x�[�X�œǂݍ��܂ꂽ�l��0�x�[�X�ƂȂ�
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//
DWORD CIniFile::GetArrayOrg1(
						  LPCSTR pszKey, // [in] �L�[
						  int* pVal,     // [out] �ǂݍ��܂ꂽ�����z�񂪕Ԃ�
						  int nVal,      // [in] pVal �̍ő�v�f��
						  LPCSTR pszDef  // [in] �f�t�H���g�l������
						  )
{
	return GetArrayFromStringOrg1(GetPrivateProfileString(m_strSection, pszKey, pszDef), pVal, nVal);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����z��Ƃ��ď�������
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,             // [in] �L�[
						  const CArray<CString>& val // [in] �������ޕ�����z��
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�z��Ƃ��ď�������
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,            // [in] �L�[
						  const CArray<double>& val	// [in] �������ގ����l�z��
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ď�������
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArray(
						  LPCSTR pszKey,         // [in] �L�[
						  const CArray<int>& val // [in] �������ސ����l�z��
						  )
{
	CString str;
	int n = SetArrayToString(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ď������݁A���������0�x�[�XINI�t�@�C����ł�1�x�[�X�ƂȂ�
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayOrg1(
						  LPCSTR pszKey,         // [in] �L�[
						  const CArray<int>& val // [in] �������ސ����l�z��
						  )
{
	CString str;
	int n = SetArrayToStringOrg1(str, val);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����z��Ƃ��ď�������
//
// �Ԃ�l : �������݂����v�f��
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,       // [in] �L�[
					const CString* pVal, // [in] �������ޕ�����z��
					int nVal             // [in] �z��
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�z��Ƃ��ď�������
//
// �Ԃ�l : �������݂����v�f��
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,		// [in] �L�[
					const double* pVal,	// [in] �������ގ����l�z��
					int nVal			// [in] �z��
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ď�������
//
// �Ԃ�l : �������݂����v�f��
//
DWORD CIniFile::SetArray(
					LPCSTR pszKey,		// [in] �L�[
					const int* pVal,	// [in] �������ސ����l�z��
					int nVal			// [in] �z��
					)
{
	CString str;
	int n = SetArrayToString(str, pVal, nVal);
	if(SetPrivateProfileString(m_strSection, pszKey, str))
		return n;
	else
		return 0;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�z��Ƃ��ď������݁A���������0�x�[�XINI�t�@�C����ł�1�x�[�X�ƂȂ�
//
// �Ԃ�l : �������݂����v�f��
//
DWORD CIniFile::SetArrayOrg1(
					LPCSTR pszKey,		// [in] �L�[
					const int* pVal,	// [in] �������ސ����l�z��
					int nVal			// [in] �z��
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
// �@�\ : ������z��̏����f�[�^����������
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,        // [in] �L�[
						   CArray<CString>& val, // [in, out] �������ޒl�z��A�����l������ꍇ�ɂ͊����l���Ԃ�
						   LPCSTR pszDef         // [in] �f�t�H���g�l������
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

// �@�\ : �����l�z��̏����f�[�^����������
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,       // [in] �L�[
						   CArray<double>& val, // [in, out] �������ޒl�z��A�����l������ꍇ�ɂ͊����l���Ԃ�
						   LPCSTR pszDef        // [in] �f�t�H���g�l������
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

// �@�\ : �����l�z��̏����f�[�^����������
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
// �@�\���� : �����̃f�[�^�����݂��Ȃ��ꍇ�ɏ������݁A���݂���ꍇ�ɂ͊����f�[�^��Ԃ��B
//            �l�����������鎞�Ɏg�p����B
//
DWORD CIniFile::InitArray(
						   LPCSTR pszKey,    // [in] �L�[
						   CArray<int>& val, // [in, out] �������ޒl�z��A�����l������ꍇ�ɂ͊����l���Ԃ�
						   LPCSTR pszDef     // [in] �f�t�H���g�l������
						   )
{
	if(ValueExists(m_strSection, pszKey))
		return GetArray(pszKey, val, pszDef);
	else
		return SetArray(pszKey, val);
}

//------------------------------------------------------------------------------
// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,          // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,        // [in] �L�[
							   CArray<CString>& val, // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       LPCSTR pszDef         // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,         // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	    // [in] �L�[
							   CArray<double>& val, // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       LPCSTR pszDef        // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,      // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	 // [in] �L�[
							   CArray<int>& val, // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       LPCSTR pszDef     // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, val);
	else
		return GetArray(pszKey, val, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ��݁AINI�t�@�C����ł�1�x�[�X�A��������ł�0�x�[�X�ƂȂ�
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArrayOrg1(
							   BOOL bWrite,      // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	 // [in] �L�[
							   CArray<int>& val, // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       LPCSTR pszDef     // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArrayOrg1(pszKey, val);
	else
		return GetArrayOrg1(pszKey, val, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,   // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey, // [in] �L�[
							   CString* pVal, // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       int nVal,      // [in] pVal �̍ő�v�f��
						       LPCSTR pszDef  // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,        // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	   // [in] �L�[
							   double* pVal,       // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       int nVal,           // [in] pVal �̍ő�v�f��
						       LPCSTR pszDef       // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ���
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArray(
							   BOOL bWrite,     // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	// [in] �L�[
							   int* pVal,       // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       int nVal,        // [in] pVal �̍ő�v�f��
						       LPCSTR pszDef    // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArray(pszKey, pVal, nVal);
	else
		return GetArray(pszKey, pVal, nVal, pszDef);
}

// �@�\ : �����z��̏������݁^�ǂݍ��݁AINI�t�@�C����ł�1�x�[�X�A��������ł�0�x�[�X�ƂȂ�
//
// �Ԃ�l : �������݁^�ǂݍ��݂����f�[�^��
//
DWORD CIniFile::ExchangeArrayOrg1(
							   BOOL bWrite,     // [in] �������ݎ��� TRUE �A�ǂݍ��ݎ��ɂ� FALSE ���w�肷��
							   LPCSTR pszKey,	// [in] �L�[
							   int* pVal,       // [in, out] �������݃f�[�^�A���͓ǂݍ��܂ꂽ�f�[�^���Ԃ�
						       int nVal,        // [in] pVal �̍ő�v�f��
						       LPCSTR pszDef    // [in] �f�t�H���g�l������
							   )
{
	if(bWrite)
		return SetArrayOrg1(pszKey, pVal, nVal);
	else
		return GetArrayOrg1(pszKey, pVal, nVal, pszDef);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃L�[���폜���܂��B
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::DeleteValue(
					LPCSTR pszSection,	// [in] �Z�N�V����
					LPCSTR pszKey		// [in] �L�[
					)
{
	return SetPrivateProfileString(pszSection, pszKey, NULL);
}
// �@�\ : �w��̃L�[���폜���܂��B
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::DeleteValue(
					LPCSTR pszKey		// [in] �L�[
					)
{
	return SetPrivateProfileString(m_strSection, pszKey, NULL);
}

// �@�\ : �w�肳�ꂽ�Z�N�V�������폜���܂�
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::DeleteSection(
							  LPCSTR pszSection // [in] �Z�N�V����
							  )
{
	return SetPrivateProfileString(pszSection, NULL, NULL);
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��ݕ�����
//
CString CIniFile::GetString(
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszDef		// [in] �f�t�H���g
					)
{
	return ConvertByMap(GetPrivateProfileString(m_strSection, pszKey, pszDef));
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ŏ����l�Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��񂾎����l
//
double CIniFile::GetDouble(
					LPCSTR pszKey,		// [in] �L�[
					double Def			// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToDouble(s);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�Ő����l�Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��񂾐����l
//
int CIniFile::GetInt(
					LPCSTR pszKey,		// [in] �L�[
					int Def				// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToInt(s);
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[���琮���l�Ƃ��Ď擾�B
//
// �Ԃ�l : �ǂݍ��񂾐����l
//
// �@�\���� : �P�x�[�X�̐����Ƃ��Ď擾.
//		�t�@�C����łP�x�[�X�ŋL�q����Ă���l��ǂݍ��݂O�x�[�X�֕ϊ�����B
int CIniFile::GetIntOrg1(
					LPCSTR pszKey,		// [in] �L�[
					int Def				// [in] �f�t�H���g
					)
{
	CString s = GetString(pszKey, "");
	return s.IsEmpty() ? Def : ToInt(s) - 1;
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŏ��������ꂽ������Ƃ��ď�������
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::FormatValue(
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszFormat,	// [in] �t�H�[�}�b�g
					 ...)
{
	va_list args;
	va_start(args, pszFormat);
	BOOL r = FormatValueV(pszKey, pszFormat, args);
	va_end(args);
	return r;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŏ��������ꂽ������Ƃ��ď�������
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::FormatValueV(
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszFormat,	// [in] �t�H�[�}�b�g
					va_list args		// [in] �t�H�[�}�b�g�p�����[�^
					)
{
	CString s;
	s.FormatV(pszFormat, args);
	return SetValue(pszKey, s);
}

//------------------------------------------------------------------------------
// �@�\ : �w�肳�ꂽ�Z�N�V�������̑S�f�[�^���擾����
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
// ���l : �擾�����f�[�^�� "���ʎq=�l" ���P�f�[�^�Ƃ��ēǂݍ��܂�A
//			�f�[�^�Ԃɂ� '\0' ���}������Ă��܂��B'\0' ���A�����ē���镔�����I�[�ł��B
//
//			�擾�����f�[�^�̗�j"A=123'\0'B=456'\0'\0'"
//
BOOL CIniFile::GetSectionData(
							   LPCSTR pszSection, // [in] �Z�N�V������
							   CString& data      // [out] �Z�N�V�������̑S�f�[�^�����񂪕Ԃ�
							   )
{
	if(!SectionExists(pszSection))
		return FALSE;

	DWORD nSize = INITIAL_BUF_SIZE;
	std::string s; // �ꎞ�o�b�t�@
	while(1)
	{
		s.resize(0); // ��[�T�C�Y�O�ɂ��Ȃ��ƃo�b�t�@�T�C�Y�g�������ۂɊ����f�[�^���R�s�[�����̂Œx���Ȃ�
		s.resize(nSize);
		DWORD result = GetPrivateProfileSection(pszSection, &s[0], nSize);
		if(result != nSize - 2)
		{
			//	�o�b�t�@�Ɋi�[
			data.SetString(s.c_str(), result);
			break;
		}
		nSize *= 2;
	}

	return TRUE;
}

// �@�\ : �w�肳�ꂽ�Z�N�V�������̑S�f�[�^�𕶎���Ŏ擾����
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::GetSectionDataAsText(
									LPCSTR pszSection, // [in] �Z�N�V������
									CString& data      // [out] �Z�N�V�������̑S�f�[�^�����񂪕Ԃ�
									 )
{
	if(!GetSectionData(pszSection, data))
		return FALSE;

	//	'\0' �� '\n' �ɒu��������
	LPSTR p = (LPSTR)(LPCSTR)data;
	LPSTR pEnd = p + data.GetLength();
	for(; p < pEnd; p++)
		if(*p == '\0')
			*p = '\n';

	return TRUE;
}

// �@�\ : �w�肳�ꂽ�Z�N�V�����֎w�肳�ꂽ�f�[�^����������
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
// ���l : �Z�N�V�������̑S�����񂪒u�������܂��B
//		�Ō�̕����� '\n' �ł͖����ꍇ�Z�N�V�����Ƀf�[�^���ǉ�����Ă��܂��܂��B(WritePrivateProfileSection �̓����)
//		�m���ɃZ�N�V�������̑S�f�[�^��ݒ肵�����ꍇ�ɂ͂܂��Z�N�V�����������Ă���ݒ肵�Ă��������B
//
BOOL CIniFile::SetSectionData(
							   LPCSTR pszSection,  // [in] �Z�N�V������
							   const CString& data // [in] �Z�N�V�����ɏ������ޕ�����
							   )
{
	int n = data.GetLength();

	//	������̍Ō�� '\0' ��������ꍇ�� '\0' ��ǉ�����
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
// �@�\ : �����񂩂琮���ւ̕ϊ��R�[���o�b�N�֐��ݒ�
//
void CIniFile::SetStrToIntFunc(
								StrToInt fnc // [in] �֐��|�C���^
								)
{
	m_fncStrToInt = fnc;
}

// �@�\ : �����񂩂�����ւ̕ϊ��R�[���o�b�N�֐��ݒ�
//
void CIniFile::SetStrToDoubleFunc(
								   StrToDouble fnc // [in] �֐��|�C���^
								   )
{
	m_fncStrToDouble = fnc;
}

// �@�\ : �������當����ւ̕ϊ��R�[���o�b�N�֐��ݒ�
//
void CIniFile::SetIntToStrFunc(
								IntToStr fnc // [in] �֐��|�C���^
								)
{
	m_fncIntToStr = fnc;
}

// �@�\ : �������當����ւ̕ϊ��R�[���o�b�N�֐��ݒ�
//
void CIniFile::SetDoubleToStrFunc(
								   DoubleToStr fnc // [in] �֐��|�C���^
								   )
{
	m_fncDoubleToStr = fnc;
}

// �@�\ : �����񂩂琮���ւ̕ϊ��R�[���o�b�N�֐��擾
//
CIniFile::StrToInt CIniFile::GetStrToIntFunc()
{
	return m_fncStrToInt;
}

// �@�\ : �����񂩂�����ւ̕ϊ��R�[���o�b�N�֐��擾
//
CIniFile::StrToDouble CIniFile::GetStrToDoubleFunc()
{
	return m_fncStrToDouble;
}

// �@�\ : �������當����ւ̕ϊ��R�[���o�b�N�֐��擾
//
CIniFile::IntToStr CIniFile::GetIntToStrFunc()
{
	return m_fncIntToStr;
}

// �@�\ : �������當����ւ̕ϊ��R�[���o�b�N�֐��擾
//
CIniFile::DoubleToStr CIniFile::GetDoubleToStrFunc()
{
	return m_fncDoubleToStr;
}

//------------------------------------------------------------------------------
// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��ݕ�����
//
DWORD CIniFile::GetPrivateProfileString(
					LPCSTR pszSection,	// [in] �Z�N�V����
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszDef,		// [in] �f�t�H���g
					LPSTR pszRetBuf,	// [out] �ǂݍ��ݕ����񂪊i�[
					DWORD nSize			// [in] ������i�[�G���A�T�C�Y
					)
{
	DWORD n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, pszRetBuf, nSize, m_strFile);
	return n;
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����Ƃ��ēǂݍ���
//
// �Ԃ�l : �ǂݍ��ݕ�����
//
CString CIniFile::GetPrivateProfileString(
					LPCSTR pszSection, // [in] �Z�N�V����
					LPCSTR pszKey,     // [in] �L�[
					LPCSTR pszDef      // [in] �f�t�H���g
					)
{
	//	�܂��Œ�T�C�Y�̈ꎞ�o�b�t�@�փ��[�h
	char buf[INITIAL_BUF_SIZE];
	DWORD n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, buf, INITIAL_BUF_SIZE, m_strFile);
	if(n != INITIAL_BUF_SIZE - 1)
		return CString(buf, n); // ������S�̂��ǂݍ��߂��Ȃ烊�^�[��

	//	�o�b�t�@�T�C�Y������Ȃ���΃o�b�t�@�T�C�Y���₵�Ȃ��烍�[�h
	DWORD nSize = INITIAL_BUF_SIZE * 2;
	std::string s; // �ꎞ�o�b�t�@
	while(1)
	{
		s.resize(0); // ��[�T�C�Y�O�ɂ��Ȃ��ƃo�b�t�@�T�C�Y�g�������ۂɊ����f�[�^���R�s�[�����̂Œx���Ȃ�
		s.resize(nSize);
		n = ::GetPrivateProfileString(pszSection, pszKey, pszDef, &s[0], nSize, m_strFile);
		if(n != nSize - 1)
			return CString(s.c_str(), n);
		nSize *= 2;
	}
}

// �@�\ : �w��̃p�X�A�Z�N�V�����A�L�[�ŕ�����Ƃ��ď�������
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
BOOL CIniFile::SetPrivateProfileString(
					LPCSTR pszSection,	// [in] �Z�N�V����
					LPCSTR pszKey,		// [in] �L�[
					LPCSTR pszVal		// [in] ������
					)
{
	BOOL b = ::WritePrivateProfileString(pszSection, pszKey, pszVal, m_strFile);
	return b;
}

// �@�\ : �w�肳�ꂽ�Z�N�V�������̑S�f�[�^�𕶎���Ŏ擾����
//
// �Ԃ�l : �ǂݍ��܂ꂽ������
//
DWORD CIniFile::GetPrivateProfileSection(
	LPCSTR pszSection, // [in] �Z�N�V����
	LPSTR pszRetBuf,   // [out] ���̃o�b�t�@�ɓǂݍ��܂ꂽ�����񂪊i�[�����
	DWORD nSize        // [in] pszRetBuf �o�b�t�@�T�C�Y
	)
{
	return ::GetPrivateProfileSection(pszSection, pszRetBuf, nSize, m_strFile);
}

// �@�\ : �w�肳�ꂽ�Z�N�V�����֎w�肳�ꂽ�f�[�^����������
//
// �Ԃ�l : ����=TRUE  ���s=FALSE
//
// ���l : �Z�N�V�������̑S�����񂪒u�������܂��B
//
BOOL CIniFile::SetPrivateProfileSection(
	LPCTSTR pszSection, // [in] �Z�N�V����
	LPCTSTR pszData     // [in] �ݒ肷��f�[�^
	)
{
	return ::WritePrivateProfileSection(pszSection, pszData, m_strFile);
}

// �@�\ : �w�肳�ꂽ�������ϊ��}�b�v�ŕϊ�����
// 
// �Ԃ�l : �ϊ���̕�����
//
CString CIniFile::ConvertByMap(
	CString sStr // [in] �ϊ����镶����
)
{
	for(int i = 0, n = (int)m_Map.GetCount(); i < n; i++)
	{
		if(m_Map[i].sKey == sStr)
			return m_Map[i].sVal;
	}
	return sStr;
}

// �@�\ : �w�肳�ꂽ�������ϊ��}�b�v�ŋt�ϊ�����
// 
// �Ԃ�l : �ϊ���̕�����
//
CString CIniFile::InvConvertByMap(
	CString sStr // [in] �ϊ����镶����
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
// �@�\ : INI�t�@�C����CSV�`���̕�������J���}�ŕ�������
//
// ���l : �_�u���N�H�[�e�[�V����(")�Ŋ����Ă��钆�̃J���}(,)�͋�؂蕶����Ɣ��f����Ȃ��B
//		Win32API �� GetPrivateProfileString �͈�ԊO���̃_�u���N�H�[�e�[�V����������ɊO���Ă��܂��B
//			��) "abc" �� abc
//			��) "a","b","c" �� a","b","c
//
//		���S�ł͂Ȃ����ꉞ���̂悤�ȃp�^�[���Ő��������삷��悤�ɂȂ��Ă���B
//			a","b","c�� a �� b �� c �ɕ���
//
void CIniFile::CsvSplit(
			  LPCSTR pszText,         // [in] �������镶����
			  CArray<CString>& fields // [out] �������ꂽ�����񂪕Ԃ�
			  )
{
	int i, start, end;
	bool bQuoteStart, bQuoteEnd;
	int len = (int)strlen(pszText);
	int term = len + 1;
	int delim = m_delimiter;

	int iField;

	//	�����񂩂�t�B�[���h�����o���Ă���
	i = 0;
	iField = 0;
	fields.SetSize(0);

	if(pszText[0] == '\0')
		return; // �����񂪋�Ȃ�t�B�[���h��0

	while (i < term)
	{
		//	�t�B�[���h�̐擪��T��
		while (i < len && (pszText[i] == ' ' || pszText[i] == '\t'))
		{
			i++;
		}

		//	�_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
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

		//	�t�B�[���h�̏I�[��T��
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

				//	" �̌�� , �������當���̏I�[���Ɣ��f
				//	(�p�x�̕b(")���o�Ă���ƑΏ��ł��Ȃ����AINI�t�@�C���̎d�l��ǂ����悤���Ȃ�)
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
					//	" �Ŋ����Ă���ꍇ�� , �͋�؂�L���Ƃ݂͂Ȃ��Ȃ�
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

		//	�t�B�[���h�z��Ƀt�B�[���h��ǉ�
		fields.SetAtGrow(iField, CString(pszText + start, end - start));
		iField++;
	}
}

// �@�\ : INI�t�@�C����CSV�`���̕�������J���}�ŕ�������
//
// ���l : �_�u���N�H�[�e�[�V����(")�Ŋ����Ă��钆�̃J���}(,)�͋�؂蕶����Ɣ��f����Ȃ��B
//		Win32API �� GetPrivateProfileString �͈�ԊO���̃_�u���N�H�[�e�[�V����������ɊO���Ă��܂��B
//			��) "abc" �� abc
//			��) "a","b","c" �� a","b","c
//
//		���S�ł͂Ȃ����ꉞ���̂悤�ȃp�^�[���Ő��������삷��悤�ɂȂ��Ă���B
//			a","b","c�� a �� b �� c �ɕ���
//
// �Ԃ�l : �����ɂ���č쐬���ꂽ�t�B�[���h��
//
DWORD CIniFile::CsvSplit(
			  LPCSTR pszText,   // [in] �������镶����
			  CString* pFields, // [out] �������ꂽ�����񂪕Ԃ�
			  int nFieldsMax    // [in] �ő�t�B�[���h��
			  )
{
	if(nFieldsMax <= 0)
		return 0;
	if(pszText[0] == '\0')
		return 0; // �����񂪋�Ȃ�t�B�[���h��0

	int i, start, end;
	bool bQuoteStart, bQuoteEnd;
	int len = (int)strlen(pszText);
	int term = len + 1;
	int delim = m_delimiter;

	int iField;

	//	�����񂩂�t�B�[���h�����o���Ă���
	i = 0;
	iField = 0;

	while (i < term)
	{
		//	�t�B�[���h�̐擪��T��
		while (i < len && (pszText[i] == ' ' || pszText[i] == '\t'))
		{
			i++;
		}

		//	�_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
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

		//	�t�B�[���h�̏I�[��T��
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

				//	" �̌�� , �������當���̏I�[���Ɣ��f
				//	(�p�x�̕b(")���o�Ă���ƑΏ��ł��Ȃ����AINI�t�@�C���̎d�l��ǂ����悤���Ȃ�)
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
					//	" �Ŋ����Ă���ꍇ�� , �͋�؂�L���Ƃ݂͂Ȃ��Ȃ�
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

		//	�t�B�[���h�z��Ƀt�B�[���h��ǉ�
		pFields[iField] = CString(pszText + start, end - start);
		iField++;
		if(nFieldsMax <= iField)
			break;
	}

	return iField;
}

//------------------------------------------------------------------------------
// �@�\ : �w�肳�ꂽCSV�`�������񂩂當����z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//			�ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString,    // [in] CSV�`���̕�����
									CArray<CString>& val // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									)
{
	CsvSplit(pszString, val);
	int n = (int)val.GetCount();
	for(int i = 0; i < n; i++)
		val[i] = ConvertByMap(val[i]);
	return n;
}

// �@�\ : �w�肳�ꂽCSV�`�������񂩂�����l�z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString,   // [in] CSV�`���̕�����
									CArray<double>& val	// [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToDouble(strs[i]);
	return n;
}

// �@�\ : �w�肳�ꂽCSV�`�������񂩂琮���l�z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV�`���̕�����
									CArray<int>& val  // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToInt(strs[i]);
	return n;
}

// �@�\ : �w�肳�ꂽCSV�`�������񂩂琮���l�z����擾����A������ł�1�x�[�X�\�� int �^�l�ł�0�x�[�X�ƂȂ�
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromStringOrg1(
									LPCSTR pszString, // [in] CSV�`���̕�����
									CArray<int>& val  // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									)
{
	CArray<CString> strs;
	int n = GetArrayFromString(pszString, strs);
	val.SetSize(n);
	for(int i = 0; i < n; i++)
		val[i] = ToInt(strs[i]) - 1;
	return n;
}

// �@�\ : �w�肳�ꂽCSV�`�������񂩂當����z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : KEY=a,b,c,d  �̒l�̒l��ǂݍ��ނ� "a" , "b" , "c" , "d" �̔z�񂪕Ԃ�܂��B
//			�ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV�`���̕�����
									CString* pVal,    // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									int nVal          // [in] pVal �̍ő�v�f��
									)
{
	int n = CsvSplit(pszString, pVal, nVal);
	for(int i = 0; i < n; i++)
		pVal[i] = ConvertByMap(pVal[i]);
	return n;
}

// �@�\ : �w�肳�ꂽCSV�`�������񂩂�����l�z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV�`���̕�����
									double* pVal,     // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									int nVal          // [in] pVal �̍ő�v�f��
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

// �@�\ : �w�肳�ꂽCSV�`�������񂩂琮���l�z����擾����
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromString(
									LPCSTR pszString, // [in] CSV�`���̕�����
									int* pVal,        // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									int nVal          // [in] pVal �̍ő�v�f��
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

// �@�\ : �w�肳�ꂽCSV�`�������񂩂琮���l�z����擾����A������ł�1�x�[�X�\�� int �^�l�ł�0�x�[�X�ƂȂ�
//
// �Ԃ�l : �ǂݍ��݃f�[�^��
//
// ���l : �ϊ��}�b�v���K�p����܂��B
//
DWORD CIniFile::GetArrayFromStringOrg1(
									LPCSTR pszString, // [in] CSV�`���̕�����
									int* pVal,        // [out] �t�B�[���h���ɕ������ꂽ�f�[�^���Ԃ�
									int nVal          // [in] pVal �̍ő�v�f��
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
// �@�\ : �w�肳�ꂽ������z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,          // [out] CSV�`���̕����񂪕Ԃ�
								  const CArray<CString>& val // [in] ������z��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,         // [out] CSV�`���̕����񂪕Ԃ�
								  const CArray<double>& val	// [in] �����l�z��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,      // [out] CSV�`���̕����񂪕Ԃ�
								  const CArray<int>& val // [in] �����l�z��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����Aint �^�l�ł�0�x�[�X������ł�1�x�[�X�\���ƂȂ�
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToStringOrg1(
								  CString& sString,      // [out] CSV�`���̕����񂪕Ԃ�
								  const CArray<int>& val // [in] �����l�z��
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

// �@�\ : �w�肳�ꂽ������z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,    // [out] CSV�`���̕����񂪕Ԃ�
								  const CString* pVal, // [in] ������z��
								  int nVal             // [in] �z��̗v�f��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString,   // [out] CSV�`���̕����񂪕Ԃ�
								  const double* pVal, // [in] �����l�z��
								  int nVal			  // [in] �z��̗v�f��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToString(
								  CString& sString, // [out] CSV�`���̕����񂪕Ԃ�
								  const int* pVal,	// [in] �����l�z��
								  int nVal			// [in] �z��̗v�f��
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

// �@�\ : �w�肳�ꂽ�����l�z���CSV�`��������ɕϊ�����Aint �^�l�ł�0�x�[�X������ł�1�x�[�X�\���ƂȂ�
//
// �Ԃ�l : �������񂾗v�f��
//
DWORD CIniFile::SetArrayToStringOrg1(
								  CString& sString, // [out] CSV�`���̕����񂪕Ԃ�
								  const int* pVal,	// [in] �����l�z��
								  int nVal			// [in] �z��̗v�f��
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
// �@�\ : GetSectionData �Ŏ擾�����f�[�^����P�s�擾����
//
// �Ԃ�l : TRUE=���� FALSE=���s
//
BOOL CIniFile::GetLineFromSectionData(
									   CString& text,         // [out] �P�s���̕����񂪕Ԃ�
									   LPCSTR& pszSectionData // [in,out] �f�[�^�|�C���^�A�P�s�擾�����Ǝ��̍s�̐擪���w���|�C���^���Ԃ�
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
// �@�\ : ������𐮐��l�ɕϊ�����
//
// �Ԃ�l : �ϊ����������l
//
int CIniFile::ToInt(
					LPCSTR pszStr			// [in] ������
					)
{
	if(m_fncStrToInt != NULL)
		return m_fncStrToInt(pszStr);
	else
		return strtol(pszStr, NULL, 0);
}

// �@�\ : ������������l�ɕϊ�����
//
// �Ԃ�l : �ϊ����������l
//
double CIniFile::ToDouble(
					LPCSTR pszStr			// [in] ������
					)
{
	if(m_fncStrToDouble != NULL)
		return m_fncStrToDouble(pszStr);
	else
		return atof(pszStr);
}

// �@�\ : �����l�𕶎���ɕϊ�����
//
// �Ԃ�l : �ϊ�����������
//
CString CIniFile::ToStr(
					int val				// [in] �����l
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

// �@�\ : �����l�𕶎���ɕϊ�����
//
// �Ԃ�l : �ϊ�����������
//
CString CIniFile::ToStr(
					double val				// [in] �����l
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

// �@�\ : INI �t�@�C���֎����l���������ލۂ̗L�������̐ݒ�
// 
void CIniFile::SetDigits(
		int nVal // [in] �ݒ肷��l
	)
{
	m_nDigits = nVal;
}

// �@�\ : INI �t�@�C���֎����l���������ލۂ̗L�������̎擾
// 
// �Ԃ�l : INI �t�@�C���֎����l���������ލۂ̗L������
// 
int CIniFile::GetDigits() const
{
	return m_nDigits;
}

// �@�\ : �}�b�v�ǉ�
// 
// ���l : INI����l�ǂ݂��ݎ��ɃL�[�Ɉ�v������̂���������w�肳�ꂽ�l�ɕϊ����邽�߂̃}�b�v��ǉ�����B
//
void CIniFile::AddMap(
	const CString& sValInIni, // [in] INI��ł̒l
	const CString& sValOnMem // [in] �������ǂݍ��ݎ��̒l
)
{
	MapItem item;
	item.sKey = sValInIni;
	item.sVal = sValOnMem;
	m_Map.Add(item);
}

// �@�\ : �}�b�v�N���A
// 
void CIniFile::ClearMap()
{
	m_Map.RemoveAll();
}

//==============================================================================
//		INI�t�@�C�����e�̓ǂ݂��݂ƕۑ��̊�{�N���X(RFC2004 �\�[�X�̌݊����̂��߂ɒǉ� 2009.03.16 T.Ishikawa)
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

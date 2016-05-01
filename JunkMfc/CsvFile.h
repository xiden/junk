#pragma once
#include <afx.h>
#include "Csv.h"

//! CSV�t�@�C���A�N�Z�X�p�N���X
//! @remarks �}���`�o�C�g�A���C�h�������Ή�
class CsvFile : public CStdioFile
{
public:
	//! Open() ���\�b�h�ɓn���t�@�C���I�[�v�����[�h
	enum OpenFlags {
		csvRead = CStdioFile::modeRead | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV�ǂݍ��݃��[�h
		csvWrite = CStdioFile::modeWrite | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV�������݃��[�h
		csvReadWrite = CStdioFile::modeReadWrite | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV�ǂݏ������[�h
		csvCreate = csvWrite | CStdioFile::modeCreate | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV�쐬���[�h
	};


	CsvFile(); //!< �R���X�g���N�^
	CsvFile(LPCTSTR lpszFileName, UINT nOpenFlags); //!< �R���X�g���N�^�A�t�@�C���I�[�v�����ď���������

	//! �t�@�C������P�s��ǂݍ���
	virtual BOOL ReadRow(
		std::vector<CStringA>& fields //!< [out] �ǂݍ��܂ꂽ�t�B�[���h�z�񂪕Ԃ�
	);

	//! �t�@�C������P�s��ǂݍ���
	virtual BOOL ReadRow(
		std::vector<CStringW>& fields //!< [out] �ǂݍ��܂ꂽ�t�B�[���h�z�񂪕Ԃ�
	);

	//! �t�@�C���ւP�s����������
	virtual void WriteRow(
		const std::vector<CStringA>& fields //!< [in] ��������Ă��镶����
	);

	//! �t�@�C���ւP�s����������
	virtual void WriteRow(
		const std::vector<CStringW>& fields //!< [in] ��������Ă��镶����
	);

	//! ��؂蕶����ݒ肷��
	void SetSeparator(wchar_t sp) {
		m_Separator = sp;
	}

	//! ���蕶����ݒ肷��
	void SetBundler(wchar_t bd) {
		m_Bundler = bd;
	}

protected:
	CString m_sRow; //!< �Ō�ɏ��������s������A�������̈�g���܂킷���߂ɗp��
	CStringA m_sRowA; //!< �Ō�ɏ��������s������A�������̈�g���܂킷���߂ɗp��
	CStringW m_sRowW; //!< �Ō�ɏ��������s������A�������̈�g���܂킷���߂ɗp��
	wchar_t m_Separator; //!< ��؂蕶��
	wchar_t m_Bundler; //!< ���蕶���A'\0'���w�肳���Ɗ��蕶������͍s���Ȃ�
};


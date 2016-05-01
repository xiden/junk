#pragma once
#ifndef __CSV_H__
#define __CSV_H__

#include <afxstr.h>
#include <vector>
#include "Cvt.h"

//! CSV�����w���p�N���X
//! @remarks �}���`�o�C�g�A���C�h�������Ή�
class Csv {
public:
	//! �w���؂蕶���Ńt�B�[���h��؂蕪���Ď擾����
	static void Split(
		const CStringA& text, //!< [in] �؂�o������������
		char separator, //!< [in] ��؂蕶��
		char bundler, //!< [in] ���蕶��
		std::vector<CStringA>& fields //!< [out] ������̃t�B�[���h�z�񂪕Ԃ�
	);

	//! �w���؂蕶���Ńt�B�[���h��؂蕪���Ď擾����
	static void Split(
		const CStringW& text, //!< [in] �؂�o������������
		wchar_t separator, //!< [in] ��؂蕶��
		wchar_t bundler, //!< [in] ���蕶��
		std::vector<CStringW>& fields //!< [out] ������̃t�B�[���h�z�񂪕Ԃ�
	);


	//! �w���؂蕶���Ńt�B�[���h��A�����Ď擾����
	static void Combine(
		const std::vector<CStringA>& fields, //!< [in] ���ʂ̕������ꂽ�����񂪒ǉ������z��
		wchar_t separator, //!< [in] ��؂蕶��
		wchar_t bundler, //!< [in] ���蕶��
		CStringA& text //!< [out] �A����̕�����
	);

	//! �w���؂蕶���Ńt�B�[���h��A�����Ď擾����
	static void Combine(
		const std::vector<CStringW>& fields, //!< [in] ���ʂ̕������ꂽ�����񂪒ǉ������z��
		wchar_t separator, //!< [in] ��؂蕶��
		wchar_t bundler, //!< [in] ���蕶��
		CStringW& text //!< [out] �A����̕�����
	);

	//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
	static CString GetField(
		const std::vector<CStringW>& fields, //!< [in] �z��
		int iIndex, //!< �z����̃C���f�b�N�X
		LPCTSTR pszDef = NULL //!< �f�t�H���g�l
	);

	//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
	static CString GetField(
		const std::vector<CStringA>& fields, //!< [in] �z��
		int iIndex, //!< �z����̃C���f�b�N�X
		LPCTSTR pszDef = NULL //!< �f�t�H���g�l
	);

	//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
	template<class T>
	static T GetField(
		const std::vector<CStringW>& fields, //!< [in] �z��
		int iIndex, //!< �z����̃C���f�b�N�X
		T def = T() //!< �f�t�H���g�l
	) {
		CString s = GetField(fields, iIndex);
		if (s.IsEmpty())
			return def;
		return FromStr<T>(s);
	}

	//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
	template<class T>
	static T GetField(
		const std::vector<CStringA>& fields, //!< [in] �z��
		int iIndex, //!< �z����̃C���f�b�N�X
		T def = T() //!< �f�t�H���g�l
	) {
		CString s = GetField(fields, iIndex);
		if (s.IsEmpty())
			return def;
		return FromStr<T>(s);
	}
};

#endif

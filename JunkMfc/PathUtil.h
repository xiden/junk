#pragma once
#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

#include <afxstr.h>
#include <Windows.h>

//! �p�X���֌W�̃w���p�������܂Ƃ߂��N���X
class PathUtil {
public:
	static CString GetExeDir(); //!< EXE�t�@�C��������f�B���N�g���p�X���̎擾
	static CString Combine(LPCTSTR pszDir, LPCTSTR pszFile); //!< �t�@�C������A������
	static CString GetParent(LPCTSTR pszFilePath); //!< �e�f�B���N�g������A������
	static CString GetFileName(LPCTSTR pszFilePath); //!< �t�@�C���p�X������t�@�C�������擾����
	static CString GetFileTitle(LPCTSTR pszFilePath); //!< �t�@�C���p�X������g���q��菜�����t�@�C�����������擾����
	static CString RemoveExt(LPCTSTR pszFile); //!< �g���q����菜��
	static CString RenameExt(LPCTSTR pszFile, LPCTSTR pszNewExt); //!< �g���q��ύX����
};

#endif

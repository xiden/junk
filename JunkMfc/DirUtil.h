#pragma once
#ifndef __DIRUTIL_H__
#define __DIRUTIL_H__

#include <afxstr.h>
#include <Windows.h>

//! �f�B���N�g���֌W�̃w���p�������܂Ƃ߂��N���X
class DirUtil {
public:
	static bool Exists(LPCTSTR pszDir); //!< �w�肳�ꂽ�f�B���N�g�������݂��邩���ׂ�
	static void Create(LPCTSTR pszDir); //!< �w�肳�ꂽ�f�B���N�g�����쐬����A���s�������O����������
};

#endif

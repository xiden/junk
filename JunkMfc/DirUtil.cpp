#include "stdafx.h"
#include "DirUtil.h"
#include <afx.h>
#include "Error.h"
#include "WinApiException.h"


//! �w�肳�ꂽ�f�B���N�g�������݂��邩���ׂ�
bool DirUtil::Exists(
	LPCTSTR pszDir //!< [in] �f�B���N�g���p�X��
) {
	DWORD a;
	a = ::GetFileAttributes(pszDir);
	if (a == DWORD(-1))
		return FALSE;
	return a & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

//! �w�肳�ꂽ�f�B���N�g�����쐬����A���s�������O����������
void DirUtil::Create(
	LPCTSTR pszDir //!< [in] �f�B���N�g���p�X��
) {
	if (!::CreateDirectory(pszDir, NULL)) {
		throw new WinApiException(::GetLastError(), pszDir);
	}
}

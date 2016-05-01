#include "stdafx.h"
#include "PathUtil.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


//! EXE�t�@�C��������f�B���N�g���p�X���̎擾
CString PathUtil::GetExeDir() {
	TCHAR buf[MAX_PATH];
	::GetModuleFileName(NULL, buf, MAX_PATH);
	::PathRemoveFileSpec(buf);
	return buf;
}

//! �t�@�C������A������
CString PathUtil::Combine(
	LPCTSTR pszDir, //!< [in] �f�B���N�g���p�X��
	LPCTSTR pszFile //!< [in] �t�@�C����
) {
	TCHAR buf[MAX_PATH];
	::PathCombine(buf, pszDir, pszFile);
	return buf;
}

//! �e�f�B���N�g������A������
CString PathUtil::GetParent(LPCTSTR pszFilePath) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFilePath);
	::PathRemoveFileSpec(buf);
	return buf;
}

//! �t�@�C���p�X������t�@�C�������擾����
CString PathUtil::GetFileName(LPCTSTR pszFilePath) {
	return ::PathFindFileName(pszFilePath);
}

//! �t�@�C���p�X������g���q��菜�����t�@�C�����������擾����
CString PathUtil::GetFileTitle(LPCTSTR pszFilePath) {
	return RemoveExt(::PathFindFileName(pszFilePath));
}

//! �g���q����菜��
CString PathUtil::RemoveExt(LPCTSTR pszFile) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFile);
	::PathRemoveExtension(buf);
	return buf;
}

//! �g���q��ύX����
CString PathUtil::RenameExt(LPCTSTR pszFile, LPCTSTR pszNewExt) {
	TCHAR buf[MAX_PATH];
	_tcscpy_s(buf, sizeof(buf), pszFile);
	::PathRenameExtension(buf, pszNewExt);
	return buf;
}

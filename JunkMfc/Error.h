#pragma once
#ifndef __ERROR_H_
#define __ERROR_H_

#include <afxstr.h>

//! �G���[�����w���p�N���X
class Error {
public:
	//! �G���[�R�[�h�𕶎��񉻂���
	static CString MessageFromWinApiErrorCode(DWORD errorCode);
	//! �Ō�ɔ��������G���[�̃G���[�R�[�h�𕶎��񉻂���
	static CString MessageFromWinApiLastError();
};

#endif

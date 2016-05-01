#pragma once
#ifndef __WNDUTIL_H__
#define __WNDUTIL_H__

#include <afx.h>
#include <Windows.h>

//! �E�B���h�E�֌W�̃w���p�������܂Ƃ߂��N���X
class CWndUtil
{
public:
	static void SetCloseButtonEnable(HWND hwnd, BOOL bEnable); //!< �E��̂��}�[�N�̕���{�^���̗L�����܂��͖�����
	static void SetMinimizeButtonEnable(HWND hwnd, BOOL bEnable); //!< �ŏ����{�^���̗L�����܂��͖�����
	static void SetMaximizeButtonEnable(HWND hwnd, BOOL bEnable); //!< �ő剻�{�^���̗L�����܂��͖�����
	static void SetShowIcon(HWND hwnd, BOOL bVisible); //!< �^�C�g���o�[���[�̃A�C�R���̕\���ݒ�

	static BOOL SetClipboardText(HWND hWnd, LPCTSTR pszText); //!< �N���b�v�{�[�h�փe�L�X�g��\��t����
	static CString GetClipboardText(HWND hWnd); //!< �N���b�v�{�[�h����e�L�X�g���擾����

};

#endif

#pragma once
#ifndef __CTLLAYOUTER_H__
#define __CTLLAYOUTER_H__

#include <afxwin.h>
#include <vector>


//! �E�B���h�E�T�C�Y�ɍ��킹�ăR���g���[�����Ĕz�u�������s���N���X
class CCtlLayouter {
public:
	//! �R���g���[���A���J�[
	enum Anchor {
		Left = 1 << 0, //!< �R���g���[�����̃X�y�[�X���ێ�����
		Top = 1 << 1, //!< �R���g���[����̃X�y�[�X���ێ�����
		Right = 1 << 2, //!< �R���g���[���E�̃X�y�[�X���ێ�����
		Bottom = 1 << 3, //!< �R���g���[�����̃X�y�[�X���ێ�����
		WidthTrace = Left | Right, //!< ���E�̃X�y�[�X���ێ�����
		HeightTrace = Top | Bottom, //!< �㉺�̃X�y�[�X���ێ�����
		SizeTrace = WidthTrace | HeightTrace, //!< �㉺���E�̃X�y�[�X���ێ�����

		NoRedraw = 1 << 31, //!< �R���g���[���Ĕz�u���ɍĕ`�悵�Ȃ�
		Redraw = 1 << 30, //!< �R���g���[���Ĕz�u���ɍĕ`�悷��
		Invalidate = 1 << 29, //!< �R���g���[���Ĕz�u���ɖ����̈��ݒ肷��
	};

	CCtlLayouter();
	~CCtlLayouter();

	void Initialize(CWnd* pParent, DWORD defaultAnchorFlags = 0); //!< �R���g���[���e�E�B���h�E���w�肵�ď���������
	void Add(CWnd* pCtl, DWORD anchorFlags); //!< �R���g���[����ǉ�����
	void OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam); //!< �R���g���[���e�E�B���h�E�̃��b�Z�[�W�������ɌĂяo��

protected:
	//! �R���g���[���ƃA���J�[���
	struct Item {
		CWnd* pCtl; //!< �R���g���[��
		long Left; //!< �R���g���[�����̃X�y�[�X(px)
		long Top; //!< �R���g���[����̃X�y�[�X(px)
		long Right; //!< �R���g���[���E�̃X�y�[�X(px)
		long Bottom; //!< �R���g���[�����̃X�y�[�X(px)
		DWORD AnchorFlags; //!< �A���J�[�t���O�AAnchor::* �̑g�ݍ��킹
	};

	CWnd* m_pParent; //!< �R���g���[���̐e�E�B���h�E
	DWORD m_DefaultAnchorFlags; //!< �f�t�H���g�A���J�[�t���O�A�e�R���g���[���̃A���J�[�t���O�����w��̏ꍇ�ɂ��̒l���ݒ肳���
	CSize m_InitialParentClientSize; //!< �R���g���[���̐e�E�B���h�E�̏����T�C�Y
	std::vector<Item> m_Items; //!< �R���g���[���ƃA���J�[���z��
	BOOL m_bNeedLayout; //!< WM_PAINT �ł̍Ĕz�u���K�v���ǂ���

protected:
	Item CreateItem(CWnd* pCtl, DWORD anchorFlags); //!< �w��R���g���[���ɑΉ�����A���J�[�����쐬����
};

#endif

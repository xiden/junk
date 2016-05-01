#include "stdafx.h"
#include "CtlLayouter.h"


CCtlLayouter::CCtlLayouter() {
	m_pParent = NULL;
	m_DefaultAnchorFlags = 0;
	m_bNeedLayout = false;
}


CCtlLayouter::~CCtlLayouter() {
}

//! �R���g���[���e�E�B���h�E���w�肵�ď���������
//! @remarks �e�E�B���h�E�T�C�Y�����\�[�X�f�U�C�����̃T�C�Y����ύX�����O�ɌĂяo���Ȃ���΂Ȃ�Ȃ��B
void CCtlLayouter::Initialize(
	CWnd* pParent, //!< [in] �R���g���[���e�E�B���h�E
	DWORD defaultAnchorFlags //!< [in] �f�t�H���g�A���J�[�t���O�A�e�R���g���[���̃A���J�[�t���O�����w��̏ꍇ�ɂ��̒l���ݒ肳���
	) {
	m_pParent = pParent;
	m_DefaultAnchorFlags = defaultAnchorFlags;
	m_Items.clear();

	// �e�E�B���h�E���烊�\�[�X�f�U�C�����̃T�C�Y���擾����
	CScrollView* pScrollView = DYNAMIC_DOWNCAST(CScrollView, m_pParent);
	if (pScrollView != NULL) {
		// HACK: �e���t�H�[���r���[�̏ꍇ�ɂ� OnInitialUpdate() �̃^�C�~���O�ł͊��Ƀ��\�[�X�f�U�C�����̃T�C�Y����ύX����Ă���̂Ő�p�̃��\�b�h�Ŏ擾����
		int nMapMode;
		CSize sizeTotal, sizePage, sizeLine;
		pScrollView->GetDeviceScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
		m_InitialParentClientSize = sizeTotal;
	} else {
		// ����̃T�C�Y�������T�C�Y�Ƃ��Ď擾����
		CRect rc;
		m_pParent->GetClientRect(&rc);
		m_InitialParentClientSize = CSize(rc.Width(), rc.Height());
	}
}

//! �R���g���[����ǉ�����
void CCtlLayouter::Add(
	CWnd* pCtl, //!< [in] �R���g���[��
	DWORD anchorFlags //!< [in] �A���J�[�t���O�AAnchor::* �̑g�ݍ��킹
	) {
	m_Items.push_back(CreateItem(pCtl, anchorFlags));
}

//! �R���g���[���e�E�B���h�E�̃��b�Z�[�W�������ɌĂяo��
void CCtlLayouter::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam) {
	if (m_pParent == NULL)
		return;

	// �T�C�Y�ύX���ɖ����̈��o�^����
	if (message == WM_SIZE) {
		m_pParent->Invalidate();
		m_bNeedLayout = true;
	}

	// �e�E�B���h�E���T�C�Y�ύX���ꂽ��R���g���[�������炩���ߎw�肳��Ă�����@�ōĔz�u����
	if (message == WM_PAINT && m_bNeedLayout) {
		m_bNeedLayout = false;

		CRect rcClient;
		m_pParent->GetClientRect(&rcClient);

		std::vector<Item>::iterator it = m_Items.begin();
		std::vector<Item>::iterator end = m_Items.end();
		while (it != end) {
			CWnd* pCtl = it->pCtl;
			CRect rcCtl, rcCtlNew;

			pCtl->GetWindowRect(&rcCtl);
			m_pParent->ScreenToClient(&rcCtl);

			int w = rcCtl.Width();
			int h = rcCtl.Height();
			DWORD anchorFlags = it->AnchorFlags;

			if (anchorFlags & Anchor::Left) {
				rcCtlNew.left = rcClient.left + it->Left;
			} else {
				rcCtlNew.left = rcClient.right - it->Right - w;
			}

			if (anchorFlags & Anchor::Right) {
				rcCtlNew.right = rcClient.right - it->Right;
			} else {
				rcCtlNew.right = rcClient.left + it->Left + w;
			}

			if (anchorFlags & Anchor::Top) {
				rcCtlNew.top = rcClient.top + it->Top;
			} else {
				rcCtlNew.top = rcClient.bottom - it->Bottom - h;
			}

			if (anchorFlags & Anchor::Bottom) {
				rcCtlNew.bottom = rcClient.bottom - it->Bottom;
			} else {
				rcCtlNew.bottom = rcClient.top + it->Top + h;
			}

			// �R���g���[���z�u���T�C�Y�ύX
			UINT nSwpFlags = SWP_NOZORDER;
			if (it->AnchorFlags & Anchor::NoRedraw)
				nSwpFlags |= SWP_NOREDRAW;
			pCtl->SetWindowPos(NULL, rcCtlNew.left, rcCtlNew.top, rcCtlNew.Width(), rcCtlNew.Height(), nSwpFlags);

			if (it->AnchorFlags & Anchor::Invalidate)
				pCtl->Invalidate();
			if (it->AnchorFlags & Anchor::Redraw)
				pCtl->RedrawWindow(); // �T�C�Y�ύX�����ꍇ�ɍĕ`�悳��Ȃ����̂�����̂Ńt���O�ɂ�苭���ĕ`��

			it++;
		}

		//if (!(m_DefaultAnchorFlags & Anchor::NoRedraw))
		//	m_pParent->RedrawWindow();
		//if (m_DefaultAnchorFlags & Anchor::Invalidate)
		//	m_pParent->Invalidate();
	}
}

//! �w��R���g���[���ɑΉ�����A���J�[�����쐬����
CCtlLayouter::Item CCtlLayouter::CreateItem(
	CWnd* pCtl, //!< [in] �R���g���[��
	DWORD anchorFlags //!< [in] �A���J�[�t���O�AAnchor::* �̑g�ݍ��킹
	) {
	CRect rcCtl;
	CRect rcClient = CRect(0, 0, m_InitialParentClientSize.cx, m_InitialParentClientSize.cy);

	pCtl->GetWindowRect(&rcCtl);
	m_pParent->ScreenToClient(&rcCtl);

	if (anchorFlags == 0)
		anchorFlags = m_DefaultAnchorFlags;

	if (!(anchorFlags & (Anchor::Left | Anchor::Right)))
		anchorFlags |= Anchor::Left;
	if (!(anchorFlags & (Anchor::Top | Anchor::Bottom)))
		anchorFlags |= Anchor::Top;

	Item item;
	item.pCtl = pCtl;
	item.Left = rcCtl.left - rcClient.left;
	item.Top = rcCtl.top - rcClient.top;
	item.Right = rcClient.right - rcCtl.right;
	item.Bottom = rcClient.bottom - rcCtl.bottom;
	item.AnchorFlags = anchorFlags;

	return item;
}

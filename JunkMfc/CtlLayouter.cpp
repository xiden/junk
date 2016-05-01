#include "stdafx.h"
#include "CtlLayouter.h"


CCtlLayouter::CCtlLayouter() {
	m_pParent = NULL;
	m_DefaultAnchorFlags = 0;
	m_bNeedLayout = false;
}


CCtlLayouter::~CCtlLayouter() {
}

//! コントロール親ウィンドウを指定して初期化する
//! @remarks 親ウィンドウサイズがリソースデザイン時のサイズから変更される前に呼び出さなければならない。
void CCtlLayouter::Initialize(
	CWnd* pParent, //!< [in] コントロール親ウィンドウ
	DWORD defaultAnchorFlags //!< [in] デフォルトアンカーフラグ、各コントロールのアンカーフラグが未指定の場合にこの値が設定される
	) {
	m_pParent = pParent;
	m_DefaultAnchorFlags = defaultAnchorFlags;
	m_Items.clear();

	// 親ウィンドウからリソースデザイン時のサイズを取得する
	CScrollView* pScrollView = DYNAMIC_DOWNCAST(CScrollView, m_pParent);
	if (pScrollView != NULL) {
		// HACK: 親がフォームビューの場合には OnInitialUpdate() のタイミングでは既にリソースデザイン時のサイズから変更されているので専用のメソッドで取得する
		int nMapMode;
		CSize sizeTotal, sizePage, sizeLine;
		pScrollView->GetDeviceScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
		m_InitialParentClientSize = sizeTotal;
	} else {
		// 現状のサイズを初期サイズとして取得する
		CRect rc;
		m_pParent->GetClientRect(&rc);
		m_InitialParentClientSize = CSize(rc.Width(), rc.Height());
	}
}

//! コントロールを追加する
void CCtlLayouter::Add(
	CWnd* pCtl, //!< [in] コントロール
	DWORD anchorFlags //!< [in] アンカーフラグ、Anchor::* の組み合わせ
	) {
	m_Items.push_back(CreateItem(pCtl, anchorFlags));
}

//! コントロール親ウィンドウのメッセージ処理時に呼び出す
void CCtlLayouter::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam) {
	if (m_pParent == NULL)
		return;

	// サイズ変更時に無効領域を登録する
	if (message == WM_SIZE) {
		m_pParent->Invalidate();
		m_bNeedLayout = true;
	}

	// 親ウィンドウがサイズ変更されたらコントロールもあらかじめ指定されている方法で再配置する
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

			// コントロール配置＆サイズ変更
			UINT nSwpFlags = SWP_NOZORDER;
			if (it->AnchorFlags & Anchor::NoRedraw)
				nSwpFlags |= SWP_NOREDRAW;
			pCtl->SetWindowPos(NULL, rcCtlNew.left, rcCtlNew.top, rcCtlNew.Width(), rcCtlNew.Height(), nSwpFlags);

			if (it->AnchorFlags & Anchor::Invalidate)
				pCtl->Invalidate();
			if (it->AnchorFlags & Anchor::Redraw)
				pCtl->RedrawWindow(); // サイズ変更無い場合に再描画されないものがあるのでフラグにより強制再描画

			it++;
		}

		//if (!(m_DefaultAnchorFlags & Anchor::NoRedraw))
		//	m_pParent->RedrawWindow();
		//if (m_DefaultAnchorFlags & Anchor::Invalidate)
		//	m_pParent->Invalidate();
	}
}

//! 指定コントロールに対応するアンカー情報を作成する
CCtlLayouter::Item CCtlLayouter::CreateItem(
	CWnd* pCtl, //!< [in] コントロール
	DWORD anchorFlags //!< [in] アンカーフラグ、Anchor::* の組み合わせ
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

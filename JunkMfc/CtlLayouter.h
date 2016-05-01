#pragma once
#ifndef __CTLLAYOUTER_H__
#define __CTLLAYOUTER_H__

#include <afxwin.h>
#include <vector>


//! ウィンドウサイズに合わせてコントロールを再配置処理を行うクラス
class CCtlLayouter {
public:
	//! コントロールアンカー
	enum Anchor {
		Left = 1 << 0, //!< コントロール左のスペースを維持する
		Top = 1 << 1, //!< コントロール上のスペースを維持する
		Right = 1 << 2, //!< コントロール右のスペースを維持する
		Bottom = 1 << 3, //!< コントロール下のスペースを維持する
		WidthTrace = Left | Right, //!< 左右のスペースを維持する
		HeightTrace = Top | Bottom, //!< 上下のスペースを維持する
		SizeTrace = WidthTrace | HeightTrace, //!< 上下左右のスペースを維持する

		NoRedraw = 1 << 31, //!< コントロール再配置時に再描画しない
		Redraw = 1 << 30, //!< コントロール再配置時に再描画する
		Invalidate = 1 << 29, //!< コントロール再配置時に無効領域を設定する
	};

	CCtlLayouter();
	~CCtlLayouter();

	void Initialize(CWnd* pParent, DWORD defaultAnchorFlags = 0); //!< コントロール親ウィンドウを指定して初期化する
	void Add(CWnd* pCtl, DWORD anchorFlags); //!< コントロールを追加する
	void OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam); //!< コントロール親ウィンドウのメッセージ処理時に呼び出す

protected:
	//! コントロールとアンカー情報
	struct Item {
		CWnd* pCtl; //!< コントロール
		long Left; //!< コントロール左のスペース(px)
		long Top; //!< コントロール上のスペース(px)
		long Right; //!< コントロール右のスペース(px)
		long Bottom; //!< コントロール下のスペース(px)
		DWORD AnchorFlags; //!< アンカーフラグ、Anchor::* の組み合わせ
	};

	CWnd* m_pParent; //!< コントロールの親ウィンドウ
	DWORD m_DefaultAnchorFlags; //!< デフォルトアンカーフラグ、各コントロールのアンカーフラグが未指定の場合にこの値が設定される
	CSize m_InitialParentClientSize; //!< コントロールの親ウィンドウの初期サイズ
	std::vector<Item> m_Items; //!< コントロールとアンカー情報配列
	BOOL m_bNeedLayout; //!< WM_PAINT での再配置が必要かどうか

protected:
	Item CreateItem(CWnd* pCtl, DWORD anchorFlags); //!< 指定コントロールに対応するアンカー情報を作成する
};

#endif

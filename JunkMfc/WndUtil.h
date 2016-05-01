#pragma once
#ifndef __WNDUTIL_H__
#define __WNDUTIL_H__

#include <afx.h>
#include <Windows.h>

//! ウィンドウ関係のヘルパ処理をまとめたクラス
class CWndUtil
{
public:
	static void SetCloseButtonEnable(HWND hwnd, BOOL bEnable); //!< 右上のｘマークの閉じるボタンの有効化または無効化
	static void SetMinimizeButtonEnable(HWND hwnd, BOOL bEnable); //!< 最小化ボタンの有効化または無効化
	static void SetMaximizeButtonEnable(HWND hwnd, BOOL bEnable); //!< 最大化ボタンの有効化または無効化
	static void SetShowIcon(HWND hwnd, BOOL bVisible); //!< タイトルバー左端のアイコンの表示設定

	static BOOL SetClipboardText(HWND hWnd, LPCTSTR pszText); //!< クリップボードへテキストを貼り付ける
	static CString GetClipboardText(HWND hWnd); //!< クリップボードからテキストを取得する

};

#endif

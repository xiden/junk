#pragma once
#ifndef __CVT_H__
#define __CVT_H__

#include <Windows.h>
#include <afxstr.h>
#include <atltime.h>
#include <stdlib.h>
#include <tchar.h>

//! 文字列からテンプレートで指定された型への変換
template<class T> T FromStr(
	const TCHAR* s //!< [in] 変換基
);

//! 指定型から文字列への変換
template<class T> CString ToStr(
	T v //!< [in] 変換基
);

//! 文字列から色への変換
inline COLORREF ColorRefFromStr(
	const TCHAR* s //!< [in] 変換基
) {
	COLORREF v;
	if(*s == _T('#')) {
		v = _tcstoul(s + 1, NULL, 16);
	} else {
		v = _tcstoul(s, NULL, 0);
	}
	return ((v & 0xff) << 16) | (v & 0xff00) | ((v & 0xff0000) >> 16);
}

//! 色から文字列への変換
inline CString ColorRefToStr(
	COLORREF v //!< [in] 変換基
) {
	CString s;
	s.Format(_T("#%06x"), ((v & 0xff) << 16) | (v & 0xff00) | ((v & 0xff0000) >> 16));
	return s;
}

template<> inline int FromStr<int>(const TCHAR* s) {
	return _tcstol(s, NULL, 0);
}
template<> inline unsigned int FromStr<unsigned int>(const TCHAR* s) {
	return _tcstoul(s, NULL, 0);
}
template<> inline long FromStr<long>(const TCHAR* s) {
	return _tcstol(s, NULL, 0);
}
template<> inline unsigned long FromStr<unsigned long>(const TCHAR* s) {
	return _tcstoul(s, NULL, 0);
}
template<> inline double FromStr<double>(const TCHAR* s) {
	return _ttof(s);
}
template<> inline CTime FromStr<CTime>(const TCHAR* p) {
	int y = 0, M = 0, d = 0, h = 0, m = 0, s = 0;
	_stscanf_s(p, _T("%d/%d/%d %d:%d:%d"), &y, &M, &d, &h, &m, &s);
	return CTime(y, M, d, h, m, s);
}

template<> inline CString ToStr(int v) {
	CString s;
	s.Format(_T("%d"), v);
	return s;
}
template<> inline CString ToStr(unsigned int v) {
	CString s;
	s.Format(_T("%u"), v);
	return s;
}
template<> inline CString ToStr(long v) {
	CString s;
	s.Format(_T("%l"), v);
	return s;
}
template<> inline CString ToStr(unsigned long v) {
	CString s;
	s.Format(_T("%ul"), v);
	return s;
}
template<> inline CString ToStr(double v) {
	CString s;
	s.Format(_T("%f"), v);
	return s;
}
template<> inline CString ToStr(CTime v) {
	return v.Format(_T("%Y/%m/%d %H:%M:%S"));
}

//! 実数からもっとも近い整数へ変換する（四捨五入）
template<class R, class T> inline R Nint(T s) {
	return s < T(0.5) ? R(s - T(0.5)) : R(s + T(0.5));
}

#endif

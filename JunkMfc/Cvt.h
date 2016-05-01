#pragma once
#ifndef __CVT_H__
#define __CVT_H__

#include <Windows.h>
#include <afxstr.h>
#include <atltime.h>
#include <stdlib.h>
#include <tchar.h>

//! �����񂩂�e���v���[�g�Ŏw�肳�ꂽ�^�ւ̕ϊ�
template<class T> T FromStr(
	const TCHAR* s //!< [in] �ϊ���
);

//! �w��^���當����ւ̕ϊ�
template<class T> CString ToStr(
	T v //!< [in] �ϊ���
);

//! �����񂩂�F�ւ̕ϊ�
inline COLORREF ColorRefFromStr(
	const TCHAR* s //!< [in] �ϊ���
) {
	COLORREF v;
	if(*s == _T('#')) {
		v = _tcstoul(s + 1, NULL, 16);
	} else {
		v = _tcstoul(s, NULL, 0);
	}
	return ((v & 0xff) << 16) | (v & 0xff00) | ((v & 0xff0000) >> 16);
}

//! �F���當����ւ̕ϊ�
inline CString ColorRefToStr(
	COLORREF v //!< [in] �ϊ���
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

//! ������������Ƃ��߂������֕ϊ�����i�l�̌ܓ��j
template<class R, class T> inline R Nint(T s) {
	return s < T(0.5) ? R(s - T(0.5)) : R(s + T(0.5));
}

#endif

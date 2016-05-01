#include "stdafx.h"
#include "NumberFormat.h"
#include <math.h>


//! 有効桁数とレンジからフォーマットクラスを生成する
NumberFormat NumberFormat::FromRange(
	double dRange, //!< [in] レンジ
	int nDigit //!< [in] 有効桁数
	) {
	int n = (int)log10(dRange);
	int nMinDigits = (int)abs(n) + 1; // 最低でも必要な桁数
	NumberFormat nfmt = FromEffectiveDigit(nDigit);

	// 最低でも必要な桁数が指定された有効桁数を超えちゃったら有効桁数を負数にして返す、ToStr() 内で CnvG() を使って文字列化される
	if (nDigit < nMinDigits) {
		nfmt.nFlags |= FlagsE;
		return nfmt;
	}

	// 小数点以下の桁数を計算して返す
	nfmt.nFlags |= FlagsDp;
	nfmt.nDp = nDigit - nMinDigits;
	return nfmt;
}

//! 数値を文字列に変換する
CString NumberFormat::ToStr(double dVal) {
	CString sDp;
	TCHAR bufE[256];
	NumberFormat nfmt = *this;

	if ((nfmt.nFlags & FlagsSecToHMS) != 0) {
		// 秒数を「HH:mm:ss」の様な形式にする
		bool sign = dVal < 0.0;
		__int64 s = abs((__int64)dVal);
		__int64 h = s / 3600;
		int m = abs((int)((s / 60) % 60));
		dVal = fabs(dVal);
		nfmt.nFlags &= ~FlagsSecToHMS;
		if (sign)
			sDp.Format(_T("-%I64d:%d:%s"), h, m, nfmt.ToStr(fmod(dVal, 60.0)));
		else
			sDp.Format(_T("%I64d:%d:%s"), h, m, nfmt.ToStr(fmod(dVal, 60.0)));
		return sDp;
	}

	if ((nfmt.nFlags & (FlagsDp | FlagsE)) == 0) {
		// FlagsDp も FlagsE も指定されていないなら %f にしておく
		sDp.Format(_T("%f"), dVal);
		return sDp;
	}

	if ((nfmt.nFlags & (BYTE)(FlagsDp | FlagsE)) == (BYTE)(FlagsDp | FlagsE)) {
		// FlagsDp と FlagsE 両方指定されている場合には希望有効桁数内で正確に表示できる方法を選ぶ
		if (dVal == 0.0) {
			nfmt.nFlags &= ~FlagsE; // 表示したい値が 0.0 なら小数点以下固定の方がいいはず
		} else {
			double l = log10(fabs(dVal));
			if (0 <= l) {
				// 値が大き過ぎる場合に対処
				int n = (int)l + nfmt.nDp + 1; // 必要桁数
				if (n <= nfmt.nDigit) {
					nfmt.nFlags &= ~FlagsE; // 値が希望有効桁数内に収まるなら小数点以下固定の方がいいはず
				} else {
					if (nfmt.nDigit + nfmt.nDigitExpand < n)
						nfmt.nFlags &= ~FlagsDp; // 値が希望有効桁数+nDigitExpandに収まらないなら指数形式の方がいいはず、それ以外は両方試して文字数少ない方を使用する
				}
			} else {
				// 値が小さすぎる場合に対処
				int n = (int)ceil(-l); // 必要小数点以下桁数
				if (n <= nfmt.nDp) {
					nfmt.nFlags &= ~FlagsE; // 値が小数点以下桁数内に収まるなら小数点以下固定の方がいいはず
				} else {
					if (nfmt.nDp + nfmt.nDpExpand < n)
						nfmt.nFlags &= ~FlagsDp; // 値が小数点以下桁数+nDpExpandに収まらないなら指数形式の方がいいはず
					else
						nfmt.nDp = n; // 値が微妙に小さくて小数点以下桁数に収まらないなら小数点以下桁数の方を増やしてみて文字数少ない方を使用する
				}
			}
		}
	}

	if (nfmt.nFlags & FlagsDp) {
		// 小数点以下の桁数で文字列化する
		TCHAR fmt[32];
		_sntprintf_s(fmt, 31, _T("%%.%df"), nfmt.nDp);
		sDp.Format(fmt, dVal);
		if ((nfmt.nFlags & 3) == FlagsDp)
			return sDp; // FlagsDp のみ指定されている場合はこれが結果となる
	}

	if (nfmt.nFlags & FlagsE) {
		// 指数形式で文字列化する
		TCHAR fmt[32];
		_sntprintf_s(fmt, 31, _T("%%.%dg"), nfmt.nDigit);
		_sntprintf_s(bufE, 255, fmt, dVal);

		TCHAR* p = _tcschr(bufE, '.');
		BOOL needToAddDot = false;
		if (p != NULL) {
			if (p[1] == _T('\0')) {
				// "1." の様に最後が '.' で終わっていたら ".0" に書き換える
				p[1] = _T('0');
				p[2] = _T('\0');
			} else if (p[1] == _T('e')) {
				// "1.e" の様に最後が '.' の直後に 'e' が来ていたら、 ".0e" のように書き換える
				size_t len = _tcslen(p + 1) + 1;
				memmove(p + 2, p + 1, len * sizeof(TCHAR));
				p[1] = _T('0');
			}
		} else {
			needToAddDot = true;
		}

		// "1.0e+001" の様な無駄な '0' を取り除く
		p = _tcschr(bufE, _T('e'));
		if (p != NULL) {
			if (p[1] == _T('+') || p[1] == _T('-')) {
				p += 2;
				TCHAR* s = p;
				while (*p == _T('0'))
					p++;

				size_t n = p - s;
				if (n != 0) {
					size_t len = _tcslen(p) + 1;
					memmove(s, p, len * sizeof(TCHAR));
					int a = 0;
				}
			}
		} else {
			if (needToAddDot)
				_tcscat_s(bufE, sizeof(bufE) / sizeof(bufE[0]), _T(".0"));
		}

		if ((nfmt.nFlags & 3) == FlagsE)
			return bufE; // FlagsE のみ指定されている場合はこれが結果となる
	}

	// 文字数が少ない方を使用する、文字数が同じなら FlagsDp 優先
	if (sDp.GetLength() <= (int)_tcslen(bufE))
		return sDp;
	else
		return bufE;
}

//! 指定範囲内の値を文字列化した際に最大の長さとなる文字列の取得
CString NumberFormat::GetMaxLengthString(double dMin, double dMax) {
	// 最大と最小で文字列が長い方を取得
	CString s = ToStr(dMin);
	CString s2 = ToStr(dMax);
	if (s.GetLength() < s2.GetLength())
		s = s2;

	// 数字部分を8に置き換える
	LPTSTR p = (LPTSTR)(LPCTSTR)s;
	while (*p != _T('\0')) {
		int c = *p;
		if (_T('0') <= c && c <= _T('9'))
			*p = _T('8');
		p++;
	}

	return s;
}

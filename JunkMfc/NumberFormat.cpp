#include "stdafx.h"
#include "NumberFormat.h"
#include <math.h>


//! �L�������ƃ����W����t�H�[�}�b�g�N���X�𐶐�����
NumberFormat NumberFormat::FromRange(
	double dRange, //!< [in] �����W
	int nDigit //!< [in] �L������
	) {
	int n = (int)log10(dRange);
	int nMinDigits = (int)abs(n) + 1; // �Œ�ł��K�v�Ȍ���
	NumberFormat nfmt = FromEffectiveDigit(nDigit);

	// �Œ�ł��K�v�Ȍ������w�肳�ꂽ�L�������𒴂����������L�������𕉐��ɂ��ĕԂ��AToStr() ���� CnvG() ���g���ĕ����񉻂����
	if (nDigit < nMinDigits) {
		nfmt.nFlags |= FlagsE;
		return nfmt;
	}

	// �����_�ȉ��̌������v�Z���ĕԂ�
	nfmt.nFlags |= FlagsDp;
	nfmt.nDp = nDigit - nMinDigits;
	return nfmt;
}

//! ���l�𕶎���ɕϊ�����
CString NumberFormat::ToStr(double dVal) {
	CString sDp;
	TCHAR bufE[256];
	NumberFormat nfmt = *this;

	if ((nfmt.nFlags & FlagsSecToHMS) != 0) {
		// �b�����uHH:mm:ss�v�̗l�Ȍ`���ɂ���
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
		// FlagsDp �� FlagsE ���w�肳��Ă��Ȃ��Ȃ� %f �ɂ��Ă���
		sDp.Format(_T("%f"), dVal);
		return sDp;
	}

	if ((nfmt.nFlags & (BYTE)(FlagsDp | FlagsE)) == (BYTE)(FlagsDp | FlagsE)) {
		// FlagsDp �� FlagsE �����w�肳��Ă���ꍇ�ɂ͊�]�L���������Ő��m�ɕ\���ł�����@��I��
		if (dVal == 0.0) {
			nfmt.nFlags &= ~FlagsE; // �\���������l�� 0.0 �Ȃ珬���_�ȉ��Œ�̕��������͂�
		} else {
			double l = log10(fabs(dVal));
			if (0 <= l) {
				// �l���傫�߂���ꍇ�ɑΏ�
				int n = (int)l + nfmt.nDp + 1; // �K�v����
				if (n <= nfmt.nDigit) {
					nfmt.nFlags &= ~FlagsE; // �l����]�L���������Ɏ��܂�Ȃ珬���_�ȉ��Œ�̕��������͂�
				} else {
					if (nfmt.nDigit + nfmt.nDigitExpand < n)
						nfmt.nFlags &= ~FlagsDp; // �l����]�L������+nDigitExpand�Ɏ��܂�Ȃ��Ȃ�w���`���̕��������͂��A����ȊO�͗��������ĕ��������Ȃ������g�p����
				}
			} else {
				// �l������������ꍇ�ɑΏ�
				int n = (int)ceil(-l); // �K�v�����_�ȉ�����
				if (n <= nfmt.nDp) {
					nfmt.nFlags &= ~FlagsE; // �l�������_�ȉ��������Ɏ��܂�Ȃ珬���_�ȉ��Œ�̕��������͂�
				} else {
					if (nfmt.nDp + nfmt.nDpExpand < n)
						nfmt.nFlags &= ~FlagsDp; // �l�������_�ȉ�����+nDpExpand�Ɏ��܂�Ȃ��Ȃ�w���`���̕��������͂�
					else
						nfmt.nDp = n; // �l�������ɏ������ď����_�ȉ������Ɏ��܂�Ȃ��Ȃ珬���_�ȉ������̕��𑝂₵�Ă݂ĕ��������Ȃ������g�p����
				}
			}
		}
	}

	if (nfmt.nFlags & FlagsDp) {
		// �����_�ȉ��̌����ŕ����񉻂���
		TCHAR fmt[32];
		_sntprintf_s(fmt, 31, _T("%%.%df"), nfmt.nDp);
		sDp.Format(fmt, dVal);
		if ((nfmt.nFlags & 3) == FlagsDp)
			return sDp; // FlagsDp �̂ݎw�肳��Ă���ꍇ�͂��ꂪ���ʂƂȂ�
	}

	if (nfmt.nFlags & FlagsE) {
		// �w���`���ŕ����񉻂���
		TCHAR fmt[32];
		_sntprintf_s(fmt, 31, _T("%%.%dg"), nfmt.nDigit);
		_sntprintf_s(bufE, 255, fmt, dVal);

		TCHAR* p = _tcschr(bufE, '.');
		BOOL needToAddDot = false;
		if (p != NULL) {
			if (p[1] == _T('\0')) {
				// "1." �̗l�ɍŌオ '.' �ŏI����Ă����� ".0" �ɏ���������
				p[1] = _T('0');
				p[2] = _T('\0');
			} else if (p[1] == _T('e')) {
				// "1.e" �̗l�ɍŌオ '.' �̒���� 'e' �����Ă�����A ".0e" �̂悤�ɏ���������
				size_t len = _tcslen(p + 1) + 1;
				memmove(p + 2, p + 1, len * sizeof(TCHAR));
				p[1] = _T('0');
			}
		} else {
			needToAddDot = true;
		}

		// "1.0e+001" �̗l�Ȗ��ʂ� '0' ����菜��
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
			return bufE; // FlagsE �̂ݎw�肳��Ă���ꍇ�͂��ꂪ���ʂƂȂ�
	}

	// �����������Ȃ������g�p����A�������������Ȃ� FlagsDp �D��
	if (sDp.GetLength() <= (int)_tcslen(bufE))
		return sDp;
	else
		return bufE;
}

//! �w��͈͓��̒l�𕶎��񉻂����ۂɍő�̒����ƂȂ镶����̎擾
CString NumberFormat::GetMaxLengthString(double dMin, double dMax) {
	// �ő�ƍŏ��ŕ����񂪒��������擾
	CString s = ToStr(dMin);
	CString s2 = ToStr(dMax);
	if (s.GetLength() < s2.GetLength())
		s = s2;

	// ����������8�ɒu��������
	LPTSTR p = (LPTSTR)(LPCTSTR)s;
	while (*p != _T('\0')) {
		int c = *p;
		if (_T('0') <= c && c <= _T('9'))
			*p = _T('8');
		p++;
	}

	return s;
}

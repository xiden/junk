#pragma once
#ifndef __NUMBERFORMAT_H_
#define __NUMBERFORMAT_H_

#include <afx.h>

#pragma pack(push,1)

//! ���l�����񉻃t�H�[�}�b�g�N���X
struct NumberFormat {
	enum Flags { // nFlags �̒l�A FlagsDp �� FlagsE �����w�肳��Ă���ꍇ�͗������s���ĕ����������Ȃ������g�p�����A�������������Ȃ� FlagsDp �D��
		FlagsDp = 1 << 0, // �����_�ȉ������w��\���A nDp�AnDpExpand ���g�p����
		FlagsE = 1 << 1, // �w���`���\���A nDigits�AnDigitsExpand ���g�p����
		FlagsSecToHMS = 1 << 2, // �b�����uHH:mm:ss�v�̗l�Ȍ`���ɂ���
	};

	BYTE nFlags; // �t���O�AFlags* �̑g�ݍ��킹
	BYTE nDigit; // ��]����L������
	BYTE nDp; // �����_�ȉ�����
	int nDigitExpand : 4; // �l�傫�����Ă��L�������ł͕\��������Ȃ��ꍇ�ɋ����錅�g�����AFlagsDp �� FlagsE �����w�肳�ꂽ�ꍇ�ɂ͂��̒l�͈͓̔��Ŏ������肳���
	int nDpExpand : 4; // �l���������Ă������_�ȉ������ł͕\���ł��Ȃ��ꍇ�ɋ����錅�g�����AFlagsDp �� FlagsE �����w�肳�ꂽ�ꍇ�ɂ͂��̒l�͈͓̔��Ŏ������肳���

	//! �����_�ȉ��̌������琔�l�t�H�[�}�b�g�N���X�𐶐�����
	static inline NumberFormat FromDp(int nDp) {
		NumberFormat fmt;
		fmt.nFlags = FlagsDp;
		fmt.nDigit = 0;
		fmt.nDp = (BYTE)nDp;
		fmt.nDigitExpand = 0;
		fmt.nDpExpand = 0;
		return fmt;
	}

	//! �L���������琔�l�t�H�[�}�b�g�N���X�𐶐�����
	static inline NumberFormat FromEffectiveDigit(int nDigit) {
		NumberFormat fmt;
		fmt.nFlags = FlagsE;
		fmt.nDigit = (BYTE)nDigit;
		fmt.nDp = 0;
		fmt.nDigitExpand = 0;
		fmt.nDpExpand = 0;
		return fmt;
	}

	//! �L�������ƃ����W����t�H�[�}�b�g�N���X�𐶐�����
	static NumberFormat FromRange(double dRange, int nDigit);

	CString ToStr(double dVal); //!< ���l�𕶎���ɕϊ�����
	CString GetMaxLengthString(double dMin, double dMax); //!< �w��͈͓��̒l�𕶎��񉻂����ۂɍő�̒����ƂȂ镶����̎擾
};

#pragma pack(pop)

#endif

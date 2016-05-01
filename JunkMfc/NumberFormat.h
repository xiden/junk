#pragma once
#ifndef __NUMBERFORMAT_H_
#define __NUMBERFORMAT_H_

#include <afx.h>

#pragma pack(push,1)

//! 数値文字列化フォーマットクラス
struct NumberFormat {
	enum Flags { // nFlags の値、 FlagsDp と FlagsE 両方指定されている場合は両方実行して文字数が少ない方が使用される、文字数が同じなら FlagsDp 優先
		FlagsDp = 1 << 0, // 小数点以下桁数指定表示、 nDp、nDpExpand を使用する
		FlagsE = 1 << 1, // 指数形式表示、 nDigits、nDigitsExpand を使用する
		FlagsSecToHMS = 1 << 2, // 秒数を「HH:mm:ss」の様な形式にする
	};

	BYTE nFlags; // フラグ、Flags* の組み合わせ
	BYTE nDigit; // 希望する有効桁数
	BYTE nDp; // 小数点以下桁数
	int nDigitExpand : 4; // 値大きすぎてが有効桁数では表示しきれない場合に許可する桁拡張数、FlagsDp と FlagsE 両方指定された場合にはこの値の範囲内で自動判定される
	int nDpExpand : 4; // 値小さすぎてが小数点以下桁数では表示できない場合に許可する桁拡張数、FlagsDp と FlagsE 両方指定された場合にはこの値の範囲内で自動判定される

	//! 小数点以下の桁数から数値フォーマットクラスを生成する
	static inline NumberFormat FromDp(int nDp) {
		NumberFormat fmt;
		fmt.nFlags = FlagsDp;
		fmt.nDigit = 0;
		fmt.nDp = (BYTE)nDp;
		fmt.nDigitExpand = 0;
		fmt.nDpExpand = 0;
		return fmt;
	}

	//! 有効桁数から数値フォーマットクラスを生成する
	static inline NumberFormat FromEffectiveDigit(int nDigit) {
		NumberFormat fmt;
		fmt.nFlags = FlagsE;
		fmt.nDigit = (BYTE)nDigit;
		fmt.nDp = 0;
		fmt.nDigitExpand = 0;
		fmt.nDpExpand = 0;
		return fmt;
	}

	//! 有効桁数とレンジからフォーマットクラスを生成する
	static NumberFormat FromRange(double dRange, int nDigit);

	CString ToStr(double dVal); //!< 数値を文字列に変換する
	CString GetMaxLengthString(double dMin, double dMax); //!< 指定範囲内の値を文字列化した際に最大の長さとなる文字列の取得
};

#pragma pack(pop)

#endif

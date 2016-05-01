#pragma once
#ifndef __LINEARTRANSFORM_H__
#define __LINEARTRANSFORM_H__
#include "Value.h"

//! 線形変換用クラス、スケーリング値と移動値を持つ
struct LinearTransform {
	double dScale; // スケーリング値
	double dTranslate; // 移動値

	// 機能 : コンストラクタ
	LinearTransform() {
	}

	// 機能 : コンストラクタ　スケーリング値と移動値を指定して初期化
	LinearTransform(
		double s, // [in] スケーリング値
		double t // [in] 移動値
		) {
		dScale = s;
		dTranslate = t;
	}

	// 機能 : コンストラクタ　範囲を２つ指定して初期化、src1 が dst1 に src2 が dst2 になるような変換オブジェクトとなる
	LinearTransform(
		double src1, // [in] 変換元値1
		double src2, // [in] 変換元値2
		double dst1, // [in] 変換先値1
		double dst2 // [in] 変換先値2
		) {
		double s = src2 - src1;
		if (s != 0.0) {
			dScale = (dst2 - dst1) / s;
			dTranslate = dst1 - src1 * dScale;
		} else {
			dScale = 1.0;
			dTranslate = 0.0;
		}
	}

	// 機能 : 正方向で変換
	__forceinline double Cnv(
		double val // [in] 変換元値
		) const {
		return val * dScale + dTranslate;
	}

	// 機能 : 逆方向で変換
	__forceinline double InvCnv(
		double val // [in] 変換元値
		) const {
		return (val - dTranslate) / dScale;
	}

	// 機能 : 線形変換オブジェクト同士を結合する、this(自分) ⇒ transform 引数 の順で変換と同じ結果となる変換オブジェクトを作成する
	LinearTransform Multiply(
		const LinearTransform& transform // [in] 結合する線形変換オブジェクト
		) const {
		return LinearTransform(dScale * transform.dScale, dTranslate * transform.dScale + transform.dTranslate);
	}

	// 機能 : 逆変換を作成
	LinearTransform Invert() const {
		return LinearTransform(1.0 / dScale, -dTranslate / dScale);
	}

	// 機能 : 内容の不一致のチェック
	BOOL operator!=(const LinearTransform& c) const {
		return dScale != c.dScale || dTranslate != c.dTranslate;
	}
};

#endif

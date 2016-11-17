#pragma once
#ifndef __JUNK_GEO_H__
#define __JUNK_GEO_H__

#include "JunkConfig.h"
#include "Vector.h"

_JUNK_BEGIN

namespace Geo {
	//! Pnを開始点としVnを方向ベクトルとする２ラインの最近点のパラメータ計算
	//! @return 最近点が計算できたら0以外が返る
	template<
		class V, //!< VectorN 形式のクラス
		class T //!< 戻りパラメータの型
	> ibool LineNearestParam(
		V p1, //!< [in] ライン１の開始点
		V v1, //!< [in] ライン１の方向ベクトル
		V p2, //!< [in] ライン２の開始点
		V v2, //!< [in] ライン２の方向ベクトル
		T& t1, //!< [out] ライン１のパラメータが返る
		T& t2 //!< [out] ライン２のパラメータが返る
	) {
		auto pv = p2 - p1;
		auto d1 = pv.Dot(v1);
		auto d2 = pv.Dot(v2);
		auto dv = v1.Dot(v2);
		auto v1sq = v1.LengthSquare();
		auto v2sq = v2.LengthSquare();
		auto den = v1sq * v2sq - dv * dv;
		if (den == V::ValueType(0)) {
			t1 = T(0);
			t2 = T(0);
			return false;
		}
		t1 = T((d1 * v2sq - d2 * dv) / den);
		t2 = T((d1 * dv - d2 * v1sq) / den);
		return true;
	}
}

_JUNK_END

#endif

#pragma once
#ifndef __JUNK_GEOMETRY_H__
#define __JUNK_GEOMETRY_H__

#include "JunkConfig.h"
#include "Matrix.h"
#include <vector>
#include <pmmintrin.h>

_JUNK_BEGIN

#ifndef _JUNK_GEOMETRY_INLINE
#define _JUNK_GEOMETRY_INLINE inline
#endif


//! Transform 関数に渡す変換情報
struct TransformInfo;

//==============================================================================
//		テンプレート、インライン関数宣言
//==============================================================================

//! 複数頂点で構成されるラインと指定ポイントとの接触判定を行う
//! @return  接触しているなら接触頂点番号が返る、それ以外は負数が返る
template<
	class T, //!< 値型
	class V1, //!< 頂点配列ベクトル型
	class V2 //!< 指定ポイントベクトル型
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTmpl(
	const V1* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count, //!< [in] 頂点数
	V2 pt, //!< [in] 接触判定ポイント座標
	T vertexRadius, //!< [in] 頂点の半径
	T edgeThickness, //!< [in] 辺の太さ
	T& t //!< [out] 辺と接触した際のパラメータ t が返る(0 なら戻り値の頂点、1なら次の頂点)
) {
	vertexRadius *= vertexRadius;

	if (1 <= count) {
		//	先頭ポイントと頂点の接触判定
		if ((pt - vts[0]).LengthSquare() <= vertexRadius) {
			//	指定ポイントから頂点までの距離が vertexRadius 以下になったら頂点に接触と判定
			t = 0.0;
			return 0;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		//	指定ポイントと頂点の接触判定
		if ((pt - vts[i]).LengthSquare() <= vertexRadius) {
			//	指定ポイントから頂点までの距離が vertexRadius 以下になったら頂点に接触と判定
			t = 0.0;
			return i;
		}

		//	指定ポイントと辺の接触判定
		Vector2d s = vts[i - 1];
		Vector2d v1 = vts[i] - s;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // 頂点が重なってるので辺は存在しない

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - s;
		double x = v1.Dot(v2);
		double y = v1.MakeVertical().Dot(v2);

		if (0.0 <= x && x <= l && -edgeThickness <= y && y <= edgeThickness) {
			//	座標変換後に矩形の範囲内に指定ポイントがあるなら辺に接触と判定
			t = x / l;
			return i - 1;
		}
	}

	return -1;
}

//! 複数頂点で構成されるラインと指定ポイントとの接触判定を行う
//! @return 接触しているなら接触頂点番号が返る、それ以外は負数が返る
template<
	class T, // 値型
	class V1, // 頂点配列ベクトル型
	class V2, // 指定ポイントベクトル型
	class V3 // pointOnEdge のベクトル型
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTmpl(
	const V1* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count, //!< [in] 頂点数
	V2 pt, //!< [in] 接触判定ポイント座標
	T vertexRadius, //!< [in] 頂点の半径
	T edgeThickness, //!< [in] 辺の太さ
	T& t, //!< [out] 辺と接触した際のパラメータ t が返る(0 なら戻り値の頂点、1なら次の頂点)
	V3& pointOnEdge //!< [out] 接触した辺の最近点座標が返る
) {
	vertexRadius *= vertexRadius;

	if (1 <= count) {
		//	先頭ポイントと頂点の接触判定
		if ((pt - vts[0]).LengthSquare() <= vertexRadius) {
			//	指定ポイントから頂点までの距離が vertexRadius 以下になったら頂点に接触と判定
			t = 0.0;
			pointOnEdge = vts[0];
			return 0;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		//	指定ポイントと頂点の接触判定
		if ((pt - vts[i]).LengthSquare() <= vertexRadius) {
			//	指定ポイントから頂点までの距離が vertexRadius 以下になったら頂点に接触と判定
			t = 0.0;
			pointOnEdge = vts[i];
			return i;
		}

		//	指定ポイントと辺の接触判定
		Vector2d s = vts[i - 1];
		Vector2d v1 = vts[i] - s;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // 頂点が重なってるので辺は存在しない

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - s;
		double x = v1.Dot(v2);
		double y = v1.Vertical().Dot(v2);

		if (0.0 <= x && x <= l && -edgeThickness <= y && y <= edgeThickness) {
			//	座標変換後に矩形の範囲内に指定ポイントがあるなら辺に接触と判定
			pointOnEdge = s + v1 * x;
			t = x / l;
			return i - 1;
		}
	}

	return -1;
}

//! 複数頂点で構成される太さを持つラインと円との接触判定を行う、全ての入力座標は座標変換関数TFを通して参照される
//! @return 接触しているなら接触頂点番号が返る、それ以外は負数が返る
template<
	class T, //!< 値型
	class V1, //!< 頂点配列ベクトル型
	class V2, //!< 指定ポイントベクトル型
	class V3, //!< pointOnEdge のベクトル型
	class TF //!< 頂点座標変換関数
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTf(
	int checkEdge, //!< [in] 辺も調べるかどうか
	const V1* vts, //!< [in] 頂点配列先頭ポインタ(座標変換前)
	intptr_t count, //!< [in] 頂点数
	V2 pt, //!< [in] 接触判定ポイント座標(座標変換後)
	T vertexRadius, //!< [in] 頂点の半径(座標変換後)
	T edgeThickness, //!< [in] 辺の太さ(座標変換後)、負数が指定されたら辺のチェックは行われない
	TF tf, //!< [in] 座標変換関数
	T& t, //!< [out] 辺と接触した際のパラメータ t が返る(0 なら戻り値の頂点、1なら次の頂点)
	V3& nearestPt, //!< [out] 最近点座標(座標変換後)が返る
	T& distance2 //!< [out] 最近点との距離の二乗
) {
	vertexRadius *= vertexRadius;

	intptr_t vtindex = -1; // 最も近い頂点のインデックス番号
	intptr_t egindex = -1; // 最も近い辺のの最初の頂点インデックス番号
	T vtdist2; // 最も近い頂点の距離の二乗
	T egdist; // 最も近い辺の距離
	V1 vlt; // 最後に座標変換した頂点座標

	SetMaxVal(vtdist2);
	SetMaxVal(egdist);

	if (edgeThickness < T(0))
		checkEdge = 0;

	if (1 <= count) {
		vlt = tf(vts[0]);

		// 先頭ポイントと頂点の接触判定
		// 指定ポイントから頂点までの距離が vertexRadius 以下になったら候補に追加
		T d2 = (pt - vlt).LengthSquare();
		if (d2 <= vertexRadius) {
			vtindex = 0;
			vtdist2 = d2;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		// 今回の対象頂点を座標変換
		V1 vt = tf(vts[i]);

		// 指定ポイントと頂点の接触判定
		// 指定ポイントから頂点までの距離が vertexRadius 以下になったら候補に追加
		T d2 = (pt - vt).LengthSquare();
		if (d2 <= vertexRadius && d2 < vtdist2) {
			vtindex = i;
			vtdist2 = d2;
		}

		// 既に指定座標が頂点と接触している場合には辺の判定は必要無い
		if (!checkEdge || vtindex != -1)
			continue;

		// 指定ポイントと辺の接触判定
		Vector2d v1 = vt - vlt;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // 頂点が重なってるので辺は存在しない

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - vlt;
		double x = v1.Dot(v2);
		double y = v1.Vertical().Dot(v2);

		// 座標変換後に矩形の範囲内に指定ポイントがあるなら候補に追加
		if (0.0 <= x && x <= l && -edgeThickness <= y && y <= edgeThickness && y < egdist) {
			egindex = i - 1;
			egdist = y;
			nearestPt = vlt + v1 * x;
			t = x / l;
		}

		vlt = vt;
	}

	if (vtindex != -1) {
		t = T(0);
		nearestPt = vts[vtindex];
		distance2 = vtdist2;
		return vtindex;
	} else if (egindex != -1) {
		distance2 = egdist * egdist;
		return egindex;
	}

	return -1;
}

// 機能: 複数頂点で構成されるラインの全ての辺の長さの合計を計算する
//
// 戻り値: 長さ
//
template<
	class V1 // 頂点配列ベクトル型
>
_JUNK_GEOMETRY_INLINE double PolylineLengthTmpl(
	const V1* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count //!< [in] 頂点数
) {
	double l = 0.0;
	for (intptr_t i = 1; i < count; i++)
		l += (vts[i] - vts[i - 1]).Length();
	return l;
}

// 機能: SSE3を使用して複数頂点で構成されるラインの全ての辺の長さの合計を計算する
//
// 戻り値: 長さ
//
_JUNK_GEOMETRY_INLINE double PolylineLengthSSE3(
	const VectorN<double, 2>* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count //!< [in] 頂点数
) {
	if (count <= 1)
		return 0.0;

	__m128d l = { 0.0, 0.0 };
	__m128d v1 = _mm_loadu_pd((double*)&vts[0]);

	intptr_t i;
	for (i = 2; i < count; i += 2) {
		__m128d v2 = _mm_loadu_pd((double*)&vts[i - 1]);
		__m128d v3 = _mm_loadu_pd((double*)&vts[i]);
		__m128d s1 = _mm_sub_pd(v2, v1);
		__m128d s2 = _mm_sub_pd(v3, v2);
		v1 = v3;
		s1 = _mm_mul_pd(s1, s1);
		s2 = _mm_mul_pd(s2, s2);
		l = _mm_add_pd(l, _mm_sqrt_pd(_mm_hadd_pd(s1, s2)));
	}
	l = _mm_hadd_pd(l, l);

	if (count == i) {
		__m128d v2 = _mm_loadu_pd((double*)&vts[count - 1]);
		__m128d s1 = _mm_sub_pd(v2, v1);
		s1 = _mm_mul_pd(s1, s1);
		l = _mm_add_pd(l, _mm_sqrt_pd(_mm_hadd_pd(s1, s1)));
	}

	return l.m128d_f64[0];
}


//==============================================================================
//		インポート関数宣言
//==============================================================================

// 機能 : 最大最小を検索する
//
JUNKAPI void JUNKCALL SearchMaxMin(
	const double* p, //!< [in] 最大最小検索するデータ
	intptr_t n, //!< [in] 検索データ数、0 以下の場合は関数を呼び出さないようにしてください
	double& min, //!< [out] 最小値が返る
	double& max //!< [out] 最大値が返る
);

// 機能 : リングバッファ内から最大最小を検索する
//
JUNKAPI void JUNKCALL SearchMaxMinRing(
	const double* pBuffer, //!< [in] リングバッファの先頭ポインタ
	intptr_t nBufLen, //!< [in] リングバッファのサイズ、0 以下の場合は関数を呼び出さないようにしてください
	intptr_t iIndex, //!< [in] 検索開始位置のインデックス番号
	intptr_t n, //!< [in] 検索データ数、0 以下の場合は関数を呼び出さないようにしてください
	double& min, //!< [out] 最小値が返る
	double& max //!< [out] 最大値が返る
);

// 機能: double 型２次元(X,Y)ベクトル配列を受け取り、XとYそれぞれの最大最小を検索する
//
JUNKAPI void JUNKCALL SearchPointMaxMin(
	const Vector2d* p, //!< [in] 最大最小検索するデータ
	intptr_t n, //!< [in] データ数
	double& minX, //!< [out] 最小X値が返る
	double& minY, //!< [out] 最小Y値が返る
	double& maxX, //!< [out] 最大X値が返る
	double& maxY //!< [out] 最大Y値が返る
);

// 機能 : 線形変換を行う
//
JUNKAPI void JUNKCALL TransformLin(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	double scale, //!< [in] スケーリング値
	double translate, //!< [in] 平行移動値
	double* pDst //!< [out] 変換後のデータ
);

// 機能 : 線形変換後intに変換を行う
//
JUNKAPI void JUNKCALL TransformLinInt(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	double scale, //!< [in] スケーリング値
	double translate, //!< [in] 平行移動値
	int* pDst //!< [out] 変換後のデータ
);

// 機能 : 非線形変換(Log,Pow)を含む変換を行う
//
JUNKAPI void JUNKCALL TransformNonLin(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const TransformInfo* pTis, //!< [in] 変換情報配列
	intptr_t nTransform, //!< [in] pTis の要素数
	double* pDst //!< [out] 変換後のデータ
);

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
JUNKAPI void JUNKCALL TransformNonLinInt(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const TransformInfo* pTis, //!< [in] 変換情報配列
	intptr_t nTransform, //!< [in] pTis の要素数
	int* pDst //!< [out] 変換後のデータ
);

// 機能 : 線形変換後intに変換を行う
//        出力の pDst は POINT 構造体(8バイト)の配列と見なし、先頭4バイトに値を書き込み残りの4バイトはそのまま残す
//
JUNKAPI void JUNKCALL TransformLinToInt2(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	double scale, //!< [in] スケーリング値
	double translate, //!< [in] 平行移動値
	int* pDst //!< [out] 変換後のデータ
);

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//        出力の pDst は POINT 構造体(8バイト)の配列と見なし、先頭4バイトに値を書き込み残りの4バイトはそのまま残す
//
JUNKAPI void JUNKCALL TransformNonLinToInt2(
	const double* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const TransformInfo* pTis, //!< [in] 変換情報配列
	intptr_t nTransform, //!< [in] pTis の要素数
	int* pDst //!< [out] 変換後のデータ
);

// 機能 : double 型２次元(X,Y)ベクトル配列を受け取り、変換して int 型２次元(X,Y)ベクトル配列に出力する
//
JUNKAPI void JUNKCALL TransformLinPointDToPointI(
	const Vector2d* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	double scaleX, //!< [in] X座標スケーリング値
	double translateX, //!< [in] X座標平行移動値
	double scaleY, //!< [in] Y座標スケーリング値
	double translateY, //!< [in] Y座標平行移動値
	Vector2i* pDst //!< [out] 変換後のデータ
);

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
JUNKAPI void JUNKCALL TransformNonLinPointDToPointI(
	const Vector2d* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const TransformInfo* pTisX, //!< [in] X座標変換情報配列
	intptr_t nTransformX, //!< [in] pTisX の要素数
	const TransformInfo* pTisY, //!< [in] Y座標変換情報配列
	intptr_t nTransformY, //!< [in] pTisY の要素数
	Vector2i* pDst //!< [out] 変換後のデータ
);

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
JUNKAPI intptr_t JUNKCALL TransformForDraw(
	const TransformInfo* pTisX, //!< [in] X軸変換情報配列
	intptr_t nTisX, //!< [in] pTisX の要素数
	const TransformInfo* pTisY, //!< [in] Y軸変換情報配列
	intptr_t nTisY, //!< [in] pTisY の要素数
	const double* pSrcY, //!< [in] 変換元のY軸値データ
	intptr_t iStartIndexX, //!< [in] X軸値計算用のインデックス番号開始値
	intptr_t n, //!< [in] 変換データ数
	Vector2i* pDst //!< [out] 変換後のXY値データ、必要な要素数は座標変換後の値により異なる、座標変換後のX値範囲*4+4程度必要
);

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する(リングバッファ版)
//	Y軸値データは pSrcY + iStartIndexY から取得開始され、pSrcY + nSrcYBufLen を超えたら pSrcY に戻って取得が続けられる
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
JUNKAPI intptr_t JUNKCALL TransformForDrawRing(
	const TransformInfo* pTisX, //!< [in] X軸変換情報配列
	intptr_t nTisX, //!< [in] pTisX の要素数
	const TransformInfo* pTisY, //!< [in] Y軸変換情報配列
	intptr_t nTisY, //!< [in] pTisY の要素数
	const double* pSrcY, //!< [in] 変換元のY軸値データ(Y軸データバッファの先頭アドレス)
	intptr_t nSrcYBufLen, //!< [in] pSrcY のバッファのサイズ(データ数)
	intptr_t iStartIndexX, //!< [in] X軸値計算用のインデックス番号開始値
	intptr_t iStartIndexY, //!< [in] Y軸値計算用のインデックス番号開始値
	intptr_t n, //!< [in] 変換データ数
	Vector2i* pDst //!< [out] 変換後のXY値データ、必要な要素数は座標変換後の値により異なる、座標変換後のX値範囲*4+4程度必要
);

// 機能 : 複数頂点で構成されるラインと指定ポイントとの接触判定を行う
//
// 戻り値: 接触しているなら接触頂点番号が返る、それ以外は負数が返る
//
JUNKAPI intptr_t JUNKCALL HitTestPointAndPolyline(
	const Vector2d* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count, //!< [in] 頂点数
	Vector2d pt, //!< [in] 接触判定ポイント座標
	double vertexRadius, //!< [in] 頂点の半径
	double edgeThickness, //!< [in] 辺の太さ
	double& t, //!< [out] 辺と接触した際のパラメータ t が返る(0 なら戻り値の頂点、1なら次の頂点)
	Vector2d& pointOnEdge //!< [out] 接触した辺の最近点座標が返る
);

// 機能 : 複数頂点で構成されるラインの全ての辺の長さの合計を計算する
//
// 戻り値: 長さ
//
JUNKAPI double JUNKCALL PolylineLength(
	const Vector2d* vts, //!< [in] 頂点配列先頭ポインタ
	intptr_t count //!< [in] 頂点数
);


//==============================================================================
//		座標変換用構造体宣言
//==============================================================================

#undef max
#undef min

#pragma pack(push,1)

struct TransformLinear // 線形座標変換構造体
{
	double Scale; // スケーリング値
	double Translate; // スケーリング後の移動量

	TransformLinear() // コンストラクタ
	{
	}

	TransformLinear(const TransformLinear& c) // コピーコンストラクタ
	{
		*this = c;
	}

	TransformLinear(double s, double t) // コンストラクタ　スケーリング値と移動値を指定して初期化
	{
		Scale = s;
		Translate = t;
	}

	TransformLinear(const RangeD& src, const RangeD& dst) // コンストラクタ、指定された範囲を指定された範囲へマッピングする初期化
	{
		double s = src.Size();
		if (s != 0.0) {
			Scale = dst.Size() / src.Size();
			Translate = dst.S - src.S * Scale;
		} else {
			Scale = 0.0;
			Translate = 0.0;
		}
	}

	double Cnv(double val) const // 正方向で変換
	{
		return val * Scale + Translate;
	}

	double InvCnv(double val) const // 逆方向で変換
	{
		return (val - Translate) / Scale;
	}

	void Cnv(const double* pSrcArray, double* pDstArray, intptr_t num) // 指定配列内の複数データの座標変換を行う
	{
		TransformLin(pSrcArray, num, Scale, Translate, pDstArray);
	}

	void Cnv(const double* pSrcArray, int* pDstArray, intptr_t num) // 指定配列内の複数データの座標変換を行う
	{
		TransformLinInt(pSrcArray, num, Scale, Translate, pDstArray);
	}

	void Cnv(const double* pSrcArray, Vector2i* pDstArray, BOOL convertY, intptr_t num) // 指定配列内の複数データの座標変換を行う、変換後データは pDstArray に格納される(convertY が0なら x に、それ以外なら y）
	{
		int* pDst = (int*)pDstArray;
		if (convertY)
			pDst++;
		TransformLinToInt2(pSrcArray, num, Scale, Translate, pDst);
	}

	TransformLinear Multiply(const TransformLinear& transform) const // 変換を合成
	{
		return TransformLinear(Scale * transform.Scale, Translate * transform.Scale + transform.Translate);
	}

	TransformLinear Invert() const // 逆変換を作成
	{
		return TransformLinear(1.0 / Scale, -Translate / Scale);
	}

	BOOL operator!=(const TransformLinear& c) const // 内容の不一致のチェック
	{
		return Scale != c.Scale || Translate != c.Translate;
	}
};

struct TransformInfo // Transform 構造体内で使用される１回の座標変換情報
{
	int LogBeforeLinear; // リニア変換の前に log10 を実行するかどうか
	int PowAfterLinear; // リニア変換の跡に pow10 を実行するかどうか
	TransformLinear Transform; // リニア座標変換構造体

	TransformInfo() // デフォルトコンストラクタ
	{
	}

	TransformInfo(int lbl, int pal, const TransformLinear& tf) // コンストラクタ、値を指定して初期化する
	{
		LogBeforeLinear = lbl;
		PowAfterLinear = pal;
		Transform = tf;
	}

	TransformInfo Invert() const // 逆変換を作成する
	{
		return TransformInfo(PowAfterLinear, LogBeforeLinear, Transform.Invert());
	}
};

//! 座標変換クラス、非線形の変換(ログ)も含む
struct Transform 
{
	std::vector<TransformInfo> TransformInfos; // 座標変換情報配列、ログを含む場合は全ての変換を１つの TransformLinear に合成することはできないため配列となっている、0番目の要素から順に変換を行う
	double MinVal; // Cnv メソッドで変換可能な最小値、Log 変換時に使用する

	Transform() // デフォルトコンストラクタ
	{
	}

	Transform(intptr_t tiCount, const TransformInfo* pTis, double minVal) // コンストラクタ、変換情報配列を指定して初期化する
	{
		TransformInfos.insert(TransformInfos.end(), pTis, pTis + tiCount);
		MinVal = DBL_MIN;
	}

	Transform(const std::vector<TransformInfo>& tis, double minVal) // コンストラクタ、変換情報配列を指定して初期化する
	{
		TransformInfos.insert(TransformInfos.end(), tis.begin(), tis.end());
		MinVal = DBL_MIN;
	}

	Transform(double scale, double translate) // コンストラクタ、縮尺と移動量を指定して初期化する
	{
		TransformInfos.push_back(TransformInfo(FALSE, FALSE, TransformLinear(scale, translate)));
		MinVal = DBL_MIN;
	}

	Transform(RangeD rangeBefore, RangeD rangeAfter, BOOL log) // コンストラクタ、変換前と変換後の範囲とログフラグを指定して初期化する
	{
		if (log) {
			MinVal = rangeBefore.S;
			rangeBefore.S = Log10(rangeBefore.S);
			rangeBefore.E = Log10(rangeBefore.E);
			if (rangeBefore.Size() == 0.0) {
				rangeBefore.S -= 1.0;
				rangeBefore.E += 1.0;
			}
			TransformInfos.push_back(TransformInfo(true, false, TransformLinear(rangeBefore, rangeAfter)));
		} else {
			TransformInfos.push_back(TransformInfo(false, false, TransformLinear(rangeBefore, rangeAfter)));
			MinVal = DBL_MIN;
		}
	}

	double Cnv(double val) const // 座標変換を行う
	{
		for (intptr_t i = 0, n = TransformInfos.size(); i < n; i++) {
			const TransformInfo& ti = TransformInfos[i];
			if (ti.LogBeforeLinear)
				val = Log10(val);
			val = ti.Transform.Cnv(val);
			if (ti.PowAfterLinear)
				val = Pow10(val);
		}
		return val;
	}

	double InvCnv(double val) const // 逆座標変換を行う
	{
		for (intptr_t i = TransformInfos.size() - 1; 0 <= i; i--) {
			const TransformInfo& ti = TransformInfos[i];
			if (ti.PowAfterLinear)
				val = Log10(val);
			val = ti.Transform.InvCnv(val);
			if (ti.LogBeforeLinear)
				val = Pow10(val);
		}
		return val;
	}

	void Cnv(const double* pSrcArray, double* pDstArray, intptr_t num) // 指定配列内の複数データの座標変換を行う
	{
		TransformNonLin(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDstArray);
	}

	void Cnv(const double* pSrcArray, int* pDstArray, intptr_t num) // 指定配列内の複数データの座標変換を行う
	{
		TransformNonLinInt(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDstArray);
	}

	void Cnv(const double* pSrcArray, Vector2i* pDstArray, BOOL convertY, intptr_t num) // 指定配列内の複数データの座標変換を行う、変換後データは pDstArray に格納される(convertY が0なら x に、それ以外なら y）
	{
		int* pDst = (int*)pDstArray;
		if (convertY)
			pDst++;
		TransformNonLinToInt2(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDst);
	}

	Transform Multiply(const Transform& transform) const // 座標変換を合成する、変換の順番は this → transform
	{
		//	座標変換情報配列を結合
		std::vector<TransformInfo> tis;
		const std::vector<TransformInfo>& tis2 = transform.TransformInfos;
		tis.insert(tis.end(), TransformInfos.begin(), TransformInfos.end());
		for (intptr_t i = 0, n = tis2.size(); i < n; i++) {
			TransformInfo& ti1 = tis[tis.size() - 1];
			const TransformInfo& ti2 = tis2[i];
			if (ti1.PowAfterLinear == ti2.LogBeforeLinear) {
				//	境目に Pow10 と Log10 両方が存在しているか両方存在しない場合
				//	この場合はリニア変換を合成できる
				tis[tis.size() - 1] = TransformInfo(ti1.LogBeforeLinear, ti2.PowAfterLinear, ti1.Transform.Multiply(ti2.Transform));
			} else {
				//	境目に Pow10 か Log10 のどちらかが存在する場合
				//	この場合はリニア変換を合成できない
				tis.push_back(ti2);
			}
		}

		return Transform(tis, std::max(MinVal, transform.MinVal));
	}

	Transform Invert() const // 逆変換を作成する
	{
		Transform tf;
		intptr_t n = TransformInfos.size();
		tf.TransformInfos.resize(n);
		tf.MinVal = Cnv(MinVal);
		for (intptr_t i = 0; i < n; i++)
			tf.TransformInfos[i] = TransformInfos[n - i - 1].Invert();
		return tf;
	}

	_FINLINE static double Log10(double val) // この座標変換処理専用の Log10
	{
		if (val == 0.0)
			return 0.0;
		if (0.0 < val)
			return log10(val);
		else
			return -log10(-val); // 普通こんな計算ないが、なんとしてでもグラフで表示する
	}

	_FINLINE static double Pow10(double val) // この座標変換処理専用の Pow10
	{
		return pow(10.0, val);
	}
};

// 機能 : double 型２次元(X,Y)ベクトル配列を受け取り、変換して int 型２次元(X,Y)ベクトル配列に出力する
//
_FINLINE void TransformPoints(
	const Vector2d* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const TransformLinear& tfx, //!< [in] X軸座標変換オブジェクト
	const TransformLinear& tfy, //!< [in] Y軸座標変換オブジェクト
	Vector2i* pDst //!< [out] 変換後のデータ
) {
	TransformLinPointDToPointI(pSrc, n, tfx.Scale, tfx.Translate, tfy.Scale, tfy.Translate, pDst);
}

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
_FINLINE void TransformPoints(
	const Vector2d* pSrc, //!< [in] 変換元のデータ
	intptr_t n, //!< [in] 変換データ数
	const Transform& tfx, //!< [in] X軸座標変換オブジェクト
	const Transform& tfy, //!< [in] Y軸座標変換オブジェクト
	Vector2i* pDst //!< [out] 変換後のデータ
) {
	TransformNonLinPointDToPointI(
		pSrc,
		n,
		&tfx.TransformInfos[0],
		tfx.TransformInfos.size(),
		&tfy.TransformInfos[0],
		tfy.TransformInfos.size(),
		pDst);
}

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
_FINLINE intptr_t TransformForDraw(
	const Transform& tfx, //!< [in] X軸座標変換オブジェクト
	const Transform& tfy, //!< [in] Y軸座標変換オブジェクト
	const double* pSrcY, //!< [in] 変換元のY軸値データ
	intptr_t iStartIndexX, //!< [in] X軸値計算用のインデックス番号開始値
	intptr_t n, //!< [in] 変換データ数
	Vector2i* pDst //!< [out] 変換後のデータ
) {
	return TransformForDraw(
		&tfx.TransformInfos[0],
		tfx.TransformInfos.size(),
		&tfy.TransformInfos[0],
		tfy.TransformInfos.size(),
		pSrcY,
		iStartIndexX,
		n,
		pDst);
}

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する(リングバッファ版)
//	Y軸値データは pSrcY + iStartIndexY から取得開始され、pSrcY + nSrcYBufLen を超えたら pSrcY に戻って取得が続けられる
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
_FINLINE intptr_t TransformForDrawRing(
	const Transform& tfx, //!< [in] X軸座標変換オブジェクト
	const Transform& tfy, //!< [in] Y軸座標変換オブジェクト
	const double* pSrcY, //!< [in] 変換元のY軸値データ(Y軸データバッファの先頭アドレス)
	intptr_t nSrcYBufLen, //!< [in] pSrcY のバッファのサイズ(データ数)
	intptr_t iStartIndexX, //!< [in] X軸値計算用のインデックス番号開始値
	intptr_t iStartIndexY, //!< [in] Y軸値計算用のインデックス番号開始値
	intptr_t n, //!< [in] 変換データ数
	Vector2i* pDst //!< [out] 変換後のデータ
) {
	return TransformForDrawRing(
		&tfx.TransformInfos[0],
		tfx.TransformInfos.size(),
		&tfy.TransformInfos[0],
		tfy.TransformInfos.size(),
		pSrcY,
		nSrcYBufLen,
		iStartIndexX,
		iStartIndexY,
		n,
		pDst);
}

#pragma pack(pop)


_JUNK_END

#endif

#pragma once
#ifndef __JUNK_JUNKDEF_H
#define __JUNK_JUNKDEF_H

#include "JunkConfig.h"
#include <limits.h>
#include <float.h>
#include <math.h>

_JUNK_BEGIN

// 数学定数
#define	JUNK_PI       3.141592653589793238462643383279502884197169399375 // π
#define	JUNK_PI_DIV_2 1.57079632679489661923132169163975                 // π/2
#define	JUNK_2_PI     6.28318530717958647692528676655901                 // 2π
#define	JUNK_INV_PI   0.318309886183790671537767526745029                // 1/π
#define	JUNK_RADTODEG 57.2957795130823208767981548141052                 // 180/π
#define	JUNK_DEGTORAD 0.0174532925199432957692369076848861               // π/180
#define	JUNK_EXP      2.71828182845904523536                             // ε
#define	JUNK_ILOG2    3.32192809488736234787                             // 1/log10(2)
#define	JUNK_INV3     0.33333333333333333333                             // 1/3
#define	JUNK_INV6     0.16666666666666666666                             // 1/6
#define	JUNK_INV7     0.14285714285714285714                             // 1/9
#define	JUNK_INV9     0.11111111111111111111                             // 1/9
#define	JUNK_INV255   0.00392156862745098039215686274509804              // 1/255
#define	JUNK_SQRT2    1.4142135623730950488016887242097                  // √2

// 型別の最小値、最大取得関数
template<class T> _FINLINE T MinVal();
template<class T> _FINLINE T MaxVal();
template<class T> _FINLINE T Epsilon();

template<> _FINLINE int8_t MinVal<int8_t>() {
	return SCHAR_MIN;
}
template<> _FINLINE int8_t MaxVal<int8_t>() {
	return SCHAR_MAX;
}
template<> _FINLINE uint8_t MinVal<uint8_t>() {
	return 0;
}
template<> _FINLINE uint8_t MaxVal<uint8_t>() {
	return UCHAR_MAX;
}

template<> _FINLINE int16_t MinVal<int16_t>() {
	return SHRT_MIN;
}
template<> _FINLINE int16_t MaxVal<int16_t>() {
	return SHRT_MAX;
}
template<> _FINLINE uint16_t MinVal<uint16_t>() {
	return 0;
}
template<> _FINLINE uint16_t MaxVal<uint16_t>() {
	return USHRT_MAX;
}

template<> _FINLINE int32_t MinVal<int32_t>() {
	return INT_MIN;
}
template<> _FINLINE int32_t MaxVal<int32_t>() {
	return INT_MAX;
}
template<> _FINLINE uint32_t MinVal<uint32_t>() {
	return 0;
}
template<> _FINLINE uint32_t MaxVal<uint32_t>() {
	return UINT_MAX;
}

template<> _FINLINE int64_t MinVal<int64_t>() {
	return LLONG_MIN;
}
template<> _FINLINE int64_t MaxVal<int64_t>() {
	return LLONG_MAX;
}
template<> _FINLINE uint64_t MinVal<uint64_t>() {
	return 0;
}
template<> _FINLINE uint64_t MaxVal<uint64_t>() {
	return ULLONG_MAX;
}

template<> _FINLINE long MinVal<long>() {
	return LONG_MIN;
}
template<> _FINLINE long MaxVal<long>() {
	return LONG_MAX;
}
template<> _FINLINE unsigned long MinVal<unsigned long>() {
	return 0;
}
template<> _FINLINE unsigned long MaxVal<unsigned long>() {
	return ULONG_MAX;
}

template<> _FINLINE float MinVal<float>() {
	return -FLT_MAX;
}
template<> _FINLINE float MaxVal<float>() {
	return FLT_MAX;
}

template<> _FINLINE double MinVal<double>() {
	return -DBL_MAX;
}
template<> _FINLINE double MaxVal<double>() {
	return DBL_MAX;
}

template<> _FINLINE float Epsilon<float>() {
	return FLT_EPSILON;
}
template<> _FINLINE double Epsilon<double>() {
	return DBL_EPSILON;
}

template<class T> _FINLINE void SetMinVal(T& val) {
	val = MinVal<T>();
}
template<class T> _FINLINE void SetMaxVal(T& val) {
	val = MaxVal<T>();
}

//! 指定された引数の絶対値を取得する
template<class T> inline T Abs(T v) {
	SetAbs(v);
	return v;
}
inline float Abs(float v) {
	return (float)fabs(v);
}
inline double Abs(double v) {
	return fabs(v);
}

//! 四捨五入
template<class R, class T> _FINLINE R Nint(T s) {
	return s < T(0.5) ? R(s - T(0.5)) : R(s + T(0.5));
}
//! 四捨五入して int へ変換
template<class T> _FINLINE int NintI(T s) {
	return Nint<int>(s);
}
//! 四捨五入して long へ変換
template<class T> _FINLINE long NintL(T s) {
	return Nint<long>(s);
}

//! 値範囲構造体
template<class T>
struct Range {
	typedef T ValueType; //!< 数値型
	typedef Range<T> Self; //!< 自分の型

	T S; //!< 開始値
	T E; //!< 終了値

	_FINLINE Range() {}
	_FINLINE Range(T se) : S(se), E(se) {}
	_FINLINE Range(T s, T e) : S(s), E(e) {}
	template<class T1> _FINLINE Range(const Range<T1>& c) : S(T(c.S)), E(T(c.E)) {}

	template<class T1> _FINLINE Self& operator=(const Range<T1>& c) {
		S = T(c.S);
		E = T(c.E);
		return *this;
	}

	_FINLINE void Set(T s, T e) {
		S = s;
		E = e;
	}
	template<class T1> _FINLINE void Set(const Range<T1>& c) {
		*this = c;
	}

	_FINLINE T Size() const {
		return E - S;
	}
	_FINLINE T Center() const {
		return (S + E) / T(2);
	}

	_FINLINE ibool InRange(T v) const {
		return S <= v && v <= E;
	}
	_FINLINE ibool InCore(T v) const {
		return S < v && v < E;
	}
	_FINLINE void Clip(T& v) const {
		if(v < S) v = S;
		if(E < v) v = E;
	}
	_FINLINE void Clip(Self& r) {
		Clip(r.S);
		Clip(r.E);
	}

	_FINLINE Self& Inflate(T i) {
		S -= i;
		E += i;
		return *this;
	}
	_FINLINE Self GetInflated(T i) const {
		return Range(S - i, E + i);
	}

	_FINLINE Self& BeginExpand() {
		S = MaxVal<T>();
		E = MinVal<T>();
		return *this;
	}
	_FINLINE Self& ExpandBy(T v) {
		if(v < S) S = v;
		if(E < v) E = v;
		return *this;
	}
	_FINLINE Self& ExpandBy(Range c) {
		if(c.S < S) S = c.S;
		if(E < c.E) E = c.E;
		return *this;
	}

	_FINLINE Range operator+(T v) const {
		return Range(S + v, E + v);
	}
	_FINLINE Range operator-(T v) const {
		return Range(S - v, E - v);
	}
	_FINLINE Range operator*(T v) const {
		return Range(S * v, E * v);
	}
	_FINLINE Range operator/(T v) const {
		return Range(S / v, E / v);
	}
	_FINLINE Range& operator+=(T v) const {
		S += v;
		E += v;
		return *this;
	}
	_FINLINE Range& operator-=(T v) const {
		S -= v;
		E -= v;
		return *this;
	}
	_FINLINE Range& operator*=(T v) const {
		S *= v;
		E *= v;
		return *this;
	}
	_FINLINE Range& operator/=(T v) const {
		S /= v;
		E /= v;
		return *this;
	}

	_FINLINE ibool operator==(const Range& c) const {
		return S==c.S && E==c.E;
	}
	_FINLINE ibool operator!=(const Range& c) const {
		return S!=c.S || E!=c.E;
	}

	_FINLINE friend Range operator*(T v, const Range& c) {
		return c * v;
	}
};

typedef Range<int> RangeI; //!< int 型 値範囲
typedef Range<float> RangeF; //!< float 型 値範囲
typedef Range<double> RangeD; //!< double 型 値範囲

//! 線形変換構造体
template<class T>
struct LinearTransform {
	typedef T ValueType; //!< 数値型
	typedef LinearTransform<T> Self; //!< 自分の型

	T Scale; //!< スケーリング値
	T Translate; //!< 移動値

	//! コンストラクタ
	_FINLINE LinearTransform() {
	}
	//! コンストラクタ　スケーリング値と移動値を指定して初期化
	_FINLINE LinearTransform(T scale, T translate) {
		Scale = scale;
		Translate = translate;
	}
	//! コンストラクタ、指定された範囲を指定された範囲へマッピングする初期化
	_FINLINE LinearTransform(const Range<T>& src, const Range<T>& dst) {
		T s = src.Size();
		if(s != T(0)) {
			Scale = dst.Size() / src.Size();
			Translate = dst.S - src.S * Scale;
		} else {
			Scale = T(0);
			Translate = T(0);
		}
	}

	//! スケーリング値と移動値を設定する
	_FINLINE void Set(T s, T t) {
		Scale = s;
		Translate = t;
	}
	//! 正方向で変換
	_FINLINE T Fwd(T val) const {
		return val * Scale + Translate;
	}
	//! 逆方向で変換
	_FINLINE T Bkw(T val) const {
		return (val - Translate) / Scale;
	}
	//! 変換を合成
	_FINLINE Self NewMul(const Self& transform) const {
		return Self(Scale * transform.Scale, Translate * transform.Scale + transform.Translate);
	}
	//! 変換を合成
	_FINLINE Self operator*(const Self& transform) const {
		return NewMul(transform);
	}
	//! 逆変換を作成
	_FINLINE Self NewInvert() const {
		return Self(T(1) / Scale, -Translate / Scale);
	}
	//! 内容の一致のチェック
	_FINLINE ibool operator==(const Self& c) const {
		return Scale == c.Scale && Translate == c.Translate;
	}
	//! 内容の不一致のチェック
	_FINLINE ibool operator!=(const Self& c) const {
		return Scale != c.Scale || Translate != c.Translate;
	}
};

typedef LinearTransform<int> LinearTransformI; //!< int 型 線形変換構造体
typedef LinearTransform<float> LinearTransformF; //!< float 型 線形変換構造体
typedef LinearTransform<double> LinearTransformD; //!< double 型 線形変換構造体

_JUNK_END

#endif

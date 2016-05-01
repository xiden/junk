#pragma once
#ifndef __VALUE_H__
#define __VALUE_H__

#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

// 強制インライン展開マクロ
#ifdef __GNUC__
#define _FINLINE inline __attribute__((always_inline))
#endif
#ifdef _MSC_VER
#define _FINLINE inline __forceinline
#endif

//! 指定型の最小値取得
template<class T> _FINLINE T MinVal();
//! 指定型の最大取得
template<class T> _FINLINE T MaxVal();
template<> _FINLINE int8_t MinVal<int8_t>() { return INT8_MIN; }
template<> _FINLINE int8_t MaxVal<int8_t>() { return INT8_MAX; }
template<> _FINLINE int16_t MinVal<int16_t>() { return INT16_MIN; }
template<> _FINLINE int16_t MaxVal<int16_t>() { return INT16_MAX; }
template<> _FINLINE int32_t MinVal<int32_t>() { return INT32_MIN; }
template<> _FINLINE int32_t MaxVal<int32_t>() { return INT32_MAX; }
template<> _FINLINE int64_t MinVal<int64_t>() { return INT64_MIN; }
template<> _FINLINE int64_t MaxVal<int64_t>() { return INT64_MAX; }
template<> _FINLINE uint8_t MinVal<uint8_t>() { return 0; }
template<> _FINLINE uint8_t MaxVal<uint8_t>() { return UINT8_MAX; }
template<> _FINLINE uint16_t MinVal<uint16_t>() { return 0; }
template<> _FINLINE uint16_t MaxVal<uint16_t>() { return UINT16_MAX; }
template<> _FINLINE uint32_t MinVal<uint32_t>() { return 0; }
template<> _FINLINE uint32_t MaxVal<uint32_t>() { return UINT32_MAX; }
template<> _FINLINE uint64_t MinVal<uint64_t>() { return 0; }
template<> _FINLINE uint64_t MaxVal<uint64_t>() { return UINT64_MAX; }
template<> _FINLINE long MinVal<long>() { return LONG_MIN; }
template<> _FINLINE long MaxVal<long>() { return LONG_MAX; }
template<> _FINLINE unsigned long MinVal<unsigned long>() { return 0; }
template<> _FINLINE unsigned long MaxVal<unsigned long>() { return ULONG_MAX; }
template<> _FINLINE float MinVal<float>() { return -FLT_MAX; }
template<> _FINLINE float MaxVal<float>() { return FLT_MAX; }
template<> _FINLINE double MinVal<double>() { return -DBL_MAX; }
template<> _FINLINE double MaxVal<double>() { return DBL_MAX; }

//! 指定型の計算機イプシロン取得
template<class T> _FINLINE T Epsilon();
template<> _FINLINE float Epsilon<float>() { return FLT_EPSILON; }
template<> _FINLINE double Epsilon<double>() { return DBL_EPSILON; }

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

	_FINLINE void Normalize() {
		if (E < S) {
			T t = S;
			S = E;
			E = t;
		}
	}

	_FINLINE bool IsNormalized() const {
		return S <= E;

	}

	_FINLINE T Size() const {
		return E - S;
	}
	_FINLINE T Center() const {
		return (S + E) / T(2);
	}

	_FINLINE bool InRange(T v) const {
		return S <= v && v <= E;
	}
	_FINLINE bool InCore(T v) const {
		return S < v && v < E;
	}
	_FINLINE void Clip(T& v) const {
		if (v < S) v = S;
		if (E < v) v = E;
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
		if (v < S) S = v;
		if (E < v) E = v;
		return *this;
	}
	_FINLINE Self& ExpandBy(Range c) {
		if (c.S < S) S = c.S;
		if (E < c.E) E = c.E;
		return *this;
	}

	_FINLINE bool TestIntersect(Range c) const {
		return this->S <= c.E && c.S <= this->E;
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

	_FINLINE bool operator==(const Range& c) const {
		return S == c.S && E == c.E;
	}
	_FINLINE bool operator!=(const Range& c) const {
		return S != c.S || E != c.E;
	}

	_FINLINE friend Range operator*(T v, const Range& c) {
		return c * v;
	}
};

typedef Range<int> Rangei;
typedef Range<double> Ranged;

#endif

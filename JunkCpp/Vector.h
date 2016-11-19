﻿#pragma once
#ifndef __JUNK_VECTOR_H__
#define __JUNK_VECTOR_H__

#include "JunkDef.h"
#include "TemplateMeta.h"
#include <cstdlib>
#include <cmath>

_JUNK_BEGIN

template<class T, intptr_t R, intptr_t C, class SEL, class Math> struct MatrixMxN;

#pragma pack(push,1)

//! math.h で宣言されている数学関数クラスの基本形、Vector、Matrix、Intersects クラスで使用する
//! SSEなどを使う数学関数を作ったらこれを置き換えることで高速化とかできるかもしれない
template<
	class T //!< 扱う値型
> struct DefaultMath {
	typedef double AngleType; //!< 角度型

	static _FINLINE T SinRad(AngleType rad) {
		return ::sin(rad);
	}

	static _FINLINE T CosRad(AngleType rad) {
		return ::cos(rad);
	}

	static _FINLINE AngleType ATan2Rad(T y, T x) {
		return ::atan2(y, x);
	}

	static _FINLINE T SinDeg(AngleType deg) {
		return ::sin(JUNK_DEGTORAD * deg);
	}

	static _FINLINE T CosDeg(AngleType deg) {
		return ::cos(JUNK_DEGTORAD * deg);
	}

	static _FINLINE AngleType ATan2Deg(T y, T x) {
		return ::atan2(y, x) * JUNK_RADTODEG;
	}

	static _FINLINE T Sqrt(T a) {
		return std::sqrt(a);
	}

	static _FINLINE T Abs(T a) {
		return std::abs(a);
	}
};

//! 入力データから２次元ベクトルのみを抽出するクラスの基本形
template<
	class OutputVector //!< 2次元ベクトル型、Vector2 を継承するクラス
> struct ProjectVector2 {
	template<
		class D //!< 入力データ型
	> _FINLINE OutputVector operator()(const D& data) {
		return OutputVector(data(0), data(1));
	}
};

//! 入力データから３次元ベクトルのみを抽出するクラスの基本形
template<
	class OutputVector //!< ３次元ベクトル型、Vector3 を継承するクラス
> struct ProjectVector3 {
	template<
		class D //!< 入力データ型
	> _FINLINE OutputVector operator()(const D& vtx) {
		return OutputVector(data(0), data(1), data(2));
	}
};

//! 入力データから４次元ベクトルのみを抽出するクラスの基本形
template<
	class OutputVector //!< ４次元ベクトル型、Vector4 を継承するクラス
> struct ProjectVector4 {
	template<
		class D //!< 入力データ型
	> _FINLINE OutputVector operator()(const D& vtx) {
		return OutputVector(data(0), data(1), data(2), data(3));
	}
};

//! 固定サイズn次元ベクトルクラステンプレート
template<
	class T, //!< 要素値の型
	intptr_t NUM, //!< 要素数
	class Math = DefaultMath<T> //!< ベクトル用算術関数群
> struct VectorN {
	enum {
		N = NUM //!< 要素数
	};

	typedef T ValueType; //!< 要素値の型

	T e[N]; //!< 要素配列

	_FINLINE VectorN() {}
	_FINLINE VectorN(const T* p) {
		Order<TmSet, N>::Op(e, p);
	}
	template<class S, class MF> _FINLINE VectorN(const VectorN<S, N, MF>& c) {
		Order<TmCastAndSet, N>::Op(e, c.e);
	}

	template<class S, class MF> _FINLINE VectorN& operator=(const VectorN<S, N, MF>& c) {
		Order<TmSet, N>::Op(e, c.e);
		return *this;
	}

	_FINLINE intptr_t Size() const {
		return N;
	}

	_FINLINE void Get(T* p) {
		Order<TmSet, N>::Op(p, e);
	}

	_FINLINE const T& operator()(intptr_t i) const {
		return e[i];
	}
	_FINLINE T& operator()(intptr_t i) {
		return e[i];
	}

	_FINLINE const T& operator[](intptr_t i) const {
		return e[i];
	}
	_FINLINE T& operator[](intptr_t i) {
		return e[i];
	}

	_FINLINE VectorN& Minus(const VectorN& v) {
		Order<TmMinus, N>::Op(e, v.e);
		return *this;
	}
	_FINLINE VectorN& Add(const VectorN& v1, const VectorN& v2) {
		Order<TmAdd, N>::Op(e, v1.e, v2.e);
		return *this;
	}
	_FINLINE VectorN& Sub(const VectorN& v1, const VectorN& v2) {
		Order<TmSub, N>::Op(e, v1.e, v2.e);
		return *this;
	}
	_FINLINE VectorN& Mul(const VectorN& v1, const VectorN& v2) {
		Order<TmMul, N>::Op(e, v1.e, v2.e);
		return *this;
	}
	_FINLINE VectorN& Div(const VectorN& v1, const VectorN& v2) {
		Order<TmDiv, N>::Op(e, v1.e, v2.e);
		return *this;
	}
	_FINLINE VectorN& Cross(const VectorN& v1, const VectorN& v2) {
		Order<TmNone, N>::Cross(e, v1.e, v2.e);
		return *this;
	}

	_FINLINE VectorN operator+() const {
		return *this;
	}
	_FINLINE VectorN operator-() const {
		VectorN nv;
		Order<TmMinus, N>::Op(nv.e, e);
		return nv;
	}
	_FINLINE VectorN operator+(const VectorN& v) const {
		VectorN nv;
		Order<TmAdd, N>::Op(nv.e, e, v.e);
		return nv;
	}
	_FINLINE VectorN operator-(const VectorN& v) const {
		VectorN nv;
		Order<TmSub, N>::Op(nv.e, e, v.e);
		return nv;
	}
	_FINLINE VectorN operator*(T s) const {
		VectorN nv;
		Order<TmMul, N>::OpS(nv.e, e, s);
		return nv;
	}
	_FINLINE VectorN operator*(const VectorN& v) const {
		VectorN nv;
		Order<TmMul, N>::Op(nv.e, e, v.e);
		return nv;
	}
	_FINLINE friend VectorN operator*(T s, const VectorN& v) {
		return v * s;
	}
	_FINLINE VectorN operator/(T s) const {
		VectorN nv;
		Order<TmDiv, N>::OpS(nv.e, e, s);
		return nv;
	}
	_FINLINE VectorN operator/(const VectorN& v) const {
		VectorN nv;
		Order<TmDiv, N>::Op(nv.e, e, v.e);
		return nv;
	}
	_FINLINE VectorN& operator+=(const VectorN& v) {
		Order<TmAdd, N>::Op(e, v.e);
		return *this;
	}
	_FINLINE VectorN& operator-=(const VectorN& v) {
		Order<TmSub, N>::Op(e, v.e);
		return *this;
	}
	_FINLINE VectorN& operator*=(T s) {
		Order<TmMul, N>::OpS(e, s);
		return *this;
	}
	_FINLINE VectorN& operator*=(const VectorN& v) {
		Order<TmMul, N>::Op(e, v.e);
		return *this;
	}
	_FINLINE VectorN& operator/=(T s) {
		Order<TmDiv, N>::OpS(e, s);
		return *this;
	}
	_FINLINE VectorN& operator/=(const VectorN& v) {
		Order<TmDiv, N>::Op(e, v.e);
		return *this;
	}

	_FINLINE bool operator==(const VectorN& v) const {
		return Order<TmNone, N>::Equal(e, v.e);
	}
	_FINLINE bool operator==(T s) const {
		return Order<TmNone, N>::Equal1(e, s);
	}
	_FINLINE bool operator!=(const VectorN& v) const {
		return Order<TmNone, N>::NotEqual(e, v.e);
	}
	_FINLINE bool operator!=(T s) const {
		return Order<TmNone, N>::NotEqual1(e, s);
	}
	_FINLINE bool operator!() const {
		return Order<TmNone, N>::Equal1(e, T(0));
	}
	_FINLINE bool operator<(const VectorN& v) const { // 要素インデックス番号の大きい方が優先度が高い
		return Order<TmNone, N>::LessThan(e, v.e);
	}

	_FINLINE T Dot(const VectorN& v) const {
		T s;
		Order<TmNone, N>::Dot(s, e, v.e);
		return s;
	}
	_FINLINE VectorN Cross(const VectorN& v) const {
		VectorN nv;
		Order<TmNone, N>::Cross(nv.e, e, v.e);
		return nv;
	}
	_FINLINE T operator|(const VectorN& v) const {
		return Dot(v);
	}
	_FINLINE VectorN operator%(const VectorN& v) const {
		return Cross(v);
	}

	_FINLINE T LengthSquare() const {
		T s;
		Order<TmNone, N>::Square(s, e);
		return s;
	}
	_FINLINE T Length() const {
		return T(Math::Sqrt(LengthSquare()));
	}
	_FINLINE void LengthSet(T len) {
		T s(Length());
		if(s == T(0))
			return;
		Order<TmMul, N>::OpS(e, len / s);
	}

	_FINLINE void NormalizeSelf() {
		T len = T(Length());
		if(len != T(0))
			Order<TmMul, N>::OpS(e, T(1) / len);
	}
	_FINLINE VectorN Normalize() {
		T len = T(Length());
		if(len != T(0)) {
			VectorN nv;
			Order<TmMul, N>::OpS(nv.e, e, T(1) / len);
			return nv;
		} else {
			return *this;
		}
	}

	_FINLINE void Reflect(const VectorN& unitVec) {
		T s(Dot(unitVec) * T(2));
		for(intptr_t i = 0, n = N; i < n; ++i)
			e[i] += e[i] * s;
	}
};

//! 固定サイズ2次元ベクトルクラステンプレート
template<
	class T, //!< 要素値の型
	class Math = DefaultMath<T> //!< ベクトル用算術関数群
> struct Vector2 : public VectorN<T, 2, Math> {
	enum {
		N = 2
	};

	typedef VectorN<T, N, Math> Base;
	typedef Vector2<T, Math> Self;
	typedef typename Base::ValueType ValueType;

	_FINLINE Vector2() {}
	_FINLINE Vector2(T x, T y) {
		this->e[0] = x;
		this->e[1] = y;
	}
	_FINLINE Vector2(const T* p) : Base(p) {}
	template<class S, class MF> _FINLINE Vector2(const VectorN<S, N, MF>& c) : Base(c) {}

	template<class S, class MF> _FINLINE Vector2& operator=(const VectorN<S, N, MF>& c) {
		Base::operator=(c);
		return *this;
	}

	_FINLINE const T& X() const {
		return this->e[0];
	}
	_FINLINE T& X() {
		return this->e[0];
	}
	_FINLINE const T& Y() const {
		return this->e[1];
	}
	_FINLINE T& Y() {
		return this->e[1];
	}

	_FINLINE void Set(T x, T y) {
		this->e[0] = x;
		this->e[1] = y;
	}

	_FINLINE void MakeByAngle(T r) {
		this->e[0] = Math::CosRad(r);
		this->e[1] = Math::SinRad(r);
	}
	_FINLINE void MakeByAngle(T r, T len) {
		this->e[0] = len * Math::CosRad(r);
		this->e[1] = len * Math::SinRad(r);
	}
	_FINLINE void ChangeAngle(T r) {
		T l = this->Length();
		this->e[0] = T(l * Math::CosRad(r));
		this->e[1] = T(l * Math::SinRad(r));
	}
	_FINLINE T Angle() const {
		return Math::ATan2Rad(this->e[1], this->e[0]);
	}

	_FINLINE void Rotate(T r) {
		Vector2 v;
		v.MakeByAngle(r);
		*this = v * this->e[0] + v.MakeVertical() * this->e[1];
	}
	_FINLINE void VerticalSelf() {
		T t = this->e[0];
		this->e[0] = -this->e[1];
		this->e[1] = t;
	}
	_FINLINE Vector2 Vertical() const {
		return Vector2(-this->e[1], this->e[0]);
	}

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL, Math>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1];
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL, Math>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2);
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2);
		return *this;
	}
};

//! 固定サイズ3次元ベクトルクラステンプレート
template<
	class T, //!< 要素値の型
	class Math = DefaultMath<T> //!< ベクトル用算術関数群
> struct Vector3 : public VectorN<T, 3, Math> {
	enum {
		N = 3
	};

	typedef VectorN<T, N, Math> Base;
	typedef Vector3<T, Math> Self;
	typedef typename Base::ValueType ValueType;

	_FINLINE Vector3() {}
	_FINLINE Vector3(T x, T y, T z) {
		this->e[0] = x;
		this->e[1] = y;
		this->e[2] = z;
	}
	_FINLINE Vector3(const T* p) : Base(p) {}
	template<class S, class MF> _FINLINE Vector3(const VectorN<S, N, MF>& c) : Base(c) {}

	template<class S, class MF> _FINLINE Vector3& operator=(const VectorN<S, N, MF>& c) {
		Base::operator=(c);
		return *this;
	}

	_FINLINE const T& X() const {
		return this->e[0];
	}
	_FINLINE T& X() {
		return this->e[0];
	}
	_FINLINE const T& Y() const {
		return this->e[1];
	}
	_FINLINE T& Y() {
		return this->e[1];
	}
	_FINLINE const T& Z() const {
		return this->e[2];
	}
	_FINLINE T& Z() {
		return this->e[2];
	}

	_FINLINE void Set(T x, T y, T z) {
		this->e[0] = x;
		this->e[1] = y;
		this->e[2] = z;
	}

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL, Math>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2];
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL, Math>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3);
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3);
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL, Math>& m, const VectorN<T, 4>& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3) * v.e[3];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3) * v.e[3];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3) * v.e[3];
		return *this;
	}

	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N, SEL, Math>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N + 1, SEL, Math>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + m(3, 2);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const VectorN<T, 4>& v, const MatrixMxN<T, R, N + 1, SEL, Math>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + v.e[3] * m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + v.e[3] * m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + v.e[3] * m(3, 2);
		return *this;
	}
};

//! 固定サイズ4次元ベクトルクラステンプレート
template<
	class T, //!< 要素値の型
	class Math = DefaultMath<T> //!< ベクトル用算術関数群
> struct Vector4 : public VectorN<T, 4, Math> {
	enum {
		N = 4
	};

	typedef VectorN<T, N, Math> Base;
	typedef Vector4<T, Math> Self;
	typedef typename Base::ValueType ValueType;

	_FINLINE Vector4() {}
	_FINLINE Vector4(T x, T y, T z, T w) {
		this->e[0] = x;
		this->e[1] = y;
		this->e[2] = z;
		this->e[3] = w;
	}
	_FINLINE Vector4(const T* p) : Base(p) {}
	template<class S, class MF> _FINLINE Vector4(const VectorN<S, N, MF>& c) : Base(c) {}

	template<class S, class MF> _FINLINE Vector4& operator=(const VectorN<S, N, MF>& c) {
		Base::operator=(c);
		return *this;
	}

	_FINLINE const T& X() const {
		return this->e[0];
	}
	_FINLINE T& X() {
		return this->e[0];
	}
	_FINLINE const T& Y() const {
		return this->e[1];
	}
	_FINLINE T& Y() {
		return this->e[1];
	}
	_FINLINE const T& Z() const {
		return this->e[2];
	}
	_FINLINE T& Z() {
		return this->e[2];
	}
	_FINLINE const T& W() const {
		return this->e[3];
	}
	_FINLINE T& W() {
		return this->e[3];
	}

	_FINLINE void Set(T x, T y, T z, T w) {
		this->e[0] = x;
		this->e[1] = y;
		this->e[2] = z;
		this->e[3] = w;
	}

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL, Math>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3) * v.e[3];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3) * v.e[3];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3) * v.e[3];
		this->e[3] = m(3, 0) * v.e[0] + m(3, 1) * v.e[1] + m(3, 2) * v.e[2] + m(3, 3) * v.e[3];
		return *this;
	}

	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N, SEL, Math>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + v.e[3] * m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + v.e[3] * m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + v.e[3] * m(3, 2);
		this->e[3] = v.e[0] * m(0, 3) + v.e[1] * m(1, 3) + v.e[2] * m(2, 3) + v.e[3] * m(3, 3);
		return *this;
	}

	template<intptr_t R, class SEL> static _FINLINE Self NewTransform(const MatrixMxN<T, R, N, SEL, Math>& m, const Self& v) {
		Self t;
		t.Transform(m, v);
		return t;
	}

	template<intptr_t R, class SEL> static _FINLINE Self NewTransform(const Self& v, const MatrixMxN<T, R, N, SEL, Math>& m) {
		Self t;
		t.Transform(v, m);
		return t;
	}

	_FINLINE Vector3<T> GetVector3() const {
		return Vector3<T>(this->e);
	}
};

typedef Vector2<int> Vector2i; //!< int 型 ２次元ベクトル
typedef Vector2<long> Vector2l; //!< long 型 ２次元ベクトル
typedef Vector2<float> Vector2f; //!< float 型 ２次元ベクトル
typedef Vector2<double> Vector2d; //!< double 型 ２次元ベクトル

typedef Vector3<int> Vector3i; //!< int 型 ２次元ベクトル
typedef Vector3<long> Vector3l; //!< long 型 ２次元ベクトル
typedef Vector3<float> Vector3f; //!< float 型 ２次元ベクトル
typedef Vector3<double> Vector3d; //!< double 型 ２次元ベクトル

typedef Vector4<int> Vector4i; //!< int 型 ２次元ベクトル
typedef Vector4<long> Vector4l; //!< long 型 ２次元ベクトル
typedef Vector4<float> Vector4f; //!< float 型 ２次元ベクトル
typedef Vector4<double> Vector4d; //!< double 型 ２次元ベクトル

#pragma pack(pop)

_JUNK_END

#endif

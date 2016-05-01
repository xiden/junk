#pragma once
#ifndef __JUNK_VECTOR_H
#define __JUNK_VECTOR_H

#include "JunkDef.h"
#include "TemplateMeta.h"

_JUNK_BEGIN

template<class T, intptr_t R, intptr_t C, class SEL> struct MatrixMxN;

#pragma pack(push,1)

//! math.h で宣言されている数学関数クラス、Vector、Matrix、Intersects クラスで使用する
struct MathFuncs_Math_H {
	static _FINLINE double SinRad(double rad) {
		return sin(rad);
	}

	static _FINLINE double CosRad(double rad) {
		return cos(rad);
	}

	static _FINLINE double ATan2Rad(double y, double x) {
		return atan2(y, x);
	}

	static _FINLINE double SinDeg(double deg) {
		return sin(JUNK_DEGTORAD * deg);
	}

	static _FINLINE double CosDeg(double deg) {
		return cos(JUNK_DEGTORAD * deg);
	}
	static _FINLINE double Sqrt(double a) {
		return sqrt(a);
	}
};

//! 固定サイズn次元ベクトルクラステンプレート
template<class T, intptr_t NUM, class MathFuncs = MathFuncs_Math_H>
struct VectorN {
	enum {
		N = NUM
	};

	typedef T ValueType;

	T e[N];

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
	//template<intptr_t R> VectorN& Transform(const MatrixMxN<T, R, N>& m, const VectorN& v) {
	//	const T* row = m.e2[0];
	//	for(intptr_t i = 0; i < N; i++) {
	//		Order<TmNone, N>::Dot(e[i], row, v.e);
	//		row += N;
	//	}
	//	return *this;
	//}
	//template<intptr_t R> VectorN& Transform(const MatrixMxN<T, R, N + 1>& m, const VectorN& v) {
	//	const T* row = m.e2[0];
	//	for(intptr_t i = 0; i < N; i++) {
	//		Order<TmNone, N>::Dot(e[i], row, v.e);
	//		e[i] += row[N];
	//		row += N + 1;
	//	}
	//	return *this;
	//}

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
		return T(MathFuncs::Sqrt(LengthSquare()));
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
	_FINLINE VectorN Normalized() {
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
template<class T, class MathFuncs = MathFuncs_Math_H>
struct Vector2 : public VectorN<T, 2, MathFuncs> {
	enum {
		N = 2
	};

	typedef VectorN<T, N, MathFuncs> Base;
	typedef Vector2<T, MathFuncs> Self;
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
		this->e[0] = MathFuncs::CosRad(r);
		this->e[1] = MathFuncs::SinRad(r);
	}
	_FINLINE void MakeByAngle(T r, T len) {
		this->e[0] = len * MathFuncs::CosRad(r);
		this->e[1] = len * MathFuncs::SinRad(r);
	}
	_FINLINE void ChangeAngle(T r) {
		T l = this->Length();
		this->e[0] = T(l * MathFuncs::CosRad(r));
		this->e[1] = T(l * MathFuncs::SinRad(r));
	}
	_FINLINE T Angle() const {
		return MathFuncs::ATan2Rad(this->e[1], this->e[0]);
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

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1];
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2);
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2);
		return *this;
	}
};

//! 固定サイズ3次元ベクトルクラステンプレート
template<class T, class MathFuncs = MathFuncs_Math_H>
struct Vector3 : public VectorN<T, 3, MathFuncs> {
	enum {
		N = 3
	};

	typedef VectorN<T, N, MathFuncs> Base;
	typedef Vector3<T, MathFuncs> Self;
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

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2];
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3);
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3);
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N + 1, SEL>& m, const VectorN<T, 4>& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3) * v.e[3];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3) * v.e[3];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3) * v.e[3];
		return *this;
	}

	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N, SEL>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N + 1, SEL>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + m(3, 2);
		return *this;
	}
	template<intptr_t R, class SEL> Self& Transform(const VectorN<T, 4>& v, const MatrixMxN<T, R, N + 1, SEL>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + v.e[3] * m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + v.e[3] * m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + v.e[3] * m(3, 2);
		return *this;
	}
};

//! 固定サイズ4次元ベクトルクラステンプレート
template<class T, class MathFuncs = MathFuncs_Math_H>
struct Vector4 : public VectorN<T, 4, MathFuncs> {
	enum {
		N = 4
	};

	typedef VectorN<T, N, MathFuncs> Base;
	typedef Vector4<T, MathFuncs> Self;
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

	template<intptr_t R, class SEL> Self& Transform(const MatrixMxN<T, R, N, SEL>& m, const Self& v) {
		this->e[0] = m(0, 0) * v.e[0] + m(0, 1) * v.e[1] + m(0, 2) * v.e[2] + m(0, 3) * v.e[3];
		this->e[1] = m(1, 0) * v.e[0] + m(1, 1) * v.e[1] + m(1, 2) * v.e[2] + m(1, 3) * v.e[3];
		this->e[2] = m(2, 0) * v.e[0] + m(2, 1) * v.e[1] + m(2, 2) * v.e[2] + m(2, 3) * v.e[3];
		this->e[3] = m(3, 0) * v.e[0] + m(3, 1) * v.e[1] + m(3, 2) * v.e[2] + m(3, 3) * v.e[3];
		return *this;
	}

	template<intptr_t R, class SEL> Self& Transform(const Self& v, const MatrixMxN<T, R, N, SEL>& m) {
		this->e[0] = v.e[0] * m(0, 0) + v.e[1] * m(1, 0) + v.e[2] * m(2, 0) + v.e[3] * m(3, 0);
		this->e[1] = v.e[0] * m(0, 1) + v.e[1] * m(1, 1) + v.e[2] * m(2, 1) + v.e[3] * m(3, 1);
		this->e[2] = v.e[0] * m(0, 2) + v.e[1] * m(1, 2) + v.e[2] * m(2, 2) + v.e[3] * m(3, 2);
		this->e[3] = v.e[0] * m(0, 3) + v.e[1] * m(1, 3) + v.e[2] * m(2, 3) + v.e[3] * m(3, 3);
		return *this;
	}

	template<intptr_t R, class SEL> static _FINLINE Self NewTransform(const MatrixMxN<T, R, N, SEL>& m, const Self& v) {
		Self t;
		t.Transform(m, v);
		return t;
	}

	template<intptr_t R, class SEL> static _FINLINE Self NewTransform(const Self& v, const MatrixMxN<T, R, N, SEL>& m) {
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

#pragma once
#ifndef __JUNK_INTERSECTS_H__
#define __JUNK_INTERSECTS_H__

#include "Vector.h"

_JUNK_BEGIN

//! 衝突判定処理クラス
template<
	class V, //!< VectorN クラスから継承したクラスを指定する
	class Math = DefaultMath<typename V::ValueType> //!< 数学関数クラス
>
struct Intersects {
	typedef V Vector; //!< 内部で扱うベクトル型
	typedef typename V::ValueType ValueType; //!< 内部で扱う数値型
	enum {
		N = V::N //!< ベクトルの次元数
	};

	//! 球形境界ボリューム
	struct Sphere {
		Vector C; //!< 球の中心
		ValueType Radius; //!< 半径
	};

	//! カプセル形境界ボリューム
	struct Capsule {
		Vector A; //!< 線分の開始点
		Vector B; //!< 線分の終了点
		ValueType Radius; //!< 半径
	};

	//! 軸平行境界ボックス
	struct AABB {
		Vector Min; //!< ボックスの最小値
		Vector Max; //!< ボックスの最大値
	};

	//! 点cと線分abの間の距離の平方を返す
	static _FINLINE ValueType SqDistPointSegment(Vector a, Vector b, Vector c) {
		Vector ab = b - a, ac = c - a, bc = c - b;
		ValueType e = ac.Dot(ab);
		// cがabの外側に射影される場合を扱う
		if(e <= ValueType(0)) return ac.Dot(ac);
		ValueType f = ab.Dot(ab);
		if(e >= f) return bc.Dot(bc);
		// cがab上に射影される場合を扱う
		return ac.Dot(ac) - e * e / f;
	}

	//! 点cと線分abの間の距離の平方を返す、aは原点とする
	static _FINLINE ValueType SqDistPointSegment(Vector ab, Vector ac) {
		Vector bc = ac - ab;
		ValueType e = ac.Dot(ab);
		// cがabの外側に射影される場合を扱う
		if(e <= ValueType(0)) return ac.Dot(ac);
		ValueType f = ab.Dot(ab);
		if(e >= f) return bc.Dot(bc);
		// cがab上に射影される場合を扱う
		return ac.Dot(ac) - e * e / f;
	}

	//! [min, max]の範囲内までnをクランプ
	static _FINLINE ValueType Clamp(ValueType n, ValueType min, ValueType max) {
		if(n < min) return min;
		if(n > max) return max;
		return n;
	}

	//! S1(s)=P1+s*(Q1-P1)およびS2(t)=P2+t*(Q2-P2)の
	//! 最近接点C1およびC2を計算、Sおよびtを返す。
	//! 関数の結果はS1(s)とS2(t)の間の距離の平方
	static ValueType ClosestPtSegmentSegment(Vector p1, Vector q1, Vector p2, Vector q2, ValueType& s, ValueType& t, Vector& c1, Vector& c2) {
		Vector d1 = q1 - p1; // 線分S1の方向ベクトル
		Vector d2 = q2 - p2; // 線分S2の方向ベクトル
		Vector r = p1 - p2;
		ValueType a = d1.Dot(d1); // 線分S1の距離の平方、常に非負
		ValueType e = d2.Dot(d2); // 線分S2の距離の平方、常に非負
		ValueType f = d2.Dot(r);
		ValueType epsilon = Epsilon<ValueType>();

		// 片方あるいは両方の線分が点に縮退しているかどうかチェック
		if (a <= epsilon && e <= epsilon) {
			// 両方の線分が点に縮退
			s = t = ValueType(0);
			c1 = p1;
			c2 = p2;
			return (c1 - c2).Dot(c1 - c2);
		}
		if (a <= epsilon) {
			// 最初の線分が点に縮退
			s = ValueType(0);
			t = f / e; // s = 0 => t = (b*s + f) / e = f / e
			t = Clamp(t, ValueType(0), ValueType(1));
		} else {
			ValueType c = d1.Dot(r);
			if (e <= epsilon) {
				// 2番目の線分が点に縮退
				t = ValueType(0);
				s = Clamp(-c / a, ValueType(0), ValueType(1)); // t = 0 => s = (b*t - c) / a = -c / a
			} else {
				// ここから一般的な縮退の場合を開始
				ValueType b = d1.Dot(d2);
				ValueType denom = a*e-b*b; // 常に非負

				// 線分が平行でない場合、L1上のL2に対する最近接点を計算、そして
				// 線分S1に対してクランプ。そうでない場合は任意s(ここでは0)を選択
				if (denom != ValueType(0)) {
					s = Clamp((b*f - c*e) / denom, ValueType(0), ValueType(1));
				} else s = ValueType(0);

				// L2上のS1(s)に対する最近接点を以下を用いて計算
				// t = Dot((P1+D1*s)-P2,D2) / D2.Dot(D2) = (b*s + f) / e
				t = (b*s + f) / e;

				// tが[0,1]の中にあれば終了。そうでなければtをクランプ、sをtの新しい値に対して以下を用いて再計算
				// s = Dot((P2+D2*t)-P1,D1) / D1.Dot(D1)= (t*b - c) / a
				// そしてsを[0, 1]に対してクランプ
				if (t < ValueType(0)) {
					t = ValueType(0);
					s = Clamp(-c / a, ValueType(0), ValueType(1));
				} else if (t > ValueType(1)) {
					t = ValueType(1);
					s = Clamp((b - c) / a, ValueType(0), ValueType(1));
				}
			}
		}

		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;
		return (c1 - c2).Dot(c1 - c2);
	}

	//! 球とカプセルの交差判定
	static intptr_t TestSphereCapsule(Sphere s, Capsule capsule) {
		// 球の中心とカプセルの線分の間の(平方した)距離を計算
		ValueType dist2 = SqDistPointSegment(capsule.A, capsule.B, s.C);

		// (平方した)距離が(平方した)半径の総和よりも小さい場合は、衝突
		ValueType radius = s.Radius + capsule.Radius;
		return dist2 <= radius * radius;
	}

	//! 原点から伸びるカプセルaと球bの交差判定
	static intptr_t TestCapsuleSphere(const Vector& a, ValueType ar, const Vector& b, ValueType br) {
		// 球の中心とカプセルの線分の間の(平方した)距離を計算
		ValueType dist2 = SqDistPointSegment(a, b);

		// (平方した)距離が(平方した)半径の総和よりも小さい場合は、衝突
		ValueType radius = ar + br;
		return dist2 <= radius * radius;
	}

	//! 球aをab方向へ移動した時に最初に球cに接触する時のtをretTに返す
	static intptr_t TestSphereMovingSphere(const Vector& a, const Vector& v, ValueType ar, const Vector& c, ValueType cr, ValueType& retT) {
		if(N == 3) { // 3次元ベクトルの場合
			ValueType ax = a(0);
			ValueType ay = a(1);
			ValueType az = a(2);
			ValueType dx = ax - c(0);
			ValueType dy = ay - c(1);
			ValueType dz = az - c(2);
			ValueType dx2 = dx * dx;
			ValueType dy2 = dy * dy;
			ValueType dz2 = dz * dz;
			ValueType f = dx2 + dy2 + dz2;
			ValueType r = ar + cr;
			ValueType r2 = r * r;
			if(f <= r2) { // 開始位置で既に接触している場合
				retT = ValueType(0);
				return 1;
			}

			ValueType vx = v(0);
			ValueType vy = v(1);
			ValueType vz = v(2);
			ValueType vx2 = vx * vx;
			ValueType vy2 = vy * vy;
			ValueType vz2 = vz * vz;
			ValueType e = vx2 + vy2 + vz2;
			if(e == ValueType(0)) { // 移動が0の場合
				if(dx2 + dy2 + dz2 <= r2) { // 球同士が接触していたら開始位置を返す
					retT = ValueType(0);
					return 1;
				} else { // 接触していない
					return 0;
				}
			}

			ValueType dvx = dx * vx;
			ValueType dvy = dy * vy;
			ValueType dvz = dz * vz;
			ValueType i(2);
			ValueType idvx = i * dvx;
			ValueType tmp1 = (r2-dy2-dx2)*vz2+(r2-dz2-dx2)*vy2+(r2-dz2-dy2)*vx2+dvz*(idvx+i*dvy)+dvy*idvx;
			if(tmp1 < 0.0)
				return 0;

			ValueType t = -(Math::Sqrt(tmp1) + dvz + dvy + dvx) / e;
			if(t < ValueType(0) || ValueType(1) < t) // 移動範囲内では接触しない
				return 0;

			// 移動範囲内で接触する
			retT = t;
			return 1;
		} else {
			assert("Not Implemented!" && 0);
			return 0;
		}
	}

	//! カプセル同士の交差判定
	static intptr_t TestCapsuleCapsule(Capsule capsule1, Capsule capsule2) {
		// カプセルの内部の構造の間の(平方した)距離を計算
		ValueType s, t;
		Vector c1, c2;
		ValueType dist2 = ClosestPtSegmentSegment(capsule1.A, capsule1.B, capsule2.A, capsule2.B, s, t, c1, c2);

		// (平方した)距離が(平方した)半径の総和よりも小さい場合は、衝突
		ValueType radius = capsule1.Radius + capsule2.Radius;
		return dist2 <= radius * radius;
	}

	//! AABB同士の交差判定
	static intptr_t TestAABBAABB(const AABB& a, const AABB& b) {
		if(N == 3) {
			if (a.Max(0) < b.Min(0) || a.Min(0) > b.Max(0)) return 0;
			if (a.Max(1) < b.Min(1) || a.Min(1) > b.Max(1)) return 0;
			if (a.Max(2) < b.Min(2) || a.Min(2) > b.Max(2)) return 0;
			return 1;
		} else {
			assert("Not Implemented!" && 0);
			return 0;
		}
	}

	//! 線分と平面の交差判定
	static intptr_t TestLinePlane(const Vector& lineStart, const Vector& lineVec, const Vector& planeX, const Vector& planeY, ValueType& retT) {
		Vector vz = planeX.Cross(planeY);
		ValueType b = vz.Dot(lineVec);
		if(b == ValueType(0))
			return 0;
		b = -b;
		ValueType t = vz.Dot(lineStart);
		if(t < ValueType(0) || b < t)
			return 0;
		t /= b;
		Vector p = lineStart + t * lineVec;
		ValueType x = planeX.Dot(p);
		if(x < ValueType(0) || planeX.Dot(planeX) < x)
			return 0;
		ValueType y = planeY.Dot(p);
		if(y < ValueType(0) || planeY.Dot(planeY) < y)
			return 0;
		retT = t;
		return 1;
	}

	//! ２次元線分同士のおおまかな交差判定、0が返ったら交差することは無い
	static _FINLINE intptr_t RoughTestLineLine2D(const Vector& a1, const Vector& a2, const Vector& b1, const Vector& b2) {
		for(intptr_t i = 0; i < N; i++) {
			if(a2(i) <= a1(i)) {
				if((a1(i) < b1(i) && a1(i) < b2(i)) || (a2(i) > b1(i) && a2(i) > b2(i)))
					return 0;
			} else if((a2(i) < b1(i) && a2(i) < b2(i)) || (a1(i) > b1(i) && a1(i) > b2(i))) {
				return 0;
			}
		}
		return 1;
	}

	//! ２次元線分同士の交差判定
	static intptr_t TestLineLine2D(const Vector& a, const Vector& av, const Vector& b, const Vector& bv, ValueType& retT, ValueType& retS) {
		if(N == 2) {
			ValueType d = av(0)*bv(1)-av(1)*bv(0);
			if(d == ValueType(0))
				return 0;

			ValueType e = b(0)-a(0);
			ValueType f = a(1)-b(1);
			ValueType t = (bv(0)*f+bv(1)*e)/d;
			if(t < ValueType(0) || ValueType(1) < t)
				return 0;
			ValueType s = (av(0)*f+av(1)*e)/d;
			if(s < ValueType(0) || ValueType(1) < s)
				return 0;

			retT = t;
			retS = s;
			return 1;
		} else {
			assert("Not Implemented!" && 0);
			return 0;
		}

	}

	//! a から b に伸びる線分ポイント c の大まかな交差判定、0が返ったら交差することは無い
	static _FINLINE intptr_t RoughTestLinePoint(const Vector& a, const Vector& b, const Vector& c) {
		for(intptr_t i = 0; i < N; i++) {
			if(a(i) <= b(i)) {
				if(c(i) < a(i) || b(i) < c(i))
					return 0;
			} else if(c(i) < b(i) || a(i) < c(i)) {
				return 0;
			}
		}
		return 1;
	}

	//! a から av ベクトルへ伸びる線分とポイント b の交差判定
	static intptr_t TestLinePoint(const Vector& a, const Vector& av, const Vector& b, ValueType& retT) {
		if(N == 2) {
			ValueType e = av.LengthSquare();
			if(e == ValueType(0))
				return 0;
			Vector ba = b - a;
			if(ba.Dot(av.NewVertical()) != ValueType(0))
				return 0;
			ValueType t = av.Dot(ba);
			if(t < ValueType(0) && e < t)
				return 0;
			retT = t / e;
			return 1;
		} else {
			assert("Not Implemented!" && 0);
			return 0;
		}
	}
};

_JUNK_END

#endif

#pragma once
#ifndef __JUNK_TEMPLATEMETA_H
#define __JUNK_TEMPLATEMETA_H

#include "JunkConfig.h"

_JUNK_BEGIN

struct TmNone {
};
struct TmSet {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v = v1;
	}
};
struct TmCastAndSet {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v = T(v1);
	}
};
struct TmPlus {
	template<class T, class T1> static _FINLINE void Op(T& v) {
	}
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v = v1;
	}
};
struct TmMinus {
	template<class T, class T1> static _FINLINE void Op(T& v) {
		v = -v;
	}
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v = -v1;
	}
};
struct TmAdd {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v += v1;
	}
	template<class T, class T1, class T2> static _FINLINE void Op(T& v, const T1& v1, const T2& v2) {
		v = v1 + v2;
	}
};
struct TmSub {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v -= v1;
	}
	template<class T, class T1, class T2> static _FINLINE void Op(T& v, const T1& v1, const T2& v2) {
		v = v1 - v2;
	}
};
struct TmMul {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v *= v1;
	}
	template<class T, class T1, class T2> static _FINLINE void Op(T& v, const T1& v1, const T2& v2) {
		v = v1 * v2;
	}
};
struct TmDiv {
	template<class T, class T1> static _FINLINE void Op(T& v, const T1& v1) {
		v /= v1;
	}
	template<class T, class T1, class T2> static _FINLINE void Op(T& v, const T1& v1, const T2& v2) {
		v = v1 / v2;
	}
};

template<class OPCLS, intptr_t N, intptr_t LIM = 32> struct Order {
	template<class T> static _FINLINE void Op(T* p) {
		if(N <= LIM) {
			OPCLS::Op(p[N-1]);
			Order<OPCLS, N-1, LIM>::Op(p);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				OPCLS::Op(p[i]);
		}
	}
	template<class T, class T1> static _FINLINE void OpS(T* p, const T1& v) {
		if(N <= LIM) {
			OPCLS::Op(p[N-1], v);
			Order<OPCLS, N-1, LIM>::OpS(p, v);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				OPCLS::Op(p[i], v);
		}
	}
	template<class T, class T1> static _FINLINE void Op(T* p, const T1* p1) {
		if(N <= LIM) {
			OPCLS::Op(p[N-1], p1[N-1]);
			Order<OPCLS, N-1, LIM>::Op(p, p1);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				OPCLS::Op(p[i], p1[i]);
		}
	}
	template<class T, class T1, class T2> static _FINLINE void OpS(T* p, const T1* p1, const T2& v) {
		if(N <= LIM) {
			OPCLS::Op(p[N-1], p1[N-1], v);
			Order<OPCLS, N-1, LIM>::OpS(p, p1, v);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				OPCLS::Op(p[i], p1[i], v);
		}
	}
	template<class T, class T1, class T2> static _FINLINE void Op(T* p, const T1* p1, const T2* p2) {
		if(N <= LIM) {
			OPCLS::Op(p[N-1], p1[N-1], p2[N-1]);
			Order<OPCLS, N-1, LIM>::Op(p, p1, p2);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				OPCLS::Op(p[i], p1[i], p2[i]);
		}
	}

	template<class T1, class T2> static _FINLINE bool Equal(const T1* p1, const T2* p2) {
		if(N <= LIM) {
			return p1[N-1] == p2[N-1] && Order<OPCLS, N-1, LIM>::Equal(p1, p2);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				if(p1[i] != p2[i])
					return false;
			return true;
		}
	}
	template<class T1, class T2> static _FINLINE bool Equal1(const T1* p1, const T2& v) {
		if(N <= LIM) {
			return p1[N-1] == v && Order<OPCLS, N-1, LIM>::Equal1(p1, v);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				if(p1[i] != v)
					return false;
			return true;
		}
	}
	template<class T1, class T2> static _FINLINE bool NotEqual(const T1* p1, const T2* p2) {
		if(N <= LIM) {
			return p1[N-1] != p2[N-1] || Order<OPCLS, N-1, LIM>::NotEqual(p1, p2);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				if(p1[i] != p2[i])
					return true;
			return false;
		}
	}
	template<class T1, class T2> static _FINLINE bool NotEqual1(const T1* p1, const T2& v) {
		if(N <= LIM) {
			return p1[N-1] != v || Order<OPCLS, N-1, LIM>::NotEqual1(p1, v);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i)
				if(p1[i] != v)
					return true;
			return false;
		}
	}
	template<class T1, class T2> static _FINLINE bool LessThan(const T1* p1, const T2* p2) {
		if(N <= LIM) {
			if(p1[N-1] < p2[N-1])
				return true;
			if(p2[N-1] < p1[N-1])
				return false;
			return Order<OPCLS, N-1, LIM>::LessThan(p1, p2);
		} else {
			for(intptr_t i = N - 1; 0 <= i; --i) {
				if(p1[i] < p2[i])
					return true;
				if(p2[i] < p1[i])
					return false;
			}
			return false;
		}
	}

	template<class T, class T1, class T2> static _FINLINE void Dot(T& v, const T1* p1, const T2* p2) {
		if(N <= LIM) {
			v = p1[N-1] * p2[N-1];
			Order<OPCLS, N-1, LIM>::DotInternal(v, p1, p2);
		} else {
			v = p1[N-1] * p2[N-1];
			for(intptr_t i = N-2; 0 <= i; --i)
				v += p1[i] * p2[i];
		}
	}

	template<class T, class T1, class T2> static _FINLINE void DotInternal(T& v, const T1* p1, const T2* p2) {
		v += p1[N-1] * p2[N-1];
		Order<OPCLS, N-1, LIM>::DotInternal(v, p1, p2);
	}

	template<intptr_t N1, intptr_t dummy = 0> struct CrossStruct {
		template<class T, class T1, class T2> static _FINLINE void Cross(T* p, const T1* p1, const T2* p2) {
			enum { I1 = (N1-1+1) % N, I2 = (N1-1+2) % N };
			p[N1-1] = p1[I1] * p2[I2] - p1[I2] * p2[I1];
			CrossStruct<N1-1>::Cross(p, p1, p2);
		}
	};
	template<intptr_t dummy> struct CrossStruct<0, dummy> {
		template<class T, class T1, class T2> static _FINLINE void Cross(T* p, const T1* p1, const T2* p2) {}
	};

	template<class T, class T1, class T2> static _FINLINE void Cross(T* p, const T1* p1, const T2* p2) {
		if(N <= LIM) {
			CrossStruct<N>::Cross(p, p1, p2);
		} else {
			for(intptr_t i = 0; i < N; ++i) {
				intptr_t i1 = (i+1) % N, i2 = (i+2) % N;
				p[i] = p1[i1] * p2[i2] - p1[i2] * p2[i1];
			}
		}
	}

	template<class T, class T1> static _FINLINE void Square(T& v, const T1* p1) {
		if(N <= LIM) {
			v = p1[N-1] * p1[N-1];
			Order<OPCLS, N-1, LIM>::SquareInternal(v, p1);
		} else {
			v = p1[N-1] * p1[N-1];
			for(intptr_t i = N-2; 0 <= i; --i)
				v += p1[i] * p1[i];
		}
	}

	template<class T, class T1> static _FINLINE void SquareInternal(T& v, const T1* p1) {
		v += p1[N-1] * p1[N-1];
		Order<OPCLS, N-1, LIM>::SquareInternal(v, p1);
	}
};

template<class OPCLS, intptr_t LIM> struct Order<OPCLS, 0, LIM> {
	template<class T> static _FINLINE void Op(T* p) {}
	template<class T, class T1> static _FINLINE void OpS(T* p, const T1& v) {}
	template<class T, class T1> static _FINLINE void Op(T* p, const T1* p1) {}
	template<class T, class T1, class T2> static _FINLINE void OpS(T* p, const T1* p1, const T2& v) {}
	template<class T, class T1, class T2> static _FINLINE void Op(T* p, const T1* p1, const T2* p2) {}
	template<class T1, class T2> static _FINLINE bool Equal(const T1* p1, const T2* p2) {
		return true;
	}
	template<class T1, class T2> static _FINLINE bool Equal1(const T1* p1, const T2& v) {
		return true;
	}
	template<class T1, class T2> static _FINLINE bool NotEqual(const T1* p1, const T2* p2) {
		return false;
	}
	template<class T1, class T2> static _FINLINE bool NotEqual1(const T1* p1, const T2& v) {
		return false;
	}
	template<class T1, class T2> static _FINLINE bool LessThan(const T1* p1, const T2* p2) {
		return false;
	}
	template<class T, class T1, class T2> static _FINLINE void DotInternal(T& v, const T1* p1, const T2* p2) {}
	template<class T, class T1, class T2> static _FINLINE void Cross(T* p, const T1* p1, const T2* p2) {}
	template<class T, class T1> static _FINLINE void SquareInternal(T& v, const T1* p1) {}
};

_JUNK_END

#endif

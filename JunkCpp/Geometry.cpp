// GraphNative.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include <windows.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <omp.h>
#include <emmintrin.h>
#include <algorithm>
#include <memory>
#include <new>

#define _JUNK_GEOMETRY_INLINE _FINLINE
#include "Geometry.h"

#undef min
#undef max

//#define ERR_CHECK_MODE

#ifdef ERR_CHECK_MODE
#define MAX_DATA_NUM 2000
#define MIN_DATA_NUM 1
#define LOOP_NUM 1
#else
#define MAX_DATA_NUM 100000
#define MIN_DATA_NUM 1000
#define LOOP_NUM 300
#endif

_JUNK_BEGIN

void (__fastcall *SearchMaxMinBest)(const double* p, intptr_t n, double& min, double& max);
int* (__fastcall *TransformLinIntBest)(const double* pSrc, intptr_t n, double scale, double translate, int* pDst);
int* (__fastcall *TransformIndexLinIntBest)(intptr_t iStartIndex, intptr_t n, double scale, double translate, int* pDst);
void (__fastcall *TransformLinPointToInt2Best)(const Vector2d* pSrc, intptr_t n, double scaleX, double translateX, double scaleY, double translateY, Vector2i* pDst);


_FINLINE BOOL IsSSE2Supported()
{
	int CpuInfo[4] = { -1 };
	__cpuid(CpuInfo, 1);
	return (CpuInfo[3] & (1 << 26)) ? 1 : 0;
}

_FINLINE BOOL IsSSE3Supported()
{
	int CpuInfo[4] = { -1 };
	__cpuid(CpuInfo, 1);
	return CpuInfo[2] & 0x01;
}

_FINLINE BOOL IsSSE41Supported()
{
	int CpuInfo[4] = { -1 };
	__cpuid(CpuInfo, 1);
	return (CpuInfo[2] & (1 << 19)) ? 1 : 0;
}

_FINLINE BOOL IsSSE42Supported()
{
	int CpuInfo[4] = { -1 };
	__cpuid(CpuInfo, 1);
	return (CpuInfo[2] & (1 << 20)) ? 1 : 0;
}


void __fastcall SearchMaxMinWithoutSimd(const double* p, intptr_t n, double& min, double& max)
{
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(20000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		intptr_t nthread = omp_get_max_threads() * omp_get_num_procs();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		double smin = DBL_MAX;
		double smax = -DBL_MAX;
		
		#pragma omp parallel for shared(smin, smax) 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			//::SetThreadAffinityMask(::GetCurrentThread(), 1 << j);
			double tmin;
			double tmax;
			const double* s = p + j * divsize;
			const double* e = j < (nthread - 1) ? s + divsize : p + n;
			tmin = tmax = *s;
			for(s++; s < e; s++)
			{ 
				double t = *s;
				if (t < tmin)
					tmin = t;
				else if (tmax < t)
					tmax = t;
			} 
			#pragma omp critical 
			{ 
				if(tmin < smin)
					smin = tmin;
				if(smax < tmax)
					smax = tmax;
			}
		}
		min = smin;
		max = smax;
	}
	else
	{
		//	シングルスレッド

		double tmax;
		double tmin;
		tmax = tmin = p[0];
		for (intptr_t i = 1; i < n; i++)
		{
			double t = p[i];
			if (t < tmin)
				tmin = t;
			else if (tmax < t)
				tmax = t;
		}
		max = tmax;
		min = tmin;
	}
}

void __fastcall SearchMaxMinWithSse2(const double* p, intptr_t n, double& min, double& max)
{
	if((n < 6) | ((intptr_t)p & 7))
	{
		//	データが6個未満またはデータの先頭が8、16以外のアライメントならSSE使えない
		SearchMaxMinWithoutSimd(p, n, min, max);
		return;
	}

	//	16アライメントに切り上げた開始ポインタ取得
	//	16アライメントに切り捨てた終了ポインタ取得
	const __m128d* ps16 = (__m128d*)((intptr_t)(p + 1) & ~(intptr_t)0xf);
	const double* pe = p + n;
	const __m128d* pe16 = (__m128d*)((intptr_t)pe & ~(intptr_t)0xf);

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(30000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		//	n を __m128d 単位の個数に変換
		n = pe16 - ps16;

		intptr_t nthread = omp_get_max_threads();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		
		//	開始位置で最大と最小を初期化
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16アライメントの場合
			smax = smin = *ps16;
		}
		else
		{
			//	8アライメントの場合
			__m128d t = { *p, *p };
			smax = smin = t;
		}

		//	スレッドで分割処理
		#pragma omp parallel for shared(smin, smax) 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			//::SetThreadAffinityMask(::GetCurrentThread(), 1 << j);
			__m128d tmax;
			__m128d tmin;
			const __m128d* s = ps16 + j * divsize;
			const __m128d* e = j < (nthread - 1) ? s + divsize : pe16;
			const __m128d* e4 = s + ((e - s) & ~(intptr_t)0x3);
			tmin = tmax = *s;
			for(; s < e4; s += 4)
			{
				__m128d t1 = s[0];
				__m128d t2 = s[1];
				__m128d t3 = s[2];
				__m128d t4 = s[3];
				tmax = _mm_max_pd(tmax, _mm_max_pd(_mm_max_pd(t1, t2), _mm_max_pd(t3, t4)));
				tmin = _mm_min_pd(tmin, _mm_min_pd(_mm_min_pd(t1, t2), _mm_min_pd(t3, t4)));
			}
			for(; s < e; s++)
			{ 
				__m128d t = *s;
				tmax = _mm_max_pd(tmax, t);
				tmin = _mm_min_pd(tmin, t);
			} 
			#pragma omp critical 
			{ 
				smax = _mm_max_pd(smax, tmax);
				smin = _mm_min_pd(smin, tmin);
			}
		}

		//	終了ポインタが16アライメントではない場合の特別処理
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t = { pe[-1], pe[-1] };
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		}

		//	上位8バイトと下位8バイトを比較して最大最小取得
		smax = _mm_max_pd(smax, _mm_shuffle_pd(smax, smax, 3));
		smin = _mm_min_pd(smin, _mm_shuffle_pd(smin, smin, 3));

		//	戻り値セット
		max = smax.m128d_f64[0];
		min = smin.m128d_f64[0];
	}
	else
	{
		//	シングルスレッド

		//	開始位置で最大と最小を初期化
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16アライメントの場合
			smax = smin = *ps16;
		}
		else
		{
			//	8アライメントの場合
			__m128d t = { *p, *p };
			smax = smin = t;
		}

		//	ループで処理
		const __m128d* e4 = ps16 + ((pe16 - ps16) & ~(intptr_t)0x3);
		for(; ps16 < e4; ps16 += 4)
		{
			__m128d t1 = ps16[0];
			__m128d t2 = ps16[1];
			__m128d t3 = ps16[2];
			__m128d t4 = ps16[3];
			smax = _mm_max_pd(smax, _mm_max_pd(_mm_max_pd(t1, t2), _mm_max_pd(t3, t4)));
			smin = _mm_min_pd(smin, _mm_min_pd(_mm_min_pd(t1, t2), _mm_min_pd(t3, t4)));
		}
		for(; ps16 < pe16; ps16++)
		{ 
			__m128d t = *ps16;
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		} 

		//	終了ポインタが16アライメントではない場合の特別処理
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t = { pe[-1], pe[-1] };
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		}

		//	上位8バイトと下位8バイトを比較して最大最小取得
		smax = _mm_max_pd(smax, _mm_shuffle_pd(smax, smax, 3));
		smin = _mm_min_pd(smin, _mm_shuffle_pd(smin, smin, 3));

		//	戻り値セット
		max = smax.m128d_f64[0];
		min = smin.m128d_f64[0];
	}
}

// 機能 : 最大最小を検索する
//
JUNKAPI void JUNKCALL SearchMaxMin(
	const double* p, // [in] 最大最小検索するデータ
	intptr_t n, // [in] 検索データ数、0 以下の場合は関数を呼び出さないようにしてください
	double& min, // [out] 最小値が返る
	double& max // [out] 最大値が返る
)
{
	if(SearchMaxMinBest == NULL)
	{
		if(IsSSE2Supported())
			SearchMaxMinBest = SearchMaxMinWithSse2; // SSE2使用可能
		else
			SearchMaxMinBest = SearchMaxMinWithoutSimd; // SSE2使用不可能
	}
	SearchMaxMinBest(p, n, min, max);
}

// 機能 : リングバッファ内から最大最小を検索する
//
JUNKAPI void JUNKCALL SearchMaxMinRing(
	const double* pBuffer, // [in] リングバッファの先頭ポインタ
	intptr_t nBufLen, // [in] リングバッファのサイズ、0 以下の場合は関数を呼び出さないようにしてください
	intptr_t iIndex, // [in] 検索開始位置のインデックス番号
	intptr_t n, // [in] 検索データ数、0 以下の場合は関数を呼び出さないようにしてください
	double& min, // [out] 最小値が返る
	double& max // [out] 最大値が返る
)
{
	iIndex %= nBufLen;
	if(iIndex + n < nBufLen)
	{
		// バッファを分割する必要が無い場合
		SearchMaxMin(pBuffer + iIndex, n, min, max);
	}
	else
	{
		// バッファを分割する必要がある場合
		intptr_t n1 = nBufLen - iIndex;
		intptr_t n2 = n - n1;
		double min1;
		double max1;
		double min2;
		double max2;
		SearchMaxMin(pBuffer + iIndex, n1, min1, max1);
		SearchMaxMin(pBuffer, n2, min2, max2);
		min = std::min(min1, min2);
		max = std::max(max1, max2);
	}
}


//==============================================================================
void (__fastcall *SearchPointMaxMinBest)(const Vector2d* p, intptr_t n, double& minX, double& maxX, double& minY, double& maxY);

// 機能: double 型２次元(X,Y)ベクトル配列を受け取り、XとYそれぞれの最大最小を検索する
//
void __fastcall SearchPointMaxMinWithoutSimd(
	const Vector2d* p, // [in] double 型２次元(X,Y)ベクトル配列の先頭ポインタ
	intptr_t n, // [in] ベクトル数
	double& minX, // [out] 最小X値が返る
	double& minY, // [out] 最小Y値が返る
	double& maxX, // [out] 最大X値が返る
	double& maxY // [out] 最大Y値が返る
)
{
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(20000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		intptr_t nthread = omp_get_max_threads() * omp_get_num_procs();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		double sminX = DBL_MAX;
		double sminY = DBL_MAX;
		double smaxX = -DBL_MAX;
		double smaxY = -DBL_MAX;
		
		#pragma omp parallel for shared(sminX, smaxX, sminY, smaxY) 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			//::SetThreadAffinityMask(::GetCurrentThread(), 1 << j);
			double tminX;
			double tminY;
			double tmaxX;
			double tmaxY;
			const Vector2d* s = p + j * divsize;
			const Vector2d* e = j < (nthread - 1) ? s + divsize : p + n;
			tminX = tmaxX = s->X();
			tminY = tmaxY = s->Y();
			for(s++; s < e; s++)
			{ 
				double t = s->X();
				if (t < tminX) tminX = t;
				else if (tmaxX < t) tmaxX = t;

				t = s->Y();
				if (t < tminY) tminY = t;
				else if (tmaxY < t) tmaxY = t;
			} 
			#pragma omp critical 
			{ 
				if(tminX < sminX) sminX = tminX;
				if(tminY < sminY) sminY = tminY;
				if(smaxX < tmaxX) smaxX = tmaxX;
				if(smaxY < tmaxY) smaxY = tmaxY;
			}
		}
		minX = sminX;
		minY = sminY;
		maxX = smaxX;
		maxY = smaxY;
	}
	else
	{
		//	シングルスレッド

		double tminX;
		double tminY;
		double tmaxX;
		double tmaxY;
		tmaxX = tminX = p->X();
		tmaxY = tminY = p->Y();
		for (intptr_t i = 1; i < n; i++)
		{
			double t = p[i].X();
			if (t < tminX) tminX = t;
			else if (tmaxX < t) tmaxX = t;

			t = p[i].Y();
			if (t < tminY) tminY = t;
			else if (tmaxY < t) tmaxY = t;
		}
		minX = tminX;
		minY = tminY;
		maxX = tmaxX;
		maxY = tmaxY;
	}
}

// 機能: double 型２次元(X,Y)ベクトル配列を受け取り、XとYそれぞれの最大最小を検索する
//
void __fastcall SearchPointMaxMinWithSse2(
	const Vector2d* p, // [in] double 型２次元(X,Y)ベクトル配列の先頭ポインタ
	intptr_t n, // [in] ベクトル数
	double& minX, // [out] 最小X値が返る
	double& minY, // [out] 最小Y値が返る
	double& maxX, // [out] 最大X値が返る
	double& maxY // [out] 最大Y値が返る
)
{
	if((n < 4) | ((intptr_t)p & 7))
	{
		//	データが6個未満またはデータの先頭が8、16以外のアライメントならSSE使えない
		SearchPointMaxMinWithoutSimd(p, n, minX, minY, maxX, maxY);
		return;
	}

	//	16アライメントに切り上げた開始ポインタ取得
	//	16アライメントに切り捨てた終了ポインタ取得
	const __m128d* ps16 = (__m128d*)((intptr_t)((char*)p + 8) & ~(intptr_t)0xf);
	const Vector2d* pe = p + n;
	const __m128d* pe16 = (__m128d*)((intptr_t)pe & ~(intptr_t)0xf);

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(30000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		//	n を __m128d 単位の個数に変換
		n = pe16 - ps16;

		intptr_t nthread = omp_get_max_threads();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		
		//	開始位置で最大と最小を初期化
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16アライメントの場合
			smax = smin = *ps16;
		}
		else
		{
			//	8アライメントの場合
			__m128d t = { p->Y(), p->X() }; // XとYが逆になる
			smax = smin = t;
		}

		//	スレッドで分割処理
		#pragma omp parallel for shared(smin, smax) 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			//::SetThreadAffinityMask(::GetCurrentThread(), 1 << j);
			__m128d tmax;
			__m128d tmin;
			const __m128d* s = ps16 + j * divsize;
			const __m128d* e = j < (nthread - 1) ? s + divsize : pe16;
			const __m128d* e4 = s + ((e - s) & ~(intptr_t)0x3);
			tmin = tmax = *s;
			for(; s < e4; s += 4)
			{
				__m128d t1 = s[0];
				__m128d t2 = s[1];
				__m128d t3 = s[2];
				__m128d t4 = s[3];
				tmax = _mm_max_pd(tmax, _mm_max_pd(_mm_max_pd(t1, t2), _mm_max_pd(t3, t4)));
				tmin = _mm_min_pd(tmin, _mm_min_pd(_mm_min_pd(t1, t2), _mm_min_pd(t3, t4)));
			}
			for(; s < e; s++)
			{ 
				__m128d t = *s;
				tmax = _mm_max_pd(tmax, t);
				tmin = _mm_min_pd(tmin, t);
			} 
			#pragma omp critical 
			{ 
				smax = _mm_max_pd(smax, tmax);
				smin = _mm_min_pd(smin, tmin);
			}
		}

		//	終了ポインタが16アライメントではない場合の特別処理
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t;
			t.m128d_f64[0] = pe[-2].Y();
			t.m128d_f64[1] = pe[-2].X();
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
			t.m128d_f64[0] = pe[-1].Y();
			t.m128d_f64[1] = pe[-1].X();
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);

			//	戻り値セット
			minX = smin.m128d_f64[1];
			minY = smin.m128d_f64[0];
			maxX = smax.m128d_f64[1];
			maxY = smax.m128d_f64[0];
		}
		else
		{
			//	戻り値セット
			minX = smin.m128d_f64[0];
			minY = smin.m128d_f64[1];
			maxX = smax.m128d_f64[0];
			maxY = smax.m128d_f64[1];
		}
	}
	else
	{
		//	シングルスレッド

		//	開始位置で最大と最小を初期化
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16アライメントの場合
			smax = smin = *ps16;
		}
		else
		{
			//	16アライメント以外の場合
			__m128d t = { p->Y(), p->X() };
			smax = smin = t;
		}

		//	ループで処理
		const __m128d* e4 = ps16 + ((pe16 - ps16) & ~(intptr_t)0x3);
		for(; ps16 < e4; ps16 += 4)
		{
			__m128d t1 = ps16[0];
			__m128d t2 = ps16[1];
			__m128d t3 = ps16[2];
			__m128d t4 = ps16[3];
			smax = _mm_max_pd(smax, _mm_max_pd(_mm_max_pd(t1, t2), _mm_max_pd(t3, t4)));
			smin = _mm_min_pd(smin, _mm_min_pd(_mm_min_pd(t1, t2), _mm_min_pd(t3, t4)));
		}
		for(; ps16 < pe16; ps16++)
		{ 
			__m128d t = *ps16;
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		} 

		//	終了ポインタが16アライメントではない場合の特別処理
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t;
			t.m128d_f64[0] = pe[-2].Y();
			t.m128d_f64[1] = pe[-2].X();
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
			t.m128d_f64[0] = pe[-1].Y();
			t.m128d_f64[1] = pe[-1].X();
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);

			//	戻り値セット
			maxX = smax.m128d_f64[1];
			minX = smin.m128d_f64[1];
			maxY = smax.m128d_f64[0];
			minY = smin.m128d_f64[0];
		}
		else
		{
			//	戻り値セット
			maxX = smax.m128d_f64[0];
			minX = smin.m128d_f64[0];
			maxY = smax.m128d_f64[1];
			minY = smin.m128d_f64[1];
		}
	}
}

//// 機能 : 最大最小を検索する、一番シンプルな実装
////
//void __fastcall _SearchPointMaxMinBase(
//	const Vector2D* p, // [in] 最大最小検索するデータ
//	intptr_t n, // [in] データ数
//	double& minX, // [out] 最小X値が返る
//	double& minY, // [out] 最小Y値が返る
//	double& maxX, // [out] 最大X値が返る
//	double& maxY // [out] 最大Y値が返る
//)
//{
//	double tminX;
//	double tminY;
//	double tmaxX;
//	double tmaxY;
//	tmaxX = tminX = p->X();
//	tmaxY = tminY = p->Y();
//	for (intptr_t i = 1; i < n; i++)
//	{
//		double t = p[i].X();
//		if (t < tminX)
//			tminX = t;
//		else if (tmaxX < t)
//			tmaxX = t;
//		t = p[i].Y();
//		if (t < tminY)
//			tminY = t;
//		else if (tmaxY < t)
//			tmaxY = t;
//	}
//	minX = tminX;
//	minY = tminY;
//	maxX = tmaxX;
//	maxY = tmaxY;
//}

// 機能: double 型２次元(X,Y)ベクトル配列を受け取り、XとYそれぞれの最大最小を検索する
//
JUNKAPI void JUNKCALL SearchPointMaxMin(
	const Vector2d* p, // [in] 最大最小検索するデータ
	intptr_t n, // [in] データ数
	double& minX, // [out] 最小X値が返る
	double& minY, // [out] 最小Y値が返る
	double& maxX, // [out] 最大X値が返る
	double& maxY // [out] 最大Y値が返る
)
{
	if(SearchMaxMinBest == NULL)
	{
		if(0) //IsSSE2Supported())
			SearchPointMaxMinBest = SearchPointMaxMinWithSse2; // SSE2使用可能
		else
			SearchPointMaxMinBest = SearchPointMaxMinWithoutSimd; // SSE2使用不可能
	}
	SearchPointMaxMinBest(p, n, minX, minY, maxX, maxY);

	//double tminX;
	//double tmaxX;
	//double tminY;
	//double tmaxY;
	//_SearchPointMaxMinBase(p, n, tminX, tminY, tmaxX, tmaxY);
	//if(minX != tminX ||
	//   minY != tminY || 
	//   maxX != tmaxX ||
	//   maxY != tmaxY)
	//{
	//	char buf[256];
	//	_snprintf(buf, 255, "p=%p,ofs=%d,n=%d\n<%f,%f>-<%f,%f> : <%f,%f>-<%f,%f>", p, (intptr_t)p % 16, n, minX, minY, maxX, maxY, tminX, tminY, tmaxX, tmaxY);
	//	::MessageBoxA(NULL, buf, "", MB_OK);
	//}
}


//==============================================================================
// 機能 : 線形変換を行う
//
JUNKAPI void JUNKCALL TransformLin(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	double* pDst // [out] 変換後のデータ
)
{
	if(20000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
	{
		//	マルチスレッド

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = pSrc[i] * scale + translate;
	}
	else
	{
		//	シングルスレッド

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = pSrc[i] * scale + translate;
	}
}

// 機能 : 線形変換後intに変換を行う
//
JUNKAPI void JUNKCALL TransformLinInt(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ
)
{
	if(10000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
	{
		//	マルチスレッド

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	シングルスレッド

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
}

// 機能 : 非線形変換(Log,Pow)を含む変換を行う
//
JUNKAPI void JUNKCALL TransformNonLin(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTis, // [in] 変換情報配列
	intptr_t nTransform, // [in] pTis の要素数
	double* pDst // [out] 変換後のデータ
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	線形変換のみの場合

		TransformLin(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(500 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = val;
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = val;
			}
		}
	}
}

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
JUNKAPI void JUNKCALL TransformNonLinInt(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTis, // [in] 変換情報配列
	intptr_t nTransform, // [in] pTis の要素数
	int* pDst // [out] 変換後のデータ
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	線形変換のみの場合

		TransformLinInt(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(300 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}
	}
}

// 機能 : 線形変換後intに変換を行う
//        出力の pDst は POINT 構造体(8バイト)の配列と見なし、先頭4バイトに値を書き込み残りの4バイトはそのまま残す
//
JUNKAPI void JUNKCALL TransformLinToInt2(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ
)
{
	if(10000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
	{
		//	マルチスレッド

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i * 2] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	シングルスレッド

		for (intptr_t i = 0; i < n; i++)
			pDst[i * 2] = (int)(pSrc[i] * scale + translate);
	}
}

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//        出力の pDst は POINT 構造体(8バイト)の配列と見なし、先頭4バイトに値を書き込み残りの4バイトはそのまま残す
//
JUNKAPI void JUNKCALL TransformNonLinToInt2(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTis, // [in] 変換情報配列
	intptr_t nTransform, // [in] pTis の要素数
	int* pDst // [out] 変換後のデータ
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	線形変換のみの場合

		TransformLinToInt2(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(300 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i * 2] = (int)val;
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i * 2] = (int)val;
			}
		}
	}
}

// 機能 : SIMDを使用せずに線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformLinIntWithoutSimd(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ
)
{
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(10000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	シングルスレッド

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}

	return pDst;
}

// 機能 : SIMDを使用して線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformLinSse2Int(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	//	8データ以上でない場合又は、8、16バイトアライメントでない場合はSSEで処理できない
	if((n < 8) | ((intptr_t)pSrc & 7))
		return _TransformLinIntWithoutSimd(pSrc, n, scale, translate, pDst);

	const double* pSrcEnd = pSrc + n; // 変換元データの終了ポインタ
	const __m128d* pSrc16 = (__m128d*)(((intptr_t)pSrc + 15) & ~(intptr_t)0xf); // 16バイト境界に切り上げた変換元データポインタ
	intptr_t n4 = (((double*)pSrcEnd - (double*)pSrc16) >> 1) & ~(intptr_t)3; // pSrc16 から4データ単位処理するデータ数
	__m128i* pDst16 = (__m128i*)(((intptr_t)pDst + 15) & ~(intptr_t)0xf); // 16バイト境界に切り上げた変換先データポインタ
	int* pDstStart; // 実際に変換先データが書き込まれる先頭ポインタ、SSEで16バイト単位で処理するために先頭ポインタを調整する必要がある

	if((intptr_t)pSrc == (intptr_t)pSrc16)
	{
		//	変換元データが既に16バイト境界に沿っている場合
		pDstStart = (int*)pDst16;
	}
	else
	{
		//	変換元データが16バイト境界に沿っていない場合

		//	先頭1データを変換する
		pDst16++;
		pDstStart = (int*)pDst16 - 1;
		*pDstStart = (int)(pSrc[0] * scale + translate);
	}

	__m128d s = { scale, scale };
	__m128d t = { translate, translate };
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(25000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		//	マルチスレッド

		#pragma omp parallel for
		for(intptr_t i = 0; i < n4; i += 2)
			pDst16[i >> 1] = _mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i], s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i + 1], s), t)), 0x4e)
						);
	}
	else
	{
		//	シングルスレッド

		for(intptr_t i = 0; i < n4; i += 2)
			pDst16[i >> 1] = _mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i], s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i + 1], s), t)), 0x4e)
						);
	}

	//	終端の4データ単位で処理できない部分を変換する
	pSrc = (double*)(pSrc16 + n4);
	pDst = (int*)(pDst16 + (n4 >> 1));
	for(; pSrc < pSrcEnd; pSrc++, pDst++)
		*pDst = (int)(*pSrc * scale + translate);

	return pDstStart;
}

// 機能 : double 型２次元(X,Y)ベクトル配列を受け取り、線形変換して int 型２次元(X,Y)ベクトル配列に出力する
//
void __fastcall _TransformLinPointToInt2WithoutSimd(
	const Vector2d* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scaleX, // [in] X座標スケーリング値
	double translateX, // [in] X座標平行移動値
	double scaleY, // [in] Y座標スケーリング値
	double translateY, // [in] Y座標平行移動値
	Vector2i* pDst // [out] 変換後のデータ
)
{
	if(10000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
	{
		//	マルチスレッド

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
		{
			const Vector2d& s = pSrc[i];
			Vector2i& d = pDst[i];
			d.X() = (int)(s.X() * scaleX + translateX);
			d.Y() = (int)(s.Y() * scaleY + translateY);
		}
	}
	else
	{
		//	シングルスレッド

		for (intptr_t i = 0; i < n; i++)
		{
			const Vector2d& s = pSrc[i];
			Vector2i& d = pDst[i];
			d.X() = (int)(s.X() * scaleX + translateX);
			d.Y() = (int)(s.Y() * scaleY + translateY);
		}
	}
}

// 機能 : double 型２次元(X,Y)ベクトル配列を受け取り、SIMDを使用して線形変換して int 型２次元(X,Y)ベクトル配列に出力する
//
void __fastcall _TransformLinPointToInt2WithSse2(
	const Vector2d* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scaleX, // [in] X座標スケーリング値
	double translateX, // [in] X座標平行移動値
	double scaleY, // [in] Y座標スケーリング値
	double translateY, // [in] Y座標平行移動値
	Vector2i* pDst // [out] 変換後のデータ
)
{
	//	8データ以上でない場合はSSEで処理できない
	if(n < 8)
		return _TransformLinPointToInt2WithoutSimd(pSrc, n, scaleX, translateX, scaleY, translateY, pDst);

	//	ループで処理するポイント数、ループ内では32バイト単位で処理する
	intptr_t nLoopPoints = n & ~(intptr_t)1;

	//	ループ処理の先頭ポインタ
	const __m128d* pSrc16 = (__m128d*)pSrc;
	__m128i* pDst16 = (__m128i*)pDst;

	//	変換係数設定
	__m128d s = { scaleX, scaleY };
	__m128d t = { translateX, translateY };

	if(25000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
	{
		//	マルチスレッド

		if((intptr_t)pSrc & 15)
		{
			if((intptr_t)pDst & 15)
			{
				//	ソースとデスティネーション両方が16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform MT SU DU\r\n");

				#pragma omp parallel for
				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_storeu_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
			else
			{
				//	ソースが16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform MT SU DA\r\n");

				#pragma omp parallel for
				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_store_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
		}
		else
		{
			if((intptr_t)pDst & 15)
			{
				//	デスティネーションが16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform MT SA DU\r\n");

				#pragma omp parallel for
				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_storeu_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
			else
			{
				//	ソースとデスティネーション両方が16バイトアライメントされている場合

				//::OutputDebugStringA("Transform MT SA DA\r\n");

				#pragma omp parallel for
				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_store_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
		}
	}
	else
	{
		//	シングルスレッド

		if((intptr_t)pSrc & 15)
		{
			if((intptr_t)pDst & 15)
			{
				//	ソースとデスティネーション両方が16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform ST SU DU\r\n");


				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_storeu_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
			else
			{
				//	ソースが16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform ST SU DA\r\n");


				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_store_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_loadu_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
		}
		else
		{
			if((intptr_t)pDst & 15)
			{
				//	デスティネーションが16バイトアライメントされていない場合

				//::OutputDebugStringA("Transform ST SA DU\r\n");


				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_storeu_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
			else
			{
				//	ソースとデスティネーション両方が16バイトアライメントされている場合

				//::OutputDebugStringA("Transform ST SA DA\r\n");


				for(intptr_t i = 0; i < nLoopPoints; i += 2)
					_mm_store_si128(
						&pDst16[i >> 1],
						_mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i]), s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(_mm_load_pd((double*)&pSrc16[i + 1]), s), t)), 0x4e)
						)
					);
			}
		}
	}

	//	ループで処理できなかったポイントを処理
	if(n & 1)
	{
		const Vector2d& s = pSrc[nLoopPoints];
		Vector2i& d = pDst[nLoopPoints];
		d.X() = (int)(s.X() * scaleX + translateX);
		d.Y() = (int)(s.Y() * scaleY + translateY);
	}
}

// 機能 : double 型２次元(X,Y)ベクトル配列を受け取り、変換して int 型２次元(X,Y)ベクトル配列に出力する
//
JUNKAPI void JUNKCALL TransformLinPointDToPointI(
	const Vector2d* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scaleX, // [in] X座標スケーリング値
	double translateX, // [in] X座標平行移動値
	double scaleY, // [in] Y座標スケーリング値
	double translateY, // [in] Y座標平行移動値
	Vector2i* pDst // [out] 変換後のデータ
)
{
	if(TransformLinPointToInt2Best == NULL)
	{
		if(IsSSE2Supported())
			TransformLinPointToInt2Best = _TransformLinPointToInt2WithSse2; // SSE2使用可能
		else
			TransformLinPointToInt2Best = _TransformLinPointToInt2WithoutSimd; // SSE2使用不可能
	}
	TransformLinPointToInt2Best(pSrc, n, scaleX, translateX, scaleY, translateY, pDst);
}

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
JUNKAPI void JUNKCALL TransformNonLinPointDToPointI(
	const Vector2d* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTisX, // [in] X座標変換情報配列
	intptr_t nTransformX, // [in] pTisX の要素数
	const TransformInfo* pTisY, // [in] Y座標変換情報配列
	intptr_t nTransformY, // [in] pTisY の要素数
	Vector2i* pDst // [out] 変換後のデータ
)
{
	if(nTransformX == 1 && nTransformY == 1 && !pTisX->LogBeforeLinear && !pTisX->PowAfterLinear && !pTisY->LogBeforeLinear && !pTisY->PowAfterLinear)
	{
		//	線形変換のみの場合

		TransformLinPointDToPointI(pSrc, n, pTisX->Transform.Scale, pTisX->Transform.Translate, pTisY->Transform.Scale, pTisY->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(300 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				Vector2d val = pSrc[i];
				for (intptr_t j = 0; j < nTransformX; j++)
				{
					const TransformInfo& ti = pTisX[j];
					if (ti.LogBeforeLinear)
						val.X() = Transform::Log10(val.X());
					val.X() = val.X() * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val.X() = Transform::Pow10(val.X());
				}
				for (intptr_t j = 0; j < nTransformY; j++)
				{
					const TransformInfo& ti = pTisY[j];
					if (ti.LogBeforeLinear)
						val.Y() = Transform::Log10(val.Y());
					val.Y() = val.Y() * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val.Y() = Transform::Pow10(val.Y());
				}
				Vector2i& d = pDst[i];
				d.X() = (int)val.X();
				d.Y() = (int)val.Y();
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				Vector2d val = pSrc[i];
				for (intptr_t j = 0; j < nTransformX; j++)
				{
					const TransformInfo& ti = pTisX[j];
					if (ti.LogBeforeLinear)
						val.X() = Transform::Log10(val.X());
					val.X() = val.X() * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val.X() = Transform::Pow10(val.X());
				}
				for (intptr_t j = 0; j < nTransformY; j++)
				{
					const TransformInfo& ti = pTisY[j];
					if (ti.LogBeforeLinear)
						val.Y() = Transform::Log10(val.Y());
					val.Y() = val.Y() * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val.Y() = Transform::Pow10(val.Y());
				}
				Vector2i& d = pDst[i];
				d.X() = (int)val.X();
				d.Y() = (int)val.Y();
			}
		}
	}
}

// 機能 : 線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformLinInt(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	if(TransformLinIntBest == NULL)
	{
		if(IsSSE2Supported())
			TransformLinIntBest = _TransformLinSse2Int; // SSE2使用可能
		else
			TransformLinIntBest = _TransformLinIntWithoutSimd; // SSE2使用不可能
	}
	return TransformLinIntBest(pSrc, n, scale, translate, pDst);
}

// 機能 : 非線形変換(Log,Pow)後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformNonLinInt(
	const double* pSrc, // [in] 変換元のデータ
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTis, // [in] 変換情報配列
	intptr_t nTransform, // [in] pTis の要素数
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	線形変換のみの場合

		return _TransformLinInt(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(300 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				double val = pSrc[i];
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}

		return pDst;
	}
}

// 機能 : インデックス番号を1ずつ加算していき、それを線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformIndexLinIntWithoutSimd(
	intptr_t iStartIndex, // [in] インデックス番号開始値
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ
)
{
	double dIdx = (double)iStartIndex;
	for (intptr_t i = 0; i < n; i++)
	{
		pDst[i] = (int)(dIdx * scale + translate);
		dIdx += 1.0;
	}
	return pDst;
}

// 機能 : インデックス番号を1ずつ加算していき、それを線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformIndexLinIntSse2(
	intptr_t iStartIndex, // [in] インデックス番号開始値
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	//	8データ以上でない場合はSSEで処理できない
	if(n < 8)
		return _TransformIndexLinIntWithoutSimd(iStartIndex, n, scale, translate, pDst);

	__m128d s = { scale, scale }; // スケーリング値
	__m128d t = { translate, translate }; // 平行移動値
	__m128d inc4 = { 4, 4 }; // 4データ処理時のインデックス番号増加値
	__m128i* pDst16 = (__m128i*)(((intptr_t)pDst + 15) & ~(intptr_t)0xf); // 16バイト境界に切り上げた変換先データポインタ
	intptr_t n128i = n >> 2; // __m128i(4つの整数のパック)データ数、4点単位で処理するためデータ数を4で割る

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(35000 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
#endif
	{
		intptr_t nthread = omp_get_max_threads();
		if(n128i < nthread)
			nthread = n128i;

		intptr_t divsize = n128i / nthread; // スレッド毎に処理する __m128i データ数(1つの __m128i データには4つの整数データがパックされている)

		#pragma omp parallel for 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			intptr_t iS = iStartIndex + j * (divsize << 2); // このスレッドでの開始インデックス番号
			__m128d idx1 = { (double)iS, (double)(iS + 1) };
			__m128d idx2;
			{
				__m128d inc2 = { 2, 2 };
				idx2 = _mm_add_pd(idx1, inc2);
			}

			__m128i* p = pDst16 + j * divsize; // このスレッドでの書き込み開始ポインタ
			__m128i* e = j < (nthread - 1) ? p + divsize : pDst16 + n128i; // このスレッドでの書き込み終了ポインタ
			for(; p < e; p++)
			{
				*p = _mm_or_si128(
						_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(idx1, s), t)),
						_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(idx2, s), t)), 0x4e)
					);
				idx1 = _mm_add_pd(idx1, inc4);
				idx2 = _mm_add_pd(idx2, inc4);
			}
		}

		pDst = (int*)(pDst16 + n128i);
		int* pDstEnd = (int*)pDst16 + n;
		double dIdx = (double)(iStartIndex + (n128i << 2));
		for(; pDst < pDstEnd; pDst++)
		{
			*pDst = (int)(dIdx * scale + translate);
			dIdx += 1.0;
		}
	}
	else
	{
		__m128d idx1 = { (double)iStartIndex, (double)(iStartIndex + 1) };
		__m128d idx2;
		{
			__m128d inc2 = { 2, 2 };
			idx2 = _mm_add_pd(idx1, inc2);
		}

		__m128i* p = pDst16; // 書き込み開始ポインタ
		__m128i* e = p + n128i; // 書き込み終了ポインタ
		for(; p < e; p++)
		{
			*p = _mm_or_si128(
					_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(idx1, s), t)),
					_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(idx2, s), t)), 0x4e)
				);
			idx1 = _mm_add_pd(idx1, inc4);
			idx2 = _mm_add_pd(idx2, inc4);
		}

		pDst = (int*)(pDst16 + n128i);
		int* pDstEnd = (int*)pDst16 + n;
		double dIdx = (double)(iStartIndex + (n128i << 2));
		for(; pDst < pDstEnd; pDst++)
		{
			*pDst = (int)(dIdx * scale + translate);
			dIdx += 1.0;
		}
	}

	return (int*)pDst16;
}


// 機能 : インデックス番号を1ずつ加算していき、それを線形変換後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformIndexLinInt(
	intptr_t iStartIndex, // [in] インデックス番号開始値
	intptr_t n, // [in] 変換データ数
	double scale, // [in] スケーリング値
	double translate, // [in] 平行移動値
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	if(TransformIndexLinIntBest == NULL)
	{
		if(IsSSE2Supported())
			TransformIndexLinIntBest = _TransformIndexLinIntSse2; // SSE2使用可能
		else
			TransformIndexLinIntBest = _TransformIndexLinIntWithoutSimd; // SSE2使用不可能
	}
	return TransformIndexLinIntBest(iStartIndex, n, scale, translate, pDst);
}

// 機能 : インデックス番号を1ずつ加算していき、それを非線形変換(Log,Pow)後intに変換を行う
//
// 返り値 : 実際に変換後データが書き込まれる先頭ポインタ
//
int* __fastcall _TransformIndexNonLinInt(
	intptr_t iStartIndex, // [in] インデックス番号開始値
	intptr_t n, // [in] 変換データ数
	const TransformInfo* pTis, // [in] 変換情報配列
	intptr_t nTransform, // [in] pTis の要素数
	int* pDst // [out] 変換後のデータ、 16バイト境界に合わせて処理されるため n + 8 要素分の領域が必要
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	線形変換のみの場合

		return _TransformIndexLinInt(iStartIndex, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	非線形変換を含む場合

		if(300 <= n) // TODO: 環境によって違うのでどの値がベストか調査する
		{
			//	マルチスレッド

			#pragma omp parallel for
			for (intptr_t i = 0; i < n; i++)
			{
				double val = (double)(iStartIndex + i);
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}
		else
		{
			//	シングルスレッド

			for (intptr_t i = 0; i < n; i++)
			{
				double val = (double)(iStartIndex + i);
				for (intptr_t j = 0; j < nTransform; j++)
				{
					const TransformInfo& ti = pTis[j];
					if (ti.LogBeforeLinear)
						val = Transform::Log10(val);
					val = val * ti.Transform.Scale + ti.Transform.Translate;
					if (ti.PowAfterLinear)
						val = Transform::Pow10(val);
				}
				pDst[i] = (int)val;
			}
		}
	}

	return pDst;
}

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
JUNKAPI intptr_t JUNKCALL TransformForDraw(
	const TransformInfo* pTisX, // [in] X軸変換情報配列
	intptr_t nTisX, // [in] pTisX の要素数
	const TransformInfo* pTisY, // [in] Y軸変換情報配列
	intptr_t nTisY, // [in] pTisY の要素数
	const double* pSrcY, // [in] 変換元のY軸値データ
	intptr_t iStartIndexX, // [in] X軸値計算用のインデックス番号開始値
	intptr_t n, // [in] 変換データ数
	Vector2i* pDst // [out] 変換後のXY値データ、必要な要素数は座標変換後の値により異なる、座標変換後のX値範囲*4+4程度必要
)
{
	if(n <= 0)
		return 0;

	try
	{
		int* pTmpX;
		int* pTmpY;
		std::auto_ptr<int> bufX(new int[n + 8]);
		std::auto_ptr<int> bufY(new int[n + 8]);

		//	インデックス番号からＸ座標値を生成する
		pTmpX = _TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, bufX.get());

		//	Ｙ座標値を変換する
		pTmpY = _TransformNonLinInt(pSrcY, n, pTisY, nTisY, bufY.get());

		int p1x;
		int p1y;
		int p2x;
		int p2y;
		intptr_t nData = 0; // 変換後データ数
		int s; // 前回のＸ座標での開始Ｙ値
		int min; // 前回のＸ座標での最小Ｙ値
		int max; // 前回のＸ座標での最大Ｙ値

		p1x = p2x = pTmpX[0];
		p1y = p2y = pTmpY[0];
		s = min = max = p1y;

		for(intptr_t i = 1; i < n; i++)
		{
			p2x = pTmpX[i];
			p2y = pTmpY[i];

			if(p1x == p2x)
			{
				if(max < p2y)
					max = p2y;
				else if(p2y < min)
					min = p2y;
			}
			else
			{
				if(min == max)
				{
					//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
					pDst[nData].X() = p1x;
					pDst[nData].Y() = min;
					nData++;
				}
				else
				{
					//	前のＸ座標の最大最小Ｙ値が異なる場合は
					//	開始値、最大、最小、終了値を追加する
					pDst[nData].X() = p1x;
					pDst[nData].Y() = s;
					nData++;
					if(s != min)
					{
						pDst[nData].X() = p1x;
						pDst[nData].Y() = min;
						nData++;
					}
					if(s != max)
					{
						pDst[nData].X() = p1x;
						pDst[nData].Y() = max;
						nData++;
					}
					if(p1y != min && p1y != max)
					{
						pDst[nData].X() = p1x;
						pDst[nData].Y() = p1y;
						nData++;
					}
				}

				//	開始Ｙ値、最小、最大値初期化
				s = min = max = p2y;
			}

			p1x = p2x;
			p1y = p2y;
		}

		if(min == max)
		{
			//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
			pDst[nData].X() = p2x;
			pDst[nData].Y() = p2y;
			nData++;
		}
		else
		{
			//	前のＸ座標の最大最小Ｙ値が異なる場合は
			//	開始値、最大、最小、終了値を追加する
			pDst[nData].X() = p2x;
			pDst[nData].Y() = s;
			nData++;
			if(s != min)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = min;
				nData++;
			}
			if(s != max)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = max;
				nData++;
			}
			if(p2y != min && p2y != max)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = p2y;
				nData++;
			}
		}

		return nData;
	}
	catch(std::bad_alloc)
	{
		return -1;
	}
}

// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する(リングバッファ版)
//	Y軸値データは pSrcY + iStartIndexY から取得開始され、pSrcY + nSrcYBufLen を超えたら pSrcY に戻って取得が続けられる
//
// 返り値 : 変換後のデータ数、負数が返ればエラー
//		-1=メモリ不足
//
JUNKAPI intptr_t JUNKCALL TransformForDrawRing(
	const TransformInfo* pTisX, // [in] X軸変換情報配列
	intptr_t nTisX, // [in] pTisX の要素数
	const TransformInfo* pTisY, // [in] Y軸変換情報配列
	intptr_t nTisY, // [in] pTisY の要素数
	const double* pSrcY, // [in] 変換元のY軸値データ(Y軸データバッファの先頭アドレス)
	intptr_t nSrcYBufLen, // [in] pSrcY のバッファのサイズ(データ数)
	intptr_t iStartIndexX, // [in] X軸値計算用のインデックス番号開始値
	intptr_t iStartIndexY, // [in] Y軸値計算用のインデックス番号開始値
	intptr_t n, // [in] 変換データ数
	Vector2i* pDst // [out] 変換後のXY値データ、必要な要素数は座標変換後の値により異なる、座標変換後のX値範囲*4+4程度必要
)
{
	if(n <= 0)
		return 0;

	try
	{
		int* pTmpX;
		intptr_t nTmpYs;
		int* pTmpYs[2];
		intptr_t nTmpYCounts[2];
		std::auto_ptr<int> bufX(new int[n + 8]);
		std::auto_ptr<int> bufYs[2];

		//	インデックス番号からＸ座標値を生成する
		pTmpX = _TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, bufX.get());

		//	Ｙ座標値を変換する
		intptr_t copyStart = iStartIndexY % nSrcYBufLen;
		intptr_t copyEnd = copyStart + n;
		if(copyEnd <= nSrcYBufLen)
		{
			//	１回のデータコピーで済む場合は普通に座標変換コピー
			nTmpYs = 1;
			nTmpYCounts[0] = n;
			bufYs[0].reset(new int[nTmpYCounts[0] + 8]);
			pTmpYs[0] = _TransformNonLinInt(pSrcY + copyStart, n, pTisY, nTisY, bufYs[0].get());
		}
		else
		{
			//	２回に分けてデータコピーする必要がある場合
			nTmpYs = 2;
			nTmpYCounts[0] = nSrcYBufLen - copyStart;
			nTmpYCounts[1] = copyEnd - nSrcYBufLen;
			bufYs[0].reset(new int[nTmpYCounts[0] + 8]);
			bufYs[1].reset(new int[nTmpYCounts[1] + 8]);
			pTmpYs[0] = _TransformNonLinInt(pSrcY + copyStart, nTmpYCounts[0], pTisY, nTisY, bufYs[0].get());
			pTmpYs[1] = _TransformNonLinInt(pSrcY, nTmpYCounts[1], pTisY, nTisY, bufYs[1].get());
		}

		int p1x;
		int p1y;
		int p2x;
		int p2y;
		intptr_t nData = 0; // 変換後データ数
		int s; // 前回のＸ座標での開始Ｙ値
		int min; // 前回のＸ座標での最小Ｙ値
		int max; // 前回のＸ座標での最大Ｙ値

		p1x = p2x = pTmpX[0];
		p1y = p2y = pTmpYs[0][0];
		s = min = max = p1y;

		intptr_t iX = 1; // 座標変換後のX座標値配列のインデックス番号

		//	リングバッファなので最大２回に分割される
		//	そのバッファ毎に処理をする
		for(intptr_t iBufY = 0; iBufY < nTmpYs; iBufY++)
		{
			int* pTmpY = pTmpYs[iBufY];
			intptr_t nCount = nTmpYCounts[iBufY];
			for(intptr_t iY = (iBufY == 0 ? 1 : 0); iY < nCount; iY++, iX++)
			{
				p2x = pTmpX[iX];
				p2y = pTmpY[iY];

				if(p1x == p2x)
				{
					if(max < p2y)
						max = p2y;
					else if(p2y < min)
						min = p2y;
				}
				else
				{
					if(min == max)
					{
						//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
						pDst[nData].X() = p1x;
						pDst[nData].Y() = min;
						nData++;
					}
					else
					{
						//	前のＸ座標の最大最小Ｙ値が異なる場合は
						//	開始値、最大、最小、終了値を追加する
						pDst[nData].X() = p1x;
						pDst[nData].Y() = s;
						nData++;
						if(s != min)
						{
							pDst[nData].X() = p1x;
							pDst[nData].Y() = min;
							nData++;
						}
						if(s != max)
						{
							pDst[nData].X() = p1x;
							pDst[nData].Y() = max;
							nData++;
						}
						if(p1y != min && p1y != max)
						{
							pDst[nData].X() = p1x;
							pDst[nData].Y() = p1y;
							nData++;
						}
					}

					//	開始Ｙ値、最小、最大値初期化
					s = min = max = p2y;
				}

				p1x = p2x;
				p1y = p2y;
			}
		}

		if(min == max)
		{
			//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
			pDst[nData].X() = p2x;
			pDst[nData].Y() = p2y;
			nData++;
		}
		else
		{
			//	前のＸ座標の最大最小Ｙ値が異なる場合は
			//	開始値、最大、最小、終了値を追加する
			pDst[nData].X() = p2x;
			pDst[nData].Y() = s;
			nData++;
			if(s != min)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = min;
				nData++;
			}
			if(s != max)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = max;
				nData++;
			}
			if(p2y != min && p2y != max)
			{
				pDst[nData].X() = p2x;
				pDst[nData].Y() = p2y;
				nData++;
			}
		}

		return nData;
	}
	catch(std::bad_alloc)
	{
		return -1;
	}
}


//// 機能 : 描画用に整数座標に変換する、同じ座標に無駄な描画を行わないように変換する
////
//// 返り値 : 変換後のデータ数
////
//JUNKAPI intptr_t JUNKCALL TransformForDraw(
//	const TransformInfo* pTisX, // [in] X軸変換情報配列
//	intptr_t nTisX, // [in] pTisX の要素数
//	const TransformInfo* pTisY, // [in] Y軸変換情報配列
//	intptr_t nTisY, // [in] pTisY の要素数
//	const double* pSrcY, // [in] 変換元のY軸値データ
//	intptr_t iStartIndexX, // [in] X軸値計算用のインデックス番号開始値
//	intptr_t n, // [in] 変換データ数
//	int* pDstX, // [out] 変換後のX軸値データ、最低 n 要素分のサイズが必要
//	int* pDstY // [out] 変換後のY軸値データ、最低 n 要素分のサイズが必要
//)
//{
//	if(n <= 0)
//		return 0;
//
//	int* pTmpX;
//	int* pTmpY;
//	std::auto_ptr<int> bufX(pTmpX = new int[n]);
//	std::auto_ptr<int> bufY(pTmpY = new int[n]);
//
//	//	インデックス番号からＸ座標値を生成する
//	TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, pTmpX);
//
//	//	Ｙ座標値を変換する
//	TransformNonLinInt(pSrcY, n, pTisY, nTisY, pTmpY);
//
//	int p1x;
//	int p1y;
//	int p2x;
//	int p2y;
//	intptr_t nData = 0; // 変換後データ数
//	int s; // 前回のＸ座標での開始Ｙ値
//	int min; // 前回のＸ座標での最小Ｙ値
//	int max; // 前回のＸ座標での最大Ｙ値
//
//	p1x = p2x = pTmpX[0];
//	p1y = p2y = pTmpY[0];
//	s = min = max = p1y;
//
//	for(intptr_t i = 1; i < n; i++)
//	{
//		p2x = pTmpX[i];
//		p2y = pTmpY[i];
//
//		if(p1x == p2x)
//		{
//			if(max < p2y)
//				max = p2y;
//			else if(p2y < min)
//				min = p2y;
//		}
//		else
//		{
//			if(min == max)
//			{
//				//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
//				pDstX[nData] = p1x;
//				pDstY[nData] = min;
//				nData++;
//			}
//			else
//			{
//				//	前のＸ座標の最大最小Ｙ値が異なる場合は
//				//	開始値、最大、最小、終了値を追加する
//				pDstX[nData] = p1x;
//				pDstY[nData] = s;
//				nData++;
//				if(s != min)
//				{
//					pDstX[nData] = p1x;
//					pDstY[nData] = min;
//					nData++;
//				}
//				if(s != max)
//				{
//					pDstX[nData] = p1x;
//					pDstY[nData] = max;
//					nData++;
//				}
//				if(p1y != min && p1y != max)
//				{
//					pDstX[nData] = p1x;
//					pDstY[nData] = p1y;
//					nData++;
//				}
//			}
//
//			//	開始Ｙ値、最小、最大値初期化
//			s = min = max = p2y;
//		}
//
//		p1x = p2x;
//		p1y = p2y;
//	}
//
//	if(min == max)
//	{
//		//	前のＸ座標の最大最小Ｙ値が同じ場合は１点だけの追加
//		pDstX[nData] = p2x;
//		pDstY[nData] = p2y;
//		nData++;
//	}
//	else
//	{
//		//	前のＸ座標の最大最小Ｙ値が異なる場合は
//		//	開始値、最大、最小、終了値を追加する
//		pDstX[nData] = p2x;
//		pDstY[nData] = s;
//		nData++;
//		if(s != min)
//		{
//			pDstX[nData] = p2x;
//			pDstY[nData] = min;
//			nData++;
//		}
//		if(s != max)
//		{
//			pDstX[nData] = p2x;
//			pDstY[nData] = max;
//			nData++;
//		}
//		if(p2y != min && p2y != max)
//		{
//			pDstX[nData] = p2x;
//			pDstY[nData] = p2y;
//			nData++;
//		}
//	}
//
//	return nData;
//}

// 機能 : 複数頂点で構成されるラインと指定ポイントとの接触判定を行う
//
// 戻り値: 接触しているなら接触頂点番号が返る、それ以外は負数が返る
//
JUNKAPI intptr_t JUNKCALL HitTestPointAndPolyline(
	const Vector2d* vts, // [in] 頂点配列先頭ポインタ
	intptr_t count, // [in] 頂点数
	Vector2d pt, // [in] 接触判定ポイント座標
	double vertexRadius, // [in] 頂点の半径
	double edgeThickness, // [in] 辺の太さ
	double& t, // [out] 辺と接触した際のパラメータ t が返る(0 なら戻り値の頂点、1なら次の頂点)
	Vector2d& pointOnEdge // [out] 接触した辺の最近点座標が返る
)
{
	return HitTestPointAndPolylineTmpl(vts, count, pt, vertexRadius, edgeThickness, t, pointOnEdge);
}

//==============================================================================
double (__fastcall *PolylineLengthBest)(const Vector2d* vts, intptr_t count);

double __fastcall PolylineLengthWithoutSimd(
	const Vector2d* vts, // [in] 頂点配列先頭ポインタ
	intptr_t count // [in] 頂点数
)
{
	return PolylineLength(vts, count);
}

double __fastcall PolylineLengthSSE3FastCall(
	const Vector2d* vts, // [in] 頂点配列先頭ポインタ
	intptr_t count // [in] 頂点数
)
{
	return PolylineLengthSSE3(vts, count);
}

// 機能 : 複数頂点で構成されるラインの全ての辺の長さの合計を計算する
//
// 戻り値: 長さ
//
JUNKAPI double JUNKCALL PolylineLength(
	const Vector2d* vts, // [in] 頂点配列先頭ポインタ
	intptr_t count // [in] 頂点数
)
{
	if(PolylineLengthBest == NULL)
	{
		if(IsSSE3Supported())
			PolylineLengthBest = PolylineLengthSSE3FastCall; // SSE3使用可能
		else
			PolylineLengthBest = PolylineLengthWithoutSimd; // SSE3使用不可能
	}
	return PolylineLengthBest(vts, count);
}

_JUNK_END

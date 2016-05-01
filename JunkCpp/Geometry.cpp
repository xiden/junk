// GraphNative.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
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
	if(20000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

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
		//	�V���O���X���b�h

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
		//	�f�[�^��6�����܂��̓f�[�^�̐擪��8�A16�ȊO�̃A���C�����g�Ȃ�SSE�g���Ȃ�
		SearchMaxMinWithoutSimd(p, n, min, max);
		return;
	}

	//	16�A���C�����g�ɐ؂�グ���J�n�|�C���^�擾
	//	16�A���C�����g�ɐ؂�̂Ă��I���|�C���^�擾
	const __m128d* ps16 = (__m128d*)((intptr_t)(p + 1) & ~(intptr_t)0xf);
	const double* pe = p + n;
	const __m128d* pe16 = (__m128d*)((intptr_t)pe & ~(intptr_t)0xf);

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(30000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

		//	n �� __m128d �P�ʂ̌��ɕϊ�
		n = pe16 - ps16;

		intptr_t nthread = omp_get_max_threads();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		
		//	�J�n�ʒu�ōő�ƍŏ���������
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16�A���C�����g�̏ꍇ
			smax = smin = *ps16;
		}
		else
		{
			//	8�A���C�����g�̏ꍇ
			__m128d t = { *p, *p };
			smax = smin = t;
		}

		//	�X���b�h�ŕ�������
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

		//	�I���|�C���^��16�A���C�����g�ł͂Ȃ��ꍇ�̓��ʏ���
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t = { pe[-1], pe[-1] };
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		}

		//	���8�o�C�g�Ɖ���8�o�C�g���r���čő�ŏ��擾
		smax = _mm_max_pd(smax, _mm_shuffle_pd(smax, smax, 3));
		smin = _mm_min_pd(smin, _mm_shuffle_pd(smin, smin, 3));

		//	�߂�l�Z�b�g
		max = smax.m128d_f64[0];
		min = smin.m128d_f64[0];
	}
	else
	{
		//	�V���O���X���b�h

		//	�J�n�ʒu�ōő�ƍŏ���������
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16�A���C�����g�̏ꍇ
			smax = smin = *ps16;
		}
		else
		{
			//	8�A���C�����g�̏ꍇ
			__m128d t = { *p, *p };
			smax = smin = t;
		}

		//	���[�v�ŏ���
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

		//	�I���|�C���^��16�A���C�����g�ł͂Ȃ��ꍇ�̓��ʏ���
		if((intptr_t)pe != (intptr_t)pe16)
		{
			__m128d t = { pe[-1], pe[-1] };
			smax = _mm_max_pd(smax, t);
			smin = _mm_min_pd(smin, t);
		}

		//	���8�o�C�g�Ɖ���8�o�C�g���r���čő�ŏ��擾
		smax = _mm_max_pd(smax, _mm_shuffle_pd(smax, smax, 3));
		smin = _mm_min_pd(smin, _mm_shuffle_pd(smin, smin, 3));

		//	�߂�l�Z�b�g
		max = smax.m128d_f64[0];
		min = smin.m128d_f64[0];
	}
}

// �@�\ : �ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchMaxMin(
	const double* p, // [in] �ő�ŏ���������f�[�^
	intptr_t n, // [in] �����f�[�^���A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	double& min, // [out] �ŏ��l���Ԃ�
	double& max // [out] �ő�l���Ԃ�
)
{
	if(SearchMaxMinBest == NULL)
	{
		if(IsSSE2Supported())
			SearchMaxMinBest = SearchMaxMinWithSse2; // SSE2�g�p�\
		else
			SearchMaxMinBest = SearchMaxMinWithoutSimd; // SSE2�g�p�s�\
	}
	SearchMaxMinBest(p, n, min, max);
}

// �@�\ : �����O�o�b�t�@������ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchMaxMinRing(
	const double* pBuffer, // [in] �����O�o�b�t�@�̐擪�|�C���^
	intptr_t nBufLen, // [in] �����O�o�b�t�@�̃T�C�Y�A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	intptr_t iIndex, // [in] �����J�n�ʒu�̃C���f�b�N�X�ԍ�
	intptr_t n, // [in] �����f�[�^���A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	double& min, // [out] �ŏ��l���Ԃ�
	double& max // [out] �ő�l���Ԃ�
)
{
	iIndex %= nBufLen;
	if(iIndex + n < nBufLen)
	{
		// �o�b�t�@�𕪊�����K�v�������ꍇ
		SearchMaxMin(pBuffer + iIndex, n, min, max);
	}
	else
	{
		// �o�b�t�@�𕪊�����K�v������ꍇ
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

// �@�\: double �^�Q����(X,Y)�x�N�g���z����󂯎��AX��Y���ꂼ��̍ő�ŏ�����������
//
void __fastcall SearchPointMaxMinWithoutSimd(
	const Vector2d* p, // [in] double �^�Q����(X,Y)�x�N�g���z��̐擪�|�C���^
	intptr_t n, // [in] �x�N�g����
	double& minX, // [out] �ŏ�X�l���Ԃ�
	double& minY, // [out] �ŏ�Y�l���Ԃ�
	double& maxX, // [out] �ő�X�l���Ԃ�
	double& maxY // [out] �ő�Y�l���Ԃ�
)
{
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(20000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

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
		//	�V���O���X���b�h

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

// �@�\: double �^�Q����(X,Y)�x�N�g���z����󂯎��AX��Y���ꂼ��̍ő�ŏ�����������
//
void __fastcall SearchPointMaxMinWithSse2(
	const Vector2d* p, // [in] double �^�Q����(X,Y)�x�N�g���z��̐擪�|�C���^
	intptr_t n, // [in] �x�N�g����
	double& minX, // [out] �ŏ�X�l���Ԃ�
	double& minY, // [out] �ŏ�Y�l���Ԃ�
	double& maxX, // [out] �ő�X�l���Ԃ�
	double& maxY // [out] �ő�Y�l���Ԃ�
)
{
	if((n < 4) | ((intptr_t)p & 7))
	{
		//	�f�[�^��6�����܂��̓f�[�^�̐擪��8�A16�ȊO�̃A���C�����g�Ȃ�SSE�g���Ȃ�
		SearchPointMaxMinWithoutSimd(p, n, minX, minY, maxX, maxY);
		return;
	}

	//	16�A���C�����g�ɐ؂�グ���J�n�|�C���^�擾
	//	16�A���C�����g�ɐ؂�̂Ă��I���|�C���^�擾
	const __m128d* ps16 = (__m128d*)((intptr_t)((char*)p + 8) & ~(intptr_t)0xf);
	const Vector2d* pe = p + n;
	const __m128d* pe16 = (__m128d*)((intptr_t)pe & ~(intptr_t)0xf);

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(30000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

		//	n �� __m128d �P�ʂ̌��ɕϊ�
		n = pe16 - ps16;

		intptr_t nthread = omp_get_max_threads();
		if(n < nthread)
			nthread = n;

		intptr_t divsize = n / nthread;
		
		//	�J�n�ʒu�ōő�ƍŏ���������
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16�A���C�����g�̏ꍇ
			smax = smin = *ps16;
		}
		else
		{
			//	8�A���C�����g�̏ꍇ
			__m128d t = { p->Y(), p->X() }; // X��Y���t�ɂȂ�
			smax = smin = t;
		}

		//	�X���b�h�ŕ�������
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

		//	�I���|�C���^��16�A���C�����g�ł͂Ȃ��ꍇ�̓��ʏ���
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

			//	�߂�l�Z�b�g
			minX = smin.m128d_f64[1];
			minY = smin.m128d_f64[0];
			maxX = smax.m128d_f64[1];
			maxY = smax.m128d_f64[0];
		}
		else
		{
			//	�߂�l�Z�b�g
			minX = smin.m128d_f64[0];
			minY = smin.m128d_f64[1];
			maxX = smax.m128d_f64[0];
			maxY = smax.m128d_f64[1];
		}
	}
	else
	{
		//	�V���O���X���b�h

		//	�J�n�ʒu�ōő�ƍŏ���������
		__m128d smax;
		__m128d smin;
		if((intptr_t)p == (intptr_t)ps16)
		{
			//	16�A���C�����g�̏ꍇ
			smax = smin = *ps16;
		}
		else
		{
			//	16�A���C�����g�ȊO�̏ꍇ
			__m128d t = { p->Y(), p->X() };
			smax = smin = t;
		}

		//	���[�v�ŏ���
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

		//	�I���|�C���^��16�A���C�����g�ł͂Ȃ��ꍇ�̓��ʏ���
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

			//	�߂�l�Z�b�g
			maxX = smax.m128d_f64[1];
			minX = smin.m128d_f64[1];
			maxY = smax.m128d_f64[0];
			minY = smin.m128d_f64[0];
		}
		else
		{
			//	�߂�l�Z�b�g
			maxX = smax.m128d_f64[0];
			minX = smin.m128d_f64[0];
			maxY = smax.m128d_f64[1];
			minY = smin.m128d_f64[1];
		}
	}
}

//// �@�\ : �ő�ŏ�����������A��ԃV���v���Ȏ���
////
//void __fastcall _SearchPointMaxMinBase(
//	const Vector2D* p, // [in] �ő�ŏ���������f�[�^
//	intptr_t n, // [in] �f�[�^��
//	double& minX, // [out] �ŏ�X�l���Ԃ�
//	double& minY, // [out] �ŏ�Y�l���Ԃ�
//	double& maxX, // [out] �ő�X�l���Ԃ�
//	double& maxY // [out] �ő�Y�l���Ԃ�
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

// �@�\: double �^�Q����(X,Y)�x�N�g���z����󂯎��AX��Y���ꂼ��̍ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchPointMaxMin(
	const Vector2d* p, // [in] �ő�ŏ���������f�[�^
	intptr_t n, // [in] �f�[�^��
	double& minX, // [out] �ŏ�X�l���Ԃ�
	double& minY, // [out] �ŏ�Y�l���Ԃ�
	double& maxX, // [out] �ő�X�l���Ԃ�
	double& maxY // [out] �ő�Y�l���Ԃ�
)
{
	if(SearchMaxMinBest == NULL)
	{
		if(0) //IsSSE2Supported())
			SearchPointMaxMinBest = SearchPointMaxMinWithSse2; // SSE2�g�p�\
		else
			SearchPointMaxMinBest = SearchPointMaxMinWithoutSimd; // SSE2�g�p�s�\
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
// �@�\ : ���`�ϊ����s��
//
JUNKAPI void JUNKCALL TransformLin(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	double* pDst // [out] �ϊ���̃f�[�^
)
{
	if(20000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
	{
		//	�}���`�X���b�h

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = pSrc[i] * scale + translate;
	}
	else
	{
		//	�V���O���X���b�h

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = pSrc[i] * scale + translate;
	}
}

// �@�\ : ���`�ϊ���int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformLinInt(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^
)
{
	if(10000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
	{
		//	�}���`�X���b�h

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	�V���O���X���b�h

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
}

// �@�\ : ����`�ϊ�(Log,Pow)���܂ޕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLin(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTis, // [in] �ϊ����z��
	intptr_t nTransform, // [in] pTis �̗v�f��
	double* pDst // [out] �ϊ���̃f�[�^
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		TransformLin(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(500 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLinInt(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTis, // [in] �ϊ����z��
	intptr_t nTransform, // [in] pTis �̗v�f��
	int* pDst // [out] �ϊ���̃f�[�^
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		TransformLinInt(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(300 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : ���`�ϊ���int�ɕϊ����s��
//        �o�͂� pDst �� POINT �\����(8�o�C�g)�̔z��ƌ��Ȃ��A�擪4�o�C�g�ɒl���������ݎc���4�o�C�g�͂��̂܂܎c��
//
JUNKAPI void JUNKCALL TransformLinToInt2(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^
)
{
	if(10000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
	{
		//	�}���`�X���b�h

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i * 2] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	�V���O���X���b�h

		for (intptr_t i = 0; i < n; i++)
			pDst[i * 2] = (int)(pSrc[i] * scale + translate);
	}
}

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//        �o�͂� pDst �� POINT �\����(8�o�C�g)�̔z��ƌ��Ȃ��A�擪4�o�C�g�ɒl���������ݎc���4�o�C�g�͂��̂܂܎c��
//
JUNKAPI void JUNKCALL TransformNonLinToInt2(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTis, // [in] �ϊ����z��
	intptr_t nTransform, // [in] pTis �̗v�f��
	int* pDst // [out] �ϊ���̃f�[�^
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		TransformLinToInt2(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(300 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : SIMD���g�p�����ɐ��`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformLinIntWithoutSimd(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^
)
{
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(10000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

		#pragma omp parallel for
		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}
	else
	{
		//	�V���O���X���b�h

		for (intptr_t i = 0; i < n; i++)
			pDst[i] = (int)(pSrc[i] * scale + translate);
	}

	return pDst;
}

// �@�\ : SIMD���g�p���Đ��`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformLinSse2Int(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	//	8�f�[�^�ȏ�łȂ��ꍇ���́A8�A16�o�C�g�A���C�����g�łȂ��ꍇ��SSE�ŏ����ł��Ȃ�
	if((n < 8) | ((intptr_t)pSrc & 7))
		return _TransformLinIntWithoutSimd(pSrc, n, scale, translate, pDst);

	const double* pSrcEnd = pSrc + n; // �ϊ����f�[�^�̏I���|�C���^
	const __m128d* pSrc16 = (__m128d*)(((intptr_t)pSrc + 15) & ~(intptr_t)0xf); // 16�o�C�g���E�ɐ؂�グ���ϊ����f�[�^�|�C���^
	intptr_t n4 = (((double*)pSrcEnd - (double*)pSrc16) >> 1) & ~(intptr_t)3; // pSrc16 ����4�f�[�^�P�ʏ�������f�[�^��
	__m128i* pDst16 = (__m128i*)(((intptr_t)pDst + 15) & ~(intptr_t)0xf); // 16�o�C�g���E�ɐ؂�グ���ϊ���f�[�^�|�C���^
	int* pDstStart; // ���ۂɕϊ���f�[�^���������܂��擪�|�C���^�ASSE��16�o�C�g�P�ʂŏ������邽�߂ɐ擪�|�C���^�𒲐�����K�v������

	if((intptr_t)pSrc == (intptr_t)pSrc16)
	{
		//	�ϊ����f�[�^������16�o�C�g���E�ɉ����Ă���ꍇ
		pDstStart = (int*)pDst16;
	}
	else
	{
		//	�ϊ����f�[�^��16�o�C�g���E�ɉ����Ă��Ȃ��ꍇ

		//	�擪1�f�[�^��ϊ�����
		pDst16++;
		pDstStart = (int*)pDst16 - 1;
		*pDstStart = (int)(pSrc[0] * scale + translate);
	}

	__m128d s = { scale, scale };
	__m128d t = { translate, translate };
#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(25000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		//	�}���`�X���b�h

		#pragma omp parallel for
		for(intptr_t i = 0; i < n4; i += 2)
			pDst16[i >> 1] = _mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i], s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i + 1], s), t)), 0x4e)
						);
	}
	else
	{
		//	�V���O���X���b�h

		for(intptr_t i = 0; i < n4; i += 2)
			pDst16[i >> 1] = _mm_or_si128(
							_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i], s), t)),
							_mm_shuffle_epi32(_mm_cvttpd_epi32(_mm_add_pd(_mm_mul_pd(pSrc16[i + 1], s), t)), 0x4e)
						);
	}

	//	�I�[��4�f�[�^�P�ʂŏ����ł��Ȃ�������ϊ�����
	pSrc = (double*)(pSrc16 + n4);
	pDst = (int*)(pDst16 + (n4 >> 1));
	for(; pSrc < pSrcEnd; pSrc++, pDst++)
		*pDst = (int)(*pSrc * scale + translate);

	return pDstStart;
}

// �@�\ : double �^�Q����(X,Y)�x�N�g���z����󂯎��A���`�ϊ����� int �^�Q����(X,Y)�x�N�g���z��ɏo�͂���
//
void __fastcall _TransformLinPointToInt2WithoutSimd(
	const Vector2d* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scaleX, // [in] X���W�X�P�[�����O�l
	double translateX, // [in] X���W���s�ړ��l
	double scaleY, // [in] Y���W�X�P�[�����O�l
	double translateY, // [in] Y���W���s�ړ��l
	Vector2i* pDst // [out] �ϊ���̃f�[�^
)
{
	if(10000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
	{
		//	�}���`�X���b�h

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
		//	�V���O���X���b�h

		for (intptr_t i = 0; i < n; i++)
		{
			const Vector2d& s = pSrc[i];
			Vector2i& d = pDst[i];
			d.X() = (int)(s.X() * scaleX + translateX);
			d.Y() = (int)(s.Y() * scaleY + translateY);
		}
	}
}

// �@�\ : double �^�Q����(X,Y)�x�N�g���z����󂯎��ASIMD���g�p���Đ��`�ϊ����� int �^�Q����(X,Y)�x�N�g���z��ɏo�͂���
//
void __fastcall _TransformLinPointToInt2WithSse2(
	const Vector2d* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scaleX, // [in] X���W�X�P�[�����O�l
	double translateX, // [in] X���W���s�ړ��l
	double scaleY, // [in] Y���W�X�P�[�����O�l
	double translateY, // [in] Y���W���s�ړ��l
	Vector2i* pDst // [out] �ϊ���̃f�[�^
)
{
	//	8�f�[�^�ȏ�łȂ��ꍇ��SSE�ŏ����ł��Ȃ�
	if(n < 8)
		return _TransformLinPointToInt2WithoutSimd(pSrc, n, scaleX, translateX, scaleY, translateY, pDst);

	//	���[�v�ŏ�������|�C���g���A���[�v���ł�32�o�C�g�P�ʂŏ�������
	intptr_t nLoopPoints = n & ~(intptr_t)1;

	//	���[�v�����̐擪�|�C���^
	const __m128d* pSrc16 = (__m128d*)pSrc;
	__m128i* pDst16 = (__m128i*)pDst;

	//	�ϊ��W���ݒ�
	__m128d s = { scaleX, scaleY };
	__m128d t = { translateX, translateY };

	if(25000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
	{
		//	�}���`�X���b�h

		if((intptr_t)pSrc & 15)
		{
			if((intptr_t)pDst & 15)
			{
				//	�\�[�X�ƃf�X�e�B�l�[�V����������16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�\�[�X��16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�f�X�e�B�l�[�V������16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�\�[�X�ƃf�X�e�B�l�[�V����������16�o�C�g�A���C�����g����Ă���ꍇ

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
		//	�V���O���X���b�h

		if((intptr_t)pSrc & 15)
		{
			if((intptr_t)pDst & 15)
			{
				//	�\�[�X�ƃf�X�e�B�l�[�V����������16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�\�[�X��16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�f�X�e�B�l�[�V������16�o�C�g�A���C�����g����Ă��Ȃ��ꍇ

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
				//	�\�[�X�ƃf�X�e�B�l�[�V����������16�o�C�g�A���C�����g����Ă���ꍇ

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

	//	���[�v�ŏ����ł��Ȃ������|�C���g������
	if(n & 1)
	{
		const Vector2d& s = pSrc[nLoopPoints];
		Vector2i& d = pDst[nLoopPoints];
		d.X() = (int)(s.X() * scaleX + translateX);
		d.Y() = (int)(s.Y() * scaleY + translateY);
	}
}

// �@�\ : double �^�Q����(X,Y)�x�N�g���z����󂯎��A�ϊ����� int �^�Q����(X,Y)�x�N�g���z��ɏo�͂���
//
JUNKAPI void JUNKCALL TransformLinPointDToPointI(
	const Vector2d* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scaleX, // [in] X���W�X�P�[�����O�l
	double translateX, // [in] X���W���s�ړ��l
	double scaleY, // [in] Y���W�X�P�[�����O�l
	double translateY, // [in] Y���W���s�ړ��l
	Vector2i* pDst // [out] �ϊ���̃f�[�^
)
{
	if(TransformLinPointToInt2Best == NULL)
	{
		if(IsSSE2Supported())
			TransformLinPointToInt2Best = _TransformLinPointToInt2WithSse2; // SSE2�g�p�\
		else
			TransformLinPointToInt2Best = _TransformLinPointToInt2WithoutSimd; // SSE2�g�p�s�\
	}
	TransformLinPointToInt2Best(pSrc, n, scaleX, translateX, scaleY, translateY, pDst);
}

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLinPointDToPointI(
	const Vector2d* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTisX, // [in] X���W�ϊ����z��
	intptr_t nTransformX, // [in] pTisX �̗v�f��
	const TransformInfo* pTisY, // [in] Y���W�ϊ����z��
	intptr_t nTransformY, // [in] pTisY �̗v�f��
	Vector2i* pDst // [out] �ϊ���̃f�[�^
)
{
	if(nTransformX == 1 && nTransformY == 1 && !pTisX->LogBeforeLinear && !pTisX->PowAfterLinear && !pTisY->LogBeforeLinear && !pTisY->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		TransformLinPointDToPointI(pSrc, n, pTisX->Transform.Scale, pTisX->Transform.Translate, pTisY->Transform.Scale, pTisY->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(300 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : ���`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformLinInt(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	if(TransformLinIntBest == NULL)
	{
		if(IsSSE2Supported())
			TransformLinIntBest = _TransformLinSse2Int; // SSE2�g�p�\
		else
			TransformLinIntBest = _TransformLinIntWithoutSimd; // SSE2�g�p�s�\
	}
	return TransformLinIntBest(pSrc, n, scale, translate, pDst);
}

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformNonLinInt(
	const double* pSrc, // [in] �ϊ����̃f�[�^
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTis, // [in] �ϊ����z��
	intptr_t nTransform, // [in] pTis �̗v�f��
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		return _TransformLinInt(pSrc, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(300 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : �C���f�b�N�X�ԍ���1�����Z���Ă����A�������`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformIndexLinIntWithoutSimd(
	intptr_t iStartIndex, // [in] �C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^
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

// �@�\ : �C���f�b�N�X�ԍ���1�����Z���Ă����A�������`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformIndexLinIntSse2(
	intptr_t iStartIndex, // [in] �C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	//	8�f�[�^�ȏ�łȂ��ꍇ��SSE�ŏ����ł��Ȃ�
	if(n < 8)
		return _TransformIndexLinIntWithoutSimd(iStartIndex, n, scale, translate, pDst);

	__m128d s = { scale, scale }; // �X�P�[�����O�l
	__m128d t = { translate, translate }; // ���s�ړ��l
	__m128d inc4 = { 4, 4 }; // 4�f�[�^�������̃C���f�b�N�X�ԍ������l
	__m128i* pDst16 = (__m128i*)(((intptr_t)pDst + 15) & ~(intptr_t)0xf); // 16�o�C�g���E�ɐ؂�グ���ϊ���f�[�^�|�C���^
	intptr_t n128i = n >> 2; // __m128i(4�̐����̃p�b�N)�f�[�^���A4�_�P�ʂŏ������邽�߃f�[�^����4�Ŋ���

#ifdef ERR_CHECK_MODE
	if(100 <= n)
#else
	if(35000 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
#endif
	{
		intptr_t nthread = omp_get_max_threads();
		if(n128i < nthread)
			nthread = n128i;

		intptr_t divsize = n128i / nthread; // �X���b�h���ɏ������� __m128i �f�[�^��(1�� __m128i �f�[�^�ɂ�4�̐����f�[�^���p�b�N����Ă���)

		#pragma omp parallel for 
		for(intptr_t j = 0; j < nthread; ++j)
		{
			intptr_t iS = iStartIndex + j * (divsize << 2); // ���̃X���b�h�ł̊J�n�C���f�b�N�X�ԍ�
			__m128d idx1 = { (double)iS, (double)(iS + 1) };
			__m128d idx2;
			{
				__m128d inc2 = { 2, 2 };
				idx2 = _mm_add_pd(idx1, inc2);
			}

			__m128i* p = pDst16 + j * divsize; // ���̃X���b�h�ł̏������݊J�n�|�C���^
			__m128i* e = j < (nthread - 1) ? p + divsize : pDst16 + n128i; // ���̃X���b�h�ł̏������ݏI���|�C���^
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

		__m128i* p = pDst16; // �������݊J�n�|�C���^
		__m128i* e = p + n128i; // �������ݏI���|�C���^
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


// �@�\ : �C���f�b�N�X�ԍ���1�����Z���Ă����A�������`�ϊ���int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformIndexLinInt(
	intptr_t iStartIndex, // [in] �C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	double scale, // [in] �X�P�[�����O�l
	double translate, // [in] ���s�ړ��l
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	if(TransformIndexLinIntBest == NULL)
	{
		if(IsSSE2Supported())
			TransformIndexLinIntBest = _TransformIndexLinIntSse2; // SSE2�g�p�\
		else
			TransformIndexLinIntBest = _TransformIndexLinIntWithoutSimd; // SSE2�g�p�s�\
	}
	return TransformIndexLinIntBest(iStartIndex, n, scale, translate, pDst);
}

// �@�\ : �C���f�b�N�X�ԍ���1�����Z���Ă����A��������`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
// �Ԃ�l : ���ۂɕϊ���f�[�^���������܂��擪�|�C���^
//
int* __fastcall _TransformIndexNonLinInt(
	intptr_t iStartIndex, // [in] �C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	const TransformInfo* pTis, // [in] �ϊ����z��
	intptr_t nTransform, // [in] pTis �̗v�f��
	int* pDst // [out] �ϊ���̃f�[�^�A 16�o�C�g���E�ɍ��킹�ď�������邽�� n + 8 �v�f���̗̈悪�K�v
)
{
	if(nTransform == 1 && !pTis->LogBeforeLinear && !pTis->PowAfterLinear)
	{
		//	���`�ϊ��݂̂̏ꍇ

		return _TransformIndexLinInt(iStartIndex, n, pTis->Transform.Scale, pTis->Transform.Translate, pDst);
	}
	else
	{
		//	����`�ϊ����܂ޏꍇ

		if(300 <= n) // TODO: ���ɂ���ĈႤ�̂łǂ̒l���x�X�g����������
		{
			//	�}���`�X���b�h

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
			//	�V���O���X���b�h

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

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
JUNKAPI intptr_t JUNKCALL TransformForDraw(
	const TransformInfo* pTisX, // [in] X���ϊ����z��
	intptr_t nTisX, // [in] pTisX �̗v�f��
	const TransformInfo* pTisY, // [in] Y���ϊ����z��
	intptr_t nTisY, // [in] pTisY �̗v�f��
	const double* pSrcY, // [in] �ϊ�����Y���l�f�[�^
	intptr_t iStartIndexX, // [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	Vector2i* pDst // [out] �ϊ����XY�l�f�[�^�A�K�v�ȗv�f���͍��W�ϊ���̒l�ɂ��قȂ�A���W�ϊ����X�l�͈�*4+4���x�K�v
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

		//	�C���f�b�N�X�ԍ�����w���W�l�𐶐�����
		pTmpX = _TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, bufX.get());

		//	�x���W�l��ϊ�����
		pTmpY = _TransformNonLinInt(pSrcY, n, pTisY, nTisY, bufY.get());

		int p1x;
		int p1y;
		int p2x;
		int p2y;
		intptr_t nData = 0; // �ϊ���f�[�^��
		int s; // �O��̂w���W�ł̊J�n�x�l
		int min; // �O��̂w���W�ł̍ŏ��x�l
		int max; // �O��̂w���W�ł̍ő�x�l

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
					//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
					pDst[nData].X() = p1x;
					pDst[nData].Y() = min;
					nData++;
				}
				else
				{
					//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
					//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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

				//	�J�n�x�l�A�ŏ��A�ő�l������
				s = min = max = p2y;
			}

			p1x = p2x;
			p1y = p2y;
		}

		if(min == max)
		{
			//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
			pDst[nData].X() = p2x;
			pDst[nData].Y() = p2y;
			nData++;
		}
		else
		{
			//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
			//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����(�����O�o�b�t�@��)
//	Y���l�f�[�^�� pSrcY + iStartIndexY ����擾�J�n����ApSrcY + nSrcYBufLen �𒴂����� pSrcY �ɖ߂��Ď擾����������
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
JUNKAPI intptr_t JUNKCALL TransformForDrawRing(
	const TransformInfo* pTisX, // [in] X���ϊ����z��
	intptr_t nTisX, // [in] pTisX �̗v�f��
	const TransformInfo* pTisY, // [in] Y���ϊ����z��
	intptr_t nTisY, // [in] pTisY �̗v�f��
	const double* pSrcY, // [in] �ϊ�����Y���l�f�[�^(Y���f�[�^�o�b�t�@�̐擪�A�h���X)
	intptr_t nSrcYBufLen, // [in] pSrcY �̃o�b�t�@�̃T�C�Y(�f�[�^��)
	intptr_t iStartIndexX, // [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t iStartIndexY, // [in] Y���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, // [in] �ϊ��f�[�^��
	Vector2i* pDst // [out] �ϊ����XY�l�f�[�^�A�K�v�ȗv�f���͍��W�ϊ���̒l�ɂ��قȂ�A���W�ϊ����X�l�͈�*4+4���x�K�v
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

		//	�C���f�b�N�X�ԍ�����w���W�l�𐶐�����
		pTmpX = _TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, bufX.get());

		//	�x���W�l��ϊ�����
		intptr_t copyStart = iStartIndexY % nSrcYBufLen;
		intptr_t copyEnd = copyStart + n;
		if(copyEnd <= nSrcYBufLen)
		{
			//	�P��̃f�[�^�R�s�[�ōςޏꍇ�͕��ʂɍ��W�ϊ��R�s�[
			nTmpYs = 1;
			nTmpYCounts[0] = n;
			bufYs[0].reset(new int[nTmpYCounts[0] + 8]);
			pTmpYs[0] = _TransformNonLinInt(pSrcY + copyStart, n, pTisY, nTisY, bufYs[0].get());
		}
		else
		{
			//	�Q��ɕ����ăf�[�^�R�s�[����K�v������ꍇ
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
		intptr_t nData = 0; // �ϊ���f�[�^��
		int s; // �O��̂w���W�ł̊J�n�x�l
		int min; // �O��̂w���W�ł̍ŏ��x�l
		int max; // �O��̂w���W�ł̍ő�x�l

		p1x = p2x = pTmpX[0];
		p1y = p2y = pTmpYs[0][0];
		s = min = max = p1y;

		intptr_t iX = 1; // ���W�ϊ����X���W�l�z��̃C���f�b�N�X�ԍ�

		//	�����O�o�b�t�@�Ȃ̂ōő�Q��ɕ��������
		//	���̃o�b�t�@���ɏ���������
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
						//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
						pDst[nData].X() = p1x;
						pDst[nData].Y() = min;
						nData++;
					}
					else
					{
						//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
						//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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

					//	�J�n�x�l�A�ŏ��A�ő�l������
					s = min = max = p2y;
				}

				p1x = p2x;
				p1y = p2y;
			}
		}

		if(min == max)
		{
			//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
			pDst[nData].X() = p2x;
			pDst[nData].Y() = p2y;
			nData++;
		}
		else
		{
			//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
			//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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


//// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����
////
//// �Ԃ�l : �ϊ���̃f�[�^��
////
//JUNKAPI intptr_t JUNKCALL TransformForDraw(
//	const TransformInfo* pTisX, // [in] X���ϊ����z��
//	intptr_t nTisX, // [in] pTisX �̗v�f��
//	const TransformInfo* pTisY, // [in] Y���ϊ����z��
//	intptr_t nTisY, // [in] pTisY �̗v�f��
//	const double* pSrcY, // [in] �ϊ�����Y���l�f�[�^
//	intptr_t iStartIndexX, // [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
//	intptr_t n, // [in] �ϊ��f�[�^��
//	int* pDstX, // [out] �ϊ����X���l�f�[�^�A�Œ� n �v�f���̃T�C�Y���K�v
//	int* pDstY // [out] �ϊ����Y���l�f�[�^�A�Œ� n �v�f���̃T�C�Y���K�v
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
//	//	�C���f�b�N�X�ԍ�����w���W�l�𐶐�����
//	TransformIndexNonLinInt(iStartIndexX, n, pTisX, nTisX, pTmpX);
//
//	//	�x���W�l��ϊ�����
//	TransformNonLinInt(pSrcY, n, pTisY, nTisY, pTmpY);
//
//	int p1x;
//	int p1y;
//	int p2x;
//	int p2y;
//	intptr_t nData = 0; // �ϊ���f�[�^��
//	int s; // �O��̂w���W�ł̊J�n�x�l
//	int min; // �O��̂w���W�ł̍ŏ��x�l
//	int max; // �O��̂w���W�ł̍ő�x�l
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
//				//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
//				pDstX[nData] = p1x;
//				pDstY[nData] = min;
//				nData++;
//			}
//			else
//			{
//				//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
//				//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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
//			//	�J�n�x�l�A�ŏ��A�ő�l������
//			s = min = max = p2y;
//		}
//
//		p1x = p2x;
//		p1y = p2y;
//	}
//
//	if(min == max)
//	{
//		//	�O�̂w���W�̍ő�ŏ��x�l�������ꍇ�͂P�_�����̒ǉ�
//		pDstX[nData] = p2x;
//		pDstY[nData] = p2y;
//		nData++;
//	}
//	else
//	{
//		//	�O�̂w���W�̍ő�ŏ��x�l���قȂ�ꍇ��
//		//	�J�n�l�A�ő�A�ŏ��A�I���l��ǉ�����
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

// �@�\ : �������_�ō\������郉�C���Ǝw��|�C���g�Ƃ̐ڐG������s��
//
// �߂�l: �ڐG���Ă���Ȃ�ڐG���_�ԍ����Ԃ�A����ȊO�͕������Ԃ�
//
JUNKAPI intptr_t JUNKCALL HitTestPointAndPolyline(
	const Vector2d* vts, // [in] ���_�z��擪�|�C���^
	intptr_t count, // [in] ���_��
	Vector2d pt, // [in] �ڐG����|�C���g���W
	double vertexRadius, // [in] ���_�̔��a
	double edgeThickness, // [in] �ӂ̑���
	double& t, // [out] �ӂƐڐG�����ۂ̃p�����[�^ t ���Ԃ�(0 �Ȃ�߂�l�̒��_�A1�Ȃ玟�̒��_)
	Vector2d& pointOnEdge // [out] �ڐG�����ӂ̍ŋߓ_���W���Ԃ�
)
{
	return HitTestPointAndPolylineTmpl(vts, count, pt, vertexRadius, edgeThickness, t, pointOnEdge);
}

//==============================================================================
double (__fastcall *PolylineLengthBest)(const Vector2d* vts, intptr_t count);

double __fastcall PolylineLengthWithoutSimd(
	const Vector2d* vts, // [in] ���_�z��擪�|�C���^
	intptr_t count // [in] ���_��
)
{
	return PolylineLength(vts, count);
}

double __fastcall PolylineLengthSSE3FastCall(
	const Vector2d* vts, // [in] ���_�z��擪�|�C���^
	intptr_t count // [in] ���_��
)
{
	return PolylineLengthSSE3(vts, count);
}

// �@�\ : �������_�ō\������郉�C���̑S�Ă̕ӂ̒����̍��v���v�Z����
//
// �߂�l: ����
//
JUNKAPI double JUNKCALL PolylineLength(
	const Vector2d* vts, // [in] ���_�z��擪�|�C���^
	intptr_t count // [in] ���_��
)
{
	if(PolylineLengthBest == NULL)
	{
		if(IsSSE3Supported())
			PolylineLengthBest = PolylineLengthSSE3FastCall; // SSE3�g�p�\
		else
			PolylineLengthBest = PolylineLengthWithoutSimd; // SSE3�g�p�s�\
	}
	return PolylineLengthBest(vts, count);
}

_JUNK_END

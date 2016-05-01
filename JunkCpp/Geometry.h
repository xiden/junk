#pragma once
#ifndef __JUNK_GEOMETRY_H
#define __JUNK_GEOMETRY_H

#include "JunkConfig.h"
#include "Matrix.h"
#include <vector>
#include <pmmintrin.h>

_JUNK_BEGIN

#ifndef _JUNK_GEOMETRY_INLINE
#define _JUNK_GEOMETRY_INLINE inline
#endif


//! Transform �֐��ɓn���ϊ����
struct TransformInfo;

//==============================================================================
//		�e���v���[�g�A�C�����C���֐��錾
//==============================================================================

//! �������_�ō\������郉�C���Ǝw��|�C���g�Ƃ̐ڐG������s��
//! @return  �ڐG���Ă���Ȃ�ڐG���_�ԍ����Ԃ�A����ȊO�͕������Ԃ�
template<
	class T, //!< �l�^
	class V1, //!< ���_�z��x�N�g���^
	class V2 //!< �w��|�C���g�x�N�g���^
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTmpl(
	const V1* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count, //!< [in] ���_��
	V2 pt, //!< [in] �ڐG����|�C���g���W
	T vertexRadius, //!< [in] ���_�̔��a
	T edgeThickness, //!< [in] �ӂ̑���
	T& t //!< [out] �ӂƐڐG�����ۂ̃p�����[�^ t ���Ԃ�(0 �Ȃ�߂�l�̒��_�A1�Ȃ玟�̒��_)
) {
	vertexRadius *= vertexRadius;

	if (1 <= count) {
		//	�擪�|�C���g�ƒ��_�̐ڐG����
		if ((pt - vts[0]).LengthSquare() <= vertexRadius) {
			//	�w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ����璸�_�ɐڐG�Ɣ���
			t = 0.0;
			return 0;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		//	�w��|�C���g�ƒ��_�̐ڐG����
		if ((pt - vts[i]).LengthSquare() <= vertexRadius) {
			//	�w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ����璸�_�ɐڐG�Ɣ���
			t = 0.0;
			return i;
		}

		//	�w��|�C���g�ƕӂ̐ڐG����
		Vector2d s = vts[i - 1];
		Vector2d v1 = vts[i] - s;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // ���_���d�Ȃ��Ă�̂ŕӂ͑��݂��Ȃ�

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - s;
		double x = v1.Dot(v2);
		double y = v1.MakeVertical().Dot(v2);

		if (0.0 <= x && x <= l && -edgeThickness <= y && y <= edgeThickness) {
			//	���W�ϊ���ɋ�`�͈͓̔��Ɏw��|�C���g������Ȃ�ӂɐڐG�Ɣ���
			t = x / l;
			return i - 1;
		}
	}

	return -1;
}

//! �������_�ō\������郉�C���Ǝw��|�C���g�Ƃ̐ڐG������s��
//! @return �ڐG���Ă���Ȃ�ڐG���_�ԍ����Ԃ�A����ȊO�͕������Ԃ�
template<
	class T, // �l�^
	class V1, // ���_�z��x�N�g���^
	class V2, // �w��|�C���g�x�N�g���^
	class V3 // pointOnEdge �̃x�N�g���^
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTmpl(
	const V1* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count, //!< [in] ���_��
	V2 pt, //!< [in] �ڐG����|�C���g���W
	T vertexRadius, //!< [in] ���_�̔��a
	T edgeThickness, //!< [in] �ӂ̑���
	T& t, //!< [out] �ӂƐڐG�����ۂ̃p�����[�^ t ���Ԃ�(0 �Ȃ�߂�l�̒��_�A1�Ȃ玟�̒��_)
	V3& pointOnEdge //!< [out] �ڐG�����ӂ̍ŋߓ_���W���Ԃ�
) {
	vertexRadius *= vertexRadius;

	if (1 <= count) {
		//	�擪�|�C���g�ƒ��_�̐ڐG����
		if ((pt - vts[0]).LengthSquare() <= vertexRadius) {
			//	�w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ����璸�_�ɐڐG�Ɣ���
			t = 0.0;
			pointOnEdge = vts[0];
			return 0;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		//	�w��|�C���g�ƒ��_�̐ڐG����
		if ((pt - vts[i]).LengthSquare() <= vertexRadius) {
			//	�w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ����璸�_�ɐڐG�Ɣ���
			t = 0.0;
			pointOnEdge = vts[i];
			return i;
		}

		//	�w��|�C���g�ƕӂ̐ڐG����
		Vector2d s = vts[i - 1];
		Vector2d v1 = vts[i] - s;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // ���_���d�Ȃ��Ă�̂ŕӂ͑��݂��Ȃ�

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - s;
		double x = v1.Dot(v2);
		double y = v1.Vertical().Dot(v2);

		if (0.0 <= x && x <= l && -edgeThickness <= y && y <= edgeThickness) {
			//	���W�ϊ���ɋ�`�͈͓̔��Ɏw��|�C���g������Ȃ�ӂɐڐG�Ɣ���
			pointOnEdge = s + v1 * x;
			t = x / l;
			return i - 1;
		}
	}

	return -1;
}

//! �������_�ō\������鑾���������C���Ɖ~�Ƃ̐ڐG������s���A�S�Ă̓��͍��W�͍��W�ϊ��֐�TF��ʂ��ĎQ�Ƃ����
//! @return �ڐG���Ă���Ȃ�ڐG���_�ԍ����Ԃ�A����ȊO�͕������Ԃ�
template<
	class T, //!< �l�^
	class V1, //!< ���_�z��x�N�g���^
	class V2, //!< �w��|�C���g�x�N�g���^
	class V3, //!< pointOnEdge �̃x�N�g���^
	class TF //!< ���_���W�ϊ��֐�
>
_JUNK_GEOMETRY_INLINE intptr_t HitTestPointAndPolylineTf(
	int checkEdge, //!< [in] �ӂ����ׂ邩�ǂ���
	const V1* vts, //!< [in] ���_�z��擪�|�C���^(���W�ϊ��O)
	intptr_t count, //!< [in] ���_��
	V2 pt, //!< [in] �ڐG����|�C���g���W(���W�ϊ���)
	T vertexRadius, //!< [in] ���_�̔��a(���W�ϊ���)
	T edgeThickness, //!< [in] �ӂ̑���(���W�ϊ���)�A�������w�肳�ꂽ��ӂ̃`�F�b�N�͍s���Ȃ�
	TF tf, //!< [in] ���W�ϊ��֐�
	T& t, //!< [out] �ӂƐڐG�����ۂ̃p�����[�^ t ���Ԃ�(0 �Ȃ�߂�l�̒��_�A1�Ȃ玟�̒��_)
	V3& nearestPt, //!< [out] �ŋߓ_���W(���W�ϊ���)���Ԃ�
	T& distance2 //!< [out] �ŋߓ_�Ƃ̋����̓��
) {
	vertexRadius *= vertexRadius;

	intptr_t vtindex = -1; // �ł��߂����_�̃C���f�b�N�X�ԍ�
	intptr_t egindex = -1; // �ł��߂��ӂ̂̍ŏ��̒��_�C���f�b�N�X�ԍ�
	T vtdist2; // �ł��߂����_�̋����̓��
	T egdist; // �ł��߂��ӂ̋���
	V1 vlt; // �Ō�ɍ��W�ϊ��������_���W

	SetMaxVal(vtdist2);
	SetMaxVal(egdist);

	if (edgeThickness < T(0))
		checkEdge = 0;

	if (1 <= count) {
		vlt = tf(vts[0]);

		// �擪�|�C���g�ƒ��_�̐ڐG����
		// �w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ�������ɒǉ�
		T d2 = (pt - vlt).LengthSquare();
		if (d2 <= vertexRadius) {
			vtindex = 0;
			vtdist2 = d2;
		}
	}

	for (intptr_t i = 1; i < count; i++) {
		// ����̑Ώے��_�����W�ϊ�
		V1 vt = tf(vts[i]);

		// �w��|�C���g�ƒ��_�̐ڐG����
		// �w��|�C���g���璸�_�܂ł̋����� vertexRadius �ȉ��ɂȂ�������ɒǉ�
		T d2 = (pt - vt).LengthSquare();
		if (d2 <= vertexRadius && d2 < vtdist2) {
			vtindex = i;
			vtdist2 = d2;
		}

		// ���Ɏw����W�����_�ƐڐG���Ă���ꍇ�ɂ͕ӂ̔���͕K�v����
		if (!checkEdge || vtindex != -1)
			continue;

		// �w��|�C���g�ƕӂ̐ڐG����
		Vector2d v1 = vt - vlt;
		double l = v1.LengthSquare();
		if (l == 0.0)
			continue; // ���_���d�Ȃ��Ă�̂ŕӂ͑��݂��Ȃ�

		l = sqrt(l);
		v1 /= l;

		Vector2d v2 = pt - vlt;
		double x = v1.Dot(v2);
		double y = v1.Vertical().Dot(v2);

		// ���W�ϊ���ɋ�`�͈͓̔��Ɏw��|�C���g������Ȃ���ɒǉ�
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

// �@�\: �������_�ō\������郉�C���̑S�Ă̕ӂ̒����̍��v���v�Z����
//
// �߂�l: ����
//
template<
	class V1 // ���_�z��x�N�g���^
>
_JUNK_GEOMETRY_INLINE double PolylineLengthTmpl(
	const V1* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count //!< [in] ���_��
) {
	double l = 0.0;
	for (intptr_t i = 1; i < count; i++)
		l += (vts[i] - vts[i - 1]).Length();
	return l;
}

// �@�\: SSE3���g�p���ĕ������_�ō\������郉�C���̑S�Ă̕ӂ̒����̍��v���v�Z����
//
// �߂�l: ����
//
_JUNK_GEOMETRY_INLINE double PolylineLengthSSE3(
	const VectorN<double, 2>* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count //!< [in] ���_��
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
//		�C���|�[�g�֐��錾
//==============================================================================

// �@�\ : �ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchMaxMin(
	const double* p, //!< [in] �ő�ŏ���������f�[�^
	intptr_t n, //!< [in] �����f�[�^���A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	double& min, //!< [out] �ŏ��l���Ԃ�
	double& max //!< [out] �ő�l���Ԃ�
);

// �@�\ : �����O�o�b�t�@������ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchMaxMinRing(
	const double* pBuffer, //!< [in] �����O�o�b�t�@�̐擪�|�C���^
	intptr_t nBufLen, //!< [in] �����O�o�b�t�@�̃T�C�Y�A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	intptr_t iIndex, //!< [in] �����J�n�ʒu�̃C���f�b�N�X�ԍ�
	intptr_t n, //!< [in] �����f�[�^���A0 �ȉ��̏ꍇ�͊֐����Ăяo���Ȃ��悤�ɂ��Ă�������
	double& min, //!< [out] �ŏ��l���Ԃ�
	double& max //!< [out] �ő�l���Ԃ�
);

// �@�\: double �^�Q����(X,Y)�x�N�g���z����󂯎��AX��Y���ꂼ��̍ő�ŏ�����������
//
JUNKAPI void JUNKCALL SearchPointMaxMin(
	const Vector2d* p, //!< [in] �ő�ŏ���������f�[�^
	intptr_t n, //!< [in] �f�[�^��
	double& minX, //!< [out] �ŏ�X�l���Ԃ�
	double& minY, //!< [out] �ŏ�Y�l���Ԃ�
	double& maxX, //!< [out] �ő�X�l���Ԃ�
	double& maxY //!< [out] �ő�Y�l���Ԃ�
);

// �@�\ : ���`�ϊ����s��
//
JUNKAPI void JUNKCALL TransformLin(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	double scale, //!< [in] �X�P�[�����O�l
	double translate, //!< [in] ���s�ړ��l
	double* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ���`�ϊ���int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformLinInt(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	double scale, //!< [in] �X�P�[�����O�l
	double translate, //!< [in] ���s�ړ��l
	int* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ����`�ϊ�(Log,Pow)���܂ޕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLin(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const TransformInfo* pTis, //!< [in] �ϊ����z��
	intptr_t nTransform, //!< [in] pTis �̗v�f��
	double* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLinInt(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const TransformInfo* pTis, //!< [in] �ϊ����z��
	intptr_t nTransform, //!< [in] pTis �̗v�f��
	int* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ���`�ϊ���int�ɕϊ����s��
//        �o�͂� pDst �� POINT �\����(8�o�C�g)�̔z��ƌ��Ȃ��A�擪4�o�C�g�ɒl���������ݎc���4�o�C�g�͂��̂܂܎c��
//
JUNKAPI void JUNKCALL TransformLinToInt2(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	double scale, //!< [in] �X�P�[�����O�l
	double translate, //!< [in] ���s�ړ��l
	int* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//        �o�͂� pDst �� POINT �\����(8�o�C�g)�̔z��ƌ��Ȃ��A�擪4�o�C�g�ɒl���������ݎc���4�o�C�g�͂��̂܂܎c��
//
JUNKAPI void JUNKCALL TransformNonLinToInt2(
	const double* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const TransformInfo* pTis, //!< [in] �ϊ����z��
	intptr_t nTransform, //!< [in] pTis �̗v�f��
	int* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : double �^�Q����(X,Y)�x�N�g���z����󂯎��A�ϊ����� int �^�Q����(X,Y)�x�N�g���z��ɏo�͂���
//
JUNKAPI void JUNKCALL TransformLinPointDToPointI(
	const Vector2d* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	double scaleX, //!< [in] X���W�X�P�[�����O�l
	double translateX, //!< [in] X���W���s�ړ��l
	double scaleY, //!< [in] Y���W�X�P�[�����O�l
	double translateY, //!< [in] Y���W���s�ړ��l
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
JUNKAPI void JUNKCALL TransformNonLinPointDToPointI(
	const Vector2d* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const TransformInfo* pTisX, //!< [in] X���W�ϊ����z��
	intptr_t nTransformX, //!< [in] pTisX �̗v�f��
	const TransformInfo* pTisY, //!< [in] Y���W�ϊ����z��
	intptr_t nTransformY, //!< [in] pTisY �̗v�f��
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
);

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
JUNKAPI intptr_t JUNKCALL TransformForDraw(
	const TransformInfo* pTisX, //!< [in] X���ϊ����z��
	intptr_t nTisX, //!< [in] pTisX �̗v�f��
	const TransformInfo* pTisY, //!< [in] Y���ϊ����z��
	intptr_t nTisY, //!< [in] pTisY �̗v�f��
	const double* pSrcY, //!< [in] �ϊ�����Y���l�f�[�^
	intptr_t iStartIndexX, //!< [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, //!< [in] �ϊ��f�[�^��
	Vector2i* pDst //!< [out] �ϊ����XY�l�f�[�^�A�K�v�ȗv�f���͍��W�ϊ���̒l�ɂ��قȂ�A���W�ϊ����X�l�͈�*4+4���x�K�v
);

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����(�����O�o�b�t�@��)
//	Y���l�f�[�^�� pSrcY + iStartIndexY ����擾�J�n����ApSrcY + nSrcYBufLen �𒴂����� pSrcY �ɖ߂��Ď擾����������
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
JUNKAPI intptr_t JUNKCALL TransformForDrawRing(
	const TransformInfo* pTisX, //!< [in] X���ϊ����z��
	intptr_t nTisX, //!< [in] pTisX �̗v�f��
	const TransformInfo* pTisY, //!< [in] Y���ϊ����z��
	intptr_t nTisY, //!< [in] pTisY �̗v�f��
	const double* pSrcY, //!< [in] �ϊ�����Y���l�f�[�^(Y���f�[�^�o�b�t�@�̐擪�A�h���X)
	intptr_t nSrcYBufLen, //!< [in] pSrcY �̃o�b�t�@�̃T�C�Y(�f�[�^��)
	intptr_t iStartIndexX, //!< [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t iStartIndexY, //!< [in] Y���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, //!< [in] �ϊ��f�[�^��
	Vector2i* pDst //!< [out] �ϊ����XY�l�f�[�^�A�K�v�ȗv�f���͍��W�ϊ���̒l�ɂ��قȂ�A���W�ϊ����X�l�͈�*4+4���x�K�v
);

// �@�\ : �������_�ō\������郉�C���Ǝw��|�C���g�Ƃ̐ڐG������s��
//
// �߂�l: �ڐG���Ă���Ȃ�ڐG���_�ԍ����Ԃ�A����ȊO�͕������Ԃ�
//
JUNKAPI intptr_t JUNKCALL HitTestPointAndPolyline(
	const Vector2d* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count, //!< [in] ���_��
	Vector2d pt, //!< [in] �ڐG����|�C���g���W
	double vertexRadius, //!< [in] ���_�̔��a
	double edgeThickness, //!< [in] �ӂ̑���
	double& t, //!< [out] �ӂƐڐG�����ۂ̃p�����[�^ t ���Ԃ�(0 �Ȃ�߂�l�̒��_�A1�Ȃ玟�̒��_)
	Vector2d& pointOnEdge //!< [out] �ڐG�����ӂ̍ŋߓ_���W���Ԃ�
);

// �@�\ : �������_�ō\������郉�C���̑S�Ă̕ӂ̒����̍��v���v�Z����
//
// �߂�l: ����
//
JUNKAPI double JUNKCALL PolylineLength(
	const Vector2d* vts, //!< [in] ���_�z��擪�|�C���^
	intptr_t count //!< [in] ���_��
);


//==============================================================================
//		���W�ϊ��p�\���̐錾
//==============================================================================

#undef max
#undef min

#pragma pack(push,1)

struct TransformLinear // ���`���W�ϊ��\����
{
	double Scale; // �X�P�[�����O�l
	double Translate; // �X�P�[�����O��̈ړ���

	TransformLinear() // �R���X�g���N�^
	{
	}

	TransformLinear(const TransformLinear& c) // �R�s�[�R���X�g���N�^
	{
		*this = c;
	}

	TransformLinear(double s, double t) // �R���X�g���N�^�@�X�P�[�����O�l�ƈړ��l���w�肵�ď�����
	{
		Scale = s;
		Translate = t;
	}

	TransformLinear(const RangeD& src, const RangeD& dst) // �R���X�g���N�^�A�w�肳�ꂽ�͈͂��w�肳�ꂽ�͈͂փ}�b�s���O���鏉����
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

	double Cnv(double val) const // �������ŕϊ�
	{
		return val * Scale + Translate;
	}

	double InvCnv(double val) const // �t�����ŕϊ�
	{
		return (val - Translate) / Scale;
	}

	void Cnv(const double* pSrcArray, double* pDstArray, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s��
	{
		TransformLin(pSrcArray, num, Scale, Translate, pDstArray);
	}

	void Cnv(const double* pSrcArray, int* pDstArray, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s��
	{
		TransformLinInt(pSrcArray, num, Scale, Translate, pDstArray);
	}

	void Cnv(const double* pSrcArray, Vector2i* pDstArray, BOOL convertY, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s���A�ϊ���f�[�^�� pDstArray �Ɋi�[�����(convertY ��0�Ȃ� x �ɁA����ȊO�Ȃ� y�j
	{
		int* pDst = (int*)pDstArray;
		if (convertY)
			pDst++;
		TransformLinToInt2(pSrcArray, num, Scale, Translate, pDst);
	}

	TransformLinear Multiply(const TransformLinear& transform) const // �ϊ�������
	{
		return TransformLinear(Scale * transform.Scale, Translate * transform.Scale + transform.Translate);
	}

	TransformLinear Invert() const // �t�ϊ����쐬
	{
		return TransformLinear(1.0 / Scale, -Translate / Scale);
	}

	BOOL operator!=(const TransformLinear& c) const // ���e�̕s��v�̃`�F�b�N
	{
		return Scale != c.Scale || Translate != c.Translate;
	}
};

struct TransformInfo // Transform �\���̓��Ŏg�p�����P��̍��W�ϊ����
{
	int LogBeforeLinear; // ���j�A�ϊ��̑O�� log10 �����s���邩�ǂ���
	int PowAfterLinear; // ���j�A�ϊ��̐Ղ� pow10 �����s���邩�ǂ���
	TransformLinear Transform; // ���j�A���W�ϊ��\����

	TransformInfo() // �f�t�H���g�R���X�g���N�^
	{
	}

	TransformInfo(int lbl, int pal, const TransformLinear& tf) // �R���X�g���N�^�A�l���w�肵�ď���������
	{
		LogBeforeLinear = lbl;
		PowAfterLinear = pal;
		Transform = tf;
	}

	TransformInfo Invert() const // �t�ϊ����쐬����
	{
		return TransformInfo(PowAfterLinear, LogBeforeLinear, Transform.Invert());
	}
};

//! ���W�ϊ��N���X�A����`�̕ϊ�(���O)���܂�
struct Transform 
{
	std::vector<TransformInfo> TransformInfos; // ���W�ϊ����z��A���O���܂ޏꍇ�͑S�Ă̕ϊ����P�� TransformLinear �ɍ������邱�Ƃ͂ł��Ȃ����ߔz��ƂȂ��Ă���A0�Ԗڂ̗v�f���珇�ɕϊ����s��
	double MinVal; // Cnv ���\�b�h�ŕϊ��\�ȍŏ��l�ALog �ϊ����Ɏg�p����

	Transform() // �f�t�H���g�R���X�g���N�^
	{
	}

	Transform(intptr_t tiCount, const TransformInfo* pTis, double minVal) // �R���X�g���N�^�A�ϊ����z����w�肵�ď���������
	{
		TransformInfos.insert(TransformInfos.end(), pTis, pTis + tiCount);
		MinVal = DBL_MIN;
	}

	Transform(const std::vector<TransformInfo>& tis, double minVal) // �R���X�g���N�^�A�ϊ����z����w�肵�ď���������
	{
		TransformInfos.insert(TransformInfos.end(), tis.begin(), tis.end());
		MinVal = DBL_MIN;
	}

	Transform(double scale, double translate) // �R���X�g���N�^�A�k�ڂƈړ��ʂ��w�肵�ď���������
	{
		TransformInfos.push_back(TransformInfo(FALSE, FALSE, TransformLinear(scale, translate)));
		MinVal = DBL_MIN;
	}

	Transform(RangeD rangeBefore, RangeD rangeAfter, BOOL log) // �R���X�g���N�^�A�ϊ��O�ƕϊ���͈̔͂ƃ��O�t���O���w�肵�ď���������
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

	double Cnv(double val) const // ���W�ϊ����s��
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

	double InvCnv(double val) const // �t���W�ϊ����s��
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

	void Cnv(const double* pSrcArray, double* pDstArray, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s��
	{
		TransformNonLin(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDstArray);
	}

	void Cnv(const double* pSrcArray, int* pDstArray, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s��
	{
		TransformNonLinInt(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDstArray);
	}

	void Cnv(const double* pSrcArray, Vector2i* pDstArray, BOOL convertY, intptr_t num) // �w��z����̕����f�[�^�̍��W�ϊ����s���A�ϊ���f�[�^�� pDstArray �Ɋi�[�����(convertY ��0�Ȃ� x �ɁA����ȊO�Ȃ� y�j
	{
		int* pDst = (int*)pDstArray;
		if (convertY)
			pDst++;
		TransformNonLinToInt2(pSrcArray, num, &TransformInfos[0], TransformInfos.size(), pDst);
	}

	Transform Multiply(const Transform& transform) const // ���W�ϊ�����������A�ϊ��̏��Ԃ� this �� transform
	{
		//	���W�ϊ����z�������
		std::vector<TransformInfo> tis;
		const std::vector<TransformInfo>& tis2 = transform.TransformInfos;
		tis.insert(tis.end(), TransformInfos.begin(), TransformInfos.end());
		for (intptr_t i = 0, n = tis2.size(); i < n; i++) {
			TransformInfo& ti1 = tis[tis.size() - 1];
			const TransformInfo& ti2 = tis2[i];
			if (ti1.PowAfterLinear == ti2.LogBeforeLinear) {
				//	���ڂ� Pow10 �� Log10 ���������݂��Ă��邩�������݂��Ȃ��ꍇ
				//	���̏ꍇ�̓��j�A�ϊ��������ł���
				tis[tis.size() - 1] = TransformInfo(ti1.LogBeforeLinear, ti2.PowAfterLinear, ti1.Transform.Multiply(ti2.Transform));
			} else {
				//	���ڂ� Pow10 �� Log10 �̂ǂ��炩�����݂���ꍇ
				//	���̏ꍇ�̓��j�A�ϊ��������ł��Ȃ�
				tis.push_back(ti2);
			}
		}

		return Transform(tis, std::max(MinVal, transform.MinVal));
	}

	Transform Invert() const // �t�ϊ����쐬����
	{
		Transform tf;
		intptr_t n = TransformInfos.size();
		tf.TransformInfos.resize(n);
		tf.MinVal = Cnv(MinVal);
		for (intptr_t i = 0; i < n; i++)
			tf.TransformInfos[i] = TransformInfos[n - i - 1].Invert();
		return tf;
	}

	_FINLINE static double Log10(double val) // ���̍��W�ϊ�������p�� Log10
	{
		if (val == 0.0)
			return 0.0;
		if (0.0 < val)
			return log10(val);
		else
			return -log10(-val); // ���ʂ���Ȍv�Z�Ȃ����A�Ȃ�Ƃ��Ăł��O���t�ŕ\������
	}

	_FINLINE static double Pow10(double val) // ���̍��W�ϊ�������p�� Pow10
	{
		return pow(10.0, val);
	}
};

// �@�\ : double �^�Q����(X,Y)�x�N�g���z����󂯎��A�ϊ����� int �^�Q����(X,Y)�x�N�g���z��ɏo�͂���
//
_FINLINE void TransformPoints(
	const Vector2d* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const TransformLinear& tfx, //!< [in] X�����W�ϊ��I�u�W�F�N�g
	const TransformLinear& tfy, //!< [in] Y�����W�ϊ��I�u�W�F�N�g
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
) {
	TransformLinPointDToPointI(pSrc, n, tfx.Scale, tfx.Translate, tfy.Scale, tfy.Translate, pDst);
}

// �@�\ : ����`�ϊ�(Log,Pow)��int�ɕϊ����s��
//
_FINLINE void TransformPoints(
	const Vector2d* pSrc, //!< [in] �ϊ����̃f�[�^
	intptr_t n, //!< [in] �ϊ��f�[�^��
	const Transform& tfx, //!< [in] X�����W�ϊ��I�u�W�F�N�g
	const Transform& tfy, //!< [in] Y�����W�ϊ��I�u�W�F�N�g
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
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

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
_FINLINE intptr_t TransformForDraw(
	const Transform& tfx, //!< [in] X�����W�ϊ��I�u�W�F�N�g
	const Transform& tfy, //!< [in] Y�����W�ϊ��I�u�W�F�N�g
	const double* pSrcY, //!< [in] �ϊ�����Y���l�f�[�^
	intptr_t iStartIndexX, //!< [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, //!< [in] �ϊ��f�[�^��
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
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

// �@�\ : �`��p�ɐ������W�ɕϊ�����A�������W�ɖ��ʂȕ`����s��Ȃ��悤�ɕϊ�����(�����O�o�b�t�@��)
//	Y���l�f�[�^�� pSrcY + iStartIndexY ����擾�J�n����ApSrcY + nSrcYBufLen �𒴂����� pSrcY �ɖ߂��Ď擾����������
//
// �Ԃ�l : �ϊ���̃f�[�^���A�������Ԃ�΃G���[
//		-1=�������s��
//
_FINLINE intptr_t TransformForDrawRing(
	const Transform& tfx, //!< [in] X�����W�ϊ��I�u�W�F�N�g
	const Transform& tfy, //!< [in] Y�����W�ϊ��I�u�W�F�N�g
	const double* pSrcY, //!< [in] �ϊ�����Y���l�f�[�^(Y���f�[�^�o�b�t�@�̐擪�A�h���X)
	intptr_t nSrcYBufLen, //!< [in] pSrcY �̃o�b�t�@�̃T�C�Y(�f�[�^��)
	intptr_t iStartIndexX, //!< [in] X���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t iStartIndexY, //!< [in] Y���l�v�Z�p�̃C���f�b�N�X�ԍ��J�n�l
	intptr_t n, //!< [in] �ϊ��f�[�^��
	Vector2i* pDst //!< [out] �ϊ���̃f�[�^
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

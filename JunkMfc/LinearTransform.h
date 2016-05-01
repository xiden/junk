#pragma once
#ifndef __LINEARTRANSFORM_H__
#define __LINEARTRANSFORM_H__
#include "Value.h"

//! ���`�ϊ��p�N���X�A�X�P�[�����O�l�ƈړ��l������
struct LinearTransform {
	double dScale; // �X�P�[�����O�l
	double dTranslate; // �ړ��l

	// �@�\ : �R���X�g���N�^
	LinearTransform() {
	}

	// �@�\ : �R���X�g���N�^�@�X�P�[�����O�l�ƈړ��l���w�肵�ď�����
	LinearTransform(
		double s, // [in] �X�P�[�����O�l
		double t // [in] �ړ��l
		) {
		dScale = s;
		dTranslate = t;
	}

	// �@�\ : �R���X�g���N�^�@�͈͂��Q�w�肵�ď������Asrc1 �� dst1 �� src2 �� dst2 �ɂȂ�悤�ȕϊ��I�u�W�F�N�g�ƂȂ�
	LinearTransform(
		double src1, // [in] �ϊ����l1
		double src2, // [in] �ϊ����l2
		double dst1, // [in] �ϊ���l1
		double dst2 // [in] �ϊ���l2
		) {
		double s = src2 - src1;
		if (s != 0.0) {
			dScale = (dst2 - dst1) / s;
			dTranslate = dst1 - src1 * dScale;
		} else {
			dScale = 1.0;
			dTranslate = 0.0;
		}
	}

	// �@�\ : �������ŕϊ�
	__forceinline double Cnv(
		double val // [in] �ϊ����l
		) const {
		return val * dScale + dTranslate;
	}

	// �@�\ : �t�����ŕϊ�
	__forceinline double InvCnv(
		double val // [in] �ϊ����l
		) const {
		return (val - dTranslate) / dScale;
	}

	// �@�\ : ���`�ϊ��I�u�W�F�N�g���m����������Athis(����) �� transform ���� �̏��ŕϊ��Ɠ������ʂƂȂ�ϊ��I�u�W�F�N�g���쐬����
	LinearTransform Multiply(
		const LinearTransform& transform // [in] ����������`�ϊ��I�u�W�F�N�g
		) const {
		return LinearTransform(dScale * transform.dScale, dTranslate * transform.dScale + transform.dTranslate);
	}

	// �@�\ : �t�ϊ����쐬
	LinearTransform Invert() const {
		return LinearTransform(1.0 / dScale, -dTranslate / dScale);
	}

	// �@�\ : ���e�̕s��v�̃`�F�b�N
	BOOL operator!=(const LinearTransform& c) const {
		return dScale != c.dScale || dTranslate != c.dTranslate;
	}
};

#endif

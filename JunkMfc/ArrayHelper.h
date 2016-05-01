#pragma once
#ifndef __ARRAYHELPER_H__
#define __ARRAYHELPER_H__

#include <afxtempl.h>
#include <vector>

class CArrayHelper // �z��ɑ΂��鏈���̕⏕�֐��Ȃǂ��W�߂��N���X
{
public:
	static void Fill(int nCount, int* pData, int val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Fill(int nCount, double* pData, double val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Sort(int nCount, int* pData); // �����z��̃\�[�g
	static void Sort(int nCount, double* pData); // �����z��̃\�[�g
	static int Unique(int nCount, int* pData); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static int Unique(int nCount, double* pData); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static void SequentialNumber(int nCount, int* pData, int nStartVal); // �����z��ɘA�Ԃ�ݒ�
	static int Find(int nCount, const int* pData, int nVal); // �����z�񂩂�w��l������
	static int Find(int nCount, const double* pData, double dVal); // �����z�񂩂�w��l������
	static int Find(int nCount, const CString* pData, CString sVal); // ������z�񂩂�w��l������
	static BOOL IsZero(int nCount, const int* pData); // �z��̑S�Ă̗v�f���O���ǂ������ׂ�

	template<class T> static void Copy(std::vector<T>& dst, const std::vector<T>& src); // �����z����R�s�[����
	template<class T> static void Copy(CArray<T>& dst, const CArray<T>& src); // �����z����R�s�[����
	template<class T> static void SetSize(CArray<T>& array, int nCount); // �z��̃T�C�Y��ݒ肷��
	template<class T> static void SetSize(std::vector<T>& array, int nCount); // �z��̃T�C�Y��ݒ肷��
	template<class T> static int GetSize(const CArray<T>& array); // �z��̃T�C�Y���擾����
	template<class T> static int GetSize(const std::vector<T>& array); // �z��̃T�C�Y���擾����
	template<class T> static int PushBack(CArray<T>& array, const T& val); // �z��̌��֎w��̒l��ǉ�����
	template<class T> static int PushBack(std::vector<T>& array, const T& val); // �z��̌��֎w��̒l��ǉ�����
	template<class T> static void Append(CArray<T>& dstArray, const CArray<T>& srcArray); // �z��̌��֕ʂ̔z���ǉ�����
	template<class T> static void Append(CArray<T>& dstArray, const std::vector<T>& srcArray); // �z��̌��֕ʂ̔z���ǉ�����
	template<class T> static void Append(std::vector<T>& dstArray, const CArray<T>& srcArray); // �z��̌��֕ʂ̔z���ǉ�����
	template<class T> static void Append(std::vector<T>& dstArray, const std::vector<T>& srcArray); // �z��̌��֕ʂ̔z���ǉ�����
	template<class DARRY, class IARRY> static BOOL SwitchesToIndices(DARRY& dstArray, const IARRY& switches); // switches[i] ��0�ȊO�ɂȂ鎞�� i �� dstArray �ɒǉ�����
	template<class DARRY, class IARRY> static BOOL IndicesToSwitches(DARRY& dstArray, const IARRY& indices); // dstArray[indices[]] �� 1 ��ݒ肷��AdstArray �̃T�C�Y�� indices �̒l�̍ő�l�Ɋg�������
	template<class T, class IARRY> static BOOL IndicesToSwitches(int nDstArraySize, T* pDstArray, const IARRY& indices); // pDstArray[indices[]] �� 1 ��ݒ肷��A indices[] �� nDstArraySize �ȏゾ�����ꍇ�ɂ͏������Ȃ�
	template<class DARRY, class SARRY, class IARRY> static BOOL Extract(DARRY& dstArray, const SARRY& srcArray, const IARRY& indices); // srcArray ���� indices �Ŏw�肳�ꂽ�C���f�b�N�X�̗v�f�𔲂��o�� dstArray �̌��ɒǉ�����
	template<class DARRY, class SARRY, class IARRY> static BOOL ExtractBySwitches(DARRY& dstArray, const SARRY& srcArray, const IARRY& switches); // srcArray ���� switches �� 0 �ȊO�̃C���f�b�N�X�̗v�f�𔲂��o�� dstArray �̌��ɒǉ�����
	template<class T> static T Sum(const CArray<T>& srcArray); // srcArray �̗v�f�̒l�S�Ă̍��v�l���v�Z����
	template<class T> static T Sum(const std::vector<T>& srcArray); // srcArray �̗v�f�̒l�S�Ă̍��v�l���v�Z����
	template<class T, class Array> static BOOL Max(const Array& srcArray, T& Max); // srcArray �̗v�f�ōő�l�� Max �֎擾����A�擾�ł����ꍇ�� TRUE ��Ԃ�����ȊO�� FALSE ���Ԃ�
	template<class T, class Array> static BOOL Min(const Array& srcArray, T& Min); // srcArray �̗v�f�ōŏ��l�� Min �֎擾����A�擾�ł����ꍇ�� TRUE ��Ԃ�����ȊO�� FALSE ���Ԃ�


	static void Fill(CArray<int>& array, int val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Fill(CArray<double>& array, double val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Sort(CArray<int>& array); // �����z��̃\�[�g
	static void Sort(CArray<double>& array); // �����z��̃\�[�g
	static void Unique(CArray<int>& array); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static void Unique(CArray<double>& array); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static void SequentialNumber(CArray<int>& array, int nStartVal); // �����z��ɘA�Ԃ�ݒ�
	static int Find(const CArray<int>& array, int nVal); // �����z�񂩂�w��l������
	static int Find(const CArray<double>& array, double dVal); // �����z�񂩂�w��l������
	static int Find(const CArray<CString>& array, CString sVal); // ������z�񂩂�w��l������
	static void MakeArray(CArray<int>& array, int nCount, const int* pSrcData); // �����z����쐬����
	static void MakeArray(CArray<double>& array, int nCount, const double* pSrcData); // �����z����쐬����
	static BOOL IsZero(const CArray<int>& array); // �z��̑S�Ă̗v�f���O���ǂ������ׂ�
	static void Append(CArray<int>& dstArray, int nCount, const int* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����
	static void Append(CArray<double>& dstArray, int nCount, const double* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����
	static void Append(CArray<CString>& dstArray, int nCount, const CString* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����

	static void Fill(std::vector<int>& array, int val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Fill(std::vector<double>& array, double val); // �����z��̑S�v�f�ɓ����l��ݒ�
	static void Sort(std::vector<int>& array); // �����z��̃\�[�g
	static void Sort(std::vector<double>& array); // �����z��̃\�[�g
	static void Unique(std::vector<int>& array); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static void Unique(std::vector<double>& array); // �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
	static void SequentialNumber(std::vector<int>& array, int nStartVal); // �����z��ɘA�Ԃ�ݒ�
	static int Find(const std::vector<int>& array, int nVal); // �����z�񂩂�w��l������
	static int Find(const std::vector<double>& array, double dVal); // �����z�񂩂�w��l������
	static int Find(const std::vector<CString>& array, CString sVal); // ������z�񂩂�w��l������
	static void MakeArray(std::vector<int>& array, int nCount, const int* pSrcData); // �����z����쐬����
	static void MakeArray(std::vector<double>& array, int nCount, const double* pSrcData); // �����z����쐬����
	static BOOL IsZero(const std::vector<int>& array); // �z��̑S�Ă̗v�f���O���ǂ������ׂ�
	static void Append(std::vector<int>& dstArray, int nCount, const int* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����
	static void Append(std::vector<double>& dstArray, int nCount, const double* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����
	static void Append(std::vector<CString>& dstArray, int nCount, const CString* pSrcData); // �z��̌��֕ʂ̔z���ǉ�����

	static void Copy(std::vector<int>& dst, const CArray<int>& src); // �����z����R�s�[����
	static void Copy(std::vector<double>& dst, const CArray<double>& src); // �����z����R�s�[����
	static void Copy(std::vector<CString>& dst, const CArray<CString>& src); // ������z����R�s�[����

	static void Copy(CArray<int>& dst, const std::vector<int>& src); // �����z����R�s�[����
	static void Copy(CArray<double>& dst, const std::vector<double>& src); // �����z����R�s�[����
	static void Copy(CArray<CString>& dst, const std::vector<CString>& src); // ������z����R�s�[����

	static void AddValue(std::vector<int>& dst, int val); // �����z��̑S�v�f�Ɏw��l�����Z����
	static void AddValue(std::vector<double>& dst, double val); // �����z��̑S�v�f�Ɏw��l�����Z����

	static void AddValue(CArray<int>& dst, int val); // �����z��̑S�v�f�Ɏw��l�����Z����
	static void AddValue(CArray<double>& dst, double val); // �����z��̑S�v�f�Ɏw��l�����Z����
};


template<class T> void CArrayHelper::Copy(std::vector<T>& dst, const std::vector<T>& src) // �����z����R�s�[����
{
	dst = src;
}

template<class T> void CArrayHelper::Copy(CArray<T>& dst, const CArray<T>& src) // �����z����R�s�[����
{
	dst.Copy(src);
}

template<class T> void CArrayHelper::SetSize(CArray<T>& array, int nCount) // �z��̃T�C�Y��ݒ肷��
{
	array.SetSize(nCount);
}
template<class T> void CArrayHelper::SetSize(std::vector<T>& array, int nCount) // �z��̃T�C�Y��ݒ肷��
{
	array.resize(nCount);
}

template<class T> int CArrayHelper::GetSize(const CArray<T>& array) // �z��̃T�C�Y���擾����
{
	return (int)array.GetSize();
}
template<class T> int CArrayHelper::GetSize(const std::vector<T>& array) // �z��̃T�C�Y���擾����
{
	return (int)array.size();
}

template<class T> int CArrayHelper::PushBack(CArray<T>& array, const T& val) // �z��̌��֎w��̒l��ǉ�����
{
	return (int)array.Add(val);
}
template<class T> int CArrayHelper::PushBack(std::vector<T>& array, const T& val) // �z��̌��֎w��̒l��ǉ�����
{
	int n = (int)array.size();
	array.push_back(val);
	return n;
}

template<class T> void CArrayHelper::Append(CArray<T>& dstArray, const CArray<T>& srcArray) // �z��̌��֕ʂ̔z���ǉ�����
{
	if(srcArray.IsEmpty())
		return;
	Append(dstArray, (int)srcArray.GetCount(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(CArray<T>& dstArray, const std::vector<T>& srcArray) // �z��̌��֕ʂ̔z���ǉ�����
{
	if(srcArray.empty())
		return;
	Append(dstArray, (int)srcArray.size(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(std::vector<T>& dstArray, const CArray<T>& srcArray) // �z��̌��֕ʂ̔z���ǉ�����
{
	if(srcArray.IsEmpty())
		return;
	Append(dstArray, (int)srcArray.GetCount(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(std::vector<T>& dstArray, const std::vector<T>& srcArray) // �z��̌��֕ʂ̔z���ǉ�����
{
	if(srcArray.empty())
		return;
	Append(dstArray, (int)srcArray.size(), &srcArray[0]);
}

template<class DARRY, class IARRY> static BOOL CArrayHelper::SwitchesToIndices(DARRY& dstArray, const IARRY& switches) // switches[i] ��0�ȊO�ɂȂ鎞�� i �� dstArray �ɒǉ�����
{
	for(int i = 0, n = GetSize(switches); i < n; i++)
	{
		if(switches[i])
			PushBack(dstArray, i);
	}
	return TRUE;
}

template<class DARRY, class IARRY> static BOOL CArrayHelper::IndicesToSwitches(DARRY& dstArray, const IARRY& indices) // dstArray[indices[]] �� 1 ��ݒ肷��AdstArray �̃T�C�Y�� indices �̒l�̍ő�l�Ɋg�������
{
	for(int i = 0, n = GetSize(indices); i < n; i++)
	{
		int index = indices[i];
		if(GetSize(dstArray) <= index)
			SetSize(dstArray, index + 1);
		dstArray[index] = 1;
	}
	return TRUE;
}

template<class T, class IARRY> static BOOL CArrayHelper::IndicesToSwitches(int nDstArraySize, T* pDstArray, const IARRY& indices) // pDstArray[indices[]] �� 1 ��ݒ肷��A indices[] �� nDstArraySize �ȏゾ�����ꍇ�ɂ͏������Ȃ�
{
	for(int i = 0, n = GetSize(indices); i < n; i++)
	{
		int index = indices[i];
		if(index < nDstArraySize)
			pDstArray[index] = 1;
	}
	return TRUE;
}

template<class DARRY, class SARRY, class IARRY> BOOL CArrayHelper::Extract(DARRY& dstArray, const SARRY& srcArray, const IARRY& indices) // srcArray ���� indices �Ŏw�肳�ꂽ�C���f�b�N�X�̗v�f�𔲂��o�� dstArray �̌��ɒǉ�����
{
	int nSize = GetSize(srcArray);
	for(int i = 0, n = GetSize(indices); i < n; i++)
	{
		int index = indices[i];
		if(index < 0 || nSize <= index)
			return FALSE; // �w�肳�ꂽ�C���f�b�N�X���z��O�Ȃ�G���[
		PushBack(dstArray, srcArray[index]);
	}
	return TRUE;
}

template<class DARRY, class SARRY, class IARRY> BOOL CArrayHelper::ExtractBySwitches(DARRY& dstArray, const SARRY& srcArray, const IARRY& switches) // srcArray ���� switches �� 0 �ȊO�̃C���f�b�N�X�̗v�f�𔲂��o�� dstArray �̌��ɒǉ�����
{
	int n = GetSize(switches);
	if(GetSize(srcArray) < n)
		return FALSE; // switches �̗v�f���� srcArray �̗v�f�����傫����΃G���[
	for(int i = 0; i < n; i++)
	{
		if(switches[i])
			PushBack(dstArray, srcArray[i]);
	}
	return TRUE;
}

template<class T> T CArrayHelper::Sum(const CArray<T>& srcArray) // srcArray �̗v�f�̒l�S�Ă̍��v�l���v�Z����
{
	T sum = T();
	for(int i = 0, n = (int)srcArray.GetCount(); i < n; i++)
		sum += srcArray[i];
	return sum;
}
template<class T> T CArrayHelper::Sum(const std::vector<T>& srcArray) // srcArray �̗v�f�̒l�S�Ă̍��v�l���v�Z����
{
	T sum = T();
	for(int i = 0, n = (int)srcArray.size(); i < n; i++)
		sum += srcArray[i];
	return sum;
}

template<class T, class Array> BOOL CArrayHelper::Max(const Array& srcArray, T& Max) // srcArray �̗v�f�ōő�l�� Max �֎擾����A�擾�ł����ꍇ�� TRUE ��Ԃ�����ȊO�� FALSE ���Ԃ�
{
	int n = GetSize(srcArray);
	if(n == 0)
		return FALSE;
	T tMax = srcArray[0];
	for(int i = 1; i < n; i++)
	{
		T t = srcArray[i];
		if(tMax < t)
			tMax = t;
	}
	Max = tMax;
	return TRUE;
}
template<class T, class Array> BOOL CArrayHelper::Min(const Array& srcArray, T& Min) // srcArray �̗v�f�ōŏ��l�� Min �֎擾����A�擾�ł����ꍇ�� TRUE ��Ԃ�����ȊO�� FALSE ���Ԃ�
{
	int n = GetSize(srcArray);
	if(n == 0)
		return FALSE;
	T tMin = srcArray[0];
	for(int i = 1; i < n; i++)
	{
		T t = srcArray[i];
		if(t < tMin)
			tMin = t;
	}
	Min = tMin;
	return TRUE;
}


//	srcArray ���̃����o�ϐ� var �� cmp �Ŕ�r���� true �Ȃ� result �ɃC���f�b�N�X�ԍ����Ԃ�A������Ȃ��Ȃ� -1 ���Ԃ�
#define ARRAYHELPER_FINDFIRST_VAR_CMP(srcArray, var, cmp, result) \
	{ \
		int __ARRAYHELPER_I__, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); \
		for(__ARRAYHELPER_I__ = 0; __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) { (result) = __ARRAYHELPER_I__; break; } \
		if(__ARRAYHELPER_I__ == __ARRAYHELPER_N__) (result) = -1; \
	}

//	srcArray ���̃����o�ϐ� var �� dstArray �֒ǉ�����
#define ARRAYHELPER_EXTRACT_VAR(dstArray, srcArray, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	srcArray ���̃����o�ϐ� var �� cmp �Ŕ�r���� true �Ȃ� dstArray �֒ǉ�����
#define ARRAYHELPER_EXTRACT_VAR_CMP(dstArray, srcArray, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	srcArray ���̃����o�ϐ� cmpvar �� cmp �Ŕ�r���� true �Ȃ� var �� dstArray �֒ǉ�����
#define ARRAYHELPER_EXTRACT_VAR2_CMP(dstArray, srcArray, cmpvar, cmp, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].cmpvar cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	dstArray.Add(srcArray[indices[]].var) �����s����Aindices �̒l�͈̔̓`�F�b�N�͍s��Ȃ�
#define ARRAYHELPER_EXTRACT_IDXVAR(dstArray, srcArray, indices, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	if(srcArray[indices[]].var cmp) dstArray.Add(srcArray[indices[]].var) �����s����Aindices �̒l�͈̔̓`�F�b�N�͍s��Ȃ�
#define ARRAYHELPER_EXTRACT_IDXVAR_CMP(dstArray, srcArray, indices, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[indices[__ARRAYHELPER_I__]].var cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	if(srcArray[indices[]].cmpvar cmp) dstArray.Add(srcArray[indices[]].var) �����s����Aindices �̒l�͈̔̓`�F�b�N�͍s��Ȃ�
#define ARRAYHELPER_EXTRACT_IDXVAR2_CMP(dstArray, srcArray, indices, cmpvar, cmp, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[indices[__ARRAYHELPER_I__]].cmpvar cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	srcArray ���̃����o�ϐ� var �� cmp �Ŕ�r���� true �Ȃ�C���f�b�N�X�ԍ��� dstArray �֒ǉ�����
#define ARRAYHELPER_EXTRACT_INDICES_VAR_CMP(dstArray, srcArray, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) \
				CArrayHelper::PushBack(dstArray, __ARRAYHELPER_I__); \
	}

//	srcArray �̗v�f�� func �֐��ŕ]������ true �Ȃ� dstArray �֒ǉ�����
#define ARRAYHELPER_EXTRACT_FUNC(dstArray, srcArray, func) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(func(srcArray[__ARRAYHELPER_I__])) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__]); \
	}

#endif

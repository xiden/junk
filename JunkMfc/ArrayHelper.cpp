#include "StdAfx.h"
#include ".\arrayhelper.h"
#include <algorithm>


// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
template<class T>
inline void Append(
	CArray<T>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const T* pSrcData // [in] �f�[�^
)
{
	INT_PTR n1 = dstArray.GetCount();
	INT_PTR n2 = nCount;
	dstArray.SetSize(n1 + n2);
	for(INT_PTR i = 0; i < n2; i++)
		dstArray[n1 + i] = pSrcData[i];
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
template<class T>
inline void Append(
	std::vector<T>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const T* pSrcData // [in] �f�[�^
)
{
	size_t n1 = dstArray.size();
	size_t n2 = nCount;
	dstArray.resize(n1 + n2);
	for(size_t i = 0; i < n2; i++)
		dstArray[n1 + i] = pSrcData[i];
}


//------------------------------------------------------------------------------
// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	int nCount, // [in] �����Ώۗv�f��
	int* pData, // [in,out] �f�[�^�ւ̃|�C���^
	int val // [in] �ݒ肷��l
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = val;
}

// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	int nCount, // [in] �����Ώۗv�f��
	double* pData, // [in,out] �f�[�^�ւ̃|�C���^
	double val // [in] �ݒ肷��l
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = val;
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	int nCount, // [in] �����Ώۗv�f��
	int* pData // [in,out] �f�[�^�ւ̃|�C���^
)
{
	std::sort(pData, pData + nCount);
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	int nCount, // [in] �����Ώۗv�f��
	double* pData // [in,out] �f�[�^�ւ̃|�C���^
)
{
	std::sort(pData, pData + nCount);
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
// �Ԃ�l : �d���l�폜��̗v�f��
//
int CArrayHelper::Unique(
	int nCount, // [in] �����Ώۗv�f��
	int* pData // [in,out] �f�[�^�ւ̃|�C���^
)
{
	int* e = std::unique(pData, pData + nCount);
	return int(e - pData);
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
// �Ԃ�l : �d���l�폜��̗v�f��
//
int CArrayHelper::Unique(
	int nCount, // [in] �����Ώۗv�f��
	double* pData // [in,out] �f�[�^�ւ̃|�C���^
)
{
	double* e = std::unique(pData, pData + nCount);
	return int(e - pData);
}

// �@�\ : �����z��ɘA�Ԃ�ݒ�
// 
void CArrayHelper::SequentialNumber(
	int nCount, // [in] �����Ώۗv�f��
	int* pData, // [in,out] �f�[�^�ւ̃|�C���^
	int nStartVal // [in] �A�Ԃ̊J�n�l
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = nStartVal + i;
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	int nCount, // [in] �z��v�f��
	const int* pData, // [in] �z��̐擪�ւ̃|�C���^
	int nVal // [in] �T���l
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == nVal)
			return i;
	return -1;
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	int nCount, // [in] �z��v�f��
	const double* pData, // [in] �z��̐擪�ւ̃|�C���^
	double dVal // [in] �T���l
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == dVal)
			return i;
	return -1;
}

// �@�\ : ������z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	int nCount, // [in] �z��v�f��
	const CString* pData, // [in] �z��̐擪�ւ̃|�C���^
	CString sVal // [in] �T���l
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == sVal)
			return i;
	return -1;
}

// �@�\ : �z��̑S�Ă̗v�f���O���ǂ������ׂ�
// 
// �Ԃ�l : TRUE=�S�Ă̗v�f���O�AFALSE=�O�ȊO�̗v�f�����݂���
// 
BOOL CArrayHelper::IsZero(
	int nCount, // [in] �z��v�f��
	const int* pData // [in] �f�[�^�ւ̃|�C���^
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i])
			return FALSE;
	return TRUE;
}

//------------------------------------------------------------------------------
// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	CArray<int>& array, // [in,out] �����Ώ۔z��
	int val // [in] �ݒ肷��l
)
{
	if(array.IsEmpty())
		return;
	Fill((int)array.GetCount(), &array[0], val);
}

// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	CArray<double>& array, // [in,out] �����Ώ۔z��
	double val // [in] �ݒ肷��l
)
{
	if(array.IsEmpty())
		return;
	Fill((int)array.GetCount(), &array[0], val);
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	CArray<int>& array // [in,out] �����Ώ۔z��
)
{
	if(array.IsEmpty())
		return;
	Sort((int)array.GetCount(), &array[0]);
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	CArray<double>& array // [in,out] �����Ώ۔z��
)
{
	if(array.IsEmpty())
		return;
	Sort((int)array.GetCount(), &array[0]);
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
void CArrayHelper::Unique(
	CArray<int>& array // [in,out] �����Ώ۔z��
)
{
	if(array.IsEmpty())
		return;
	array.SetSize(Unique((int)array.GetCount(), &array[0]));
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
void CArrayHelper::Unique(
	CArray<double>& array // [in,out] �����Ώ۔z��
)
{
	if(array.IsEmpty())
		return;
	array.SetSize(Unique((int)array.GetCount(), &array[0]));
}

// �@�\ : �����z��ɘA�Ԃ�ݒ�
// 
void CArrayHelper::SequentialNumber(
	CArray<int>& array, // [in,out] �����Ώ۔z��
	int nStartVal // [in] �A�Ԃ̊J�n�l
)
{
	if(array.IsEmpty())
		return;
	SequentialNumber((int)array.GetCount(), &array[0], nStartVal);
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const CArray<int>& array, // [in] �����Ώ۔z��
	int nVal // [in] �T���l
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], nVal);
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const CArray<double>& array, // [in] �����Ώ۔z��
	double dVal // [in] �T���l
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], dVal);
}

// �@�\ : ������z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const CArray<CString>& array, // [in] �����Ώ۔z��
	CString sVal // [in] �T���l
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], sVal);
}

// �@�\ : �����z����쐬����
// 
void CArrayHelper::MakeArray(
	CArray<int>& array, // [out] �z�񂪍쐬�����
	int nCount, // [in] �v�f��
	const int* pSrcData // [in] �R�s�[���f�[�^
)
{
	array.SetSize(nCount);
	if(array.IsEmpty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(int));
}

// �@�\ : �����z����쐬����
// 
void CArrayHelper::MakeArray(
	CArray<double>& array, // [out] �z�񂪍쐬�����
	int nCount, // [in] �v�f��
	const double* pSrcData // [in] �R�s�[���f�[�^
)
{
	array.SetSize(nCount);
	if(array.IsEmpty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(double));
}

// �@�\ : �z��̑S�Ă̗v�f���O���ǂ������ׂ�
// 
BOOL CArrayHelper::IsZero(
	const CArray<int>& array // [in] �����Ώ۔z��
)
{
	return IsZero((int)array.GetCount(), &array[0]);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	CArray<int>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const int* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	CArray<double>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const double* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	CArray<CString>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const CString* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}

//------------------------------------------------------------------------------
// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	std::vector<int>& array, // [in,out] �����Ώ۔z��
	int val // [in] �ݒ肷��l
)
{
	if(array.empty())
		return;
	Fill((int)array.size(), &array[0], val);
}

// �@�\ : �����z��̑S�v�f�ɓ����l��ݒ�
// 
void CArrayHelper::Fill(
	std::vector<double>& array, // [in,out] �����Ώ۔z��
	double val // [in] �ݒ肷��l
)
{
	if(array.empty())
		return;
	Fill((int)array.size(), &array[0], val);
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	std::vector<int>& array // [in,out] �����Ώ۔z��
)
{
	if(array.empty())
		return;
	Sort((int)array.size(), &array[0]);
}

// �@�\ : �����z��̃\�[�g
// 
void CArrayHelper::Sort(
	std::vector<double>& array // [in,out] �����Ώ۔z��
)
{
	if(array.empty())
		return;
	Sort((int)array.size(), &array[0]);
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
void CArrayHelper::Unique(
	std::vector<int>& array // [in,out] �����Ώ۔z��
)
{
	if(array.empty())
		return;
	array.resize(Unique((int)array.size(), &array[0]));
}

// �@�\ : �����z��̏d���l�폜�A�\�[�g�ς݂̔z��ɑ΂��Ă̂ݎg�p�\
// 
void CArrayHelper::Unique(
	std::vector<double>& array // [in,out] �����Ώ۔z��
)
{
	if(array.empty())
		return;
	array.resize(Unique((int)array.size(), &array[0]));
}

// �@�\ : �����z��ɘA�Ԃ�ݒ�
// 
void CArrayHelper::SequentialNumber(
	std::vector<int>& array, // [in,out] �����Ώ۔z��
	int nStartVal // [in] �A�Ԃ̊J�n�l
)
{
	if(array.empty())
		return;
	SequentialNumber((int)array.size(), &array[0], nStartVal);
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const std::vector<int>& array, // [in] �����Ώ۔z��
	int nVal // [in] �T���l
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], nVal);
}

// �@�\ : �����z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const std::vector<double>& array, // [in] �����Ώ۔z��
	double dVal // [in] �T���l
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], dVal);
}

// �@�\ : ������z�񂩂�w��l������
// 
// �Ԃ�l : ���������C���f�b�N�X�ԍ��A������Ȃ������ꍇ�� -1 ���Ԃ�B
// 
int CArrayHelper::Find(
	const std::vector<CString>& array, // [in] �����Ώ۔z��
	CString sVal // [in] �T���l
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], sVal);
}

// �@�\ : �����z����쐬����
// 
void CArrayHelper::MakeArray(
	std::vector<int>& array, // [out] �z�񂪍쐬�����
	int nCount, // [in] �v�f��
	const int* pSrcData // [in] �R�s�[���f�[�^
)
{
	array.resize(nCount);
	if(array.empty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(int));
}

// �@�\ : �����z����쐬����
// 
void CArrayHelper::MakeArray(
	std::vector<double>& array, // [out] �z�񂪍쐬�����
	int nCount, // [in] �v�f��
	const double* pSrcData // [in] �R�s�[���f�[�^
)
{
	array.resize(nCount);
	if(array.empty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(double));
}

// �@�\ : �z��̑S�Ă̗v�f���O���ǂ������ׂ�
// 
BOOL CArrayHelper::IsZero(
	const std::vector<int>& array // [in] �����Ώ۔z��
)
{
	return IsZero((int)array.size(), &array[0]);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	std::vector<int>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const int* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	std::vector<double>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const double* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}

// �@�\ : �z��̌��֕ʂ̔z���ǉ�����
// 
void CArrayHelper::Append(
	std::vector<CString>& dstArray, // [out] ���̔z��̌��ɕʂ̔z�񂪒ǉ������
	int nCount, // [in] �ǉ��v�f��
	const CString* pSrcData // [in] �f�[�^
)
{
	::Append(dstArray, nCount, pSrcData);
}


//------------------------------------------------------------------------------
// �@�\ : �����z����R�s�[����
// 
void CArrayHelper::Copy(
	std::vector<int>& dst, // [out] �R�s�[��z��
	const CArray<int>& src // [in] �R�s�[���z��
)
{
	dst.resize((size_t)src.GetCount());
	if(src.IsEmpty())
		return;
	memcpy(&dst[0], &src[0], dst.size() * sizeof(int));
}

// �@�\ : �����z����R�s�[����
// 
void CArrayHelper::Copy(
	std::vector<double>& dst, // [out] �R�s�[��z��
	const CArray<double>& src // [in] �R�s�[���z��
)
{
	dst.resize((size_t)src.GetCount());
	if(src.IsEmpty())
		return;
	memcpy(&dst[0], &src[0], dst.size() * sizeof(double));
}

// �@�\ : ������z����R�s�[����
// 
void CArrayHelper::Copy(
	std::vector<CString>& dst, // [out] �R�s�[��z��
	const CArray<CString>& src // [in] �R�s�[���z��
)
{
	int n = (int)src.GetCount();
	dst.resize(n);
	if(n == 0)
		return;
	for(int i = 0; i < n; i++)
		dst[i] = src[i];
}


// �@�\ : �����z����R�s�[����
// 
void CArrayHelper::Copy(
	CArray<int>& dst, // [out] �R�s�[��z��
	const std::vector<int>& src // [in] �R�s�[���z��
)
{
	dst.SetSize((INT_PTR)src.size());
	if(src.empty())
		return;
	memcpy(&dst[0], &src[0], dst.GetCount() * sizeof(int));
}

// �@�\ : �����z����R�s�[����
// 
void CArrayHelper::Copy(
	CArray<double>& dst, // [out] �R�s�[��z��
	const std::vector<double>& src // [in] �R�s�[���z��
)
{
	dst.SetSize((INT_PTR)src.size());
	if(src.empty())
		return;
	memcpy(&dst[0], &src[0], dst.GetCount() * sizeof(double));
}

// �@�\ : ������z����R�s�[����
// 
void CArrayHelper::Copy(
	CArray<CString>& dst, // [out] �R�s�[��z��
	const std::vector<CString>& src // [in] �R�s�[���z��
)
{
	int n = (int)src.size();
	dst.SetSize(n);
	if(n == 0)
		return;
	for(int i = 0; i < n; i++)
		dst[i] = src[i];
}

//------------------------------------------------------------------------------
// �@�\ : �����z��̑S�v�f�Ɏw��l�����Z����
// 
void CArrayHelper::AddValue(
	std::vector<int>& dst, // [out] �z��
	int val // [in] ���Z����l
)
{
	for(int i = 0, n = (int)dst.size(); i < n; i++)
		dst[i] += val;
}

// �@�\ : �����z��̑S�v�f�Ɏw��l�����Z����
// 
void CArrayHelper::AddValue(
	std::vector<double>& dst, // [out] �z��
	double val // [in] ���Z����l
)
{
	for(int i = 0, n = (int)dst.size(); i < n; i++)
		dst[i] += val;
}


// �@�\ : �����z��̑S�v�f�Ɏw��l�����Z����
// 
void CArrayHelper::AddValue(
	CArray<int>& dst, // [out] �z��
	int val // [in] ���Z����l
)
{
	for(int i = 0, n = (int)dst.GetCount(); i < n; i++)
		dst[i] += val;
}

// �@�\ : �����z��̑S�v�f�Ɏw��l�����Z����
// 
void CArrayHelper::AddValue(
	CArray<double>& dst, // [out] �z��
	double val // [in] ���Z����l
)
{
	for(int i = 0, n = (int)dst.GetCount(); i < n; i++)
		dst[i] += val;
}

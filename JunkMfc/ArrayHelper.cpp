#include "StdAfx.h"
#include ".\arrayhelper.h"
#include <algorithm>


// 機能 : 配列の後ろへ別の配列を追加する
// 
template<class T>
inline void Append(
	CArray<T>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const T* pSrcData // [in] データ
)
{
	INT_PTR n1 = dstArray.GetCount();
	INT_PTR n2 = nCount;
	dstArray.SetSize(n1 + n2);
	for(INT_PTR i = 0; i < n2; i++)
		dstArray[n1 + i] = pSrcData[i];
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
template<class T>
inline void Append(
	std::vector<T>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const T* pSrcData // [in] データ
)
{
	size_t n1 = dstArray.size();
	size_t n2 = nCount;
	dstArray.resize(n1 + n2);
	for(size_t i = 0; i < n2; i++)
		dstArray[n1 + i] = pSrcData[i];
}


//------------------------------------------------------------------------------
// 機能 : 整数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	int nCount, // [in] 処理対象要素数
	int* pData, // [in,out] データへのポインタ
	int val // [in] 設定する値
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = val;
}

// 機能 : 実数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	int nCount, // [in] 処理対象要素数
	double* pData, // [in,out] データへのポインタ
	double val // [in] 設定する値
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = val;
}

// 機能 : 整数配列のソート
// 
void CArrayHelper::Sort(
	int nCount, // [in] 処理対象要素数
	int* pData // [in,out] データへのポインタ
)
{
	std::sort(pData, pData + nCount);
}

// 機能 : 実数配列のソート
// 
void CArrayHelper::Sort(
	int nCount, // [in] 処理対象要素数
	double* pData // [in,out] データへのポインタ
)
{
	std::sort(pData, pData + nCount);
}

// 機能 : 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
// 返り値 : 重複値削除後の要素数
//
int CArrayHelper::Unique(
	int nCount, // [in] 処理対象要素数
	int* pData // [in,out] データへのポインタ
)
{
	int* e = std::unique(pData, pData + nCount);
	return int(e - pData);
}

// 機能 : 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
// 返り値 : 重複値削除後の要素数
//
int CArrayHelper::Unique(
	int nCount, // [in] 処理対象要素数
	double* pData // [in,out] データへのポインタ
)
{
	double* e = std::unique(pData, pData + nCount);
	return int(e - pData);
}

// 機能 : 整数配列に連番を設定
// 
void CArrayHelper::SequentialNumber(
	int nCount, // [in] 処理対象要素数
	int* pData, // [in,out] データへのポインタ
	int nStartVal // [in] 連番の開始値
)
{
	for(int i = 0; i < nCount; i++)
		pData[i] = nStartVal + i;
}

// 機能 : 整数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	int nCount, // [in] 配列要素数
	const int* pData, // [in] 配列の先頭へのポインタ
	int nVal // [in] 探す値
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == nVal)
			return i;
	return -1;
}

// 機能 : 整数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	int nCount, // [in] 配列要素数
	const double* pData, // [in] 配列の先頭へのポインタ
	double dVal // [in] 探す値
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == dVal)
			return i;
	return -1;
}

// 機能 : 文字列配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	int nCount, // [in] 配列要素数
	const CString* pData, // [in] 配列の先頭へのポインタ
	CString sVal // [in] 探す値
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i] == sVal)
			return i;
	return -1;
}

// 機能 : 配列の全ての要素が０かどうか調べる
// 
// 返り値 : TRUE=全ての要素が０、FALSE=０以外の要素が存在する
// 
BOOL CArrayHelper::IsZero(
	int nCount, // [in] 配列要素数
	const int* pData // [in] データへのポインタ
)
{
	for(int i = 0; i < nCount; i++)
		if(pData[i])
			return FALSE;
	return TRUE;
}

//------------------------------------------------------------------------------
// 機能 : 整数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	CArray<int>& array, // [in,out] 処理対象配列
	int val // [in] 設定する値
)
{
	if(array.IsEmpty())
		return;
	Fill((int)array.GetCount(), &array[0], val);
}

// 機能 : 実数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	CArray<double>& array, // [in,out] 処理対象配列
	double val // [in] 設定する値
)
{
	if(array.IsEmpty())
		return;
	Fill((int)array.GetCount(), &array[0], val);
}

// 機能 : 整数配列のソート
// 
void CArrayHelper::Sort(
	CArray<int>& array // [in,out] 処理対象配列
)
{
	if(array.IsEmpty())
		return;
	Sort((int)array.GetCount(), &array[0]);
}

// 機能 : 実数配列のソート
// 
void CArrayHelper::Sort(
	CArray<double>& array // [in,out] 処理対象配列
)
{
	if(array.IsEmpty())
		return;
	Sort((int)array.GetCount(), &array[0]);
}

// 機能 : 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
void CArrayHelper::Unique(
	CArray<int>& array // [in,out] 処理対象配列
)
{
	if(array.IsEmpty())
		return;
	array.SetSize(Unique((int)array.GetCount(), &array[0]));
}

// 機能 : 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
void CArrayHelper::Unique(
	CArray<double>& array // [in,out] 処理対象配列
)
{
	if(array.IsEmpty())
		return;
	array.SetSize(Unique((int)array.GetCount(), &array[0]));
}

// 機能 : 整数配列に連番を設定
// 
void CArrayHelper::SequentialNumber(
	CArray<int>& array, // [in,out] 処理対象配列
	int nStartVal // [in] 連番の開始値
)
{
	if(array.IsEmpty())
		return;
	SequentialNumber((int)array.GetCount(), &array[0], nStartVal);
}

// 機能 : 整数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const CArray<int>& array, // [in] 検索対象配列
	int nVal // [in] 探す値
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], nVal);
}

// 機能 : 実数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const CArray<double>& array, // [in] 検索対象配列
	double dVal // [in] 探す値
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], dVal);
}

// 機能 : 文字列配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const CArray<CString>& array, // [in] 検索対象配列
	CString sVal // [in] 探す値
)
{
	if(array.IsEmpty())
		return -1;
	return Find((int)array.GetCount(), &array[0], sVal);
}

// 機能 : 整数配列を作成する
// 
void CArrayHelper::MakeArray(
	CArray<int>& array, // [out] 配列が作成される
	int nCount, // [in] 要素数
	const int* pSrcData // [in] コピー元データ
)
{
	array.SetSize(nCount);
	if(array.IsEmpty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(int));
}

// 機能 : 整数配列を作成する
// 
void CArrayHelper::MakeArray(
	CArray<double>& array, // [out] 配列が作成される
	int nCount, // [in] 要素数
	const double* pSrcData // [in] コピー元データ
)
{
	array.SetSize(nCount);
	if(array.IsEmpty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(double));
}

// 機能 : 配列の全ての要素が０かどうか調べる
// 
BOOL CArrayHelper::IsZero(
	const CArray<int>& array // [in] 検索対象配列
)
{
	return IsZero((int)array.GetCount(), &array[0]);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	CArray<int>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const int* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	CArray<double>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const double* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	CArray<CString>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const CString* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}

//------------------------------------------------------------------------------
// 機能 : 整数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	std::vector<int>& array, // [in,out] 処理対象配列
	int val // [in] 設定する値
)
{
	if(array.empty())
		return;
	Fill((int)array.size(), &array[0], val);
}

// 機能 : 実数配列の全要素に同じ値を設定
// 
void CArrayHelper::Fill(
	std::vector<double>& array, // [in,out] 処理対象配列
	double val // [in] 設定する値
)
{
	if(array.empty())
		return;
	Fill((int)array.size(), &array[0], val);
}

// 機能 : 整数配列のソート
// 
void CArrayHelper::Sort(
	std::vector<int>& array // [in,out] 処理対象配列
)
{
	if(array.empty())
		return;
	Sort((int)array.size(), &array[0]);
}

// 機能 : 実数配列のソート
// 
void CArrayHelper::Sort(
	std::vector<double>& array // [in,out] 処理対象配列
)
{
	if(array.empty())
		return;
	Sort((int)array.size(), &array[0]);
}

// 機能 : 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
void CArrayHelper::Unique(
	std::vector<int>& array // [in,out] 処理対象配列
)
{
	if(array.empty())
		return;
	array.resize(Unique((int)array.size(), &array[0]));
}

// 機能 : 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
// 
void CArrayHelper::Unique(
	std::vector<double>& array // [in,out] 処理対象配列
)
{
	if(array.empty())
		return;
	array.resize(Unique((int)array.size(), &array[0]));
}

// 機能 : 整数配列に連番を設定
// 
void CArrayHelper::SequentialNumber(
	std::vector<int>& array, // [in,out] 処理対象配列
	int nStartVal // [in] 連番の開始値
)
{
	if(array.empty())
		return;
	SequentialNumber((int)array.size(), &array[0], nStartVal);
}

// 機能 : 整数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const std::vector<int>& array, // [in] 検索対象配列
	int nVal // [in] 探す値
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], nVal);
}

// 機能 : 実数配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const std::vector<double>& array, // [in] 検索対象配列
	double dVal // [in] 探す値
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], dVal);
}

// 機能 : 文字列配列から指定値を検索
// 
// 返り値 : 見つかったインデックス番号、見つからなかった場合は -1 が返る。
// 
int CArrayHelper::Find(
	const std::vector<CString>& array, // [in] 検索対象配列
	CString sVal // [in] 探す値
)
{
	if(array.empty())
		return -1;
	return Find((int)array.size(), &array[0], sVal);
}

// 機能 : 整数配列を作成する
// 
void CArrayHelper::MakeArray(
	std::vector<int>& array, // [out] 配列が作成される
	int nCount, // [in] 要素数
	const int* pSrcData // [in] コピー元データ
)
{
	array.resize(nCount);
	if(array.empty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(int));
}

// 機能 : 実数配列を作成する
// 
void CArrayHelper::MakeArray(
	std::vector<double>& array, // [out] 配列が作成される
	int nCount, // [in] 要素数
	const double* pSrcData // [in] コピー元データ
)
{
	array.resize(nCount);
	if(array.empty())
		return;
	memcpy(&array[0], pSrcData, nCount * sizeof(double));
}

// 機能 : 配列の全ての要素が０かどうか調べる
// 
BOOL CArrayHelper::IsZero(
	const std::vector<int>& array // [in] 検索対象配列
)
{
	return IsZero((int)array.size(), &array[0]);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	std::vector<int>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const int* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	std::vector<double>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const double* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}

// 機能 : 配列の後ろへ別の配列を追加する
// 
void CArrayHelper::Append(
	std::vector<CString>& dstArray, // [out] この配列の後ろに別の配列が追加される
	int nCount, // [in] 追加要素数
	const CString* pSrcData // [in] データ
)
{
	::Append(dstArray, nCount, pSrcData);
}


//------------------------------------------------------------------------------
// 機能 : 正数配列をコピーする
// 
void CArrayHelper::Copy(
	std::vector<int>& dst, // [out] コピー先配列
	const CArray<int>& src // [in] コピー元配列
)
{
	dst.resize((size_t)src.GetCount());
	if(src.IsEmpty())
		return;
	memcpy(&dst[0], &src[0], dst.size() * sizeof(int));
}

// 機能 : 実数配列をコピーする
// 
void CArrayHelper::Copy(
	std::vector<double>& dst, // [out] コピー先配列
	const CArray<double>& src // [in] コピー元配列
)
{
	dst.resize((size_t)src.GetCount());
	if(src.IsEmpty())
		return;
	memcpy(&dst[0], &src[0], dst.size() * sizeof(double));
}

// 機能 : 文字列配列をコピーする
// 
void CArrayHelper::Copy(
	std::vector<CString>& dst, // [out] コピー先配列
	const CArray<CString>& src // [in] コピー元配列
)
{
	int n = (int)src.GetCount();
	dst.resize(n);
	if(n == 0)
		return;
	for(int i = 0; i < n; i++)
		dst[i] = src[i];
}


// 機能 : 正数配列をコピーする
// 
void CArrayHelper::Copy(
	CArray<int>& dst, // [out] コピー先配列
	const std::vector<int>& src // [in] コピー元配列
)
{
	dst.SetSize((INT_PTR)src.size());
	if(src.empty())
		return;
	memcpy(&dst[0], &src[0], dst.GetCount() * sizeof(int));
}

// 機能 : 実数配列をコピーする
// 
void CArrayHelper::Copy(
	CArray<double>& dst, // [out] コピー先配列
	const std::vector<double>& src // [in] コピー元配列
)
{
	dst.SetSize((INT_PTR)src.size());
	if(src.empty())
		return;
	memcpy(&dst[0], &src[0], dst.GetCount() * sizeof(double));
}

// 機能 : 文字列配列をコピーする
// 
void CArrayHelper::Copy(
	CArray<CString>& dst, // [out] コピー先配列
	const std::vector<CString>& src // [in] コピー元配列
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
// 機能 : 正数配列の全要素に指定値を加算する
// 
void CArrayHelper::AddValue(
	std::vector<int>& dst, // [out] 配列
	int val // [in] 加算する値
)
{
	for(int i = 0, n = (int)dst.size(); i < n; i++)
		dst[i] += val;
}

// 機能 : 実数配列の全要素に指定値を加算する
// 
void CArrayHelper::AddValue(
	std::vector<double>& dst, // [out] 配列
	double val // [in] 加算する値
)
{
	for(int i = 0, n = (int)dst.size(); i < n; i++)
		dst[i] += val;
}


// 機能 : 正数配列の全要素に指定値を加算する
// 
void CArrayHelper::AddValue(
	CArray<int>& dst, // [out] 配列
	int val // [in] 加算する値
)
{
	for(int i = 0, n = (int)dst.GetCount(); i < n; i++)
		dst[i] += val;
}

// 機能 : 実数配列の全要素に指定値を加算する
// 
void CArrayHelper::AddValue(
	CArray<double>& dst, // [out] 配列
	double val // [in] 加算する値
)
{
	for(int i = 0, n = (int)dst.GetCount(); i < n; i++)
		dst[i] += val;
}

#pragma once
#ifndef __ARRAYHELPER_H__
#define __ARRAYHELPER_H__

#include <afxtempl.h>
#include <vector>

class CArrayHelper // 配列に対する処理の補助関数などを集めたクラス
{
public:
	static void Fill(int nCount, int* pData, int val); // 整数配列の全要素に同じ値を設定
	static void Fill(int nCount, double* pData, double val); // 実数配列の全要素に同じ値を設定
	static void Sort(int nCount, int* pData); // 整数配列のソート
	static void Sort(int nCount, double* pData); // 実数配列のソート
	static int Unique(int nCount, int* pData); // 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static int Unique(int nCount, double* pData); // 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static void SequentialNumber(int nCount, int* pData, int nStartVal); // 整数配列に連番を設定
	static int Find(int nCount, const int* pData, int nVal); // 整数配列から指定値を検索
	static int Find(int nCount, const double* pData, double dVal); // 実数配列から指定値を検索
	static int Find(int nCount, const CString* pData, CString sVal); // 文字列配列から指定値を検索
	static BOOL IsZero(int nCount, const int* pData); // 配列の全ての要素が０かどうか調べる

	template<class T> static void Copy(std::vector<T>& dst, const std::vector<T>& src); // 正数配列をコピーする
	template<class T> static void Copy(CArray<T>& dst, const CArray<T>& src); // 正数配列をコピーする
	template<class T> static void SetSize(CArray<T>& array, int nCount); // 配列のサイズを設定する
	template<class T> static void SetSize(std::vector<T>& array, int nCount); // 配列のサイズを設定する
	template<class T> static int GetSize(const CArray<T>& array); // 配列のサイズを取得する
	template<class T> static int GetSize(const std::vector<T>& array); // 配列のサイズを取得する
	template<class T> static int PushBack(CArray<T>& array, const T& val); // 配列の後ろへ指定の値を追加する
	template<class T> static int PushBack(std::vector<T>& array, const T& val); // 配列の後ろへ指定の値を追加する
	template<class T> static void Append(CArray<T>& dstArray, const CArray<T>& srcArray); // 配列の後ろへ別の配列を追加する
	template<class T> static void Append(CArray<T>& dstArray, const std::vector<T>& srcArray); // 配列の後ろへ別の配列を追加する
	template<class T> static void Append(std::vector<T>& dstArray, const CArray<T>& srcArray); // 配列の後ろへ別の配列を追加する
	template<class T> static void Append(std::vector<T>& dstArray, const std::vector<T>& srcArray); // 配列の後ろへ別の配列を追加する
	template<class DARRY, class IARRY> static BOOL SwitchesToIndices(DARRY& dstArray, const IARRY& switches); // switches[i] が0以外になる時の i を dstArray に追加する
	template<class DARRY, class IARRY> static BOOL IndicesToSwitches(DARRY& dstArray, const IARRY& indices); // dstArray[indices[]] に 1 を設定する、dstArray のサイズは indices の値の最大値に拡張される
	template<class T, class IARRY> static BOOL IndicesToSwitches(int nDstArraySize, T* pDstArray, const IARRY& indices); // pDstArray[indices[]] に 1 を設定する、 indices[] が nDstArraySize 以上だった場合には処理しない
	template<class DARRY, class SARRY, class IARRY> static BOOL Extract(DARRY& dstArray, const SARRY& srcArray, const IARRY& indices); // srcArray から indices で指定されたインデックスの要素を抜き出し dstArray の後ろに追加する
	template<class DARRY, class SARRY, class IARRY> static BOOL ExtractBySwitches(DARRY& dstArray, const SARRY& srcArray, const IARRY& switches); // srcArray から switches が 0 以外のインデックスの要素を抜き出し dstArray の後ろに追加する
	template<class T> static T Sum(const CArray<T>& srcArray); // srcArray の要素の値全ての合計値を計算する
	template<class T> static T Sum(const std::vector<T>& srcArray); // srcArray の要素の値全ての合計値を計算する
	template<class T, class Array> static BOOL Max(const Array& srcArray, T& Max); // srcArray の要素で最大値を Max へ取得する、取得できた場合は TRUE を返しそれ以外は FALSE が返る
	template<class T, class Array> static BOOL Min(const Array& srcArray, T& Min); // srcArray の要素で最小値を Min へ取得する、取得できた場合は TRUE を返しそれ以外は FALSE が返る


	static void Fill(CArray<int>& array, int val); // 整数配列の全要素に同じ値を設定
	static void Fill(CArray<double>& array, double val); // 実数配列の全要素に同じ値を設定
	static void Sort(CArray<int>& array); // 整数配列のソート
	static void Sort(CArray<double>& array); // 実数配列のソート
	static void Unique(CArray<int>& array); // 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static void Unique(CArray<double>& array); // 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static void SequentialNumber(CArray<int>& array, int nStartVal); // 整数配列に連番を設定
	static int Find(const CArray<int>& array, int nVal); // 整数配列から指定値を検索
	static int Find(const CArray<double>& array, double dVal); // 実数配列から指定値を検索
	static int Find(const CArray<CString>& array, CString sVal); // 文字列配列から指定値を検索
	static void MakeArray(CArray<int>& array, int nCount, const int* pSrcData); // 整数配列を作成する
	static void MakeArray(CArray<double>& array, int nCount, const double* pSrcData); // 実数配列を作成する
	static BOOL IsZero(const CArray<int>& array); // 配列の全ての要素が０かどうか調べる
	static void Append(CArray<int>& dstArray, int nCount, const int* pSrcData); // 配列の後ろへ別の配列を追加する
	static void Append(CArray<double>& dstArray, int nCount, const double* pSrcData); // 配列の後ろへ別の配列を追加する
	static void Append(CArray<CString>& dstArray, int nCount, const CString* pSrcData); // 配列の後ろへ別の配列を追加する

	static void Fill(std::vector<int>& array, int val); // 整数配列の全要素に同じ値を設定
	static void Fill(std::vector<double>& array, double val); // 実数配列の全要素に同じ値を設定
	static void Sort(std::vector<int>& array); // 整数配列のソート
	static void Sort(std::vector<double>& array); // 実数配列のソート
	static void Unique(std::vector<int>& array); // 整数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static void Unique(std::vector<double>& array); // 実数配列の重複値削除、ソート済みの配列に対してのみ使用可能
	static void SequentialNumber(std::vector<int>& array, int nStartVal); // 整数配列に連番を設定
	static int Find(const std::vector<int>& array, int nVal); // 整数配列から指定値を検索
	static int Find(const std::vector<double>& array, double dVal); // 実数配列から指定値を検索
	static int Find(const std::vector<CString>& array, CString sVal); // 文字列配列から指定値を検索
	static void MakeArray(std::vector<int>& array, int nCount, const int* pSrcData); // 整数配列を作成する
	static void MakeArray(std::vector<double>& array, int nCount, const double* pSrcData); // 実数配列を作成する
	static BOOL IsZero(const std::vector<int>& array); // 配列の全ての要素が０かどうか調べる
	static void Append(std::vector<int>& dstArray, int nCount, const int* pSrcData); // 配列の後ろへ別の配列を追加する
	static void Append(std::vector<double>& dstArray, int nCount, const double* pSrcData); // 配列の後ろへ別の配列を追加する
	static void Append(std::vector<CString>& dstArray, int nCount, const CString* pSrcData); // 配列の後ろへ別の配列を追加する

	static void Copy(std::vector<int>& dst, const CArray<int>& src); // 正数配列をコピーする
	static void Copy(std::vector<double>& dst, const CArray<double>& src); // 実数配列をコピーする
	static void Copy(std::vector<CString>& dst, const CArray<CString>& src); // 文字列配列をコピーする

	static void Copy(CArray<int>& dst, const std::vector<int>& src); // 正数配列をコピーする
	static void Copy(CArray<double>& dst, const std::vector<double>& src); // 実数配列をコピーする
	static void Copy(CArray<CString>& dst, const std::vector<CString>& src); // 文字列配列をコピーする

	static void AddValue(std::vector<int>& dst, int val); // 正数配列の全要素に指定値を加算する
	static void AddValue(std::vector<double>& dst, double val); // 実数配列の全要素に指定値を加算する

	static void AddValue(CArray<int>& dst, int val); // 正数配列の全要素に指定値を加算する
	static void AddValue(CArray<double>& dst, double val); // 実数配列の全要素に指定値を加算する
};


template<class T> void CArrayHelper::Copy(std::vector<T>& dst, const std::vector<T>& src) // 正数配列をコピーする
{
	dst = src;
}

template<class T> void CArrayHelper::Copy(CArray<T>& dst, const CArray<T>& src) // 正数配列をコピーする
{
	dst.Copy(src);
}

template<class T> void CArrayHelper::SetSize(CArray<T>& array, int nCount) // 配列のサイズを設定する
{
	array.SetSize(nCount);
}
template<class T> void CArrayHelper::SetSize(std::vector<T>& array, int nCount) // 配列のサイズを設定する
{
	array.resize(nCount);
}

template<class T> int CArrayHelper::GetSize(const CArray<T>& array) // 配列のサイズを取得する
{
	return (int)array.GetSize();
}
template<class T> int CArrayHelper::GetSize(const std::vector<T>& array) // 配列のサイズを取得する
{
	return (int)array.size();
}

template<class T> int CArrayHelper::PushBack(CArray<T>& array, const T& val) // 配列の後ろへ指定の値を追加する
{
	return (int)array.Add(val);
}
template<class T> int CArrayHelper::PushBack(std::vector<T>& array, const T& val) // 配列の後ろへ指定の値を追加する
{
	int n = (int)array.size();
	array.push_back(val);
	return n;
}

template<class T> void CArrayHelper::Append(CArray<T>& dstArray, const CArray<T>& srcArray) // 配列の後ろへ別の配列を追加する
{
	if(srcArray.IsEmpty())
		return;
	Append(dstArray, (int)srcArray.GetCount(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(CArray<T>& dstArray, const std::vector<T>& srcArray) // 配列の後ろへ別の配列を追加する
{
	if(srcArray.empty())
		return;
	Append(dstArray, (int)srcArray.size(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(std::vector<T>& dstArray, const CArray<T>& srcArray) // 配列の後ろへ別の配列を追加する
{
	if(srcArray.IsEmpty())
		return;
	Append(dstArray, (int)srcArray.GetCount(), &srcArray[0]);
}
template<class T> void CArrayHelper::Append(std::vector<T>& dstArray, const std::vector<T>& srcArray) // 配列の後ろへ別の配列を追加する
{
	if(srcArray.empty())
		return;
	Append(dstArray, (int)srcArray.size(), &srcArray[0]);
}

template<class DARRY, class IARRY> static BOOL CArrayHelper::SwitchesToIndices(DARRY& dstArray, const IARRY& switches) // switches[i] が0以外になる時の i を dstArray に追加する
{
	for(int i = 0, n = GetSize(switches); i < n; i++)
	{
		if(switches[i])
			PushBack(dstArray, i);
	}
	return TRUE;
}

template<class DARRY, class IARRY> static BOOL CArrayHelper::IndicesToSwitches(DARRY& dstArray, const IARRY& indices) // dstArray[indices[]] に 1 を設定する、dstArray のサイズは indices の値の最大値に拡張される
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

template<class T, class IARRY> static BOOL CArrayHelper::IndicesToSwitches(int nDstArraySize, T* pDstArray, const IARRY& indices) // pDstArray[indices[]] に 1 を設定する、 indices[] が nDstArraySize 以上だった場合には処理しない
{
	for(int i = 0, n = GetSize(indices); i < n; i++)
	{
		int index = indices[i];
		if(index < nDstArraySize)
			pDstArray[index] = 1;
	}
	return TRUE;
}

template<class DARRY, class SARRY, class IARRY> BOOL CArrayHelper::Extract(DARRY& dstArray, const SARRY& srcArray, const IARRY& indices) // srcArray から indices で指定されたインデックスの要素を抜き出し dstArray の後ろに追加する
{
	int nSize = GetSize(srcArray);
	for(int i = 0, n = GetSize(indices); i < n; i++)
	{
		int index = indices[i];
		if(index < 0 || nSize <= index)
			return FALSE; // 指定されたインデックスが配列外ならエラー
		PushBack(dstArray, srcArray[index]);
	}
	return TRUE;
}

template<class DARRY, class SARRY, class IARRY> BOOL CArrayHelper::ExtractBySwitches(DARRY& dstArray, const SARRY& srcArray, const IARRY& switches) // srcArray から switches が 0 以外のインデックスの要素を抜き出し dstArray の後ろに追加する
{
	int n = GetSize(switches);
	if(GetSize(srcArray) < n)
		return FALSE; // switches の要素数が srcArray の要素数より大きければエラー
	for(int i = 0; i < n; i++)
	{
		if(switches[i])
			PushBack(dstArray, srcArray[i]);
	}
	return TRUE;
}

template<class T> T CArrayHelper::Sum(const CArray<T>& srcArray) // srcArray の要素の値全ての合計値を計算する
{
	T sum = T();
	for(int i = 0, n = (int)srcArray.GetCount(); i < n; i++)
		sum += srcArray[i];
	return sum;
}
template<class T> T CArrayHelper::Sum(const std::vector<T>& srcArray) // srcArray の要素の値全ての合計値を計算する
{
	T sum = T();
	for(int i = 0, n = (int)srcArray.size(); i < n; i++)
		sum += srcArray[i];
	return sum;
}

template<class T, class Array> BOOL CArrayHelper::Max(const Array& srcArray, T& Max) // srcArray の要素で最大値を Max へ取得する、取得できた場合は TRUE を返しそれ以外は FALSE が返る
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
template<class T, class Array> BOOL CArrayHelper::Min(const Array& srcArray, T& Min) // srcArray の要素で最小値を Min へ取得する、取得できた場合は TRUE を返しそれ以外は FALSE が返る
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


//	srcArray 内のメンバ変数 var を cmp で比較して true なら result にインデックス番号が返る、見つからないなら -1 が返る
#define ARRAYHELPER_FINDFIRST_VAR_CMP(srcArray, var, cmp, result) \
	{ \
		int __ARRAYHELPER_I__, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); \
		for(__ARRAYHELPER_I__ = 0; __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) { (result) = __ARRAYHELPER_I__; break; } \
		if(__ARRAYHELPER_I__ == __ARRAYHELPER_N__) (result) = -1; \
	}

//	srcArray 内のメンバ変数 var を dstArray へ追加する
#define ARRAYHELPER_EXTRACT_VAR(dstArray, srcArray, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	srcArray 内のメンバ変数 var を cmp で比較して true なら dstArray へ追加する
#define ARRAYHELPER_EXTRACT_VAR_CMP(dstArray, srcArray, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	srcArray 内のメンバ変数 cmpvar を cmp で比較して true なら var を dstArray へ追加する
#define ARRAYHELPER_EXTRACT_VAR2_CMP(dstArray, srcArray, cmpvar, cmp, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].cmpvar cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__].var); \
	}

//	dstArray.Add(srcArray[indices[]].var) を実行する、indices の値の範囲チェックは行わない
#define ARRAYHELPER_EXTRACT_IDXVAR(dstArray, srcArray, indices, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	if(srcArray[indices[]].var cmp) dstArray.Add(srcArray[indices[]].var) を実行する、indices の値の範囲チェックは行わない
#define ARRAYHELPER_EXTRACT_IDXVAR_CMP(dstArray, srcArray, indices, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[indices[__ARRAYHELPER_I__]].var cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	if(srcArray[indices[]].cmpvar cmp) dstArray.Add(srcArray[indices[]].var) を実行する、indices の値の範囲チェックは行わない
#define ARRAYHELPER_EXTRACT_IDXVAR2_CMP(dstArray, srcArray, indices, cmpvar, cmp, var) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(indices); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[indices[__ARRAYHELPER_I__]].cmpvar cmp) \
				CArrayHelper::PushBack(dstArray, srcArray[indices[__ARRAYHELPER_I__]].var); \
	}

//	srcArray 内のメンバ変数 var を cmp で比較して true ならインデックス番号を dstArray へ追加する
#define ARRAYHELPER_EXTRACT_INDICES_VAR_CMP(dstArray, srcArray, var, cmp) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(srcArray[__ARRAYHELPER_I__].var cmp) \
				CArrayHelper::PushBack(dstArray, __ARRAYHELPER_I__); \
	}

//	srcArray の要素を func 関数で評価して true なら dstArray へ追加する
#define ARRAYHELPER_EXTRACT_FUNC(dstArray, srcArray, func) \
	{ \
		for(int __ARRAYHELPER_I__ = 0, __ARRAYHELPER_N__ = CArrayHelper::GetSize(srcArray); __ARRAYHELPER_I__ < __ARRAYHELPER_N__; __ARRAYHELPER_I__++) \
			if(func(srcArray[__ARRAYHELPER_I__])) \
				CArrayHelper::PushBack(dstArray, srcArray[__ARRAYHELPER_I__]); \
	}

#endif

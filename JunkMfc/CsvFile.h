#pragma once
#include <afx.h>
#include "Csv.h"

//! CSVファイルアクセス用クラス
//! @remarks マルチバイト、ワイド文字両対応
class CsvFile : public CStdioFile
{
public:
	//! Open() メソッドに渡すファイルオープンモード
	enum OpenFlags {
		csvRead = CStdioFile::modeRead | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV読み込みモード
		csvWrite = CStdioFile::modeWrite | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV書き込みモード
		csvReadWrite = CStdioFile::modeReadWrite | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV読み書きモード
		csvCreate = csvWrite | CStdioFile::modeCreate | CStdioFile::shareDenyNone | CStdioFile::typeText, //!< CSV作成モード
	};


	CsvFile(); //!< コンストラクタ
	CsvFile(LPCTSTR lpszFileName, UINT nOpenFlags); //!< コンストラクタ、ファイルオープンして初期化する

	//! ファイルから１行を読み込む
	virtual BOOL ReadRow(
		std::vector<CStringA>& fields //!< [out] 読み込まれたフィールド配列が返る
	);

	//! ファイルから１行を読み込む
	virtual BOOL ReadRow(
		std::vector<CStringW>& fields //!< [out] 読み込まれたフィールド配列が返る
	);

	//! ファイルへ１行を書き込む
	virtual void WriteRow(
		const std::vector<CStringA>& fields //!< [in] 分割されている文字列
	);

	//! ファイルへ１行を書き込む
	virtual void WriteRow(
		const std::vector<CStringW>& fields //!< [in] 分割されている文字列
	);

	//! 区切り文字を設定する
	void SetSeparator(wchar_t sp) {
		m_Separator = sp;
	}

	//! 括り文字を設定する
	void SetBundler(wchar_t bd) {
		m_Bundler = bd;
	}

protected:
	CString m_sRow; //!< 最後に処理した行文字列、メモリ領域使いまわすために用意
	CStringA m_sRowA; //!< 最後に処理した行文字列、メモリ領域使いまわすために用意
	CStringW m_sRowW; //!< 最後に処理した行文字列、メモリ領域使いまわすために用意
	wchar_t m_Separator; //!< 区切り文字
	wchar_t m_Bundler; //!< 括り文字、'\0'が指定されると括り文字判定は行われない
};


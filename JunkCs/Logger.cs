using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;

namespace Jk {
	/// <summary>
	/// ログ出力用クラス、ファイル構造は以下の様になる
	///		ルートディレクトリ
	///			年月ディレクトリ
	///				日ディレクトリ
	///				日ディレクトリ
	///			年月ディレクトリ
	///				日ディレクトリ
	///				日ディレクトリ
	///			・
	///			・
	///			・
	/// </summary>
	public class Logger {
		/// <summary>
		/// ログの位置
		/// </summary>
		struct LogPlace {
			/// <summary>
			/// 年月ディレクトリ
			/// </summary>
			public string YMDir;

			/// <summary>
			/// ファイル名
			/// </summary>
			public string File;

			/// <summary>
			/// フルパス名
			/// </summary>
			public string FullPath;
		}

		/// <summary>
		/// ファイルアクセス時のエンコード
		/// </summary>
		public static readonly Encoding Encoding = Encoding.GetEncoding("shift-jis");

		/// <summary>
		/// 年月にマッチする正規表現
		/// </summary>
		static Regex RxMatchYYYYMM = new Regex("^[0-9]{6}$", RegexOptions.Compiled);

		/// <summary>
		/// ログファイル保存ルートフォルダパス名
		/// </summary>
		public string LogRootDirPath { get; protected set; }

		/// <summary>
		/// ログファイル名の先頭に付く名称
		/// </summary>
		public string Prefix { get; protected set; }

		/// <summary>
		/// ログファイルの拡張子
		/// </summary>
		public string Ext { get; protected set; }

		/// <summary>
		/// 現在のログファイル書き込み用 StreamWriter
		/// </summary>
		public CsvWriter CurrentStreamWriter { get; protected set; }

		/// <summary>
		/// ファイルのヘッダとして出力される文字列、改行必須
		/// </summary>
		public string FileHeader { get; set; }

		/// <summary>
		/// 自動削除経過月数、この月数以上古いログは自動で削除される、int.MaxValue が設定されているなら自動削除は行わない
		/// </summary>
		public int AutoDeleteMonth { get; set; }

		/// <summary>
		/// 最終書き込み時の日
		/// </summary>
		DateTime _LastWriteDate;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="logRootDirPath">ログルートディレクトリパス名</param>
		/// <param name="prefix">ログファイル名の先頭に付く名称</param>
		/// <param name="ext">ログファイルの拡張子</param>
		public Logger(string logRootDirPath, string prefix, string ext) {
			this.LogRootDirPath = logRootDirPath;
			this.Prefix = prefix;
			this.Ext = ext;
			this.AutoDeleteMonth = int.MaxValue;
			this._LastWriteDate = new DateTime();
		}

		/// <summary>
		/// ログファイルを書き込みモードで開く
		/// </summary>
		public void OpenLogFile(DateTime now) {
			// ルートフォルダが無かったら作成する
			if (!Directory.Exists(this.LogRootDirPath))
				Directory.CreateDirectory(this.LogRootDirPath);

			// 日時からログファイル位置を取得
			var lp = GetLogFilePlace(now);

			// 年月フォルダが無かったら作成する
			string ymdir = now.ToString("yyyyMM");
			if (!Directory.Exists(lp.YMDir))
				Directory.CreateDirectory(lp.YMDir);

			// ファイルが既に存在していたら追加書き込み
			// 存在していなければ新規作成する
			var fs = new FileStream(lp.FullPath, FileMode.Append, FileAccess.Write, FileShare.ReadWrite);

			// ストリームライターセット
			this.CurrentStreamWriter = new CsvWriter(new StreamWriter(fs, Logger.Encoding));

			// 必要ならファイルヘッダを書き込む
			var h = this.FileHeader;
			if (h != null && fs.Length == 0) {
				this.CurrentStreamWriter.TextWriter.Write(h);
			}
		}

		/// <summary>
		/// ログファイルを閉じる
		/// </summary>
		public void Close() {
			if (this.CurrentStreamWriter != null) {
				this.CurrentStreamWriter.Close();
				this.CurrentStreamWriter.Dispose();
				this.CurrentStreamWriter = null;
			}
		}

		/// <summary>
		/// ログ文字列を１行追加する
		/// </summary>
		/// <param name="now">現在日時</param>
		/// <param name="fields">ログ追加文字列</param>
		/// <returns>ログに出力されたCSVデータが返る</returns>
		public void AddLogLineWithoutTimeStamp(DateTime now, IEnumerable<string> fields) {
			// 前回の書き込み日と今回が異なれば新たにログファイルを開きなおす
			var today = now.Date;
			if (_LastWriteDate != today) {
				Close();
				OpenLogFile(today);

				if (this.AutoDeleteMonth != int.MaxValue) {
					AutoDeleteOldLogs(today);
				}
			}

			this.CurrentStreamWriter.WriteRow(fields);
			this.CurrentStreamWriter.TextWriter.Flush();

			_LastWriteDate = today;
		}

		/// <summary>
		/// ログ文字列を１行追加する
		/// </summary>
		/// <param name="now">現在日時</param>
		/// <param name="text">ログ追加文字列</param>
		/// <returns>ログに出力されたCSVデータが返る</returns>
		public List<string> AddLogLine(DateTime now, params string[] text) {
			var fields = new List<string>();
			fields.Add(now.ToString());
			foreach (var f in text)
				fields.Add(f);
			AddLogLineWithoutTimeStamp(now, fields);
			return fields;
		}

		/// <summary>
		/// ログ文字列を１行追加する
		/// </summary>
		/// <param name="text">ログ追加文字列</param>
		/// <returns>ログに出力されたCSVデータが返る</returns>
		public List<string> AddLogLine(params string[] text) {
			return AddLogLine(DateTime.Now, text);
		}

		/// <summary>
		/// 指定のログデータを指定ファイルに書き込む
		/// </summary>
		/// <param name="logs">ログデータ</param>
		public static void WriteLog(IEnumerable<List<string>> logs, string file) {
			using (var cw = new CsvWriter(new StreamWriter(file, false, Logger.Encoding))) {
				foreach (var row in logs) {
					cw.WriteRow(row);
				}
			}
		}

		/// <summary>
		/// 日別のログファイルの位置を取得
		/// </summary>
		/// <param name="date">日付</param>
		/// <returns>ログファイル位置</returns>
		LogPlace GetLogFilePlace(DateTime date) {
			var lp = new LogPlace();
			lp.YMDir = Path.Combine(this.LogRootDirPath, date.ToString("yyyyMM")); ;
			lp.File = this.Prefix + "-" + date.ToString("yyyyMMdd") + this.Ext;
			lp.FullPath = Path.Combine(lp.YMDir, lp.File);
			return lp;
		}

		/// <summary>
		/// 指定された日から換算して古い月のログを削除する
		/// </summary>
		void AutoDeleteOldLogs(DateTime today) {
			// 最後にログ書き込んだ月取得
			var lastWriteMonth = new DateTime(_LastWriteDate.Year, _LastWriteDate.Month, 1);
			// 今回の月取得
			var month = new DateTime(today.Year, today.Month, 1);
			// 最後のログ書き込み月と今回が異なるなら古いログ削除試行する
			if (lastWriteMonth != month) {
				var deleteMonth = month.AddMonths(-this.AutoDeleteMonth);
				var deleteMonthStr = deleteMonth.ToString("yyyyMM");
				foreach (var dirPath in Directory.GetDirectories(this.LogRootDirPath)) {
					var dir = Path.GetFileName(dirPath);
					if (RxMatchYYYYMM.IsMatch(dir)) {
						if (string.Compare(dir, deleteMonthStr) <= 0) {
							try {
								DeleteDirectory(dirPath);
							} catch (IOException) {
							}
						}
					}
				}
			}
		}


		/// ----------------------------------------------------------------------------
		/// <summary>
		///     指定したディレクトリをすべて削除します。</summary>
		/// <param name="stDirPath">
		///     削除するディレクトリのパス。</param>
		/// ----------------------------------------------------------------------------
		static void DeleteDirectory(string stDirPath) {
			DeleteDirectory(new System.IO.DirectoryInfo(stDirPath));
		}

		/// ----------------------------------------------------------------------------
		/// <summary>
		///     指定したディレクトリをすべて削除します。</summary>
		/// <param name="hDirectoryInfo">
		///     削除するディレクトリの DirectoryInfo。</param>
		/// ----------------------------------------------------------------------------
		static void DeleteDirectory(System.IO.DirectoryInfo hDirectoryInfo) {
			// すべてのファイルの読み取り専用属性を解除する
			foreach (System.IO.FileInfo cFileInfo in hDirectoryInfo.GetFiles()) {
				if ((cFileInfo.Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly) {
					cFileInfo.Attributes = System.IO.FileAttributes.Normal;
				}
			}

			// サブディレクトリ内の読み取り専用属性を解除する (再帰)
			foreach (System.IO.DirectoryInfo hDirInfo in hDirectoryInfo.GetDirectories()) {
				DeleteDirectory(hDirInfo);
			}

			// このディレクトリの読み取り専用属性を解除する
			if ((hDirectoryInfo.Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly) {
				hDirectoryInfo.Attributes = System.IO.FileAttributes.Directory;
			}

			// このディレクトリを削除する
			hDirectoryInfo.Delete(true);
		}
	}
}

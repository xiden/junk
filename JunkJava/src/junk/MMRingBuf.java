/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package junk;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * メモリマップトファイルを使ったリングバッファ、内部構造的に書き込み側と読み込み側を明確に分けておく必要がある.
 */
public class MMRingBuf implements AutoCloseable {

	/**
	 * メモリマップトファイル内の合計書き込みサンプル数のポインタ.
	 */
	private final static int PTR_SUM_SAMPLES = 8;

	/**
	 * メモリマップトファイル内の書き込み先インデックスのポインタ.
	 */
	private final static int PTR_WRITE_INDEX = 16;

	/**
	 * メモリマップトファイル内の有効サンプル数のポインタ.
	 */
	private final static int PTR_LENGTH = 24;

	/**
	 * メモリマップトファイル内のデータ領域開始ポインタ.
	 */
	private final static int PTR_DATA_START = 32;

	/**
	 * 1サンプルのバイト数.
	 */
	private final static int ELEMENT_SIZE = 4;

	private MMFile _mmf;
	private ByteBuffer _buf;
	private final long _capacity; // 最大サンプル数、良く使う値なのでメンバ変数に入れておく
	private long _sumSamplesForWriter; // 合計書き込みサンプル数、書込み側で良く使う値なのでメンバ変数に入れておく
	private long _writeIndexForWriter; // 書き込み先インデックス、書き込み側で良く使う値なのでメンバ変数に入れておく
	private long _lengthForWriter; // 合計書き込みサンプル数、書き込み側で良く使う値なのでメンバ変数に入れておく

	/**
	 * 指定名称のメモリマップトファイルリングバッファを作成する.
	 *
	 * @param name メモリマップトファイル名称.
	 * @param capacity 最大サンプル数.
	 * @throws IOException
	 */
	public MMRingBuf(String name, long capacity) throws IOException {
		long sizeInBytes = 0;

		// ＜＜＜ファイル内フォーマット＞＞＞
		sizeInBytes += 8; // キャパシティ（最大サンプル数） : 8 byte
		sizeInBytes += 8; // 書き込まれた合計サンプル数 : 8 byte
		sizeInBytes += 8; // 現在の書き込み先インデックス番号（０～） : 8 byte
		sizeInBytes += 8; // 有効データ数（０～） : 8 byte
		sizeInBytes += ELEMENT_SIZE * capacity; // データ領域 : 4 * capacity

		// メモリマップトファイル作成
		_mmf = new MMFile(name, sizeInBytes);

		// バッファ内を初期化
		_buf = _mmf.getBuf();
		_buf.putLong(0, capacity);
		_buf.putLong(PTR_SUM_SAMPLES, 0);
		_buf.putLong(PTR_WRITE_INDEX, 0);
		_buf.putLong(PTR_LENGTH, 0);

		// キャッシュが必要なものをメンバ変数に取得
		_capacity = capacity;
		_sumSamplesForWriter = 0;
		_writeIndexForWriter = 0;
		_lengthForWriter = 0;
	}

	/**
	 * 指定名称のメモリマップトファイルリングバッファを指定モードで開く.
	 *
	 * @param name メモリマップトファイル名称.
	 * @param writable 書き込み可能で開くかどうか.
	 * @throws IOException
	 */
	public MMRingBuf(String name, boolean writable) throws IOException {
		// メモリマップトファイル開く
		// ※本来はサイズのチェックなども行った方がいい
		_mmf = new MMFile(name, writable);

		// バッファ取得してキャッシュが必要なものをメンバ変数に取得
		_buf = _mmf.getBuf();
		_capacity = _buf.getLong(0);
		_sumSamplesForWriter = getSumSamples();
		_writeIndexForWriter = getWriteIndex();
		_lengthForWriter = getLength();
	}

	/**
	 * メモリマップトファイルを閉じる、ファイル削除は行われない.
	 *
	 * @throws IOException
	 */
	@Override
	public void close() throws IOException {
		if (_buf != null) {
			_buf = null;
		}
		if (_mmf != null) {
			_mmf.close();
			_mmf = null;
		}
	}

	/**
	 * リングバッファの最大サンプル数を取得する.
	 *
	 * @return リングバッファ最大サンプル数.
	 */
	public final long getCapacity() {
		return _capacity;
	}

	/**
	 * リングバッファに書き込まれた合計サンプル数を取得する.
	 *
	 * @return 書き込まれた合計サンプル数.
	 */
	public final long getSumSamples() {
		return _buf.getLong(PTR_SUM_SAMPLES);
	}

	/**
	 * リングバッファの現在の書き込み先インデックス（０～）を取得する.
	 *
	 * @return 書き込み先インデックス.
	 */
	public final long getWriteIndex() {
		return _buf.getLong(PTR_WRITE_INDEX);
	}

	/**
	 * 現在の有効サンプル数を取得する.
	 *
	 * @return 有効サンプル数.
	 */
	public final long getLength() {
		return _buf.getLong(PTR_LENGTH);
	}

	/**
	 * 現在の書き込み位置に指定値を書き込む.
	 *
	 * @param value 書き込む値.
	 */
	public final void write(int value) {
		// 先に現在の情報を取得する
		long wi = _writeIndexForWriter;
		long len = _lengthForWriter;

		// 現在の書き込み位置に書き込む
		_buf.putInt((int) (PTR_DATA_START + wi * ELEMENT_SIZE), value);

		// 書き込み先を次の位置へ移動し有効データ数を調整
		long cap = _capacity;
		wi = (wi + 1) % cap;
		len++;
		if (cap < len) {
			len = cap;
		}
		_writeIndexForWriter = wi;
		_lengthForWriter = len;
		_sumSamplesForWriter++;

		// 先に書き込み先インデックスを書き込む
		// ※これを先に行わないと読み込み側が有効データ外を参照してしまう可能性がある
		_buf.putLong(PTR_WRITE_INDEX, wi);

		// 次に有効データ数
		_buf.putLong(PTR_LENGTH, len);

		// 最後に合計書き込みサンプル数
		// ※読み込み側ではこれが変わったタイミングで書き込まれたと判断するのでこれが最後
		_buf.putLong(PTR_SUM_SAMPLES, _sumSamplesForWriter);
	}

	/**
	 * 書き込みの終端から指定サンプル数取得する、但し有効サンプル数以下に制限される.
	 *
	 * @param count 取得サンプル数.
	 * @return 取得されたサンプルバッファ.
	 */
	public final int[] peekTail(int count) {
		// ※write() での書き込みと逆順で値をチェック
		// 先ず有効データ数を取得
		long len = getLength();
		if (len < count) {
			count = (int) len;
		}
		if (count <= 0) {
			return new int[0];
		}

		long wi = getWriteIndex();
		long cap = _capacity;
		ByteBuffer b = _buf;
		int[] result = new int[count];

		if (wi == 0 || count <= wi) {
			// バッファの終端を跨がない場合
			int p = PTR_DATA_START + (int) ((wi == 0 ? cap : wi) - count) * ELEMENT_SIZE;
			for (int i = 0; i < count; i++) {
				result[i] = b.getInt(p);
				p += ELEMENT_SIZE;
			}

			return result;
		} else {
			// バッファの終端を跨ぐ場合
			int count1 = (int) (count - wi);

			int p = PTR_DATA_START + (int) (cap - count1) * ELEMENT_SIZE;
			for (int i = 0; i < count1; i++) {
				result[i] = b.getInt(p);
				p += ELEMENT_SIZE;
			}

			p = PTR_DATA_START;
			for (int i = count1; i < count; i++) {
				result[i] = b.getInt(p);
				p += ELEMENT_SIZE;
			}

			return result;
		}
	}
}

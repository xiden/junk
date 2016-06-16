/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package junk;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

/**
 * メモリマップトファイルを簡単に使える様にするためのクラス.
 */
public class MMFile implements AutoCloseable {

	// Linuxだと /dev/shm がメモリディスク扱いなのでそれを使えるがWindowsだと無いので適当に "c:/dev/shm/" でエミュレート
	private final static String SHM_DIR = System.getProperty("os.name").toLowerCase().startsWith("windows") ? "c:/dev/shm/" : "/dev/shm/";

	private RandomAccessFile _file;
	private FileChannel _ch;
	private MappedByteBuffer _buf;

	/**
	 * 指定名称のメモリマップトファイルを読み書き可能で作成する.
	 *
	 * @param name メモリマップトファイル名称.
	 * @param sizeInBytes サイズ(bytes).
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public MMFile(String name, long sizeInBytes) throws FileNotFoundException, IOException {
		_file = new RandomAccessFile(SHM_DIR + name, "rw");
		_ch = _file.getChannel();
		_buf = _ch.map(FileChannel.MapMode.READ_WRITE, 0, sizeInBytes);
		_buf.order(ByteOrder.LITTLE_ENDIAN); // ARMの設定がリトルエンディアンならこれ必要、 lscpu コマンドで確認できる
	}

	/**
	 * 指定名称のメモリマップトファイルを指定モードで開く.
	 *
	 * @param name メモリマップトファイル名称.
	 * @param writable 書き込み可能で開くかどうか.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public MMFile(String name, boolean writable) throws FileNotFoundException, IOException {
		_file = new RandomAccessFile(SHM_DIR + name, writable ? "rw" : "r");
		_ch = _file.getChannel();
		_buf = _ch.map(FileChannel.MapMode.READ_ONLY, 0, _ch.size());
		_buf.order(ByteOrder.LITTLE_ENDIAN); // ARMの設定がリトルエンディアンならこれ必要、 lscpu コマンドで確認できる
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
		if (_ch != null) {
			_ch.close();
			_ch = null;
		}
		if (_file != null) {
			_file.close();
			_file = null;
		}
	}

	/**
	 * @return メモリマップアクセス用バイトバッファ.
	 */
	public MappedByteBuffer getBuf() {
		return _buf;
	}
}

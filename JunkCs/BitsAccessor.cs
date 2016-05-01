using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Junk {
	/// <summary>
	/// byte 配列内の任意ビットにアクセスするためのクラス
	/// </summary>
	public class BitsAccessor : IDisposable {
		[DllImport("Kernel32.dll", EntryPoint = "RtlMoveMemory", SetLastError = false)]
		static unsafe extern void MoveMemory(void* dest, void* src, IntPtr size); 

		GCHandle _BufferHandle;
		unsafe byte* _pBuffer;
		long _BitLength;
		bool _LittleEndian = true;

		/// <summary>
		/// バッファ内のデータがリトルエンディアンであるかどうか
		/// </summary>
		public bool LittleEndian {
			get { return _LittleEndian; }
			set { _LittleEndian = value; }
		}

		/// <summary>
		/// コンストラクタ、アクセス対象バッファを指定して初期化する
		/// </summary>
		/// <param name="buffer">アクセス対象バッファ</param>
		public BitsAccessor(byte[] buffer) {
			_BufferHandle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
			var ptr = _BufferHandle.AddrOfPinnedObject();
			unsafe {
				_pBuffer = (byte*)ptr.ToPointer();
			}
			_BitLength = buffer.Length * 8;
		}

		/// <summary>
		/// 指定ビット位置から指定バイト数取得しビット位置を次の位置へ移動する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="count">バイト数</param>
		/// <returns>byte 配列</returns>
		public byte[] ReadBytes(ref int position, int count) {
			var r = GetBytes(position, count);
			position += count * 8;
			return r;
		}

		/// <summary>
		/// 指定ビット位置から最大32bitまでの整数を取得しビット位置を次の位置へ移動する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="bits">ビット数</param>
		/// <returns>整数</returns>
		public int ReadInt(ref int position, int bits) {
			var r = GetInt(position, bits);
			position += bits;
			return r;
		}

		/// <summary>
		/// 指定ビット位置から最大32bitまでの符号なし整数を取得しビット位置を次の位置へ移動する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="bits">ビット数</param>
		/// <returns>整数</returns>
		public uint ReadUInt(ref int position, int bits) {
			var r = GetUInt(position, bits);
			position += bits;
			return r;
		}

		/// <summary>
		/// 指定ビット位置から指定バイト数取得する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="count">バイト数</param>
		/// <returns>byte 配列</returns>
		public byte[] GetBytes(int position, int count) {
			int bits = count * 8;
			long l = _BitLength;
			if (count < 0 || (ulong)l < (uint)position || (ulong)l < (uint)(position + bits))
				throw new ArgumentException();

			int index1 = position / 8; // バイト配列インデックス
			int lsb = position % 8; // index1 から最下位ビットへのオフセットビット数
			byte[] bytes = new byte[count];
			unsafe {
				byte* pSrc = _pBuffer + index1;
				fixed (byte* pDst = bytes) {
					if (lsb == 0) {
						MoveMemory(pDst, pSrc, new IntPtr(count));
					} else if (count != 0) {
						int shift = 8 - lsb;
						byte t1 = pSrc[0];
						for (int i = 0; i < count; i++) {
							byte t2 = pSrc[i + 1];
							pDst[i] = (byte)((t2 << shift) | (t1 >> lsb));
							t1 = t2;
						}
					}
				}
			}

			return bytes;
		}

		/// <summary>
		/// 指定ビット位置から最大32bitまでの整数を取得する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="bits">ビット数</param>
		/// <returns>整数</returns>
		public int GetInt(int position, int bits) {
			long l = _BitLength;
			if (bits == 0 || 32 < (uint)bits || (ulong)l < (uint)position || (ulong)l < (uint)(position + bits))
				throw new ArgumentException();

			int index1 = position / 8; // バイト配列インデックス
			int lsb = position % 8; // index1 から最下位ビットへのオフセットビット数
			int msb = lsb + bits; // index1 から最上位ビットの位置へのオフセット+1
			int size = (msb + 7) / 8; // バイト境界を考慮した値取得の必要変数サイズ

			unsafe {
				byte* p = _pBuffer + index1;
				switch (size) {
				case 1: {
						sbyte t = *(sbyte*)p;
						t <<= 8 - msb;
						t >>= 8 - bits;
						return t;
					}

				case 2: {
						short t = *(short*)p;
						t <<= 16 - msb;
						t >>= 16 - bits;
						return this.LittleEndian ? t : (short)ReverseBytes((ushort)t);
					}

				case 3: {
						uint t1 = *(ushort*)p;
						uint t2 = *(byte*)(p + 2);
						int t = (int)((t1 << (32 - bits - lsb)) | (t2 << (32 - (msb & 3))));
						t >>= 32 - bits;
						return this.LittleEndian ? t : (int)ReverseBytes((uint)t);
					}

				case 4: {
						int t = *(int*)p;
						t <<= 32 - msb;
						t >>= 32 - bits;
						return this.LittleEndian ? t : (int)ReverseBytes((uint)t);
					}

				case 5: {
						ulong t1 = *(uint*)p;
						ulong t2 = *(byte*)(p + 4);
						long t = (long)((t1 << (64 - bits - lsb)) | (t2 << (64 - (msb & 3))));
						t >>= 64 - bits;
						return this.LittleEndian ? (int)t : (int)ReverseBytes((uint)(int)t);
					}

				default:
					throw new NotImplementedException();
				}
			}
		}

		/// <summary>
		/// 指定ビット位置から最大32bitまでの符号なし整数を取得する
		/// </summary>
		/// <param name="position">ビット位置</param>
		/// <param name="bits">ビット数</param>
		/// <returns>整数</returns>
		public uint GetUInt(int position, int bits) {
			long l = _BitLength;
			if (bits == 0 || 32 < (uint)bits || (ulong)l < (uint)position || (ulong)l < (uint)(position + bits))
				throw new ArgumentException();

			int index1 = position / 8; // バイト配列インデックス
			int lsb = position % 8; // index1 から最下位ビットへのオフセットビット数
			int msb = lsb + bits; // index1 から最上位ビットの位置へのオフセット+1
			int size = (msb + 7) / 8; // バイト境界を考慮した値取得の必要変数サイズ

			unsafe {
				byte* p = _pBuffer + index1;
				switch (size) {
				case 1: {
						byte t = *(byte*)p;
						t <<= 8 - msb;
						t >>= 8 - bits;
						return t;
					}

				case 2: {
						ushort t = *(ushort*)p;
						t <<= 16 - msb;
						t >>= 16 - bits;
						return this.LittleEndian ? t : ReverseBytes(t);
					}

				case 3: {
						uint t1 = *(ushort*)p;
						uint t2 = *(byte*)(p + 2);
						uint t = (uint)((t1 << (32 - bits - lsb)) | (t2 << (32 - (msb & 3))));
						t >>= 32 - bits;
						return this.LittleEndian ? t : ReverseBytes(t);
					}

				case 4: {
						uint t = *(uint*)p;
						t <<= 32 - msb;
						t >>= 32 - bits;
						return this.LittleEndian ? t : ReverseBytes(t);
					}

				case 5: {
						ulong t1 = *(uint*)p;
						ulong t2 = *(byte*)(p + 4);
						ulong t = (ulong)((t1 << (64 - bits - lsb)) | (t2 << (64 - (msb & 3))));
						t >>= 64 - bits;
						return this.LittleEndian ? (uint)t : ReverseBytes((uint)t);
					}

				default:
					throw new NotImplementedException();
				}
			}
		}

		/// <summary>
		/// 内部 Dispose 処理
		/// </summary>
		/// <param name="disposing">Dispose() 内から呼ばれているかどうか</param>
		protected virtual void Dispose(bool disposing) {
			if (_BufferHandle.IsAllocated) {
				_BufferHandle.Free();
				unsafe {
					_pBuffer = null;
				}
			}
		}

		/// <summary>
		/// アンマネージ リソースの解放およびリセットに関連付けられているアプリケーション定義のタスクを実行します。
		/// </summary>
		public void Dispose() {
			Dispose(true);
			GC.SuppressFinalize(this);  // Violates rule
		}

		/// <summary>
		/// ファイナライザ
		/// </summary>
		~BitsAccessor() {
			Dispose(false);
		}

		static ushort ReverseBytes(ushort val) {
			return (ushort)((
				(val << 8) & 0xff00)
				| (val >> 8)
			);
		}

		static uint ReverseBytes(uint val) {
			return (uint)((
				(val << 24) & 0xff000000)
				| ((val << 8) & 0x00ff0000)
				| ((val >> 8) & 0x0000ff00)
				| (val >> 24)
			);
		}
	}
}

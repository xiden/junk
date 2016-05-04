using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Jk;

namespace General {
	class Program {
		static void Main(string[] args) {
			var bytes = new byte[188];
			var ba = new BitAccessor(bytes);
			var mt = BitMarshal.ToObject<MpegTs>(ba, 0);
			Console.Write(mt.SyncByte);
		}
	}

	[BitStruct(false)]
	public struct MpegTs {
		[Bits(8)]
		public uint SyncByte; //!< 同期判定用マジックナンバー 0x47
		[Bits(13)]
		public uint PID; //!< 後続バイトが何を表現しているのかを表しているもの
		[Bits(1)]
		public uint TransportPriority; //!< なぞ
		[Bits(1)]
		public uint PayloadUnitStartIndicator; //!< 一塊の情報が複数パケットに分割されるときにどれが先頭パケットなのかを示すためのフラグ
		[Bits(1)]
		public uint TransportErrorIndicator; //!< エラーチェックのために使用され、0 でなければならない？
		[Bits(2)]
		public uint TransportScramblingControl;
		[Bits(2)]
		public uint AdaptationFieldControl; //!< 後続パケットにAdaptationFieldControlやPayloadがあるかを示すフラグ、上1ビットが立っている場合はAdaptationFieldControlがあり、下1ビットが立っている場合はPayloadがあることを表す
		[Bits(4)]
		public uint ContinuityCounter; //!< パケットが欠落していないかの確認のために用いられる、同じPIDのパケットが来るたびに1ずつインクリメントされ、0x0Fまで達したら次は0x00に戻る
		[Bits(8)]
		public uint PayloadSize; //!< ペイロードデータ長
		[Bits((188 - 5) * 8)]
		public byte[] Payload; //!< ペイロードデータ長
	};
}

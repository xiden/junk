#include <iostream>
#include <fstream>
#include <time.h>
#include "../../../JunkCpp/Socket.h"
#include "../../../JunkCpp/Error.h"
#include "../../../JunkCpp/Thread.h"
#include "../../../JunkCpp/RingBuffer.h"

using namespace jk;

std::vector<std::string> args; //!< コマンドライン引数

bool TcpClient() {
	if (args.size() < 2) {
		std::cout << "TcpClient <hostaddress> <port>" << std::endl;
		return false;
	}

	auto host = args[0];
	auto port = args[1];

	Socket::Endpoint serverEp;
	if (!serverEp.Create(host.c_str(), port.c_str(), Socket::St::Stream, Socket::Af::IPv4))
		return false;

	Socket sk;
	if (!sk.Create(serverEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}
	if (!sk.Connect(serverEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}

	std::cout << "入力された文字が送信されます" << std::endl;
	for (std::string line; std::getline(std::cin, line);) {
		if (line == "quit")
			break;
		line += "\n";
		sk.Send(line.c_str(), line.size());
	}

	return true;
}

bool TcpServer() {
	if (args.size() < 1) {
		std::cout << "TcpServer <port>" << std::endl;
		return false;
	}

	auto port = args[0];

	Socket::Endpoint serverEp;
	if (!serverEp.Create(NULL, port.c_str(), Socket::St::Stream, Socket::Af::IPv4)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}

	Socket skListen;
	if (!skListen.Create(serverEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}
	if (!skListen.Bind(serverEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}

	if (!skListen.Listen(10)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}

	std::cout << "送信された文字がひたすら表示されます" << std::endl;
	std::vector<uint8_t> buf(0x100000);
	for (;;) {
		sockaddr_storage fromAddr;
		socklen_t fromLen = sizeof(fromAddr);
		memset(&fromAddr, 0, sizeof(fromAddr));
		std::cout << "接続待機中・・・" << std::endl;
		Socket sk = skListen.Accept(&fromAddr, &fromLen);
		if (sk.IsInvalidHandle()) {
			Error::SetOsLastError();
			std::cout << Error::GetLastError() << std::endl;
			continue;
		}
		std::cout << Socket::GetRemoteName(fromAddr) << "から接続されました" << std::endl;

		for (;;) {
			auto n = sk.Recv(&buf[0], buf.size() - 1);
			if (n <= 0)
				break;
			buf[n] = 0;
			std::cout << (char*)&buf[0];
		}

		sk.Shutdown(Socket::Sd::Both);
		std::cout << "切断されました" << std::endl;
	}

	return true;
}

bool UdpClient() {
	if (args.size() < 2) {
		std::cout << "UdpClient <hostaddress> <port>" << std::endl;
		return false;
	}

	auto host = args[0];
	auto port = args[1];

	Socket::Endpoint clientEp;
	if (!clientEp.Create(NULL, NULL, Socket::St::Dgram, Socket::Af::IPv4))
		return false;
	Socket::Endpoint serverEp;
	if (!serverEp.Create(host.c_str(), port.c_str(), Socket::St::Dgram, Socket::Af::IPv4))
		return false;

	Socket sk;
	if (!sk.Create(clientEp))
		return false;
	if (!sk.Bind(clientEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}

	std::cout << "入力された文字が送信されます" << std::endl;
	for (std::string line; std::getline(std::cin, line);) {
		if (line == "quit")
			break;
		sk.SendTo(line.c_str(), line.size(), serverEp);
	}

	return true;
}

bool UdpServer() {
	if (args.size() < 1) {
		std::cout << "UdpServer <port>" << std::endl;
		return false;
	}

	auto port = args[0];

	Socket::Endpoint serverEp;
	if (!serverEp.Create(NULL, port.c_str(), Socket::St::Dgram, Socket::Af::IPv4))
		return false;

	Socket sk;
	if (!sk.Create(serverEp))
		return false;
	if (!sk.Bind(serverEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}
	//sk.SetBlockingMode(false);
	sk.SetNoDelay(1);

	std::cout << "送信された文字がひたすら表示されます" << std::endl;
	std::vector<uint8_t> buf(0x100000);
	for (;;) {
		sockaddr_storage fromAddr;
		socklen_t fromLen = sizeof(fromAddr);
		memset(&fromAddr, 0, sizeof(fromAddr));
		auto n = sk.RecvFrom(&buf[0], buf.size(), &fromAddr, &fromLen);
		if (1 <= n) {
			std::cout << std::string((char*)&buf[0], (char*)&buf[n]) << std::endl;
		}
	}

	return true;
}

bool UdpMpeg2tsClient() {
	struct CrcCalculator {
		uint32_t table[256];

		CrcCalculator() {
			for (uint32_t i = 0; i < 256; i++) {
				uint32_t crc = i << 24;
				for (int j = 0; j < 8; j++) {
					crc = (crc << 1) ^ ((crc & 0x80000000) ? 0x04c11db7 : 0);
				}
				table[i] = crc;
			}
		}

		uint32_t operator()(uint8_t *buf, intptr_t len) {
			uint32_t crc = 0xffffffff;
			for (intptr_t i = 0; i < len; i++) {
				crc = (crc << 8) ^ table[((crc >> 24) ^ buf[i]) & 0xff];
			}
			return crc;
		}
	};
	static auto cc = CrcCalculator();


	if (args.size() < 1) {
		std::cout << "UdpMpeg2tsClient <port>" << std::endl;
		return false;
	}

	auto port = args[0];

	Socket::Endpoint clientEp;
	if (!clientEp.Create(NULL, port.c_str(), Socket::St::Dgram, Socket::Af::IPv4, true))
		return false;

	Socket sk;
	if (!sk.Create(clientEp))
		return false;
	if (!sk.Bind(clientEp)) {
		Error::SetOsLastError();
		std::cout << Error::GetLastError() << std::endl;
		return false;
	}
	//sk.SetBlockingMode(false);
	//sk.SetNoDelay(1);

	std::vector<uint8_t> buf(0x100000); // パケット解析用バッファ
	auto s = &buf[0]; // バッファ内の処理先頭ポインタ
	auto p = s; // バッファ内パケット先頭サーチポインタ
	auto e = s; // バッファ内有効データ終端の次のポインタ
	auto cap = p + buf.size(); // バッファ終端
	intptr_t state = 0; // 解析状態
	intptr_t payload_unit_start_indicator; // 複数パケットに分割されたペイロードの先頭なら1
	intptr_t transport_priority; // 同じPID内での優先度が高いなら1
	intptr_t PID; // ペイロードの内容を示すID
	intptr_t transport_scrambling_control; // スクランブルフラグ
	intptr_t adaption_field_control; // ペイロードかアダプテーションあるかどうかのフラグ
	intptr_t continuity_counter; // ペイロードのシーケンス番号
	intptr_t payload_len; // ペイロードサイズ
	uint32_t crc; // ペイロードあり時のCRC

	std::ofstream fout;
	fout.open("mpeg2ts_parse.dat", std::ios::out | std::ios::binary | std::ios::trunc);

	for (;;) {
		// UDPから受信
		sockaddr_storage fromAddr;
		socklen_t fromLen = sizeof(fromAddr);
		memset(&fromAddr, 0, sizeof(fromAddr));

		// 未解析データが1パケット(188バイト)未満なら読み込む
		if (e - p < 188) {
			intptr_t n = sk.RecvFrom(e, cap - e, &fromAddr, &fromLen);
			if (0 < n)
				e += n;
		}

		// 188バイト未満ならパケットに成り得ない
		if (e - p < 188)
			continue;

		switch (state) {
		case 0:
			// 同期バイトを探す
			for (; p < e && *p == 0x47; p++);
			if (p == e) {
				p = e = s;
				continue; // バッファ内に見つからなかったのでバッファ内容全部捨てる
			}
			state = 1; // ヘッダ解析状態へ移る
			break;

		case 1:
			// ヘッダ解析
			if (e - p < 4)
				continue; // ヘッダは4バイトないとだめ

						  // 転送エラーフラグチェック
			if ((p[1] & 0x80) >> 7) {
				p++;
				continue; // エラーなのでパケットの先頭を探しなおす
			}

						  // 分割複数ペイロードの先頭を示すフラグ
			payload_unit_start_indicator = (p[1] & 0x40) >> 6;
			// 同じPIDの他のパケットよりも優先度が高い事を示すフラグ
			transport_priority = (p[1] & 0x20) >> 5;
			// ペイロード内容をを示すID
			PID = (uintptr_t)((p[1] & 0x1F) >> 8) | (uintptr_t)p[2];
			// スクランブルフラグ
			transport_scrambling_control = (p[3] & 0x60) >> 6;
			// ペイロードまたはアダプテーション存在フラグ
			adaption_field_control = (p[3] & 0x30) >> 4;
			// ペイロードパケットのシーケンス番号
			continuity_counter = p[3] & 0x0F;

			if (adaption_field_control) {
				// ペイロードありならペイロードサイズチェック状態へ
				state = 2;
			} else {
				// ペイロード無いならパケット完了待ち状態へ
				state = 3;
			}
			break;

		case 2:
			// ペイロードサイズチェック状態
			if (e - p < 5)
				continue; // 5バイトないとペイロードサイズが取得できない
			payload_len = p[5];
			if (188 < payload_len + 5) {
				// ペイロードサイズ異常なので同期バイト検出間違いかもしれない
				// 1バイト進めてやり直し
				p++;
				continue;
			}

			// パケット完了待ち状態へ移る
			state = 3;
			break;

		case 3:
			// パケット完了待ち
			if (e - p < 188)
				continue; // 188バイト以上無いとパケット

			// ペイロードありならCRCチェックする
			if (adaption_field_control) {
				crc = cc(p + 5, payload_len - 4);
				uint32_t crc2 = (uint32_t)p[payload_len + 1] << 24;
				crc2 |= (uint32_t)p[payload_len + 2] << 16;
				crc2 |= (uint32_t)p[payload_len + 3] << 8;
				crc2 |= (uint32_t)p[payload_len + 4];
				if (crc != crc2) {
					// CRC違うなら同期バイト検出間違いかもしれない
					// 1バイト進めてやり直し
					p++;
					std::cout << "CRCエラー: " << std::hex << crc << " : " << crc2 << std::endl << std::dec;
					continue;
				}
			}

			fout.write((char*)p, 188);
			std::cout << "PID=" << PID << std::endl;

			// このパケットはもう用済み
			p += 188;

			// もしもうバッファ内にパケットがあり得ないなら残りをバッファ先頭にずらす
			if (e - p < 188) {
				memmove(s, p, e - p);
				e -= e - p;
				p = s;
			}

			// 同期バイトを探す状態に戻る
			state = 0;
			break;
		}
	}

	return true;
}

bool StreamTest() {
	std::cout << "PythonとのTCP/IP通信テスト用に作ったもの" << std::endl;

	Socket sk;
	if (!sk.Create())
		return false;

	sockaddr_in adr = Socket::IPv4StrToAddress("127.0.0.1", 4000);
	if (!sk.Connect(&adr))
		return false;

	float buf[4096];
	for (int i = 0; i < 30; i++) {
		int count = i + 1;
		for (int j = 0; j < count; j++)
			buf[j] = i + j + 1;
		sk.Send(&count, 4);
		sk.Send(buf, 4 * count);
		auto n = sk.Recv(buf, sizeof(buf));
		if (4 <= n) {
			printf("%f\n", *(float*)buf);
		}
	}
	sk.Shutdown(Socket::Sd::Both);

	return true;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "usage : SocketTest1 <mode>" << std::endl;
		std::cout << "<mode> : StreamTest / UdpServer / UdpClient / UdpMpeg2tsClient" << std::endl;
		return 0;
	}

	// コマンドライン引数取得
	args.resize(argc - 2);
	for (int i = 2; i < argc; i++) {
		args[i - 2] = argv[i];
	}

	Socket::Startup();

	std::string mode = argv[1];

	if (mode == "StreamTest")
		StreamTest();
	else if (mode == "TcpServer")
		TcpServer();
	else if (mode == "TcpClient")
		TcpClient();
	else if (mode == "UdpServer")
		UdpServer();
	else if (mode == "UdpClient")
		UdpClient();
	else if (mode == "UdpMpeg2tsClient")
		UdpMpeg2tsClient();
	else
		std::cout << "Not implimented mode" << std::endl;

	Socket::Cleanup();

	return 0;
}

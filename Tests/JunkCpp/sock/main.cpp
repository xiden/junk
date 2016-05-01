#include <iostream>
#include "../../../JunkCpp/Socket.h"
#include "../../../JunkCpp/Error.h"
#include "../../../JunkCpp/Thread.h"
#include "../../../JunkCpp/RingBuffer.h"

#define BUFFER_SIZE 256
#include <time.h>

using namespace jk;

std::vector<std::string> args; //!< コマンドライン引数

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
#pragma pack(push, 1)
	union TsPkt {
		uint8_t Array[188];
		struct {
			uint8_t SyncByte; //!< 同期判定用マジックナンバー 0x47
			uint8_t TransportErrorIndicator : 1; //!< エラーチェックのために使用され、0 でなければならない？
			uint8_t PayloadUnitStartIndicator : 1; //!< 一塊の情報が複数パケットに分割されるときにどれが先頭パケットなのかを示すためのフラグ
			uint8_t TransportPriority : 1; //!< なぞ
			uint32_t PID : 13; //!< 後続バイトが何を表現しているのかを表しているもの
			uint8_t TransportScramblingControl : 2; 
			uint8_t AdaptationFieldControl : 2; //!< 後続パケットにAdaptationFieldControlやPayloadがあるかを示すフラグ、上1ビットが立っている場合はAdaptationFieldControlがあり、下1ビットが立っている場合はPayloadがあることを表す
			uint8_t ContinuityCounter : 4; //!< パケットが欠落していないかの確認のために用いられる、同じPIDのパケットが来るたびに1ずつインクリメントされ、0x0Fまで達したら次は0x00に戻る
			union {
				uint8_t AdaptationSize; //!< ペイロードデータ長
				uint8_t Payloads[188 - 4]; //!< PIDで指定されたデータ
			};
		};
	};
#pragma pack(pop)

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
	sk.SetNoDelay(1);

	std::vector<uint8_t> buf(0x100000);
	RingBufferSizeFixed<uint8_t, 4096> ring;
	TsPkt pkt;
	intptr_t state = 0; // 読み込みモード、0:なし、1:Adaptation、2:Payload
	intptr_t adaptEndSize;
	intptr_t payloadUnitStartIndicator = 0;
	intptr_t transportPriority = 0;
	intptr_t pid = 0;
	intptr_t transportScramblingControl = 0;
	intptr_t adaptationFieldControl = 0;
	intptr_t continuityCounter = 0;
	intptr_t payloadSize = 0;
	for (;;) {
		sockaddr_storage fromAddr;
		socklen_t fromLen = sizeof(fromAddr);
		memset(&fromAddr, 0, sizeof(fromAddr));
		auto n = sk.RecvFrom(&buf[0], buf.size(), &fromAddr, &fromLen);
		if (1 <= n) {
			for (int i = 0; i < n; i++) {
				auto b = buf[i];
				ring.Write(b);

				switch (state) {
				case 0:
					if (b == 0x47)
						state++;
					break;
				case 1:
					if (b & 1) {
						state = 0;
						ring.Clear();
					} else {
						payloadUnitStartIndicator = b & 2;
						transportPriority = b & 4;
						state++;
					}
					break;
				case 2:
					pid = ring.PeekHead<uint16_t>(1) >> 3;
					state++;
					break;
				case 3:
					transportScramblingControl = b & 3;
					adaptationFieldControl = (b >> 2) & 3;
					continuityCounter = (b >> 4) & 15;
					if (adaptationFieldControl)
						state++;
					else
						state = 0;
						ring.Clear();
					break;
				case 4:
					payloadSize = b;
					if(payloadSize)
						std::cout << "PID=" << pid << " Payload=" << payloadSize << " continuityCounter=" << continuityCounter << std::endl;
					else
						std::cout << "PID=" << pid << std::endl;
					state = 0;
					ring.Clear();
					break;
				}

				//switch (state) {
				//case 0:
				//	if (ring.Size() == 4) {
				//		ring.PeekHead(0, 4, (uint8_t*)&pkt);
				//		if (pkt.SyncByte == 0x47) {
				//			if (pkt.TransportErrorIndicator == 0) {
				//				std::cout << "PID=" << pkt.PID << std::endl;
				//				if (pkt.AdaptationFieldControl & 2) {
				//					auto e = 4 + 1 + pkt.AdaptationSize;
				//					state = 1;
				//					adaptEndSize = e;
				//					std::cout << "AdaptationSize=" << (int)pkt.AdaptationSize << std::endl;
				//				}
				//			}
				//		} else {
				//			ring.DropHead(1);
				//		}
				//	}
				//	break;
				//case 1:
				//	if (ring.Size() == adaptEndSize) {
				//		state = 2;
				//		std::cout << "Adaptation end" << std::endl;
				//	}
				//	break;
				//case 2:
				//	if (ring.Size() == 188) {
				//		ring.Clear();
				//		std::cout << "Packet end" << std::endl;
				//		state = 0;
				//	}
				//	break;
				//}
			}
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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: user1
 *
 * Created on 2015/10/19, 22:57
 */

#include <cstdlib>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdio.h>
#include <string.h>
#include "../../../JunkCpp/ThreadLocalStorage.h"
#include "../../../JunkCpp/MMFile.h"
#include "../../../JunkCpp/Error.h"
#include "../../../JunkCpp/Directory.h"

using namespace std::chrono;

void TlsTest() {
	static JUNK_TLS(int) tlsInt;
	std::mutex mtx;

	auto f1 = [&] {
		auto& i = tlsInt.Get();
		while (true) {
			i += 1;
			{
				std::lock_guard<std::mutex> lock(mtx);
				std::cout << i << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};

	auto f2 = [&] {
		auto& i = tlsInt.Get();
		while (true) {
			i += 2;
			{
				std::lock_guard<std::mutex> lock(mtx);
				std::cout << i << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};

	std::thread t1(f1);
	std::thread t2(f2);

	t1.join();
	t2.join();
}

void MMFileTest() {
	std::string cdir;
	jk::Directory::GetCurrent(cdir);
	std::cout << cdir.c_str() << std::endl;
	/*jk::Error::SetLastError("afefefefe");
	std::cout << jk::Error::GetLastError() << std::endl;
	std::cout << strerror(1) << std::endl;
	char buf[256];
	memset(buf, 0, sizeof(buf));
	auto p = strerror_r(1, buf, sizeof(buf) - 1);
	std::cout << buf << std::endl;*/
	

	{
		jk::MMFile mmf;
		if (!mmf.Create("mmf.dat", 256)) {
			std::cout << jk::Error::GetLastError() << std::endl;
			return;
		}

		{
			jk::MMView mmv;
			if (!mmv.Map(&mmf, 0, 256)) {
				std::cout << jk::Error::GetLastError() << std::endl;
				return;
			}

			auto p = (uint8_t*)mmv.Ptr();
			for (int i = 0; i < 256; i++)
				p[i] = i;

			if (!mmv.Unmap()) {
				std::cout << jk::Error::GetLastError() << std::endl;
				return;
			}
		}
	}

	{
		jk::MMFile mmf;
		if (!mmf.Open("mmf.dat", jk::MMFile::OpenRead)) {
			std::cout << jk::Error::GetLastError() << std::endl;
			return;
		}

		{
			jk::MMView mmv;
			if (!mmv.Map(&mmf, 128, 128)) {
				std::cout << jk::Error::GetLastError() << std::endl;
				return;
			}

			auto p = (uint8_t*)mmv.Ptr();
			for (int i = 0; i < 128; i++)
				std::cout << (int)p[i] << std::endl;

			if (!mmv.Unmap()) {
				std::cout << jk::Error::GetLastError() << std::endl;
				return;
			}
		}
	}
}

/*
 * 
 */
int main(int argc, char** argv) {
//	char buf[256];
//	memset(buf, 0, sizeof(buf));
//	strerror_r(EIO, buf, sizeof(buf) - 1);

	//jk::Error::SetLastErrorFromErrno(32);

	auto start = high_resolution_clock::now(); // 計測スタート時刻を保存
	auto end = high_resolution_clock::now(); // 計測スタート時刻を保存
	auto dur = end - start;

	//std::cout << duration_cast<nanoseconds>(dur).count() << std::endl;

	//TlsTest();
	MMFileTest();

	return 0;
}

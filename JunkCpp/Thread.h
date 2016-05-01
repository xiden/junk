#pragma once
#ifndef __JUNK_THREAD_H__
#define __JUNK_THREAD_H__

#include "JunkConfig.h"

#if defined __GNUC__

#include <pthread.h>
#include <unistd.h>

#else

#include <Windows.h>

#endif


_JUNK_BEGIN

//! スレッド
class Thread {
public:
#if defined __GNUC__
	typedef pthread_t Handle; //!< スレッドハンドル型
	typedef pid_t Id; //!< スレッドID型
#else
	typedef uintptr_t Handle; //!< スレッドハンドル型
	typedef DWORD Id; //!< スレッドID型
#endif
	typedef intptr_t (*ProcPtr)(void*); //!< スレッド開始ルーチンポインタ

	Thread(); //!< コンストラクタ
	~Thread(); //!< デストラクタ
	ibool Start(ProcPtr startRoutine, void* pArg); //!< スレッドを開始する
	void Join(); //!< スレッドの終了を待ち、リソースを開放する
	void Close(); //!< スレッドの終了を待たずに、リソースを開放する
	static void Sleep(uint32_t msec); //!< 指定された時間現在のスレッドを停止する

#if defined __GNUC__
	//! 現在のスレッドIDの取得
	static _FINLINE Id CurrentThreadId() {
		return pthread_self();
	}
#else
	//! 現在のスレッドIDの取得
	static _FINLINE Id CurrentThreadId() {
		return ::GetCurrentThreadId();
	}
#endif

	//! Start() メソッドでスレッド作成されたかどうかの取得
	_FINLINE ibool IsStarted() {
		return m_hThread != 0;
	}

	//! ハンドルの取得
	_FINLINE Handle GetHandle() {
		return m_hThread;
	}

protected:
#if defined __GNUC__
	static void* StartRoutine(void*); //!< スレッド初期化、終了処理用
#else
	static unsigned int WINAPI StartRoutine(void*); //!< スレッド初期化、終了処理用
#endif

protected:
	Handle m_hThread; //!< スレッドハンドル
	ProcPtr m_UserStartRoutine; //!< Start() メソッドに渡された開始アドレス
	void* m_pUserArgs; //!< Start() メソッドに渡されたコンテキスト
};

//! ミューテックス
class Mutex {
public:
	Mutex(); //!< コンストラクタ
	~Mutex(); // デストラクタ
	void Lock(); //!< ロックする
	void Unlock(); //!< アンロックする
	void Initialize(); //!< コンストラクタと同じように内部オブジェクトを初期化する
	void Destroy(); //!< デストラクタと同じように内部オブジェクトを破棄する

protected:
#if defined __GNUC__
	pthread_mutex_t m_mutex; //!< ミューテックス
#else
	HANDLE m_hMutex; //!< ミューテックスハンドル
#endif
};

//! イベント
class Event {
public:
	Event(); //!< コンストラクタ
	~Event(); // デストラクタ
	void Set(); //!< イベントをシグナル状態にする
	void Reset(); //!< イベントを非シグナル状態にする
	ibool Wait(); //!< イベントがシグナル状態になるのを待つ
	void Initialize(); //!< コンストラクタと同じように内部オブジェクトを初期化する
	void Destroy(); //!< デストラクタと同じように内部オブジェクトを破棄する

protected:
#if defined __GNUC__
	pthread_cond_t m_ready;
	pthread_mutex_t m_lock;
#else
	HANDLE m_hEvent; //!< イベントハンドル
#endif
};

//! 同期用オブジェクトロック＆アンロックヘルパ
template<
class _Sync //!< 同期用オブジェクト、Lock() と Unlock() メソッドを実装している必要がある
>
class Lock {
public:
	//! コンストラクタ、指定された同期オブジェクトをロックする
	Lock(_Sync* p) {
		assert(p);
		pSync = p;
		pSync->Lock();
	}

	//! デストラクタ、コンストラクタでロックされたオブジェクトをアンロックする
	~Lock() {
		if(pSync != NULL)
			pSync->Unlock();
	}

	//! コンストラクタで指定された同期オブジェクトを切り離す、デストラクタでアンロックされなくなる
	void Detach() {
		pSync = NULL;
	}

protected:
	_Sync* pSync; //!< 同期用オブジェクト、Lock() と Unlock() メソッドを実装している必要がある
};

typedef Lock<Mutex> MutexLock; //!< Mutex 用ロック

_JUNK_END

#endif

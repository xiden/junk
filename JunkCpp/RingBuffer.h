#pragma once
#ifndef __JUNK_RINGBUFFER_H__
#define __JUNK_RINGBUFFER_H__

#include "JunkConfig.h"
#include "JunkDef.h"
#include <memory.h>
#include <vector>

_JUNK_BEGIN

//! サイズ固定リングバッファ
//! @remarks 書き込みと読み込みスレッドが１つずつならスレッドセーフ、クリアなどは使うならスレッドセーフではない
//! @remarks 書き込み時にバッファフルなら書き込みは行われない
template<class T, intptr_t SIZE>
struct RingBufferSizeFixed {
	T Buffer[SIZE];
	intptr_t Head;
	intptr_t Tail;

	RingBufferSizeFixed() {
		Head = Tail = 0;
	}

	constexpr intptr_t Capacity() const {
		return SIZE;
	}
	_FINLINE intptr_t Size() const {
		return Size(Head, Tail, Capacity());
	}
	static _FINLINE intptr_t Size(intptr_t h, intptr_t t, intptr_t cap) {
		intptr_t size = t - h;
#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4293)
#endif
		intptr_t mask = size >> (sizeof(size) * 8);
#if defined _MSC_VER
#pragma warning(pop)
#endif
		return size + (mask & cap);
	}
	_FINLINE bool IsEmpty() const {
		return Tail == Head;
	}
	_FINLINE bool IsFull() const {
		return Size() == Capacity();
	}
	_FINLINE bool Write(const T& w) {
		if (IsFull())
			return false;
		auto t = Tail % Capacity();
		Buffer[t++] = w;
		Tail = t;
		return true;
	}
	_FINLINE T Read() {
		if (IsEmpty())
			return T();
		auto h = Head % Capacity();
		auto r = Buffer[h++];
		Head = h;
		return r;
	}
	_FINLINE bool Read(T& r) {
		if (IsEmpty())
			return false;
		auto h = Head % Capacity();
		r = Buffer[h++];
		Head = h;
		return true;
	}
	inline intptr_t PeekHead(intptr_t indexFromHead, intptr_t count, T* pBuf) const {
		auto t = Tail;
		auto h = Head;
		auto cap = Capacity();
		auto size = Size(h, t, cap);
		if ((uintptr_t)size <= (uintptr_t)indexFromHead)
			return 0;
		if (size < indexFromHead + count)
			count = size - indexFromHead;
		if (count == 0)
			return 0;
		auto s = (h % cap) + indexFromHead;
		auto end = s + count;
		if (cap < end) {
			auto count1 = cap - s;
			memcpy(pBuf, &Buffer[s], count1);
			memcpy(pBuf + count1, &Buffer[0], count - count1);
		} else {
			memcpy(pBuf, &Buffer[s], count);
		}
		return count;
	}
	template<class R> _FINLINE R PeekHead(intptr_t indexFromHead) const {
		R r();
		PeekHead(indexFromHead, sizeof(R), (T*)&r);
		return r;
	}
	void DropHead(intptr_t count) {
		auto t = Tail;
		auto h = Head;
		auto cap = Capacity();
		auto size = Size(h, t, cap);
		if (size < count)
			count = size;
		if (count == 0)
			return;
		h += count;
		if (cap < h)
			h -= cap + 1;
		Head = h;
	}
	_FINLINE void Clear() {
		Head = Tail = 0;
	}
	_FINLINE const T& operator[](intptr_t index) const {
		return Buffer[(Head +  index) % Capacity()];
	}
	_FINLINE T& operator[](intptr_t index) {
		return Buffer[(Head + index) % Capacity()];
	}
};

_JUNK_END

#endif

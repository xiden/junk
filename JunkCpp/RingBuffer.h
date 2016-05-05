#pragma once
#ifndef __JUNK_RINGBUFFER_H__
#define __JUNK_RINGBUFFER_H__

#include "JunkConfig.h"
#include "JunkDef.h"
#include <memory.h>
#include <vector>

_JUNK_BEGIN

//! �����O�o�b�t�@���̗̈���w���|�C���^
//! @remarks �������ꂽ�Q�̗̈���w���悤�ɂȂ��Ă�
template<
	class T //!< �v�f�^
>
struct RingPtr {
	T* p1; //!< �������ꂽ�O���̈�ւ̃|�C���^�ANULL �ɂȂ蓾��A���ꂪ NULL �Ȃ瑼�����o�S������
	intptr_t n1; //!< p1 ���w���̈�̃T�C�Y�A0 �ɂȂ蓾��
	T* p2; //!< �������ꂽ�㔼�̈�ւ̃|�C���^�ANULL �ɂȂ蓾��
	intptr_t n2; //!< p2 ���w���̈�̃T�C�Y�A0 �ɂȂ蓾��

	RingPtr() {
	}
	RingPtr(T* p1, intptr_t n1, T* p2, intptr_t n2) {
		this->p1 = p1;
		this->n1 = n1;
		this->p2 = p2;
		this->n2 = n2;
	}
	bool IsNull() const {
		return p1 == nullptr;
	}
	intptr_t Size() const {
		return n1 + n2;
	}
};

//! �T�C�Y�Œ胊���O�o�b�t�@
//! @remarks �������ݐ�p�X���b�h�Ɠǂݍ��ݐ�p�X���b�h�̍\���Ȃ�X���b�h�Z�[�t�A�N���A�͓Ǐ��������ύX����̂ŃX���b�h�Z�[�t�ł͂Ȃ�
//! @remarks �������ݎ��Ƀo�b�t�@�t���Ȃ珑�����݂͍s���Ȃ�
//! @remarks �������ނ� Tail ���ړ����A�ǂݍ��ނ� Head ���ړ����܂��A�Â��f�[�^�̕��� Head �ł�
template<
	class T, //!< �v�f�^
	intptr_t SIZE //!< �ő�v�f��
>
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
		intptr_t negMask = size >> (sizeof(size) * 8);
#if defined _MSC_VER
#pragma warning(pop)
#endif
		return size + (negMask & cap);
	}
	static _FINLINE intptr_t FreeSize(intptr_t h, intptr_t t, intptr_t cap) {
		intptr_t size = t - h;
#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4293)
#endif
		intptr_t negMask = size >> (sizeof(size) * 8);
#if defined _MSC_VER
#pragma warning(pop)
#endif
		return cap - (size + (negMask & cap));
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
	intptr_t Write(const T* pBuf, intptr_t count) {
		auto t = Tail;
		auto h = Head;
		auto cap = Capacity();
		auto space = FreeSize(h, t, cap);
		if (space < count)
			count = space;
		if (count == 0)
			return 0;
		t %= cap;
		auto end = t + count;
		auto p = &Buffer[0];
		if (cap < end) {
			auto count1 = cap - t;
			auto count2 = count - count1;
			for (intptr_t i = 0; i < count1; i++, t++)
				p[t] = pBuf[i];
			t = 0;
			for (intptr_t i = count1; i < count2; i++, t++)
				p[t] = pBuf[i];
			Tail = t;
		} else {
			for (intptr_t i = 0; i < count; i++, t++)
				p[t] = pBuf[i];
			Tail = t;
		}
		return count;
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
	intptr_t Read(T* pBuf, intptr_t count) {
		auto t = Tail;
		auto h = Head;
		auto cap = Capacity();
		auto size = Size(h, t, cap);
		if (size < count)
			count = size;
		if (count == 0)
			return 0;
		h %= cap;
		auto end = h + count;
		const auto p = &Buffer[0];
		if (cap < end) {
			auto count1 = cap - h;
			auto count2 = count - count1;
			for (intptr_t i = 0; i < count1; i++, h++)
				pBuf[i] = p[h];
			h = 0;
			for (intptr_t i = count1; h < count2; i++, h++)
				pBuf[i] = p[h];
			Head = h;
		} else {
			for (intptr_t i = 0; i < count; i++, h++)
				pBuf[i] = p[h];
			Head = h;
		}
		return count;
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
		auto p = &Buffer[0];
		if (cap < end) {
			auto count1 = cap - s;
			auto count2 = count - count1;
			for (intptr_t i = 0, j = s; i < count1; i++, j++)
				pBuf[i] = p[j];
			for (intptr_t i = 0, j = count1; i < count2; i++, j++)
				pBuf[j] = p[i];
		} else {
			for (intptr_t i = 0, j = s; i < count; i++, j++)
				pBuf[i] = p[j];
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
	_FINLINE RingPtr<T> GetFreeSpacePtr() {
		RingPtr<T> rp(nullptr, 0, nullptr, 0);
		auto t = Tail;
		auto h = Head;
		auto cap = Capacity();
		auto size = Size(h, t, cap);
		if (size == cap)
			return rp;

		if (h <= t) {
			rp.p1 = &Buffer[t];
			rp.n1 = cap - t;
			rp.n2 = h;
			rp.p2 = h ? &Buffer[0] : nullptr;
		} else {
			rp.p1 = &Buffer[t];
			rp.n1 = h - t;
			rp.p2 = nullptr;
			rp.n2 = 0;
		}
		return rp;
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

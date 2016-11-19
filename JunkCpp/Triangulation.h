#pragma once
#ifndef __JUNK_TRIANGULATION_H__
#define __JUNK_TRIANGULATION_H__

#include "Vector.h"
#include <vector>

_JUNK_BEGIN

//! 三角形分割クラス、三角形分割して頂点インデックスを作成する
template<
	class Vtx, //!< 頂点型、内部に2次元以上のベクトルを持っていなければならない
	class V2, //!< 2次元ベクトル型、Vector2 を継承するクラス
	class Ext = ProjectVector2<V2> //!< 頂点から2次元ベクトルを抽出するクラス
>
struct Triangulation {
	typedef typename V2::ValueType ValueType;
	typedef Ext Projecter;

	struct Node {
		V2 Vec2;
		intptr_t Index;
		ValueType Distance2;
		Node* pPrev;
		Node* pNext;
	};

	static _FINLINE intptr_t Side(const V2& v1, const V2& v2) {
		ValueType a = v1(0) * v2(1) - v1(1) * v2(0);
		if (a < ValueType(0))
			return -1;
		if (ValueType(0) < a)
			return 1;
		return 0;
	}

	static _FINLINE intptr_t SideOfNormal(const V2& input1, const V2& input2, const V2& input3) {
		return Side(input1 - input2, input1 - input3);
	}

	static _FINLINE intptr_t PointInTriangle(const V2& p1, const V2& p2, const V2& p3, const V2& check) {
		V2 v1 = check - p1;
		V2 v2 = check - p2;
		V2 v3 = check - p3;
		intptr_t result1 = Side(v1, v2);
		intptr_t result2 = Side(v2, v3);
		intptr_t result3 = Side(v3, v1);
		return result1 == result2 && result2 == result3;
	}

	static _FINLINE intptr_t OtherPointsInside(Node* node) {
		Node* p = node->pNext->pNext;
		V2 p1 = node->Vec2;
		V2 p2 = node->pNext->Vec2;
		V2 p3 = node->pPrev->Vec2;
		while (1) {
			if (PointInTriangle(p1, p2, p3, p->Vec2))
				return 1;
			p = p->pNext;
			if (p == node->pPrev)
				break;
		}
		return 0;
	}

	static _FINLINE Node* GetFarthestPoint(Node* pHead) {
		ValueType distance = 0.0f;
		Node* p = pHead;
		Node* max = pHead;
		while (1) {
			ValueType abs = p->Distance2;
			if (abs > distance) {
				distance = abs;
				max = p;
			}
			p = p->pNext;
			if (p == pHead)
				break;
		}
		return max;
	}

	//! 三角形分割する
	template<class Index> void Do(
		Vtx* pVertices, //!< [in] 頂点配列へのポインタ
		intptr_t nVertices, //!< [in] pVertices が指す頂点数
		std::vector<Index>& triangleIndices //!< [out] 三角形を構成する頂点インデックス配列が返る
	) {
		if (nVertices < 3)
			return;

		std::vector<Node> nodes(nVertices);
		Node* pLast = &nodes[0];
		for (intptr_t i = nVertices - 1; i != -1; i--) {
			Node* pNode = &nodes[i];
			pNode->Vec2 = Projecter::Project(pVertices[i]);
			pNode->Index = i;
			pNode->Distance2 = pNode->Vec2.LengthSquare();
			pNode->pNext = pLast;
			pLast->pPrev = pNode;
			pLast = pNode;
		}
		Node* pHead = pLast;
		intptr_t counter = nVertices;

		while (counter > 2) {
			Node* start = GetFarthestPoint(pHead);
			Node* p = pHead;
			intptr_t way = SideOfNormal(start->Vec2, start->pPrev->Vec2, start->pNext->Vec2);
			p = start;
			ibool ok = false;
			while (1) {
				if (way == SideOfNormal(p->Vec2, p->pPrev->Vec2, p->pNext->Vec2)) {
					if (!OtherPointsInside(p)) {
						Node tp = *p;
						counter--;
						if (p->pPrev == p || p->pNext == p) {
							pHead = NULL;
						} else {
							if (p == pHead)
								pHead = p->pNext;
							p->pNext->pPrev = p->pPrev;
							p->pPrev->pNext = p->pNext;
						}
						triangleIndices.push_back(tp.Index);
						triangleIndices.push_back(tp.pNext->Index);
						triangleIndices.push_back(tp.pPrev->Index);
						ok = true;
						break;
					}
				}
				p = p->pNext;
				if (p == start)
					break;
			}
			if (!ok)
				break;
		}
	}
};

_JUNK_END

#endif

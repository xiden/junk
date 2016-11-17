// Geo.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include "../../../JunkCpp/Geo.h"

using namespace jk;

int main() {
	Vector3d p1(0, 0, 0);
	Vector3d v1(10, 10, 10);
	Vector3d p2(-5, -5, 5);
	Vector3d v2(10, 10, 1);

	double t1, t2;
	if (Geo::LineNearestParam(p1, v1, p2, v2, t1, t2)) {
		std::cout << "t1=" << t1 << std::endl;
		std::cout << "t2=" << t2 << std::endl;
	} else {
		std::cout << "平行" << std::endl;
	}

	return 0;
}

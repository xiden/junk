// Geo.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <numeric>
#include <vector>
#include "../../../JunkCpp/Geo.h"
#include "../../../JunkCpp/Matrix.h"

using namespace jk;


int main() {
	//auto a = 12.0f;
	//auto b = a + std::numeric_limits<decltype(a)>().epsilon();
	//std::cout << "a==b: " << (a == b) << std::endl;
	//std::cout << std::abs(-123ll) << std::endl;
	//std::cout << "double min=" << std::numeric_limits<float>().epsilon() << std::endl;
	//std::cout << "double epsilon=" << std::numeric_limits<float>().epsilon() << std::endl;
	//std::cout << sizeof(decltype(1 + 1ll)) << std::endl;

	std::vector<jk::Vector2f> vts{
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
	};
	auto pointToCheck = jk::Vector2f(1.0f, 1.0f);
	std::cout << "Polygon2Area=" << jk::Geo::Polygon2Area(&vts[0], vts.size()) << std::endl;
	std::cout << "pointToCheck=" << pointToCheck.X() << ", " << pointToCheck.Y() << std::endl;
	std::cout << "PointInPolygon2=" << jk::Geo::PointInPolygon2(pointToCheck, &vts[0], vts.size(), true) << std::endl;
	std::cout << "PointTouchPolygon2=" << jk::Geo::PointTouchPolygon2(pointToCheck, &vts[0], vts.size(), true) << std::endl;

	{
		// ３次元ベクトルでライン同士の最近点パラメータを求める
		Vector3d p1(0, 0, 0);
		Vector3d v1(10, 10, 10);
		Vector3d p2(-5, -5, 5);
		Vector3d v2(10, 10, 1);

		double t1, t2;
		std::cout << "========" << std::endl;
		if (Geo::LineNearestParam(p1, v1, p2, v2, t1, t2)) {
			std::cout << "t1=" << t1 << std::endl;
			std::cout << "t2=" << t2 << std::endl;
		} else {
			std::cout << "平行" << std::endl;
		}
	}

	{
		// ２次元ベクトルでライン同士の最近点パラメータを求めると交点のパラメータになる
		Vector2f p1(10.0f, 0.0f);
		Vector2f v1(10.0f, 0.0f);
		Vector2f p2(10.0f, 10.0f + std::numeric_limits<float>().epsilon());
		Vector2f v2(0.0f, -10.0f);

		float t1, t2;
		std::cout << "========" << std::endl;
		if (Geo::LineNearestParam(v1, p2 - p1, v2, t1, t2)) {
			std::cout << "t1=" << t1 << std::endl;
			std::cout << "t2=" << t2 << std::endl;
		} else {
			std::cout << "平行" << std::endl;
		}
	}

	return 0;
}

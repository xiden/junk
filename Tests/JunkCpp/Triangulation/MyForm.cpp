#include "MyForm.h"

#pragma unmanaged
#include "../../../JunkCpp/Triangulation.h"

void TestTriangulation(std::vector<int>& indices) {
	std::vector<jk::Vector2d> points;
	points.push_back(jk::Vector2d(0, 0));
	points.push_back(jk::Vector2d(100, 0));
	points.push_back(jk::Vector2d(100, 100));

	jk::Triangulation<jk::Vector2d, jk::Vector2d, jk::ExtractVector2FromVectorN<jk::Vector2d>> t;
	t.Do(&points[0], points.size(), indices);
}
#pragma managed

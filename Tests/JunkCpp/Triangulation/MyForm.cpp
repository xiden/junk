#include "MyForm.h"

std::vector<jk::Vector2f> UnmanagedPolygon;

#pragma unmanaged
int PtInPolygon(int x, int y) {
	return (int)jk::Geo::PointTouchPolygon2(jk::Vector2f(x, y), &UnmanagedPolygon[0], UnmanagedPolygon.size(), true);
}

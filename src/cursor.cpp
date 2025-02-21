#include "cursor.h"

void Cursor::Update(double nx, double ny) {
	dx = nx - x;
	dy = y - ny;

	x = nx;
	y = ny;
}

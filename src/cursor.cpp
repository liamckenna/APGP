#include "cursor.h"
#include <math.h>
void Cursor::Update(double nx, double ny) {
	dx = (nx - x) / 100.f;
	dy = (y - ny) / 100.f;

	if (fabs(dx) <= 0.001f) dx = 0.f;
	if (fabs(dy) <= 0.001f) dy = 0.f;

	x = nx;
	y = ny;
}

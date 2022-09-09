#include "vect3.h"
#include <math.h>

void
vect3_from_lat_lng(const LatLng *coord, Vect3 *vect)
{
	vect->x = cos(coord->lat) * cos(coord->lng);
	vect->y = cos(coord->lat) * sin(coord->lng);
	vect->z = sin(coord->lat);
}

void
vect3_to_lat_lng(const Vect3 *vect, LatLng *coord)
{
	coord->lng = atan2(vect->y, vect->x);
	coord->lat = asin(vect->z);
}

void
vect3_normalize(Vect3 *vect)
{
	double len = sqrt(vect->x * vect->x + vect->y * vect->y + vect->z * vect->z);
	if (len > 0) {
		vect->x = vect->x / len;
		vect->y = vect->y / len;
		vect->z = vect->z / len;
	} else {
		vect->x = 0;
		vect->y = 0;
		vect->z = 0;
	}
}

void
vect3_cross(const Vect3 *vect1, const Vect3 *vect2, Vect3 *prod)
{
	prod->x = vect1->y * vect2->z - vect1->z * vect2->y;
	prod->y = vect1->z * vect2->x - vect1->x * vect2->z;
	prod->z = vect1->x * vect2->y - vect1->y * vect2->x;
}

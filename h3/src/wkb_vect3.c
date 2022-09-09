/*
 * Copyright 2022 Bytes & Brains
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wkb_vect3.h"
#include <math.h>

void
vect3_from_lat_lng(const LatLng * coord, Vect3 * vect)
{
	vect->x = cos(coord->lat) * cos(coord->lng);
	vect->y = cos(coord->lat) * sin(coord->lng);
	vect->z = sin(coord->lat);
}

void
vect3_to_lat_lng(const Vect3 * vect, LatLng * coord)
{
	coord->lng = atan2(vect->y, vect->x);
	coord->lat = asin(vect->z);
}

void
vect3_normalize(Vect3 * vect)
{
	double		len = sqrt(vect->x * vect->x + vect->y * vect->y + vect->z * vect->z);

	if (len > 0)
	{
		vect->x = vect->x / len;
		vect->y = vect->y / len;
		vect->z = vect->z / len;
	}
	else
	{
		vect->x = 0;
		vect->y = 0;
		vect->z = 0;
	}
}

void
vect3_cross(const Vect3 * vect1, const Vect3 * vect2, Vect3 * prod)
{
	prod->x = vect1->y * vect2->z - vect1->z * vect2->y;
	prod->y = vect1->z * vect2->x - vect1->x * vect2->z;
	prod->z = vect1->x * vect2->y - vect1->y * vect2->x;
}

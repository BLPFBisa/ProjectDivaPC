#pragma once

#define _USE_MATH_DEFINES

#include <d3dx9math.h>
#include <math.h>

FLOAT DistanceLine2Point(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir, const D3DXVECTOR3 &point)
{
	D3DXVECTOR3 dir2 = point-eye;
	D3DXVECTOR3 vertical, normal;
	D3DXVec3Cross(&vertical, &dir, &dir2);
	D3DXVec3Cross(&normal, &vertical, &dir);
	vertical = point-eye;
	return abs(D3DXVec3Dot(&normal, &vertical)/D3DXVec3Length(&normal));
}

FLOAT AngleBetween2Vectors(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2)
{
	FLOAT val_cos = D3DXVec3Dot(&v1, &v2)/(D3DXVec3Length(&v1)*D3DXVec3Length(&v2));
	return acos(val_cos);
}

bool LineIntersectSphere(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir, const D3DXVECTOR3 &point, FLOAT radius)
{
	FLOAT distance = DistanceLine2Point(eye, dir, point);
	return distance<radius;
}

bool LineIntersectTriangle(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir,
	const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2, const D3DXVECTOR3 &p3)
{
	D3DXVECTOR3 v1 = p1-eye, v2 = p2-eye, v3 = p3-eye;
	D3DXVECTOR3 n1, n2, n3;
	D3DXVec3Cross(&n1, &v1, &v2);
	D3DXVec3Cross(&n2, &v2, &v3);
	D3DXVec3Cross(&n3, &v3, &v1);
	FLOAT d1=D3DXVec3Dot(&dir, &n1), d2=D3DXVec3Dot(&dir, &n2), d3=D3DXVec3Dot(&dir, &n3);
	return (d1>=0&&d2>=0&&d2>=0)||(d1<=0&&d2<=0&&d3<=0);
}

void ScreenXY2Ray(	D3DXVECTOR3 &outDirv, const D3DXVECTOR3 &eyev,
					const D3DXVECTOR3 &centerv, const D3DXVECTOR3 &upv,
					int mouse_hit_x, int mouse_hit_y, int Width, int Height, float Yangle)
{	// compute direction ray vector from screen mouse XY, -1<=x,y<=1
	float x = float(mouse_hit_x-Width*0.5f)/(Width*0.5f);
	float y = -float(mouse_hit_y-Height*0.5f)/(Height*0.5f);

	D3DXVECTOR3 dirv = centerv-eyev;
	D3DXVECTOR3 rightv_norm, upv_norm;

	D3DXVec3Normalize(&dirv, &dirv);

	// obtain right unit vector
	D3DXVec3Cross(&rightv_norm, &dirv, &upv);

	// obtain up unit vector
	D3DXVec3Cross(&upv_norm, &rightv_norm, &dirv);

	// vertical and horizontal maximum
	float vert_dist = tan(Yangle*0.5f)*D3DXVec3Length(&dirv);
	float hori_dist = vert_dist*Width/Height;

	outDirv = dirv + rightv_norm*hori_dist*x + upv_norm*vert_dist*y;
}



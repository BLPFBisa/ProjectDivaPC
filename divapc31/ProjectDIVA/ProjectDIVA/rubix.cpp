#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "rubix.h"
#include "resource.h"
#include "vect_sse.h"
#include "d3dmath.hpp"
#include "base.h"
#include "defines.h"

float Cube::radius = 1;

Cube::Cube(float *center, float radius, DWORD side_mask, int posi)
// side_mask: bit 012, XYZ start side visible; bit 345, XYZ end side visible
{
	Cube::radius = radius;
	this->center = center;
	this->center_orig = center;

	// initially dir=0
	dir = axes[0];
	posi_orig = posi;

	{	// Set the 8 vertex coordinates
		float X=center[0], Y=center[1], Z=center[2];
		Vertex vert8[8] = {
			{X-radius,Y-radius,Z-radius}, {X+radius,Y-radius,Z-radius},
			{X+radius,Y+radius,Z-radius}, {X-radius,Y+radius,Z-radius},

			{X-radius,Y-radius,Z+radius}, {X+radius,Y-radius,Z+radius},
			{X+radius,Y+radius,Z+radius}, {X-radius,Y+radius,Z+radius},
		};
		memcpy(vertices_orig, vert8, sizeof(vert8));
	}

	{	// add faces according to side_mask
		int indices[] = {0,1,2,0,2,3};
		DWORD alpha = ((int)(255*g_alpha+0.5f))<<24;
		for(int x=0, mask=1; x<6; x++, mask<<=1)	// for each of the 6 faces
		{
			if(!(side_mask&mask))
				continue;
			for(int i=0; i<6; i++)
			{
				int y = indices[i];
				Vertex &vert = vertices_orig[g_face_index[x][y]];
				vert.color = side_colors[x] | alpha;
				vert.tex_xy[0] = g_text_coord[y][0];
				vert.tex_xy[1] = g_text_coord[y][1];
				vertex_buffer_prev.push_back(vert);
			}
		}
	}

	// duplicate vertex buffer
	vertex_buffer_orig = vertex_buffer_now = vertex_buffer_prev;
}
Cube::~Cube(){}

void Cube::highlight(bool mark)
{
	DWORD alpha = mark?0xff000000:(((int)(255*g_alpha+0.5f))<<24);
	for(int x=0; x<vertex_buffer_now.size(); x++){
		DWORD &color = vertex_buffer_now[x].color;
		color = (color&0x00ffffff)|alpha;
	}
}

float Cube::IntersectDist(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir) const
{	// if intersect, return distance from intersect point to eye, otherwise, return -1
	for(int x=0,X=vertex_buffer_now.size(); x<X; x+=3){
		D3DXVECTOR3 &p1 = *(D3DXVECTOR3*)vertex_buffer_now[x].xyz;
		D3DXVECTOR3 &p2 = *(D3DXVECTOR3*)vertex_buffer_now[x+1].xyz;
		D3DXVECTOR3 &p3 = *(D3DXVECTOR3*)vertex_buffer_now[x+2].xyz;
		if(LineIntersectTriangle(eye, dir, p1, p2, p3)){
			D3DXVECTOR3 v = (p1+p2+p3)*0.333333f-eye;
			return D3DXVec3Length(&v);
		}
	}
	return -1;
}

void Cube::rotate(D3DXMATRIX &mat)
{
	rotate_angle(mat);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&center, (D3DXVECTOR3*)&center, &mat);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&dir, (D3DXVECTOR3*)&dir, &mat);
	vertex_buffer_prev = vertex_buffer_now;
}

void Cube::rotate_angle(D3DXMATRIX &mat)
{
	for(int x=0,X=vertex_buffer_now.size(); x<X; x++)
		D3DXVec3TransformCoord((D3DXVECTOR3*)&vertex_buffer_now[x], (D3DXVECTOR3*)&vertex_buffer_prev[x], &mat);
}

void Cube::Reset()
{
	center = center_orig;
	vertex_buffer_prev = vertex_buffer_now = vertex_buffer_orig;
	dir = axes[0];
}

int Cube::getDir()
{
	vector <float> dists;
	for(int x=0; x<6; x++){
		D3DXVECTOR3 diffv = axes[x]-dir;
		dists.push_back(D3DXVec3Length(&diffv));
	}
	return std::min_element(dists.begin(), dists.end())-dists.begin();
}


Rubic::Rubic(int order, bool solid)
{
	this->order = order;
	this->solid = solid;
	aslice.resize(order*order);

	int posi = 0;
	float side_size = 1.0f/(order+(order-1)*GAPSIDERATIO);
	float gap_size  = side_size*GAPSIDERATIO;
	float step_size = side_size+gap_size;
	float xyz[3];

	xyz[2] = -0.5f;
	for(int z=0; z<order; z++, xyz[2]+=step_size)
	{
		xyz[1] = -0.5f;
		for(int y=0; y<order; y++, xyz[1]+=step_size)
		{
			xyz[0] = -0.5f;
			for(int x=0; x<order; x++, xyz[0]+=step_size)
			{
				// mask
				DWORD mask = 0;
				int L=0, R=order-1;
				if(solid)
				{
					L=min(x,min(y,z));
					R=R-max(x,max(y,z));
					L=min(L,R);
					R=order-L-1;
				}
				if(x==L) mask |= (1<<LEFTSIDE);
				if(x==R) mask |= (1<<RIGHTSIDE);
				if(y==L) mask |= (1<<BOTTOMSIDE);
				if(y==R) mask |= (1<<TOPSIDE);
				if(z==L) mask |= (1<<BACKSIDE);
				if(z==R) mask |= (1<<FRONTSIDE);

				float center[3] = {xyz[0]+side_size/2, xyz[1]+side_size/2, xyz[2]+side_size/2};
				cubes.push_back(Cube(center, side_size/2, mask, posi++));
			}
		}
	}
}
Rubic::~Rubic(){}
int Rubic::Draw(vector <Vertex> &vertex_buffer)
{
	vertex_buffer.clear();
	for(int x=0; x<cubes.size(); x++){
		auto &vb = cubes[x].vertex_buffer_now;
		vertex_buffer.insert(vertex_buffer.end(), vb.begin(), vb.end());
	}
	return 0;
}

int Rubic::Intersect(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir)
{	// if intersect, return cube index, otherwise, return -1
	D3DXVECTOR3 origin(0,0,0);

	// if it does not intersect with whole cube -> skip, 0.75=0.5^2+0.5^2+0.5^2
	if(!LineIntersectSphere(eye, dir, origin, sqrt(0.75f)))
		return -1;

	// find the closest intersecting cube
	FLOAT radius3 = sqrt(Cube::radius*Cube::radius*3.0f);
	float best_dist = 1e10;
	int	best_index = -1;
	for(int x=0,X=cubes.size(); x<X; x++){
		Cube &cube = cubes[x];
		if(!LineIntersectSphere(eye, dir, cube.center, radius3))
			continue;
		float intersect_dist = cube.IntersectDist(eye, dir);
		if(intersect_dist>=0) if(intersect_dist<best_dist){
			best_dist = intersect_dist;
			best_index = x;
		}
	}

	return best_index;
}

void Rubic::ClearHighlight()
{
	for each(int i in highlighted_indices)
		cubes[i].highlight(false);
	highlighted_indices.clear();
}

void Rubic::RotateSlice(int axis, int slice, int angle)
{	// Rotate a slice by angle degrees, if angle=90, finalize the position
	/*	axis:	0:x-axis, 1:y-axis, 2:z-axis
				3:-x-axis, 4:-y-axis, 5:-z-axis
		slice:	starting from 0:at most negative axis position */
	int order2=order*order;
	int	x,y,z,*px,*py;
	D3DXMATRIX mat_rot;

	D3DXMatrixRotationAxis(&mat_rot, &axes[axis], Degree2Radian(angle));
	SelectSlice(axis%3,slice,&x,&y,&z,&px,&py);

	if(angle>=90){	// finalize new position
		// Save rotated slice on aslice buffer
		if(axis<3){
			for(*px=0;*px<order;(*px)++) for(*py=0;*py<order;(*py)++)
				aslice[(*px)*order+order-1-(*py)] = cubes[z*order2+y*order+x];
		}else{
			for(*px=0;*px<order;(*px)++) for(*py=0;*py<order;(*py)++)
				aslice[(order-1-(*px))*order+(*py)] = cubes[z*order2+y*order+x];
		}
		// Perform rotation and copy back from aslice buffer
		for(*px=0;*px<order;(*px)++) for(*py=0;*py<order;(*py)++){
			Cube &cube = aslice[(*py)*order+(*px)];
			cube.rotate(mat_rot);
			cubes[z*order2+y*order+x] = cube;
		}
		// check alignment
		int n_in_posi = 0;
		for(int x=0; x<cubes.size(); x++){
			if(cubes[x].posi_orig!=x || cubes[x].getDir()!=0)
				break;
			++n_in_posi;
		}
		if(n_in_posi==cubes.size())
			base::ShowMessage("Solved!");
	}else{		// rotate by an angle <90 deg
		for(*px=0;*px<order;(*px)++) for(*py=0;*py<order;(*py)++)
			cubes[z*order2+y*order+x].rotate_angle(mat_rot);
	}
}

void Rubic::SelectSlice(int axis, int slice, int *x, int *y, int *z, int **px, int **py){
	switch(axis){
	case 0:	//X
		*x = slice;		*y = *z = 0;	*px = y;		*py = z;		break;
	case 1:	//Y
		*y = slice;		*x = *z = 0;	*px = z;		*py = x;		break;
	case 2:	//Z
		*z = slice;		*x = *y = 0;	*px = x;		*py = y;		break;
	default:
		assert(false);
	}
}// return 2D-permutation axis

void Rubic::HighlightOne(int id, bool clear)
{
	if(clear)
		ClearHighlight();
	cubes[id].highlight(true);
	highlighted_indices.push_back(id);
}

void Rubic::HighlightSlice(int axis, int slice)
{
	int order2=order*order;
	int	x,y,z,*px,*py;

	ClearHighlight();
	SelectSlice(axis%3,slice,&x,&y,&z,&px,&py);

	// highlight NxN
	for(*px=0;*px<order;(*px)++) for(*py=0;*py<order;(*py)++)
		HighlightOne(z*order2+y*order+x, false);
}

void Rubic::CubeIndex2XYZ(int index, int &x, int &y, int &z)
{
	int order2 = order*order;
	x = index%order;
	y = (index%order2)/order;
	z = index/order2;
}


RubixWindow::RubixWindow(){pRubic=NULL;}

RubixWindow::~RubixWindow(){}

LRESULT RubixWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	LONG	retval = 1;

	switch(uMsg){
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			if(selected_cube_index>=0){
				float dx = mouse_l_x-LOWORD(lParam);
				float dy = mouse_l_y-HIWORD(lParam);
				if(dx*dx+dy*dy>100 && rotation_angle!=90)	// mouse drag for at least 10 screen pixels
					DetectRotation(LOWORD(lParam), HIWORD(lParam));
			}else{
				Xrot	=	Degree2Radian((float)mouse_l_x-LOWORD(lParam));
				Yrot	=	Degree2Radian((float)HIWORD(lParam)-mouse_l_y);
				mouse_l_x =	LOWORD(lParam);
				mouse_l_y =	HIWORD(lParam);
				UpdateMatrix();
			}
		}else if(wParam & MK_RBUTTON){
			distance	*=	1.0f+((float)HIWORD(lParam)-mouse_r_y)/Height;
			viewAngle	*=	1.0f+((float)LOWORD(lParam)-mouse_r_x)/Width;
			mouse_r_x =	LOWORD(lParam);
			mouse_r_y =	HIWORD(lParam);
			UpdateMatrix();
		}else if(wParam & MK_MBUTTON){
			H_shift	-=	(float)mouse_m_x-LOWORD(lParam);
			V_shift	+=	(float)HIWORD(lParam)-mouse_m_y;
			mouse_m_x =	LOWORD(lParam);
			mouse_m_y =	HIWORD(lParam);
			UpdateMatrix();
		}
		break;
	case WM_MOUSEWHEEL:
		Zrot = Degree2Radian(-(float)((short)HIWORD(wParam))/30.0f);
		UpdateMatrix();
		break;
	case WM_LBUTTONDOWN:
		mouse_l_x = LOWORD(lParam);
		mouse_l_y = HIWORD(lParam);
		mouse_button_state |= 1;
		selected_cube_index = DetectCollision(mouse_l_x, mouse_l_y);
		break;
	case WM_LBUTTONUP:
		mouse_button_state &= (0xffffffff^1);
		retval = 1;
		break;
	case WM_RBUTTONDOWN:
		mouse_r_x = LOWORD(lParam);
		mouse_r_y = HIWORD(lParam);
		mouse_button_state |= 2;
		break;
	case WM_RBUTTONUP:
		mouse_button_state &= (0xffffffff^2);
		retval=1;
		break;
	case WM_MBUTTONDOWN:
		mouse_m_x = LOWORD(lParam);
		mouse_m_y = HIWORD(lParam);
		mouse_button_state |= 4;
		break;
	case WM_MBUTTONUP:
		mouse_button_state &= (0xffffffff^4);
		retval=1;
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		Width = LOWORD(lParam);
		Height = HIWORD(lParam);
		UpdateMatrix();
		break;
	case WM_KEYDOWN:
		switch(wParam){
		case VK_F5:
			Init(hWnd, Device);
			break;
		case VK_F6:
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

	return	retval;
}//Window procedure

void RubixWindow::Init(HWND hwnd, IDirect3DDevice9 *pDevice)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	Width = rect.right-rect.left;
	Height = rect.bottom-rect.top;
	Device = pDevice;
	hWnd = hwnd;
	
	if(pRubic)
		delete pRubic;
	pRubic = new Rubic(10, true);

	D3DXCreateTextureFromResource(pDevice, NULL, (char*)IDB_ARROW, &pTex_arrow);
	D3DXCreateTextureFromResource(pDevice, NULL, (char*)IDB_SQUARE, &pTex_square);
	upv	= D3DXVECTOR3(0,1,0);
	eyev = D3DXVECTOR3(0,0,4);
	centerv = D3DXVECTOR3(0,0,0);
	viewAngle	=	Degree2Radian(30.0f);
	distance	=	D3DXVec3Length(&eyev);
	Xrot		=	Yrot	=	Zrot	=	H_shift	=	V_shift	=	0;
	selected_cube_index = -1;
	selected_axis = -1;
	rotation_angle = -1;
	dirty = 2;
	mouse_button_state = 0;
	UpdateMatrix();
}
D3DXVECTOR3 g_eyev;
void RubixWindow::UpdateMatrix(){
	D3DXVECTOR3	newupv, neweyev, newrightv, newcenterv;
	D3DXVECTOR3	rightv;
	D3DXMATRIX view_mat, mat;

	// obtain rightv w.r.t origin
	D3DXVec3Cross(&rightv, &upv, &eyev);
	D3DXVec3Normalize(&rightv, &rightv);

	// shift centerv according to H_shift and V_shift
	newupv = upv*(V_shift*distance/Width);
	newrightv = rightv*(H_shift*distance/Height);
	centerv += (newupv-newrightv);

	// obtain rotation matrix w.r.t origin
	D3DXMatrixRotationAxis(&view_mat, &upv, Xrot);
	D3DXMatrixRotationAxis(&mat, &rightv, -Yrot);
	view_mat = mat*view_mat;
	D3DXMatrixRotationAxis(&mat, &eyev, Zrot);
	view_mat = mat*view_mat;

	// rotate eyev w.r.t origin
	D3DXVec3TransformCoord(&neweyev, &eyev, &view_mat);
	D3DXVec3TransformCoord(&newcenterv, &centerv, &view_mat);
	D3DXVec3TransformCoord(&newupv, &upv, &view_mat);

	// set eyev distance from origin
	g_eyev = eyev = neweyev*(distance/D3DXVec3Length(&neweyev));
	D3DXVec3Normalize(&upv, &newupv);
	centerv = newcenterv;

	// set matrices
	D3DXMatrixLookAtRH( &viewMat, &eyev, &centerv, &upv );
	D3DXMatrixPerspectiveFovRH( &projMat, viewAngle, (float)Width/Height, 1.0f, 100.0f);
	Xrot = Yrot = Zrot = H_shift = V_shift = 0;

	dirty = max(dirty,1);
}

void RubixWindow::Update(double time_ms)
{
	if(rotation_angle!=-1 && mouse_button_state==0)
	{
		rotation_angle += time_ms*g_rotate_speed*0.001f*(rotation_angular_velocity>=0?1:-1);
		rotation_angle = min(rotation_angle, 90);
		rotation_angle = max(rotation_angle, 0);
		pRubic->RotateSlice(selected_axis, selected_XYZ[selected_axis%3], rotation_angle);
		if(rotation_angle==0 || rotation_angle>=90){
			rotation_angle = -1;
			selected_axis = -1;
		}

		dirty = max(dirty, 2);
	}
}

int distFunc(const void *lhs, const void *rhs)
{
	Vertex *pL = (Vertex*)lhs;
	Vertex *pR = (Vertex*)rhs;
	D3DXVECTOR3 L = (*(D3DXVECTOR3*)pL[0].xyz + *(D3DXVECTOR3*)pL[1].xyz + *(D3DXVECTOR3*)pL[2].xyz)*0.333333f-g_eyev;
	D3DXVECTOR3 R = (*(D3DXVECTOR3*)pR[0].xyz + *(D3DXVECTOR3*)pR[1].xyz + *(D3DXVECTOR3*)pR[2].xyz)*0.333333f-g_eyev;
	return D3DXVec3Length(&L)>=D3DXVec3Length(&R)?-1:1;
}
void RubixWindow::SortVertices(vector <Vertex> &vertex_buffer)
{
	qsort(vertex_buffer.data(), vertex_buffer.size()/3, sizeof(Triangle), distFunc);
}

void RubixWindow::Draw()
{
	// prepare DX states
	Device->SetTransform(D3DTS_VIEW, &viewMat);
	Device->SetTransform(D3DTS_PROJECTION, &projMat);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	Device->SetTexture(0, pTex_arrow);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_GAUSSIANQUAD);
	Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	if(dirty>=2)
		pRubic->Draw(vertex_buffer);
	if(dirty>=1)
		SortVertices(vertex_buffer);
	dirty = 0;
	Device->SetFVF(Vertex_format);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, vertex_buffer.size()/3, vertex_buffer.data(), sizeof(Vertex));
}

void RubixWindow::ComputeRay(D3DXVECTOR3 &outDirv, float x, float y)
{	// compute direction ray vector from screen mouse XY, -1<=x,y<=1
	D3DXVECTOR3 dirv = centerv-eyev;
	D3DXVECTOR3 rightv_norm, upv_norm;

	D3DXVec3Normalize(&dirv, &dirv);

	// obtain right unit vector
	D3DXVec3Cross(&rightv_norm, &dirv, &upv);

	// obtain up unit vector
	D3DXVec3Cross(&upv_norm, &rightv_norm, &dirv);

	// vertical and horizontal maximum
	float vert_dist = tan(viewAngle*0.5f)*D3DXVec3Length(&dirv);
	float hori_dist = vert_dist*Width/Height;

	outDirv = dirv + rightv_norm*hori_dist*x + upv_norm*vert_dist*y;
}

int RubixWindow::DetectRotation(int mouse_hit_x, int mouse_hit_y)
{
	// compute mouse hit ray
	D3DXVECTOR3 dirv, rotatev;
	ScreenXY2Ray(dirv, eyev, centerv, upv, mouse_hit_x, mouse_hit_y, Width, Height, viewAngle);
	
	// obtain rotation axis together with last hit ray
	D3DXVec3Cross(&rotatev, &dirv, &last_hit_dirv);

	if(selected_axis==-1)
	{// compute the angle between rotation axis and +X,+Y,+Z,-X,-Y,-Z; the one with minimal angle is the wanted
		vector <float> angles;
		for(int x=0; x<6; x++)
			angles.push_back(AngleBetween2Vectors(rotatev, axes[x]));
		selected_axis = std::min_element(angles.begin(),angles.end())-angles.begin();
	}

	// compute angle of rotation
	float new_rotation_angle = Radian2Degree(AngleBetween2Vectors(dirv, last_hit_dirv))*2;
	rotation_angular_velocity = new_rotation_angle-rotation_angle;
	rotation_angle = min(new_rotation_angle, 90.0f);

	// highlight the slice
	pRubic->CubeIndex2XYZ(selected_cube_index, selected_XYZ[0], selected_XYZ[1], selected_XYZ[2]);
	pRubic->HighlightSlice(selected_axis, selected_XYZ[selected_axis%3]);
	pRubic->RotateSlice(selected_axis, selected_XYZ[selected_axis%3], rotation_angle);

	dirty = max(dirty, 2);

	return selected_axis;
}

int RubixWindow::DetectCollision(int mouse_hit_x, int mouse_hit_y)
{
	// compute mouse hit ray
	ScreenXY2Ray(last_hit_dirv, eyev, centerv, upv, mouse_hit_x, mouse_hit_y, Width, Height, viewAngle);

	// check intersect
	int cube_id = pRubic->Intersect(eyev, last_hit_dirv);

	if(cube_id>=0){
		pRubic->HighlightOne(cube_id);
		rotation_angle = -1;
		string s = "posi="+ToString(cube_id) +" posi_orig="+ToString(pRubic->cubes[cube_id].posi_orig);
		base::ShowMessage(s);
	}


	dirty = max(dirty, 2);

	return cube_id;
}


#pragma once

#define _USE_MATH_DEFINES

// Main Rubic cube data
#include <vector>
#include <wtypes.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <math.h>

using namespace std;

#define DEFAULTORDER	3

// Graphic data
#define GAPSIDERATIO	0.1

// Standard RGB colors
#define	RED		0x00ff0000
#define	GREEN	0x0000ff00
#define	BLUE	0x000000ff
#define	YELLOW	0x00ffff00
#define	ORANGE	0x0000ffff
#define	PURPLE	0x00ff00ff

#define	RED1	0x00010000
#define	GREEN1	0x00000100
#define	BLUE1	0x00000001
#define	YELLOW1	0x00010100
#define	ORANGE1	0x00000101
#define	PURPLE1	0x00010001
#define	WHITE1	0x00010101

// Definition of 4 orientations
enum DIR {UP, RIGHT, DOWN, LEFT, DIR_N};

// Definition of 6 faces
enum SIDE {RIGHTSIDE, TOPSIDE, FRONTSIDE, LEFTSIDE, BOTTOMSIDE, BACKSIDE, SIDE_N};

// Definition of 6 axis
static D3DXVECTOR3 axes[6]={
	D3DXVECTOR3(1,0,0),	D3DXVECTOR3(0,1,0),	D3DXVECTOR3(0,0,1),
	D3DXVECTOR3(-1,0,0),D3DXVECTOR3(0,-1,0),D3DXVECTOR3(0,0,-1),
};

static DWORD Vertex_format = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
static DWORD side_colors[6] = {RED,GREEN,BLUE,YELLOW,ORANGE,PURPLE};
static char g_face_index[6][4]={
	{5,1,2,6}, //RIGHTSIDE
	{7,6,2,3}, //TOPSIDE
	{4,5,6,7}, //FRONTSIDE
	{0,4,7,3}, //LEFTSIDE
	{5,4,0,1}, //BOTTOMSIZE
	{1,0,3,2}, //BACKSIDE
};
static float g_text_coord[4][2]={{1,1},{0,1},{0,0},{1,0}};
static float g_alpha = 0.6f;
static float g_rotate_speed = 180; // degrees per second
static float g_min_angle_continue_rotate = 10;

static float Degree2Radian(float deg){	return deg*(M_PI/180.0f);}
static float Radian2Degree(float rad){	return 180*(rad/M_PI);}


class Vertex{
public:
	float xyz[3];
	DWORD color;
	float tex_xy[2];
};

class Triangle{
public:
	Vertex verts[3];
};

class Cube{
public:
	static FLOAT radius;
	D3DXVECTOR3 center, center_orig;
	D3DXVECTOR3 dir;

	Vertex vertices_orig[8];				// the 8 vertices in original coordinate
	vector <Vertex> vertex_buffer_orig;		// original coordinate
	vector <Vertex> vertex_buffer_prev;		// previous coordinate
	vector <Vertex> vertex_buffer_now;		// new coordinate
	int		posi_orig;	// original position  index in rubix cube

	void rotate(D3DXMATRIX &trans);
	void rotate_angle(D3DXMATRIX &trans);
	void highlight(bool mark=true);

	// side_mask: bit 012, XYZ start side visible; bit 345, XYZ end side visible
	Cube(float *center, float radius, DWORD side_mask, int posi_index);
	Cube(){}
	~Cube();

	// if intersect, return distance from intersect point to eye, otherwise, return -1
	float	IntersectDist(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir) const;
	void	Reset();
	int		getDir();
};

class Rubic{
public:
	int		order;
	bool	solid;
	vector	<Cube> cubes, aslice;
	vector	<int> highlighted_indices;

	Rubic(int order=DEFAULTORDER, bool solid=false);
	~Rubic();
	int		Draw(vector <Vertex> &vertex_buffer);

	// if intersect, return cube index, otherwise, return -1
	int		Intersect(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &dir);
	void	HighlightOne(int id, bool clear=true);
	void	ClearHighlight();
	void	RotateSlice(int axis, int slice, int angle);	// angle in degrees
	void	SelectSlice(int axis, int slice, int *x, int *y, int *z, int **px, int **py);
	void	HighlightSlice(int axis, int slice);
	void	CubeIndex2XYZ(int index, int &x, int &y, int &z);
};

class RubixWindow{
	IDirect3DDevice9 *Device;
	HWND	hWnd;

	DWORD	mouse_button_state;	// bit0:left, 1:right, 2:middle
	WORD	mouse_l_x, mouse_l_y;
	WORD	mouse_r_x, mouse_r_y;
	WORD	mouse_m_x, mouse_m_y;
	float	distance, viewAngle;
	float	Xrot, Yrot, Zrot;
	float	H_shift, V_shift;
	float	rotation_angle;		// -1: not rotated, 0~90: rotation angle
	float	rotation_angular_velocity;
	int		texture_name;
	int		Width, Height;
	int		BMP_width;
	int		selected_cube_index;
	int		selected_axis;
	int		selected_XYZ[3];
	int		dirty;
	BYTE	*bmpData_arrow, *bmpData_square;
	Rubic	*pRubic;
	IDirect3DTexture9	*pTex_arrow, *pTex_square;
	D3DXVECTOR3		eyev, upv, centerv, last_hit_dirv;
	D3DXMATRIX		viewMat, projMat;
	vector <Vertex> vertex_buffer;

	void	UpdateMatrix();
	BYTE*	m_LoadBitmap(char*,int*,int*);
	bool	m_InitScene(Rubic*);
	void	SortVertices(vector <Vertex> &vertex_buffer);
	void	ComputeRay(D3DXVECTOR3 &pOut, float x, float y);	// -1<=x,y<=1
	int		DetectCollision(int mouse_hit_x, int mouse_hit_y);
	int		DetectRotation(int mouse_hit_x, int mouse_hit_y);

public:
	RubixWindow();
	~RubixWindow();

	LRESULT WndProc(HWND, UINT, WPARAM ,LPARAM);
	void	Init(HWND hwnd, IDirect3DDevice9 *pDevice);
	void	Update(double time_ms);
	void	Draw();
};



	/*
	vector	<float> vCube_indices;
	WORD	*config;
	WORD	*aslice;
	float	*vertex_buf;
	float	*colour_buf;
	float	*normal_buf;
	float	*texture_buf;
	DWORD	vertex_cnt;

	struct CubeModel{
		vector	<float>	vOpaque_vb;
		vector	<float>	vTransp_vb;
		vector	<DWORD>		vOpaque_cb;
	};// there're 26 diff. types of cubicle models for any Rubic cube

	// Member functions
	Rubic(int _order=DEFAULTORDER, WORD _bDirn=0, WORD _bSolid=0){
		if(!N_configs) GenAllConfigs();

		memset(this,0,sizeof(Rubic));
		order = _order;
		bDirn = _bDirn;
		bSolid = _bSolid;
		color[0] = RED1;
		color[1] = GREEN1;
		color[2] = BLUE1;
		color[3] = YELLOW1;
		color[4] = ORANGE1;
		color[5] = PURPLE1;
		color[6] = WHITE1;
		color[7] = WHITE1;

		config	= new WORD [order*order*order];
		aslice	= new WORD [order*order];
		vertex_buf = new float [order*order*order*24*3];
		colour_buf = new float [order*order*order*sizeof(DWORD)];
		normal_buf = new float [order*order*order*3];
		texture_buf= new float [order*order*order*24*2];
		ResetConfig();
	}

	~Rubic(){
		if(config)	delete [] config;
		if(aslice)	delete [] aslice;
		if(vertex_buf)	delete [] vertex_buf;
		if(colour_buf)	delete [] colour_buf;
		if(normal_buf)	delete [] normal_buf;
	}
	void ResetConfig(){
		// Initialize cube indices;
		vCube_indices.resize(order*order*order);
		for(int x=0,X=order*order*order;x<X;x++) vCube_indices[x]=x;

		// Initial all cubicles are in configuration 0
		memset(config,0,order*order*order*sizeof(WORD));

		// Mark surfaces which needs to be drawn w.r.t cubicle space
		for(int i=0,I=(order+1)/2;i<I;i++)
			for(int face=0;face<6;face++) MarkSlice(face,i,i,order-i*2);

		// Reset vertex coordinates
		float	side_size = 1.0f/(order+(order-1)*GAPSIDERATIO);
		float	gap_size  = side_size*GAPSIDERATIO;
		float	step_size = side_size+gap_size;
		float	X,Y,Z;
		register float *pvert = vertex_buf;
		register float *ptext = texture_buf;
		char	face_index[6][4]={
			{4,7,6,5},{0,3,7,4},{3,2,6,7},{1,5,6,2},{0,4,5,1},{0,1,2,3}
		};// vertex start from left-bottom-back, face obey Rubic order
		float	text_coord[4][2]={{0,0},{1,0},{1,1},{0,1}};
		vertex_cnt = 0;
		Z = -0.5f;
		for(int z=0;z<order;z++,Z+=step_size){
			Y = -0.5f;
			for(int y=0;y<order;y++,Y+=step_size){
				X = -0.5f;
				for(int x=0;x<order;x++,X+=step_size){
					// Compute the 8 vertices
					float	corners[8][3]={
						{X,Y,Z},{X+side_size,Y,Z},{X+side_size,Y+side_size,Z},{X,Y+side_size,Z},
						{X,Y,Z+side_size},{X+side_size,Y,Z+side_size},{X+side_size,Y+side_size,Z+side_size},{X,Y+side_size,Z+side_size},
					};
					// Write vertices into vertex buffer
					for(int i=0;i<24;i++,pvert+=3,ptext+=2){
						memcpy(pvert,corners[((char*)face_index)[i]],12);
						memcpy(ptext,text_coord[i%4],8);
						vertex_cnt++;
					}
				}
			}
		}
	}// reset configuration

	void glDraw(){
		glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, vertex_buf );
		glTexCoordPointer( 2, GL_FLOAT, 0, texture_buf );
		glDrawArrays( GL_QUADS, 0, vertex_cnt );
		glPopClientAttrib();
	}
};
*/
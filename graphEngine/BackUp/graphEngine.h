//--------------------------------------------------------------------------------------
// File: GraphEngine.h
//
// Copyright by CK. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef GRAPHENGINE_H
#define GRAPHENGINE_H

#include "defines.h"

#include<string>
#include<map>
using namespace std;

class Graph_Treap_Node
{
public:
	Graph_Treap_Node(string filename);
	~Graph_Treap_Node();
	string _filename;
	int _p;
	Texture *_tex;
	Graph_Treap_Node *_l,*_r;
};

bool Graph_Treap_cmp(Graph_Treap_Node *x,Graph_Treap_Node *y);

class Graph_Treap
{
public:
	Graph_Treap_Node *v;
	Graph_Treap();
	~Graph_Treap();
	
	void clear();

	void del(string filename);
	void del(Texture *tex);
	Texture* ins(string filename);
	Texture* find(string filename);

private:
	Texture* ins(string filename,Graph_Treap_Node *&p);
	Texture* find(string filename,Graph_Treap_Node *p);
	void lr(Graph_Treap_Node *&p);
	void rr(Graph_Treap_Node *&p);
	void del(string filename,Graph_Treap_Node *&p);
	void clear(Graph_Treap_Node *&p);

};

class UserFont
{
public:
	UserFont(string _ftFaceName, int _ftSize, bool _ftItalic = false, DWORD _ftWeight = FW_NORMAL):
	  ftFaceName(_ftFaceName),ftSize(_ftSize),ftItalic(_ftItalic),ftWeight(_ftWeight){}
	UserFont(){ftFaceName="Arial";ftSize=12;ftItalic=false;ftWeight=FW_NORMAL;}
	string ftFaceName;
	int ftSize;
	bool ftItalic;
	DWORD ftWeight;
};


#define graphEngine GraphEngine::Instance()

//-------------------We use the object of this class to draw textures-------------------
class GraphEngine
{
		GraphEngine():_pDevice(NULL),_pSprite(NULL) {}
		GraphEngine(IDirect3DDevice9* pDevice,ID3DXSprite* pSprite);
		~GraphEngine();
	public: 
		void GraphEngine::InitEngine(IDirect3DDevice9* pDevice,ID3DXSprite* pSprite);
		void GraphEngine::ReleaseTex();
		void GraphEngine::ReleaseTex(int Poolnum);
		static GraphEngine& Instance(void) {static GraphEngine instance; return instance;}
		//-------------------------Draw Texture Method----------------------------------------  
		void Draw(Texture *tex, int dx, int dy);					   //Draw Whole Texture
		void Draw(Texture *tex, RECT drawRect);						   //Draw Whole Texture With Scale
		void Draw(Texture *tex, RECT srcRect, int dx,int dy);		   //Draw Part of the Texture
		void Draw(Texture *tex, RECT srcRect, RECT drawRect);		   //Draw Part of the Texture With Scale

		//---------------------DrawMethod With Color------------------------    
		void Draw(Texture *tex, int dx, int dy, D3DCOLOR color);						//Draw Whole colored Texture
		void Draw(Texture *tex, RECT drawRect, D3DCOLOR color);							//Draw Whole colored Texture With Scale
		void Draw(Texture *tex, RECT srcRect, int dx, int dy, D3DCOLOR color);			//Draw Part of the colored Texture
		void Draw(Texture *tex, RECT srcRect, RECT drawRect, D3DCOLOR color);			//Draw Part of the colored Texture With Scale
		//------------------------------------------------------------------------------------

		int _Pooluse;


		//-------------------------------Draw Texts-------------------------------------------
		void DrawText(string text, int dx, int dy);
		void DrawText(string text, int dx, int dy, DWORD dwFlags);
		void DrawText(string text, RECT drawRect);
		void DrawText(string text, RECT drawRect, DWORD dwFlags);
		
		void DrawText(string text, int dx, int dy, D3DXCOLOR color);
		void DrawText(string text, int dx, int dy, DWORD dwFlags, D3DXCOLOR color);
		void DrawText(string text, RECT drawRect, D3DXCOLOR color);
		void DrawText(string text, RECT drawRect, DWORD dwFlags, D3DXCOLOR color);

		void DrawText(int dx, int dy, LPCSTR strMsg, ...);
		void DrawText(int dx, int dy, DWORD dwFlags, LPCSTR strMsg, ...);
		void DrawText(RECT drawRect, LPCSTR strMsg, ...);
		void DrawText(RECT drawRect, DWORD dwFlags, LPCSTR strMsg, ...);

		void DrawText(int dx, int dy, D3DXCOLOR color, LPCSTR strMsg, ...);
		void DrawText(int dx, int dy, DWORD dwFlags, D3DXCOLOR color, LPCSTR strMsg, ...);
		void DrawText(RECT drawRect, D3DXCOLOR color, LPCSTR strMsg, ...);
		void DrawText(RECT drawRect, DWORD dwFlags, D3DXCOLOR color, LPCSTR strMsg, ...);

		void SetFont(string ftFaceName, int ftSize, bool ftItalic = false, DWORD ftWeight = FW_NORMAL);
		void SetFontDefaultColor(int a,int r,int g,int b);
		//------------------------------------------------------------------------------------

		//------------------------------Deal With Texture Pool--------------------------------
		Texture* AddTexture(string filename, int Poolnum = -1);		//Add a Texture To the Pool
		void DeleteTexture(string filename, int Poolnum = -1);		//Delete the texture in the Pool

		Texture* findTexture(string filename, bool ForceAdd = true); //Find the address of the giving texture
		//------------------------------------------------------------------------------------
		
		
		//Get the texture's desc
		void GetTextureDesc(string filename, D3DSURFACE_DESC *p_out);
		void GetTextureDesc(Texture* tex,D3DSURFACE_DESC *p_out);
	private:
		ID3DXSprite *_pSprite; //Store the Sprite
		IDirect3DDevice9 *_pDevice; //Store the Device
		ID3DXFont* _pFont;
		D3DCOLOR _ftDefaultColor;
		map<string, map<int, map<bool, map<DWORD, ID3DXFont*>>>> fonts;

		static const int MaxPoolnum = 10;
		
		Graph_Treap *_TexPool[MaxPoolnum];
		
		
};
//-----------------------------------GraphEngine----------------------------------------

#endif
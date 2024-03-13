#include "Base.h"
#include "GameMana.h"
#include "UIScreen.h"

ID3DXFont* Font = 0;



void CreateFont(IDirect3DDevice9 *Device)
{
	D3DXFONT_DESCA lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESCA));

	lf.Height         = 20;    // in logical units
	lf.Width          = 8;    // in logical units       
	lf.Weight         = 500;   // boldness, range 0(light) - 1000(bold)
	lf.Italic         = false;     
	lf.CharSet        = DEFAULT_CHARSET;                    
	lf.Quality        = 0;           
	lf.PitchAndFamily = 0;           
	strcpy(lf.FaceName, "Times New Roman"); // font style

	//
	// Create an ID3DXFont based on 'lf'.
	//

	if(FAILED(D3DXCreateFontIndirect(Device, &lf, &Font)))
	{
		::MessageBox(0, "D3DXCreateFontIndirect() - FAILED", 0, 0);
		::PostQuitMessage(0);
	}
}

void TextOut(string text, int x, int y)
{
	RECT rect = {x, y, base::Width, base::Height};
	Font->DrawText(
		base::Sprite,
			text.data(), 
			-1, // size of string or -1 indicates null terminating string
			&rect,            // rectangle text is to be formatted to in windows coords
			DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
			D3DCOLOR_XRGB(255,255,255));      // black text
}

void base::BaseSet()
{
}

void base::Start(IDirect3DDevice9 *Device)
{
	CreateFont(Device);
	::InitUIScreen();
}

void base::Update(double dwTime)
{
	::GameMainUpdate(dwTime);
}

void base::Draw(IDirect3DDevice9 *Device)
{
	::GameMainDraw();
	/*
	char s[256];
	sprintf(s,"FPS: %d",fps);
	TextOut(s,0,0);
	*/
}

void base::DeleteAll()
{
	::DeleteUIScreen();
}

void base::OnMouseEvent(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, 
						   bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
{
	::GameMainOnMouseEvent(MouseEventValue);
}

void base::OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown)
{
	::GameMainOnKeyEvent(KeyEventValue);
}

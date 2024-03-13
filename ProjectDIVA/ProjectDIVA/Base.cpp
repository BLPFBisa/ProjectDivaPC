#include "Base.h"
#include "GameMana.h"
#include "UIScreen.h"
#include <sstream>
#include "resource.hpp"

extern GameCore core;
extern MainMenu *menu;
extern int ScreenState;
IDirect3DDevice9* ImageResource::pD3D = NULL;
ResourceLoader g_res;
//extern bool bAlarm;
//extern int	n_alarmed;

void drawSP();

//-----------------------base-------------------------
namespace base
{
	IDirect3DDevice9 *Device = NULL;
	ID3DXSprite* Sprite = NULL;
	D3DPRESENT_PARAMETERS d3dpp;
	RECT window_rect, screen_rect;
	HWND hwnd;
	HINSTANCE hInst;
	HHOOK old_keyboard_hook = NULL;
	DWORD keyboard_thread_id = 0;
	char Name[] = "Project DIVA PC HD03";

	//For timing
	static LONGLONG lastTime = GetCurrentCount();
	LONGLONG currTime  = GetCurrentCount();
	double SECOND = 1000.0;
	double FPS = 60, timeDelta = SECOND/FPS;

	//For debug and test
	DWORD focus_time = 0;
	int totalFramesDrawn = 1;
	double meanTimeInterval = 0;
	LONGLONG beginTime = GetCurrentCount();

	//�����϶����
	RECT WindowCurPos, WindowDeltaPos; //
	POINT MousePrePos, MouseCurPos;
	bool bNcLButtonDown = false;

	LRESULT CALLBACK LowLevelKeyboardProc ( int code, WPARAM wParam, LPARAM lParam ){
		if( GetForegroundWindow()!=hwnd || code<0 )
			return CallNextHookEx(old_keyboard_hook,code,wParam,lParam);

		DWORD vkCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
		if( ScreenState==SCREEN_GAMEMAIN )
			if( vkCode>=0x20 && vkCode<=0x5a ){
				OnKeyEvent(vkCode,wParam==WM_KEYDOWN);
				return 1;
			}
		return CallNextHookEx(old_keyboard_hook,code,wParam,lParam);
	}

	DWORD WINAPI KeyboardThreadFunc( void *data ){
		if(!(old_keyboard_hook=SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInst, 0)))
		{	// Global keyboard hook failed, fall back to default
			::MessageBox(hwnd,StringTable(61).c_str(),Name,MB_OK|MB_ICONEXCLAMATION);
			keyboard_thread_id = 0;
			return 0;
		}
		SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);

		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		while(msg.message != WM_QUIT)
		{
			GetMessage(&msg, hwnd, 0, 0);
		}
		UnhookWindowsHookEx(old_keyboard_hook);
		keyboard_thread_id = 0;
		return 0;
	}

	bool Init(	HINSTANCE hInstance, D3DDEVTYPE deviceType )
	{
		hInst = hInstance;
		BaseSet();
		systemIni.Init();
		RECT winrect = SetWindowRect(systemIni.winWidth,systemIni.winHeight,systemIni.windowed);

		if( !InitWindow(hInstance,systemIni.winWidth,systemIni.winHeight,systemIni.windowed) )
			return false;
		if( !InitD3D(systemIni.windowed,window_rect.right,window_rect.bottom,deviceType) )
			return false;

		ImageResource::pD3D = Device;
		g_res = ResourceLoader("gamedata.txt");

		InitTime();
		Start(Device);

		return true;
	}
	bool InitD3D(bool windowed,
		int width, int height,
		D3DDEVTYPE deviceType)
	{
		//
		// Init D3D: 
		//

		HRESULT hr = 0;

		// Step 1: Create the IDirect3D9 object.

		IDirect3D9* d3d9 = 0;
		d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		if( !d3d9 )
		{
			::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
			return false;
		}

		// Step 2: Check for hardware vp.

		D3DCAPS9 caps;
		d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

		int vp = 0;
		if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.

		d3dpp.BackBufferWidth            = windowed?width:GetSystemMetrics(SM_CXSCREEN);
		d3dpp.BackBufferHeight           = windowed?height:GetSystemMetrics(SM_CYSCREEN);
		d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount            = 1;
		d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality         = 0;
		d3dpp.SwapEffect                 = D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_DISCARD; 
		d3dpp.hDeviceWindow              = hwnd;
		d3dpp.Windowed                   = windowed;
		d3dpp.EnableAutoDepthStencil     = true; 
		d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
		d3dpp.Flags                      = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

		// Step 4: Create the device.

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT, // primary adapter
			deviceType,         // device type
			hwnd,               // window associated with device
			vp,                 // vertex processing
			&d3dpp,             // present parameters
			&Device);            // return created device

		if( FAILED(hr) )
		{
			// try again using a 16-bit depth buffer
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

			hr = d3d9->CreateDevice(
				D3DADAPTER_DEFAULT,
				deviceType,
				hwnd,
				vp,
				&d3dpp,
				&Device);

			if( FAILED(hr) )
			{
				d3d9->Release(); // done with d3d9 object
				::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
				return false;
			}
		}

		d3d9->Release(); // done with d3d9 object

		if( FAILED( D3DXCreateSprite( Device, &Sprite ) ) )
			return false;

		return true;
	}
	bool InitWindow(HINSTANCE hInstance,
		int width, int height,
		bool windowed)
	{
		// Create the main application window.
		WNDCLASS wc;

		if(width==0 || height==0 || !windowed){
			width = 480;
			height = 272;
		}

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = (WNDPROC)base::WndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(hInstance, "diva");
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = Name;

		if( !RegisterClass(&wc) ) 
		{
			::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
			return false;
		}

		hwnd = 0;
		hwnd = ::CreateWindow(Name, Name, 
			WS_OVERLAPPEDWINDOW,//WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 
			0, 0, width, height,
			0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

		if( !hwnd )
		{
			::MessageBox(0, "CreateWindow() - FAILED", 0, 0);
			return false;
		}

		//�������ڴ�С-----------------------------------
		RECT window_rect = {0,0,width,height};

		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(hwnd),
			GetMenu(hwnd)!=NULL,
			GetWindowExStyle(hwnd));
		MoveWindow(hwnd,0,0,window_rect.right-window_rect.left,window_rect.bottom-window_rect.top,FALSE);
		//-----------------------------------------------

		//��������λ��(����)-----------------------------
		RECT   screen_rect;  
		SystemParametersInfo(   SPI_GETWORKAREA,   sizeof(RECT),   &screen_rect,   0   );   

		SetWindowPos(  hwnd,NULL,  
			(screen_rect.right-(window_rect.right-window_rect.left))   /   2,  
			(screen_rect.bottom-(window_rect.bottom-window_rect.top))   /   2,  
			0,0,  
			SWP_NOSIZE   |   SWP_SHOWWINDOW   );   
		//------------------------------------------------

		::ShowWindow(hwnd, SW_SHOW);
		::UpdateWindow(hwnd);

		return true;
	}

	bool InitTime()
	{
		LARGE_INTEGER qwTime;
		if (QueryPerformanceFrequency(&qwTime) == FALSE)
			return false;
		SECOND = (double)qwTime.QuadPart;
		timeDelta = SECOND/FPS;
		return true;
	}
	void Release()
	{
		DeleteAll();
		SAFE_RELEASE(Sprite);
		SAFE_RELEASE(Device);
	}

	bool D3DResetDevice(){
		if(base::Device->TestCooperativeLevel() != D3DERR_DEVICENOTRESET) return false;
		Sprite->OnLostDevice();
		if(Sprite->OnResetDevice() != S_OK) return false;
		if(Device->Reset( &d3dpp ) != S_OK) return false;
		return true;
	}

	int GetAspectRatio( int width, int height ){
		double ratio = (double)height/width;
		const static int aspect_ratiosN=7;
		static double aspect_ratios[aspect_ratiosN] = {
			9.0/16.0, 
			10.0/16.0, 
			3.0/4.0, 
			4.0/5.0,
			1.0/2.35,
			1.0/1.85,
			1.0/1.90,
		};
		int x;
		for(x=0; x<aspect_ratiosN; x++)
			if(abs(ratio-aspect_ratios[x])/ratio<0.03) break;
		return x;
	}// 0: 9/16, 1:10/16, 2:3/4, 3:4/5, 4:odd ratio

	fRECT FitWindowRect(fRECT srcRect, fRECT dstRect, bool bCenter){
		float srcRatio = (float)(srcRect.bottom-srcRect.top)/(srcRect.right-srcRect.left);
		if( dstRect.bottom-dstRect.top > (dstRect.right-dstRect.left)*srcRatio ){	// dstRect is too thin
			float space = ((dstRect.bottom-dstRect.top)-(dstRect.right-dstRect.left)*srcRatio)*0.5;
			if(bCenter){
				dstRect.top =  dstRect.top + space;
				dstRect.bottom = dstRect.bottom - space;
			}else
				dstRect.bottom = dstRect.bottom - space*2;
		}else{														// dstRect is too fat
			double space = ((dstRect.right-dstRect.left)-(dstRect.bottom-dstRect.top)/srcRatio)*0.5;
			if(bCenter){
				dstRect.left = dstRect.left + space;
				dstRect.right = dstRect.right - space;
			}else
				dstRect.right = dstRect.right - space*2;
		}
		return dstRect;
	}// modifies dstRect so that srcRect's perspective ratio is preserved

	RECT SetWindowRect(int width, int height, bool windowed){
		if(width==0 || height==0 || !windowed){
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
		}
		RECT winrect = {0,0,width,height};
		screen_rect = window_rect = winrect;
		if(!windowed && GetAspectRatio(width,height)>0){	// Do perspective clipping
			window_rect = MakeRect(FitWindowRect(MakefRect(0,0,16,9), MakefRect(window_rect), false));
			screen_rect = MakeRect(FitWindowRect(MakefRect(0,0,16,9), MakefRect(screen_rect), true));
		}
		return winrect;
	}

	void ResizeWindow(int width, int height){
		if(width==0 || height==0){
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
			SetWindowRect(width,height,false);
			d3dpp.Windowed = false;
		}else{
			RECT winrect = SetWindowRect(width,height,true);
			AdjustWindowRectEx(&winrect,
				GetWindowStyle(hwnd),
				GetMenu(hwnd)!=NULL,
				GetWindowExStyle(hwnd));
			RECT rect;
			GetWindowRect(hwnd,&rect);
			SetWindowPos(hwnd,NULL,
				rect.left,rect.top,
				winrect.right-winrect.left,winrect.bottom-winrect.top,
				SWP_SHOWWINDOW);
			d3dpp.Windowed = true;
		}

		d3dpp.BackBufferWidth = width;
		d3dpp.BackBufferHeight = height;

		/*	There's a bug in Microsoft DirectX SDK that sometimes you need to call
			Device->Reset()	several times in order to reset successfully.	*/
		if(Device->Reset( &d3dpp ) != D3D_OK)
			while(!D3DResetDevice());
	}

	// The following are unneccessary if the original output coordinates are not hardcoded
	RECT MakeDrawRect(int x, int y, int width, int height){
		RECT rect={
			(float)x*window_rect.right/480+0.5f,
			(float)y*window_rect.bottom/272+0.5f,
			(float)(x+width)*window_rect.right/480+0.5f,
			(float)(y+height)*window_rect.bottom/272+0.5f
		};
		return rect;
	}
	fRECT MakeDrawRect(const fRECT &in_rect){
		fRECT rect={
			(float)in_rect.left*window_rect.right/480,
			(float)in_rect.top*window_rect.bottom/272,
			(float)in_rect.right*window_rect.right/480,
			(float)in_rect.bottom*window_rect.bottom/272
		};
		return rect;
	}
	RECT MakeDrawRect(const RECT &in_rect){
		RECT rect={
			(float)in_rect.left*window_rect.right/480+0.5f,
			(float)in_rect.top*window_rect.bottom/272+0.5f,
			(float)in_rect.right*window_rect.right/480+0.5f,
			(float)in_rect.bottom*window_rect.bottom/272+0.5f
		};
		return rect;
	}
	void MakeDrawRect(const fRECT &in_rect, float &left, float &top, float &right, float &bottom){
		left = (float)in_rect.left*window_rect.right/480;
		top = (float)in_rect.top*window_rect.bottom/272;
		right = (float)in_rect.right*window_rect.right/480;
		bottom = (float)in_rect.bottom*window_rect.bottom/272;
	}
	void MakeDrawRect(float &x, float &y){
		x = (float)x*window_rect.right/480;
		y =	(float)y*window_rect.bottom/272;
	}
	D3DXVECTOR2& MakeDrawRect(D3DXVECTOR2 &v){
		v.x *= (float)window_rect.right/480;
		v.y *= (float)window_rect.bottom/272;
		return v;
	}
	//---------------------------------------------------------------------------------------

	string sWindowSize(){
		ostringstream oss;
		oss << window_rect.right << "��" << window_rect.bottom;
		if(!systemIni.windowed)	oss << ' ' << StringTable(36);
		return oss.str();
	}

	/* For future 3D rendering
	void InitDevice()
	{
		Device->SetRenderState( D3DRS_ZENABLE, TRUE );

		Device->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
		Device->SetTransform( D3DTS_PROJECTION, &matProj );

		D3DXVECTOR3 vEyeVec=D3DXVECTOR3(0.0f,0.0f,-1.0f);
		D3DXVECTOR3 vLookatVec=D3DXVECTOR3(0.0f,0.0f,0.0f);
		D3DXVECTOR3 vUpVec=D3DXVECTOR3(0.0f,1.0f,0.0f);

		D3DXMATRIX matView;
		D3DXMatrixLookAtLH( &matView, &vEyeVec, &vLookatVec, &vUpVec );
		Device->SetTransform( D3DTS_VIEW, &matView );
	}
	*/

	bool AutoPause()
	{
		if(!systemIni.autoPause)	// never
			return false;
		if(systemIni.autoPause==1)	// during play only
		{
			if(ScreenState!=SCREEN_GAMEMAIN || GAMEDATA.gamemode!=GAME_MODE_NORMAL)
				return false;
		}
		else if(ScreenState!=SCREEN_GAMEMAIN)	// alway pause
			return false;

		core.PauseGame(true);
		core.menu->ShowMenu();
		return true;
	}

	int MsgLoop()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		while(msg.message != WM_QUIT)
		{
			if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				//����϶����ڻ���������Ĵ���
			/*	if(bNcLButtonDown)
				{
					::GetCursorPos(&MouseCurPos); //��갴�»�ȡ��ǰλ�ã��Ա�ȡ���ֶ�λ��
					::GetWindowRect(::GetActiveWindow(), &WindowCurPos);
					if(MouseCurPos.y < WindowCurPos.top)
					{
						WindowCurPos.left = MouseCurPos.x - WindowDeltaPos.left;
						WindowCurPos.top = MouseCurPos.y - WindowDeltaPos.top;
						//debugf(_T("Current Window Pos X Is %d Y Is %d"),WindowCurPos.left,WindowCurPos.top);
						::SetWindowPos(::GetActiveWindow(), HWND_NOTOPMOST, WindowCurPos.left, WindowCurPos.top, WindowCurPos.right, WindowCurPos.bottom, SWP_NOSIZE);
						MousePrePos = MouseCurPos;
					}
				}*/
				MainLoop();
			}
		}
		if(keyboard_thread_id)
			PostThreadMessage(base::keyboard_thread_id, WM_QUIT, 0, 0);
		return msg.wParam;
	}

	void MainLoop()
	{
		currTime = GetCurrentCount();
		LONGLONG dwTime = currTime-lastTime;
		HRESULT hr;

		if(dwTime>=timeDelta)
		{
			totalFramesDrawn++;

			lastTime = currTime;

			if(!meanTimeInterval)
				meanTimeInterval = dwTime;
			else
				meanTimeInterval = dwTime*0.05+meanTimeInterval*0.95;

			//upDate��֡����
			//while(currTime-lastTime>=timeDelta)
			Update(dwTime);//, lastTime += timeDelta;
			/*
			if(dwTime>timeDelta*4){
				bAlarm = true;
				++n_alarmed;
			}*/

			// Draw all
			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
			Device->BeginScene();
			Sprite->Begin( D3DXSPRITE_ALPHABLEND  );
			Draw(Device);
			Sprite->End();
			Device->EndScene();

			// Flip to screen
			if(systemIni.windowed)	// Render to entire window, allow stretching
				hr = Device->Present(NULL, NULL, 0, 0);
			else
				hr = Device->Present(&window_rect, &screen_rect, 0, 0);

			// Handle device lost
			if( hr == D3DERR_DEVICELOST ){
				if(!core.paused) AutoPause();
				if(!D3DResetDevice()) Sleep(100);
			}

//			if(timeDelta+timeDelta-dwTime>0)
	//			Sleep((timeDelta+timeDelta-dwTime)/base::SECOND*1000);
		}else Sleep(1);
	}
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		if( msg == WM_KEYDOWN || msg == WM_KEYUP )
		{
			OnKeyEvent( (UINT)wParam, msg == WM_KEYDOWN);
			return 0;
		}

		switch( msg )
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			if(GetTickCount()-focus_time < 10) break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
			OnMouseEvent( LOWORD(lParam), HIWORD(lParam), msg, wParam );
			return 0;
			//�����ǹ����϶����ڱ�����Ϣ��ʧ�Ĵ���-------------------------------------------------------------------------------------
		case WM_NCLBUTTONUP: //���̧��ʱ��ǣ���ʾ�����϶����������Ϊfalse
			bNcLButtonDown = false;
			return DefWindowProc(hwnd,msg,wParam,lParam);
		case WM_NCRBUTTONDOWN://�ڱ������ϵ���Ҽ������������ǲ�Ҫ����ϵͳ�˵�
			return 0;
		case WM_NCMOUSELEAVE://�����˱�������ʱ����ҲҪ����Ϊfalse����������޷���λ
			bNcLButtonDown = false;
			return DefWindowProc(hwnd,msg,wParam,lParam);
		case WM_SYSCOMMAND:
			switch( wParam & 0xfff0 )
			{
				///////////////����ֱ�ӷ��أ����������Ϣѭ����������ʹ��Ϸfreese////////////////////
				//������ڱ������ϰ�ס����ʱ��ϵͳ�ᷢ��WM_SYSCOMMAND��Ϣ����DefWindowProc
				//�յ�SC_MOVE�󣬻ᷢ��WM_ENTERSIZEMOVE�����ʱ��������Ϣѭ���ͻ�ֹͣ��ֱ��
				//DefWindowProc������ɷ��أ����������Ϣ���ܽ���ϵͳ����ֱ�ӷ���
				//SC_KEYMENU��Ϣ�ǰ���ݼ���ʱ�������ϵͳ�˵�����ʱ����Ϣѭ��Ҳ��ֹͣ�ģ�ֱ��
				//DefWindowProc������ɷ���
				//SC_MOUSEMENU��Ϣ��������������Ͻ�Сͼ���ǲ�����ϵͳ�˵���ͬ��
			case SC_MOVE:  //�����Ƕ������
				::GetCursorPos(&MouseCurPos); //��갴�»�ȡ��ǰλ�ã��Ա�ȡ���ֶ�λ��
				::GetWindowRect(hwnd, &WindowCurPos);
				WindowDeltaPos.left = MouseCurPos.x - WindowCurPos.left;
				WindowDeltaPos.top = MouseCurPos.y - WindowCurPos.top;
				bNcLButtonDown = true;

				//����ھ��䴰��(Windows Classic Style)ģʽ�£��ղ���WM_NCMOUSELEAVE������
				if(bNcLButtonDown)
				{
					TRACKMOUSEEVENT   MouseEvent;
					MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
					MouseEvent.dwFlags = TME_LEAVE | TME_NONCLIENT;
					MouseEvent.hwndTrack = hwnd;
					MouseEvent.dwHoverTime = 0;
					::TrackMouseEvent(&MouseEvent);
				}
			case SC_KEYMENU:
			case SC_MOUSEMENU:
				return 0;
			}
		case WM_NCHITTEST:
			// Prevent the user from selecting the menu in fullscreen mode.
			////////////��������������λ�����ô���λ��///////////////////////////////////////////////////////
			//�����Ǹ��������Ϣģ�ⴰ���ƶ��Ĺ���
			if(bNcLButtonDown)
			{
				::GetCursorPos(&MouseCurPos);
				::GetWindowRect(hwnd, &WindowCurPos);
				WindowCurPos.left = MouseCurPos.x - WindowDeltaPos.left;
				WindowCurPos.top = MouseCurPos.y - WindowDeltaPos.top;
				::SetWindowPos(hwnd, HWND_NOTOPMOST, WindowCurPos.left, WindowCurPos.top, WindowCurPos.right, WindowCurPos.bottom, SWP_NOSIZE);
				MousePrePos = MouseCurPos;
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			return DefWindowProc(hwnd,msg,wParam,lParam);
			//---------------------------------------------����----------------------------------------------------------------------

		case WM_SETFOCUS:
			focus_time = GetTickCount();
			return 0;

		case WM_KILLFOCUS:
			if(!core.paused) AutoPause();
			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
//-----------------------base-----------------------------


int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	//test();
	ResourceLoader a("a.txt");
	if(!base::Init(hinstance, D3DDEVTYPE_HAL))
	{
		::MessageBox(0, "Init() - FAILED", 0, 0);
		return 0;
	}

	//if(!Setup())
	//{
	//	::MessageBox(0, "Setup() - FAILED", 0, 0);
	//	return 0;
	//}

	base::MsgLoop();

	//Cleanup();

	base::Release();

	return 0;
}

/*
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
void drawSP(){
	static Texture * particle_texture  =NULL;
	if(!particle_texture)
		D3DXCreateTextureFromFile(Device,"pic\\particle.png",&particle_texture);
	static float *g_pVB=NULL;
	if(!g_pVB){
		g_pVB= new float [100*4];
		for(int x=0;x<100;x++){
			for(int y=0;y<2;y++)g_pVB[x*4+y]=(rand()*200.0f/RAND_MAX)-100.0f;
			g_pVB[x*4+2]=0.0f;
			*(DWORD*)&g_pVB[x*4+3]=RandomColor();
		}
	}
	//Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0, 1.0f), 1.0f, 0);
	//Device->BeginScene();
	{
		HRESULT hr=S_OK;
		D3DXMATRIX proj, old_view, old_proj, old_world;
		IDirect3DStateBlock9 *sb;

		base::Sprite->Flush();
		base::Device->CreateStateBlock(D3DSBT_ALL,&sb );
		base::Device->GetTransform( D3DTS_VIEW, &old_view );
		base::Device->GetTransform( D3DTS_PROJECTION, &old_proj );
		base::Device->GetTransform( D3DTS_WORLD, &old_world );

		D3DXMatrixIdentity(&proj);
		base::Device->SetTransform( D3DTS_WORLD, &proj );
		base::Device->SetTransform( D3DTS_VIEW, &proj );
		
		D3DXMatrixOrthoOffCenterRH(&proj,-100.0,100.0,-100.0,100.0,-100,100);
		base::Device->SetTransform( D3DTS_PROJECTION, &proj );

		hr |= base::Device->SetTexture( 0, particle_texture );
		hr |= base::Device->SetRenderState( D3DRS_LIGHTING,  FALSE );
		hr |= base::Device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		hr |= base::Device->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		hr |= base::Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		hr |= base::Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		hr |= base::Device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );       // Turn on point sprites
		hr |= base::Device->SetRenderState( D3DRS_POINTSIZE,     FtoDW(64.0f) ); // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.

		hr |= base::Device->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE);
		if(0){
			hr |= base::Device->DrawPrimitiveUP( D3DPT_POINTLIST, 100, g_pVB, 16);
		}else{
			static D3DXVECTOR3 center(32,32,0);
			D3DXMATRIX orig, view;
			base::Sprite->GetTransform(&orig);
			//base::Sprite->SetTransform(&proj);
			for(int x=0;x<100;x++)
				base::Sprite->Draw(particle_texture,NULL,NULL,(const D3DXVECTOR3*)&g_pVB[x*4],*(DWORD*)&g_pVB[x*4+3]);
			base::Sprite->SetTransform(&orig);
		}

		base::Sprite->Flush();
		base::Device->SetTransform( D3DTS_VIEW, &old_view );
		base::Device->SetTransform( D3DTS_PROJECTION, &old_proj );
		base::Device->SetTransform( D3DTS_WORLD, &old_world );

		sb->Apply();
		sb->Release();
	}
}
*/

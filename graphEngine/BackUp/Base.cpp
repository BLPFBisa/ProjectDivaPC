#include "Base.h"

//-----------------------base-------------------------
namespace base
{
	IDirect3DDevice9 *Device = NULL;
	ID3DXSprite* Sprite = NULL;
	HWND hwnd;
	double SECOND = 1000.0;
	double FPS = 60, timeDelta = SECOND/FPS;
	int Width = 480, Height = 272;
	char Name[] = "Project DIVA";
	int fps = 0;

	//窗口拖动相关
	RECT WindowCurPos, WindowDeltaPos; //
	POINT MousePrePos, MouseCurPos;
	bool bNcLButtonDown = false;

	bool Init(
		HINSTANCE hInstance,    
		int width, int height,   
		bool windowed,           
		D3DDEVTYPE deviceType)
	{
		BaseSet();
		if( !InitWindow(hInstance,Width,Height,windowed) )
			return false;
		if( !InitD3D(windowed,deviceType) )
			return false;
		InitTime();
		Start(Device);
		return true;
	}
	bool InitD3D(bool windowed,D3DDEVTYPE deviceType)
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
	 
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth            = Width;
		d3dpp.BackBufferHeight           = Height;
		d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount            = 1;
		d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality         = 0;
		d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
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
		//
		// Create the main application window.
		//

		WNDCLASS wc;

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = (WNDPROC)base::WndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
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
			WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 
			0, 0, width, height,
			0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

		if( !hwnd )
		{
			::MessageBox(0, "CreateWindow() - FAILED", 0, 0);
			return false;
		}

		 //调整窗口大小-----------------------------------
		 RECT window_rect = {0,0,width,height};

		 AdjustWindowRectEx(&window_rect,
			 GetWindowStyle(hwnd),
			 GetMenu(hwnd)!=NULL,
			 GetWindowExStyle(hwnd));
		 MoveWindow(hwnd,0,0,window_rect.right-window_rect.left,window_rect.bottom-window_rect.top,FALSE);
		 //-----------------------------------------------

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
				//鼠标拖动窗口滑出窗口外的处理
				if(bNcLButtonDown)
				{
					::GetCursorPos(&MouseCurPos); //鼠标按下获取当前位置，以便取出现对位置
					::GetWindowRect(::GetActiveWindow(), &WindowCurPos);
					if(MouseCurPos.y < WindowCurPos.top)
					{
						WindowCurPos.left = MouseCurPos.x - WindowDeltaPos.left;
						WindowCurPos.top = MouseCurPos.y - WindowDeltaPos.top;
						//debugf(_T("Current Window Pos X Is %d Y Is %d"),WindowCurPos.left,WindowCurPos.top);
						::SetWindowPos(::GetActiveWindow(), HWND_NOTOPMOST, WindowCurPos.left, WindowCurPos.top, WindowCurPos.right, WindowCurPos.bottom, SWP_NOSIZE);
						MousePrePos = MouseCurPos;
					}
				}
				MainLoop();
			}
		}
		return msg.wParam;
	}
	void MainLoop()
	{
		static double lastTime = GetCurrentCount();
		static int cnt = 0;
		double currTime  = GetCurrentCount();
		double dwTime = currTime-lastTime;
		
		if(dwTime>=timeDelta)
		{
			cnt++;
			lastTime = currTime;
			if(dwTime!=0)
				fps = int(SECOND/dwTime);
			else
				fps = int(SECOND);

			//upDate跳帧处理
			//while(currTime-lastTime>=timeDelta)
			Update(dwTime);//, lastTime += timeDelta;

			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			Device->BeginScene();
			Sprite->Begin( D3DXSPRITE_ALPHABLEND  );

			Draw(Device);
			
			Sprite->End();
			Device->EndScene();
			Device->Present(0, 0, 0, 0);

			if(timeDelta+timeDelta-dwTime>0)
				Sleep((timeDelta+timeDelta-dwTime)/base::SECOND*1000);
		}
	}
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		if( msg == WM_KEYDOWN ||
			msg == WM_SYSKEYDOWN || 
			msg == WM_KEYUP ||
			msg == WM_SYSKEYUP )
		{
			bool bKeyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
			DWORD dwMask = (1 << 29);
			bool bAltDown = ( (lParam & dwMask) != 0 );
			
			OnKeyEvent( (UINT)wParam, bKeyDown, bAltDown );           
		}

		if( msg == WM_LBUTTONDOWN ||
			msg == WM_LBUTTONUP ||
			msg == WM_LBUTTONDBLCLK ||
			msg == WM_MBUTTONDOWN ||
			msg == WM_MBUTTONUP ||
			msg == WM_MBUTTONDBLCLK ||
			msg == WM_RBUTTONDOWN ||
			msg == WM_RBUTTONUP ||
			msg == WM_RBUTTONDBLCLK ||
			msg == WM_XBUTTONDOWN ||
			msg == WM_XBUTTONUP ||
			msg == WM_XBUTTONDBLCLK ||
			msg == WM_MOUSEWHEEL || 
			msg == WM_MOUSEMOVE)
		{
			int xPos = (short)LOWORD(lParam);
			int yPos = (short)HIWORD(lParam);

			if( msg == WM_MOUSEWHEEL )
			{
				POINT pt;
				pt.x = xPos; pt.y = yPos;
				ScreenToClient( hwnd, &pt );
				xPos = pt.x; yPos = pt.y;
			}

			int nMouseWheelDelta = 0;
			if( msg == WM_MOUSEWHEEL ) 
				nMouseWheelDelta = (short) HIWORD(wParam);

			int nMouseButtonState = LOWORD(wParam);
			bool bLeftButton  = ((nMouseButtonState & MK_LBUTTON) != 0);
			bool bRightButton = ((nMouseButtonState & MK_RBUTTON) != 0);
			bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON) != 0);
			bool bSideButton1 = ((nMouseButtonState & MK_XBUTTON1) != 0);
			bool bSideButton2 = ((nMouseButtonState & MK_XBUTTON2) != 0);

			OnMouseEvent( bLeftButton, bRightButton, bMiddleButton, bSideButton1, bSideButton2, nMouseWheelDelta, xPos, yPos);
		}
		switch( msg )
		{
		//下面是关于拖动窗口避免消息丢失的处理-------------------------------------------------------------------------------------
		case WM_NCLBUTTONUP: //鼠标抬起时标记，表示不再拖动，标记设置为false
		bNcLButtonDown = false;
		return DefWindowProc(hwnd,msg,wParam,lParam);
		case WM_NCRBUTTONDOWN://在标题栏上点击右键不做处理，就是不要弹出系统菜单
		return 0;
		case WM_NCMOUSELEAVE://鼠标出了标题栏的时候标记也要设置为false，避免鼠标无法定位
		bNcLButtonDown = false;
		return DefWindowProc(hwnd,msg,wParam,lParam);
		case WM_SYSCOMMAND:
		switch( wParam & 0xfff0 )
		{
		///////////////这里直接返回，避免进入消息循环不出来，使游戏freese////////////////////
		//当鼠标在标题栏上按住不放时，系统会发送WM_SYSCOMMAND消息，当DefWindowProc
		//收到SC_MOVE后，会发送WM_ENTERSIZEMOVE，这个时候整个消息循环就会停止，直到
		//DefWindowProc处理完成返回，所以这个消息不能交给系统处理，直接返回
		//SC_KEYMENU消息是按快捷键的时候产生的系统菜单，这时候消息循环也是停止的，直到
		//DefWindowProc处理完成返回
		//SC_MOUSEMENU消息是鼠标左键点击左上角小图标是产生的系统菜单，同上
		case SC_MOVE:  //这里是多出来的
		::GetCursorPos(&MouseCurPos); //鼠标按下获取当前位置，以便取出现对位置
		::GetWindowRect(hwnd, &WindowCurPos);
		WindowDeltaPos.left = MouseCurPos.x - WindowCurPos.left;
		WindowDeltaPos.top = MouseCurPos.y - WindowCurPos.top;
		bNcLButtonDown = true;

		//解决在经典窗口(Windows Classic Style)模式下，收不到WM_NCMOUSELEAVE的问题
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
		////////////这里根据鼠标的相对位置设置窗口位置///////////////////////////////////////////////////////
		//这里是根据鼠标消息模拟窗口移动的过程
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
		//---------------------------------------------结束----------------------------------------------------------------------

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
	if(!base::Init(hinstance,
		base::Width, base::Height, true, D3DDEVTYPE_HAL))
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
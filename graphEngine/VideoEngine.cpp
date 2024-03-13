#include "VideoEngine.h"
#include "dshowtextures.h"
#include "graphEngine.h"
#include <string>
using namespace std;

LPDIRECT3DTEXTURE9      g_pTexture; // Our texture
D3DFORMAT               m_TextureFormat;
BYTE  pBuffer[10000000];
bool bBufferRefresh;

LPDIRECT3DVERTEXBUFFER9 m_pVB;

CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder

CComPtr<IMediaControl>  m_pMC;          // Media Control

CComPtr<IMediaPosition> m_pMP;          // Media Position

CComPtr<IMediaEvent>    m_pME;          // Media Event

CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer

CComPtr<IBaseFilter>    m_pFSrc;        // Source Filter

CComPtr<IPin>           m_pFSrcPinOut;  // Source Filter Output Pin

CTextureRenderer        *pCTR=0;

HRESULT VideoEngine::UpgradeGeometry(LONG lActualW, LONG lTextureW,LONG lActualH, LONG lTextureH )

{

 HRESULT hr = S_OK;

    if( 0 == lTextureW || 0 == lTextureH )

    {

        return E_INVALIDARG;

    }

    FLOAT tuW = (FLOAT)lActualW / (FLOAT)lTextureW;

    FLOAT tvH = (FLOAT)lActualH / (FLOAT)lTextureH;

 

    // Fill the vertex buffer. We are setting the tu and tv texture

    // coordinates, which range from 0.0 to 1.0

    CUSTOMVERTEX* pVertices;

    if ( FAILED( hr = m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )

    {

        //Msg(TEXT("Could not lock the vertex buffer!  hr=0x%x"), hr);

        return E_FAIL;

    }

    

 

     int i=0;

 

     //注意v轴的单位,选负数!

     //左下角

    pVertices[i].tu = 0.0f;

    pVertices[i].tv = 1.0f*tvH;

     i++;

 

     //左上角

    pVertices[i].tu = 0.0f;

    pVertices[i].tv = 0.0f;

     i++;

 

 

     //右下角

    pVertices[i].tu = 1.0f*tuW;

    pVertices[i].tv = 1.0f*tvH;

     i++;

 

     //右上角

    pVertices[i].tu = 1.0f*tuW;

    pVertices[i].tv = 0.0f;

 

    m_pVB->Unlock();

    return S_OK;

}

//-----------------------------------------------------------------------------

// InitDShowTextureRenderer : Create DirectShow filter graph and run the graph

//-----------------------------------------------------------------------------

HRESULT VideoEngine::InitDShowTextureRenderer(string filename)

{

    HRESULT hr = S_OK;

    //CTextureRenderer        *pCTR=0;        // DirectShow Texture renderer

 

    // Create the filter graph

    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGB);

    if (FAILED(hr))
	{
		Msg(TEXT("Could not create DirectShow!  hr=0x%x"), hr);
			
        return E_FAIL;
	}
 

    // Create the Texture Renderer object

    pCTR = new CTextureRenderer(NULL, &hr);

    if (FAILED(hr) || !pCTR)

    {

        delete pCTR;

        Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);

        return E_FAIL;

    }

    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph

    m_pRenderer = pCTR;

    if (FAILED(hr = m_pGB->AddFilter(m_pRenderer, L"TEXTURERENDERER")))

    {

        Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);

        return hr;

    }

    // Add the source filter to the graph.

	LPCWSTR _filename;

	_filename = SToW(filename);

	hr = m_pGB->AddSourceFilter(_filename, L"SOURCE", &m_pFSrc);


	SAFE_RELEASE_ARRAY(_filename);

    // If the media file was not found, inform the user.

    if (hr == VFW_E_NOT_FOUND)

    {

        Msg(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n"));

         //   TEXT("This sample reads a media file from your windows directory.\r\n")

           // TEXT("This file is missing from this machine."));

        return hr;

    }

    else if(FAILED(hr))

    {

        Msg(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);

        return hr;

    }

 

    if (FAILED(hr = m_pFSrc->FindPin(L"Output", &m_pFSrcPinOut)))

    {

        //Msg(TEXT("Could not find output pin!  hr=0x%x"), hr);

        return hr;

    }

 

    // Get the graph's media control, event & position interfaces

    m_pGB.QueryInterface(&m_pMC);

    m_pGB.QueryInterface(&m_pMP);

    m_pGB.QueryInterface(&m_pME);

 

    // Render the source filter's output pin.  The Filter Graph Manager

    // will connect the video stream to the loaded CTextureRenderer

    // and will load and connect an audio renderer (if needed).

 

    if (FAILED(hr = m_pGB->Render(m_pFSrcPinOut)))

    {

        //Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);

        return hr;

    }


 

     // Start the graph running;

     hr = m_pMP->put_CurrentPosition(m_pTime);

     if( S_OK == hr )

     {

         hr = m_pMC->Run();
		 m_pMC->Pause();
     }

	srcRect = MakeRect(0,0,pCTR->m_lVidWidth,pCTR->m_lVidHeight);

	return S_OK;

}


HRESULT VideoEngine::Init(void)
{
     HRESULT hr=S_OK;

	 hr = CoInitialize(NULL);

     return hr;
}

HRESULT VideoEngine::Read(string filename)
{
	if(read)
		Stop();
	read = true;
	return InitDShowTextureRenderer(filename);  //初始化DirectShow，它会间接建立纹理
}

HRESULT VideoEngine::Play(string filename, int x, int y)
{
	HRESULT hr = S_OK;
	if(play)
		Stop();
	if(!read)
		Read(filename);
	play = true;
	bBufferRefresh = false;
	//srcRect = MakeRect(0,0,480,272);
	int width = base::Width-x, height = base::Height-y;
	if(width*(srcRect.bottom-srcRect.top)/(srcRect.right-srcRect.left)<height)
		height = width*(srcRect.bottom-srcRect.top)/(srcRect.right-srcRect.left);
	if(height*(srcRect.right-srcRect.left)/(srcRect.bottom-srcRect.top)<width)
		width = height*(srcRect.right-srcRect.left)/(srcRect.bottom-srcRect.top);
	drawRect = MakeRect(x,y,width,height);
	hr = m_pMC->Run();
	return hr;
}

void VideoEngine::Update()
{
	if(play)
	{
		if(!IsPlaying())
			Stop();
		else
		{
			if(pCTR)
				pCTR->LoadTexture();
		}
	}
}

void VideoEngine::Draw()
{
	if(play&&g_pTexture)
		graphEngine.Draw(g_pTexture,srcRect,drawRect);
}

void VideoEngine::Paused(bool Paused)
{
	if(play)
	{
		if(Paused)
			m_pMC->Pause();
		else
			m_pMC->Run();
	}
}

void VideoEngine::Stop()
{
	if(play)
	{
		play = false;
		read = false;
		Clear();
	}
}

bool VideoEngine::IsPlaying()
{
	LONG   evCode,   evParam1,   evParam2;
	if (m_pME)
	{
		if   (SUCCEEDED(m_pME-> GetEvent(&evCode,   (LONG_PTR   *)   &evParam1,
											(LONG_PTR   *)   &evParam2,   0)))
		{
			return   !(evCode   ==   EC_COMPLETE);
		}
		return true;
	}
	return false; 
}

void VideoEngine::Clear()
{
	bBufferRefresh = false;

	if(g_pTexture)
	{
		g_pTexture->Release();
		g_pTexture = NULL;
	}
	if(!(!pCTR)) {pCTR->Stop(); pCTR = NULL;}
    if (!(!m_pMC)) m_pMC->Stop();

    if (!(!m_pMC)) m_pMC.Release();

    if (!(!m_pME)) m_pME.Release();

    if (!(!m_pMP)) m_pMP.Release();

    if (!(!m_pGB)) m_pGB.Release();

     if (!(!m_pFSrc))        m_pFSrc.Release();        // Source Filter

     if (!(!m_pFSrcPinOut))  m_pFSrcPinOut.Release();  // Source Filter Output Pin

    if (!(!m_pRenderer)) m_pRenderer.Release();

}
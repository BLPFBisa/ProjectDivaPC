#ifndef VIDEO_ENGINE_H
#define VIDEO_ENGINE_H

#include "base.h"

extern LPDIRECT3DTEXTURE9      g_pTexture ; // Our texture
extern D3DFORMAT               m_TextureFormat;
extern BYTE  pBuffer[10000000];
extern bool bBufferRefresh;

extern LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold vertices 

extern CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder

extern CComPtr<IMediaControl>  m_pMC;          // Media Control

extern CComPtr<IMediaPosition> m_pMP;          // Media Position

extern CComPtr<IMediaEvent>    m_pME;          // Media Event

extern CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer

extern CComPtr<IBaseFilter>    m_pFSrc;        // Source Filter

extern CComPtr<IPin>           m_pFSrcPinOut;  // Source Filter Output Pin


// A structure for our custom vertex type. We added texture coordinates.
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#define videoEngine VideoEngine::Instance()

class VideoEngine
{
     HRESULT  UpgradeGeometry(LONG lActualW, LONG lTextureW,LONG lActualH, LONG lTextureH );

    // InitDShowTextureRenderer用来初始化DirectShow

     HRESULT InitDShowTextureRenderer(string filename);

	 REFTIME                  m_pTime;  // DirectShow当前播放位置

	 bool play,read;

	 RECT srcRect,drawRect;

	 VideoEngine():m_pTime(0),play(false),read(false) {}
	 ~VideoEngine() {}
// 这里省略若干行代码

	BOOL m_bUseDynamicTextures;
    LONG m_lVidWidth;   // Video width
    LONG m_lVidHeight;  // Video Height
    LONG m_lVidPitch;   // Video Pitch
public:

	 inline static VideoEngine& Instance(void) {static VideoEngine instance; return instance;}

	 HRESULT Init();

	 HRESULT Read(string filename);

     HRESULT Play(string filename, int x, int y);

	 void SetNew() {g_pTexture = NULL;}

	 void Update();

	 void Draw();

	 void Paused(bool Paused);
	
	 void Stop();

	 void Clear();

	 bool IsPlaying();

	 friend class CTextureRenderer;
};

void Msg(TCHAR *szFormat, ...);

#endif
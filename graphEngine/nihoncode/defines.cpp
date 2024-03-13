#include "defines.h"

void ErrorExit(string error_infor,int exit_code)
{
	MessageBox(NULL,error_infor.data(),"DIVA",MB_OK);
	exit(exit_code);
}

void trimLeft(string &target)
{
    if (target.size() != 0)
    {
        for (string::iterator pos = target.begin();  pos < target.end(); )
        {
            if (*pos == ' ')
            {
                target.erase(pos);
                pos = target.begin();
            }
            else
            {
                return;
            }
        }
    }
}

void trimRight(string &target)
{
    if (target.size() != 0)
    {
        for (string::iterator pos = target.end()-1; pos != target.begin(); )
        {
            if (*pos == ' ')
            {
                target.erase(pos);
                pos = target.end();
            }
            else
            {
                return;
            }
        }
    }
}

double GetCurrentCount(void)
{
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	return (double)tmp.QuadPart;
}
RECT MakeRect(int px,int py,int width,int height)
{
	RECT ret;
	ret.top=py;
	ret.left=px;
	ret.right=px+width;
	ret.bottom=py+height;
	return ret;
}

void SetRectPos(int px,int py,RECT &src)
{
	RECT ret;
	ret.left = px;
	ret.top = py;
	ret.right = px + src.right - src.left;
	ret.bottom = py + src.bottom - src.top;
	src = ret;
}

///-------------------------------------
///将string转换为LPCWSTR
///-------------------------------------
LPCWSTR SToW(string tex)
{
	/*
	size_t size = tex.length();
	wchar_t *buffer = new wchar_t[size+1];
	MultiByteToWideChar( CP_ACP, 0, tex.data(), size, buffer, size * sizeof(wchar_t));
	int realLength=0;
	for(int a=0;a<tex.length();a++)
	{
		realLength++;
		if((int)(tex[a])<0) a++;
	}
	buffer[realLength] = 0; //以\0结尾
	return buffer;
	*/
	LPWSTR pszOut = NULL;
	int nInputStrLen = strlen (tex.c_str()); 
	 // Double NULL Termination 
	 int nOutputStrLen = MultiByteToWideChar(CP_ACP, 0, tex.c_str(), nInputStrLen, NULL, 0) + 2; 
	 pszOut = new WCHAR [nOutputStrLen]; 
	 if (pszOut) 
	 { 
		memset (pszOut, 0x00, sizeof (WCHAR)*nOutputStrLen); 
		MultiByteToWideChar (CP_ACP, 0, tex.c_str(), nInputStrLen, pszOut, nInputStrLen); 
	 }
	 wcheckTail0(pszOut,nOutputStrLen);
	 return pszOut;

}

wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if(nSize <= 0) return NULL;
	WCHAR *pwszDst = new WCHAR[nSize+1];
	if( NULL == pwszDst) return NULL;
	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;
	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
	for(int i = 0; i < nSize; i ++) 
	pwszDst[i] = pwszDst[i+1]; 
	wstring wcharString(pwszDst);
	delete pwszDst;
	return wcharString;
}

wstring s2ws(const string& s){ return Ansi2WChar(s.c_str(),s.size());}

void checkTail0(char *s,int maxl)
{
	int l=0;
	while(l<maxl)
	{
		if(s[l]==10)
		{
			s[l]=0;
			break;
		}
		l++;
	}
}

void wcheckTail0(wchar_t *s,int maxl)
{
	int l=0;
	while(l<maxl)
	{
		if(s[l]==10)
		{
			s[l]=0;
			break;
		}
		l++;
	}
}

bool isImage(string filename)
{
	string ret;
	for(int i=filename.length()-1;i>=0;i--)
	{
		if(filename[i]=='.')
			break;

		ret=char((filename[i]>='A'&&filename[i]<='Z')?(filename[i]-('A'-'a')):(filename[i]))+ret;
	}
	
	if(ret=="jpg"||ret=="png"||ret=="bmp"||ret=="dds"||ret=="tga")
		return true;
	else
		return false;
}

bool operator==(const RECT &rect1,const RECT &rect2)
{
	return (rect1.left==rect2.left&&rect1.bottom==rect2.bottom&&rect1.right==rect2.right&&rect1.top==rect2.top);
}


SystemIni::SystemIni()
{
	if(!ReadFromIniFile())
	{
		ChangeToDefault();
	}
}

void SystemIni::ChangeToDefault()
{
	ChangeGameKeyType(1); //PC
	ChangeSndVolume(10);
	ChangeWindowSize(1);
}

void SystemIni::ChangeGameKeyType(int type)
{
	GameKeyType = type;
	switch(type)
	{
	case 2://PSP
		GameKeyUp = 'W';
		GameKeyDown = 'S';
		GameKeyLeft = 'A';
		GameKeyRight = 'D';
		GameKeyTriangle = KeyUp;
		GameKeyCross = KeyDown;
		GameKeyRectangle = KeyLeft;
		GameKeyCircle = KeyRight;
		break;
	case 3://O2JAM PC
		GameKeyUp = 'I';
		GameKeyDown = 'K';
		GameKeyLeft = 'J';
		GameKeyRight = 'L';
		GameKeyTriangle = 'E';
		GameKeyCross = 'D';
		GameKeyRectangle = 'S';
		GameKeyCircle = 'F';
		break;
	case 4://O2JAM PSP
		GameKeyUp = 'E';
		GameKeyDown = 'D';
		GameKeyLeft = 'S';
		GameKeyRight = 'F';
		GameKeyTriangle = 'I';
		GameKeyCross = 'K';
		GameKeyRectangle = 'J';
		GameKeyCircle = 'L';
		break;
	default://PC
		GameKeyUp = KeyUp;
		GameKeyDown = KeyDown;
		GameKeyLeft = KeyLeft;
		GameKeyRight = KeyRight;
		GameKeyTriangle = 'W';
		GameKeyCross = 'S';
		GameKeyRectangle = 'A';
		GameKeyCircle = 'D';
		break;
	}
	SaveIniFile();
}
void SystemIni::ChangeSndVolume(int volume)
{
	soundVolume = volume;
	sndVolume = double(soundVolume)/10.0;
	SaveIniFile();
}
void SystemIni::ChangeWindowSize(int sizeType)
{
	switch(sizeType)
	{
	case 1:
		winWidth = 480;
		winHeight = 272;
		break;
	case 2:
		winWidth = 720;
		winHeight = 408;
		break;
	case 3:
		winWidth = 960;
		winHeight = 544;
		break;
	}
	SaveIniFile();
}

void SystemIni::SaveIniFile()
{
	FILE *fout;
	fout = fopen("config.ini", "w");
	fprintf(fout,"[CONFIG]\n");
	fprintf(fout,"GameKeyType = %d\n", GameKeyType);
	fprintf(fout,"WindowWidth = %d\n", winWidth);
	fprintf(fout,"WindowHeight = %d\n", winHeight);
	fprintf(fout,"SoundVolume = %d\n", soundVolume);
	fclose(fout);
}
bool SystemIni::ReadFromIniFile()
{
	FILE *fin;
	fin = fopen("config.ini","r");
	if(fin)
	{
		char stBuffer[100];
		string tflag;
		fscanf(fin,"%s",&stBuffer);
		while(fscanf(fin,"%s",&stBuffer)!=-1)
		{
			tflag = stBuffer;
			if(tflag == "GameKeyType")
			{
				fscanf(fin,"%s",&stBuffer);
				fscanf(fin,"%d",&GameKeyType);
			}
			else if(tflag == "WindowWidth")
			{
				fscanf(fin,"%s",&stBuffer);
				fscanf(fin,"%d",&winWidth);
			}
			else if(tflag == "WindowHeight")
			{
				fscanf(fin,"%s",&stBuffer);
				fscanf(fin,"%d",&winHeight);
			}
			else if(tflag == "SoundVolume")
			{
				fscanf(fin,"%s",&stBuffer);
				fscanf(fin,"%d",&soundVolume);
			}
			else
				fgets(stBuffer,500,fin);
		}
		fclose(fin);
	}
	else
		return false;
	if(winWidth==480&&winHeight==272)
		winSizeType = 1;
	else if(winWidth==720&&winHeight==408)
		winSizeType = 2;
	else if(winWidth==960&&winHeight==544)
		winSizeType = 3;
	else
		winSizeType = 4;
	ChangeGameKeyType(GameKeyType);
	ChangeSndVolume(soundVolume);
	return true;
}


string ToString(int num)
{
	char buffer[20];
	itoa(num,buffer,10);
	return string(buffer);
}

string ToString(double num,const char *_Format)
{
	char buffer[20];
	sprintf(buffer,_Format,num);
	return string(buffer);
}
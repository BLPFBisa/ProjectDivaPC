#ifndef __PSystemH__
#define __PSystemH__

#include "defines.h"
#include "Base.h"
#include <map>

using namespace std;


class ParticleBase
{
protected:
	int _age,_x,_y;
	Texture *_drawTex;
public:
	ParticleBase(int x,int y,Texture *drawTex):_age(0),_x(x),_y(y),_drawTex(drawTex){}
	virtual void Draw() = NULL;
	virtual void Update() = NULL;

	virtual bool Die() = NULL;
};

class ParticleTail : public ParticleBase
{
private:
	int _a,_r,_g,_b;
public:
	ParticleTail(int x,int y,int a,int r,int g,int b,Texture* drawTex):ParticleBase(x,y,drawTex),_a(a),_r(r),_g(g),_b(b){}

	void Draw();
	void Update();
	bool Die();
};


class ParticleNode
{
public:
	ParticleNode(){prev=next=NULL;data=NULL;}
	~ParticleNode(){SAFE_DELETE(data);}
	ParticleNode(ParticleNode *pprev,ParticleNode *pnext,ParticleBase *pdata):prev(pprev),next(pnext),data(pdata){}
	ParticleBase *data;
	ParticleNode *prev,*next;
};

class PSystem
{
	PSystem(){}
	~PSystem(){Clear();}

	//ParticleNode *head,*tail;
	map<int,ParticleNode*> head;
	map<int,ParticleNode*> tail;

public:
	static PSystem& Instance(){static PSystem instance;return instance;}

	void Draw();
	void Update();

	void AddParticle(ParticleBase* p,int group=0);

	void Clear();
	void Clear(int group);
};


#define pSystem PSystem::Instance() 

#endif
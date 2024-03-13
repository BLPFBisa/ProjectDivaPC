#include"PSystem.h"
#include"Base.h"
#include"GameMana.h"

using namespace std;

void ParticleTail::Draw()
{
	graphEngine.Draw(_drawTex,MakeRect(0,0,8,8),MakeRect(_x-2,_y-2,4,4),D3DCOLOR_ARGB(_a,_r,_g,_b));
}

void ParticleTail::Update()
{
	_age++;
	if(_age>10)
	{
		_a-=6;
		if(_a<0)_a=0;
	}
}

bool ParticleTail::Die()
{
	return _a==0;
}

void PSystem::Draw()
{
	for(map<int,ParticleNode*>::iterator i = head.begin();i!=head.end();i++)
	{
		ParticleNode *ptr = i->second;
		while(ptr)
		{
			ptr->data->Draw();
			ptr=ptr->next;
		}
	}
}

void PSystem::Update()
{
	for(map<int,ParticleNode*>::iterator i = head.begin();i!=head.end();i++)
	{
		ParticleNode *ptr = i->second,*tnext;
		while(ptr)
		{
			ptr->data->Update();
			if(ptr->data->Die())
			{
				if(ptr->next)
					ptr->next->prev=ptr->prev;
				if(ptr->prev)
					ptr->prev->next=ptr->next;
				tnext = ptr->next;
				if(ptr->next==NULL)
					tail[i->first]=ptr->prev;
				if(ptr->prev==NULL)
					head[i->first]=ptr->next;
				SAFE_DELETE(ptr);
				ptr=tnext;
			}
			else
				ptr=ptr->next;
		}
	}
}

void PSystem::Clear()
{
	for(map<int,ParticleNode*>::iterator i = head.begin();i!=head.end();i++)
	{
		ParticleNode *ptr = i->second,*tnext;
		while(ptr)
		{
			tnext = ptr->next;
			SAFE_DELETE(ptr);
			ptr=tnext;
		}
	}
	head.clear();
	tail.clear();
}

void PSystem::Clear(int group)
{
	ParticleNode *ptr = head[group],*tnext;
	while(ptr)
	{
		tnext = ptr->next;
		SAFE_DELETE(ptr);
		ptr=tnext;
	}
	for(map<int,ParticleNode*>::iterator i = head.begin();i!=head.end();i++)
		if(i->first==group)
		{
			head.erase(i);
			break;
		}
	for(map<int,ParticleNode*>::iterator i = tail.begin();i!=tail.end();i++)
		if(i->first==group)
		{
			tail.erase(i);
			break;
		}
}

void PSystem::AddParticle(ParticleBase *p,int group)
{
	ParticleNode*& ttail = tail[group];
	if(!ttail)
	{
		ttail = new ParticleNode(NULL,NULL,p);
		head[group] = ttail;
	}
	else
	{
		ttail->next = new ParticleNode(ttail,NULL,p);
		ttail=ttail->next;
	}
}
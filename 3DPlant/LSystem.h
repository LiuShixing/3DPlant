#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<stack>
#include<queue>
#include<map>
#include <xnamath.h>
#include<ctime>
#include"Vertex.h"
/*
������
v ��������
d ����
a ��ת�Ƕ�

��ĸ��
F ��v����ǰ��d
x+ ��x��˳ʱ����תa��
x- ��x����ʱ����תa��
y+ ��y��˳ʱ����תa��
y- ��y����ʱ����תa��
z+ ��z��˳ʱ����תa��
z- ��z����ʱ����תa��
[ ��ջ
] ��ջ


*/

struct State
{
	State()
	{
		pos = XMFLOAT3(0.0f,0.0f,0.0f);
		v = XMFLOAT3(0.0f, 1.0f, 0.0f);
		verIndiex = 0;
	}
	XMFLOAT3 pos;
	XMFLOAT3 v;
	UINT     verIndiex;
};

class LSparameter
{
public:
	UINT  mIterations;  
	float mStepMin;
	float mStepMax;
	float mRotAngleMin;
	float mRotAngleMax;
	float mTrunkSize;

	char  mStart;
	std::map<char, std::vector<std::string> > mRules;

	std::string GetRandomRule(char key);
	float      GetRandomStep();
	float      GetRandomAngle();
};
class LSystem
{
public:
	LSystem();
	~LSystem();
	void CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, LSparameter& param);
private:
	std::string mProduction;
	LSparameter mParamiter;

	float md;
	float ma;
};


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
	float    trunkScal;
};

class LSparameter
{
public:
	LSparameter();

	UINT  mIterations;  
	float mStepMin;
	float mStepMax;
	float mStepAtt;
	float mRotAngleMin;
	float mRotAngleMax;
	float mTrunkSize;
	float mTrunkSizeAtt;
	float mRadiusRate;
	float mLeaveSize;
	char  mStart;
	int mIsTrunk;
	int mIsLeave;
	bool  mIsToSun;
	float mSunFactor;
	std::map<char, std::vector<std::string> > mRules;

	std::string GetRandomRule(char key);
	float      GetRandomStep(float att=0.0f);  //att �ǲ���˥����
	float      GetRandomAngle();
};
struct Trunk
{
	XMFLOAT3 pos;
	float sizeScal;
	float scalY;
	XMFLOAT3 rotAxis;
	float    angle;
};
struct Leave
{
	XMFLOAT3 pos;
	float scal;
	XMFLOAT3 rotAxis;
	float    angle;
};

class LSystem
{
public:
	LSystem();
	~LSystem();
	void CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, LSparameter& param);
	void ToSun(XMVECTOR& V,float factor);
	std::vector<Trunk> mTrunks;
	std::vector<Leave> mLeaves;
	std::vector<Vertex::PosColor> mVertexs;
	std::vector<UINT> mIndices;
};


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


class RuleRight
{
public:
	std::string production;
	float      probability;
};

class LSparameter
{
public:
	LSparameter();

	int  mIterations;  
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
	int  mIsToSun;
	float mSunFactor;
	int  mLeafOrder;

	std::map<char, std::vector<RuleRight> > mRules;

	std::string GetRandomRule(char key);
	bool       CheckParam();
	float      GetRandomStep(float att=1.0f);  //att �ǲ���˥����
	float      GetRandomAngle();
};
struct Trunk
{
	XMFLOAT3 pos;
	XMFLOAT3 rotAxis;
	float sizeScal;
	float scalY;
	float    angle;
};
struct Leaf
{
	XMFLOAT3 pos;
	XMFLOAT3 rotAxis;
	float scal;
	float angle;
	float rotY;
};

class PlantData
{
public:
	std::vector<Trunk> mTrunks;
	std::vector<Leaf> mLeaves;
	std::vector<Vertex::PosColor> mVertexs;
	std::vector<UINT> mIndices;
};

struct State
{
	State()
	{
		pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		v = XMFLOAT3(0.0f, 1.0f, 0.0f);
		verIndiex = 0;
	}
	XMFLOAT3 pos;
	XMFLOAT3 v;
	UINT     verIndiex;
	float    trunkScal;
};

class LSystem
{
public:
	LSparameter mParam;

	LSystem();
	~LSystem();
	std::string GenerateLstring();

	void GenerateLeaf(std::vector<Leaf>& leaves, const State& curState, const State& newState, float step, Trunk& trunk, int i);
	void GenerateTrunk(std::vector<Trunk>& trunks, const State& curState, float step);
	void RoteDirection(char axis,char sign,XMFLOAT3& direction);
	void CreatePlant(PlantData& plantData);
	void ToSun(XMVECTOR& V,float factor);
};


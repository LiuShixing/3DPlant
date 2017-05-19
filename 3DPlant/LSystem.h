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
参数：
v 方向向量
d 步长
a 旋转角度

字母表：
F 向v方向前进d
x+ 绕x轴顺时针旋转a度
x- 绕x轴逆时针旋转a度
y+ 绕y轴顺时针旋转a度
y- 绕y轴逆时针旋转a度
z+ 绕z轴顺时针旋转a度
z- 绕z轴逆时针旋转a度
[ 进栈
] 出栈


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
	float      GetRandomStep(float att=1.0f);  //att 是步长衰减量
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


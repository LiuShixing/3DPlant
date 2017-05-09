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
	float      GetRandomStep(float att=0.0f);  //att 是步长衰减量
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


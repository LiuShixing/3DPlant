#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<stack>
#include<queue>
#include <xnamath.h>
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
};
class LSystem
{
public:
	LSystem();
	~LSystem();
	void CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, int iterations);
private:
	std::string mProduction;
	int        mIterations;

	float md;
	float ma;
};


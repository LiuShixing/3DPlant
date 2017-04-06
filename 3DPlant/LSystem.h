#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<stack>
#include<queue>
#include <xnamath.h>
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


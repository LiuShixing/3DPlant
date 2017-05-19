#include "stdafx.h"
#include "LSystem.h"


LSparameter::LSparameter() :mIsTrunk(1)
, mIsLeave(1), mIsToSun(true), mLeaveSize(1.0f), mLeafOrder(2)
{
	mIterations = 5;
	mStepMin = 1.0f;
	mStepMax = 5.0f;
	mStepAtt = 0.8f;
	mRotAngleMin = 15.0f;
	mRotAngleMax = 30.0f;
	mTrunkSize = 0.3f;
	mTrunkSizeAtt = 1.0f;
	mRadiusRate = 0.6f;
	mSunFactor = 0.6f;

	//默认规则
	std::vector<RuleRight> vs;

	RuleRight R;
	R.production = "F[z+x-X][z-x-X][x+X]";
	R.probability = 1.0f;

	vs.push_back(R);
	R.production = "F[z+x-X][z-x-X]";
	vs.push_back(R);
	R.production = "F[z+x-X][x+X]";
	vs.push_back(R);
	R.production = "F[z-x-X][x+X]";
	vs.push_back(R);
	R.production = "F[z+x-X]";
	vs.push_back(R);
	
	mRules['X'] = vs;
	mStart = 'X';

//	std::vector<std::string> tmp;
//	tmp.push_back("FF");
//	mRules['F'] = tmp;

	srand(time(NULL));
}

std::string LSparameter::GetRandomRule(char key)
{
	//查找以key为左部的规则集合
	std::map<char, std::vector<RuleRight> >::iterator it = mRules.find(key);
	if (it != mRules.end())
	{//找到规则

		//使用轮盘赌算法，从规则集合选择一个规则

		//计算概率比值的总和
		float total = 0.0f;
		for (int i = 0; i < (it->second).size(); i++)
			total += (it->second)[i].probability;

		//随机选择停止值
		float x = (float)rand() / (float)(RAND_MAX + 1);
		float stop = x*total-0.1f;

		//“旋转轮盘”，累加概率直到大于停止值
		float f = 0.0f;
		int index = 0;
		for (int i = 0; i < (it->second).size(); i++)
		{
			f += (it->second)[i].probability;
			if (f>stop)
			{
				index = i;
				break;
			}
		}
		//返回选中规则的右部字符串
		return (it->second)[index].production;
	}
	//没找到规则，返回key本身
	std::string s("");
	s += key;
	return s;
}

float LSparameter::GetRandomStep(float att)
{
	float x = (float)rand() / (float)(RAND_MAX + 1);
	return (mStepMin + (mStepMax - mStepMin) * x)*att;
}

float LSparameter::GetRandomAngle()
{
	float x = (float)rand() / (float)(RAND_MAX + 1);
	return (mRotAngleMin + (mRotAngleMax - mRotAngleMin) * x)*XM_PI / 180.0f;
}
bool LSparameter::CheckParam()
{
/*
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
	int  mIsToSun;
	float mSunFactor;
	int  mLeafOrder;
	*/
	if (mIterations <= 0 || mStepMin <= 0.0f || mStepMax <= 0.0f || mTrunkSize<=0.0f
		|| mLeaveSize<=0.0f)
		return false;
	
	return true;
}

LSystem::LSystem() 
{
}


LSystem::~LSystem()
{
}
std::string LSystem::GenerateLstring()
{
	std::string Lstring = mParam.GetRandomRule(mParam.mStart);
	UINT iterations = mParam.mIterations;
	while (--iterations)
	{
		std::string tmpstr = "";
		for (int i = 0; i < Lstring.size(); i++)
		{
			tmpstr += mParam.GetRandomRule(Lstring[i]);
		}
		Lstring = tmpstr;
	}
	return Lstring;
}

void LSystem::GenerateTrunk(std::vector<Trunk>& trunks, const State& curState,float step)
{
	Trunk trunk;
	trunk.pos = curState.pos;

	//计算树干的最终旋转轴，旋转角度
	XMVECTOR Y = XMVector3Normalize(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));//树干起始方向向量
	XMVECTOR DOT = XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&curState.v)), Y);
	XMFLOAT3 DotTmp;
	XMStoreFloat3(&DotTmp, DOT);
	float dot = DotTmp.x;
	if (fabs(dot - 1.0f) < 0.000001f)
	{//最终方向和起始方向相同
		trunk.rotAxis = XMFLOAT3(0.0f, 0.1f, 0.0f);
		trunk.angle = 0.0f;
	}
	else
	{
		trunk.angle = acosf(dot);
		XMVECTOR DIR = XMVector3Normalize(XMVector3Cross(Y, XMVector3Normalize(XMLoadFloat3(&curState.v))));
		XMStoreFloat3(&trunk.rotAxis, DIR);
	}

	//树干整体缩放和长度缩放
	trunk.sizeScal = curState.trunkScal;
	trunk.scalY = step / mParam.mStepMax;
	trunks.push_back(trunk);
}
void LSystem::GenerateLeaf(std::vector<Leaf>& leaves, const State& curState, const State& newState, float step, Trunk& trunk, int i)
{
	XMVECTOR LP, NP, CP, CV;
	CP = XMLoadFloat3(&curState.pos);
	CV = XMLoadFloat3(&curState.v);
	NP = XMLoadFloat3(&newState.pos);

	XMFLOAT3 TMP;
	XMStoreFloat3(&TMP, XMVector3Length(NP - CP));

	float dt = 0.5f;
	int count = TMP.x / dt;

	if (mParam.mLeafOrder == 0)
	{
		//互生叶序
		Leaf leaf;
		leaf.rotAxis = trunk.rotAxis;
		leaf.angle = trunk.angle;
		leaf.scal = mParam.mLeaveSize;

		float rotY[2][2] = { { 0.0f, XM_PI }, { XM_PI / 2.0f, XM_PI*1.5f } };
		int j;
		for (j = 0; j < count; j++)
		{
			LP = CP + CV*dt*j;
			XMStoreFloat3(&leaf.pos, LP);
			leaf.rotY = rotY[i % 2][j % 2];
			leaves.push_back(leaf);
		}
		//枝端的叶子
		leaf.pos = newState.pos;
		leaf.rotY = rotY[i % 2][j % 2];
		leaves.push_back(leaf);
	}
	else if (mParam.mLeafOrder == 1)
	{
		//对生叶序
		Leaf leaf;
		leaf.rotAxis = trunk.rotAxis;
		leaf.angle = trunk.angle;
		leaf.scal = mParam.mLeaveSize;
		float rotY[2][2] = { { 0.0f, XM_PI }, { XM_PI / 2.0f, XM_PI*1.5f } };
		int j;
		for (j = 0; j < count; j++)
		{
			LP = CP + CV*dt*j;
			XMStoreFloat3(&leaf.pos, LP);
			leaf.rotY = rotY[i % 2][j % 2];
			leaves.push_back(leaf);

			//相对那片
			leaf.rotY = rotY[i % 2][(j + 1) % 2];
			leaves.push_back(leaf);
		}
		leaf.pos = newState.pos;
		leaf.rotY = rotY[i % 2][j % 2];
		leaves.push_back(leaf);
		leaf.rotY = rotY[i % 2][(j + 1) % 2];
		leaves.push_back(leaf);
	}
	else if (mParam.mLeafOrder == 2)
	{
		//轮生叶序
		dt = 0.5f;
		count = TMP.x / dt;
		Leaf leaf;
		leaf.rotAxis = trunk.rotAxis;
		leaf.angle = trunk.angle;
		leaf.scal = mParam.mLeaveSize;
		float rotY[4] = { 0.0f, XM_PI / 2.0f, XM_PI, XM_PI*1.5f };
		int j;
		for (j = 0; j < count; j++)
		{
			LP = CP + CV*dt*j;
			XMStoreFloat3(&leaf.pos, LP);
			for (int k = 0; k < 4; k++)
			{
				leaf.rotY = rotY[k % 4];
				leaves.push_back(leaf);
			}
		}
		leaf.pos = newState.pos;
		for (int k = 0; k < 4; k++)
		{
			leaf.rotY = rotY[k % 4];
			leaves.push_back(leaf);
		}

	}
}
void LSystem::RoteDirection(char axis, char sign, XMFLOAT3& direction)
{
	float tmpa = mParam.GetRandomAngle();
	float angle = sign == '+' ? tmpa : -tmpa;

	XMVECTOR OldV = XMLoadFloat3(&direction);
	XMVECTOR NewV;
	switch (axis)
	{
	case 'x':NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationX(angle)));
		break;
	case 'y':NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationY(angle)));
		break;
	case 'z':NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationZ(angle)));
		break;
	}

	if (mParam.mIsToSun)
		ToSun(NewV, mParam.mSunFactor);

	XMStoreFloat3(&direction, NewV);
}
void LSystem::CreatePlant(PlantData& plantData)
{
	plantData.mVertexs.clear();
	plantData.mIndices.clear();
	plantData.mTrunks.clear();
	plantData.mLeaves.clear();

	//生成最终字符串
	std::string Lstring = GenerateLstring();

	//设置初始状态
	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;
	orinState.trunkScal = 1.0f;

	//保存初始顶点
	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	plantData.mVertexs.push_back(orinVer);

	//步长衰减
	float totalStepAtt = 1.0f;

	//记录深度
	int depth = 1;

	//给L字符串末尾加]
	Lstring += "]";
	
	State curState = orinState;
	std::stack<State> stateStack; 
	for (int i = 0; i < Lstring.size() - 1; i++)
	{
		switch (Lstring[i])
		{
		case 'F':
		{
					State newState;
					newState.v = curState.v;
					XMVECTOR OPOS = XMLoadFloat3(&curState.pos);
					XMVECTOR V = XMVector3Normalize(XMLoadFloat3(&curState.v));
					
					float step = mParam.GetRandomStep(totalStepAtt);
					XMVECTOR NPOS = OPOS + V*step;
					XMStoreFloat3(&newState.pos, NPOS);

					Vertex::PosColor newVer;
					newVer.pos = newState.pos;
					newVer.color = reinterpret_cast<const float*>(&Colors::Green);
					newState.verIndiex = plantData.mVertexs.size();
					newState.trunkScal = curState.trunkScal*mParam.mRadiusRate;

					plantData.mIndices.push_back(curState.verIndiex);
					plantData.mIndices.push_back(newState.verIndiex);
					plantData.mVertexs.push_back(newVer);

					//--trunk--每前进一步生成一个树干
					GenerateTrunk(plantData.mTrunks, curState, step);

					//生成叶子
					if (Lstring[i + 1] == ']' || depth > mParam.mIterations - 1)
					{
						GenerateLeaf(plantData.mLeaves, curState, newState, step, plantData.mTrunks[plantData.mTrunks.size()-1],i);
					}

					curState = newState;
					break;
		}
		case 'x':
		{
					RoteDirection('x',Lstring[++i], curState.v);
					break;
		}
		case 'y':
		{
					RoteDirection('y', Lstring[++i], curState.v);
					break;
		}
		case 'z':
		{
					RoteDirection('z', Lstring[++i], curState.v);
					break;
		}
		case '[':
			stateStack.push(curState);	
			totalStepAtt *= mParam.mStepAtt;
			curState.trunkScal *= mParam.mTrunkSizeAtt;
			depth++;
			break;
		case ']':
			curState = stateStack.top();
			stateStack.pop();
			totalStepAtt /= mParam.mStepAtt;
			depth--;
			break;
		default:	
			break;
		}
	}
}

void LSystem::ToSun(XMVECTOR& V, float factor)
{
	XMVECTOR toLight = XMVector3Normalize(XMVectorSet(-1.0f, 1.0f, 0.0f, 1.0f));
	V += toLight*factor*0.2f;
}

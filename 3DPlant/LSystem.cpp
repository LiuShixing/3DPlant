#include "stdafx.h"
#include "LSystem.h"


LSparameter::LSparameter() :mIsTrunk(1)
, mIsLeave(1), mIsToSun(true)
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
	std::vector<std::string> vs;
	std::string defaultRule("F[z+x-X][z-x-X][x+X]");
//	std::string defaultRule("Fz-[[X]z+X]z+F[z+FX]z-X");

	vs.push_back(defaultRule);
	vs.push_back("F[z+x-X][z-x-X]");
	vs.push_back("F[z+x-X][x+X]");
	vs.push_back("F[z-x-X][x+X]");
	vs.push_back("F[z+x-X]");
	mRules['X'] = vs;
	mStart = 'X';

//	std::vector<std::string> tmp;
//	tmp.push_back("FF");
//	mRules['F'] = tmp;

	srand(time(NULL));
}

std::string LSparameter::GetRandomRule(char key)
{
	std::map<char, std::vector<std::string> >::iterator it = mRules.find(key);
	if (it != mRules.end())
	{
		int length = (it->second).size();
		int index = rand() % length;
		return (it->second)[index];
	}
	std::string s("");
	s += key;
	return s;
}

float LSparameter::GetRandomStep(float att)
{
	float x = (float)rand() / (float)(RAND_MAX + 1);
//	float tmin = mStepMin - att > 0.0f ? mStepMin - att : 0.5f;
//	float tmax = mStepMax - att > 0.0f ? mStepMax - att : 0.5f;
//	return tmin + (tmax - tmin) * x;
	return (mStepMin + (mStepMax - mStepMin) * x)*att;
}

float LSparameter::GetRandomAngle()
{
	float x = (float)rand() / (float)(RAND_MAX + 1);
	return (mRotAngleMin + (mRotAngleMax - mRotAngleMin) * x)*XM_PI / 180.0f;
}

LSystem::LSystem() 
{
}


LSystem::~LSystem()
{
}

void LSystem::CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, LSparameter& param)
{
	vertexs.clear();
	indices.clear();
	mTrunks.clear();
	mLeaves.clear();

	//生成最终字符串
	std::string plantStr = param.GetRandomRule(param.mStart);

	UINT iterations = param.mIterations;
	while (--iterations)
	{
		std::string tmpstr = "";
		for (int i = 0; i < plantStr.size(); i++)
		{
			tmpstr += param.GetRandomRule(plantStr[i]);
		}
		plantStr = tmpstr;
	}

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
	vertexs.push_back(orinVer);

	//步长衰减
	float totalStepAtt = 1.0f;

	//for trunk size
//	float trunkScal = 1.0f;
//	float trunkScalFact = param.mRadiusRate;

	//记录深度
	int depth = 1;
	
	State curState = orinState;
	std::stack<State> stateStack; 
	for (int i = 0; i < plantStr.size(); i++)
	{
		switch (plantStr[i])
		{
		case 'F':
		{
					State newState;
					newState.v = curState.v;
					XMVECTOR OPOS = XMLoadFloat3(&curState.pos);
					XMVECTOR V = XMVector3Normalize(XMLoadFloat3(&curState.v));
					
					float step = param.GetRandomStep(totalStepAtt);
					XMVECTOR NPOS = OPOS + V*step;
					XMStoreFloat3(&newState.pos, NPOS);

					Vertex::PosColor newVer;
					newVer.pos = newState.pos;
					newVer.color = reinterpret_cast<const float*>(&Colors::Green);
					newState.verIndiex = vertexs.size();
					newState.trunkScal = curState.trunkScal*param.mRadiusRate;

					indices.push_back(curState.verIndiex);
					indices.push_back(newState.verIndiex);

					//--trunk--每前进一步生成一个树干
					Trunk trunk;
					trunk.pos = curState.pos;

					//计算树干的最终旋转轴，旋转角度
					XMVECTOR Y = XMVector3Normalize(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));//树干起始方向向量
					XMVECTOR DOT = XMVector3Dot(V, Y);
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
						XMVECTOR DIR = XMVector3Normalize(XMVector3Cross(Y, V));
						XMStoreFloat3(&trunk.rotAxis, DIR);
					}
					
					//树干整体缩放和长度缩放
					trunk.sizeScal = curState.trunkScal;
					trunk.scalY = step / param.mStepMax;
					mTrunks.push_back(trunk);
				//	curState.trunkScal *= param.mRadiusRate;

					//生成叶子
					if (depth > param.mIterations - 1)
					{
						param.mLeaveSize = 1.0f;
						Leave leave1;
						leave1.pos.x = (curState.pos.x + newState.pos.x) / 2.0f;
						leave1.pos.y = (curState.pos.y + newState.pos.y) / 2.0f;
						leave1.pos.z = (curState.pos.z + newState.pos.z) / 2.0f;
						leave1.rotAxis = trunk.rotAxis;
						leave1.angle = trunk.angle;
						leave1.scal = param.mLeaveSize;

						//两对叶子
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);

						Leave leave2;
						leave2.pos = newState.pos;
						leave2.rotAxis = trunk.rotAxis;
						leave2.angle = trunk.angle;
						leave2.scal = param.mLeaveSize;

						//两对叶子
						mLeaves.push_back(leave2);
						mLeaves.push_back(leave2);
						mLeaves.push_back(leave2);
						mLeaves.push_back(leave2);
					}

					vertexs.push_back(newVer);
					curState = newState;
					break;
		}
		case 'x':
		{
					float tmpa = param.GetRandomAngle();
					float angle = plantStr[++i] == '+' ? tmpa : -tmpa;

					XMVECTOR OldV = XMLoadFloat3(&curState.v); 
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationX(angle)));
					
					if (param.mIsToSun)
						ToSun(NewV, param.mSunFactor);

					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'y':
		{
					float tmpa = param.GetRandomAngle();
					float angle = plantStr[++i] == '+' ? tmpa : -tmpa;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationY(angle)));
					if (param.mIsToSun)
						ToSun(NewV, param.mSunFactor);
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'z':
		{
					float tmpa = param.GetRandomAngle();
					float angle = plantStr[++i] == '+' ? tmpa : -tmpa;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationZ(angle)));
					if (param.mIsToSun)
						ToSun(NewV, param.mSunFactor);
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case '[':
			stateStack.push(curState);	
			totalStepAtt *= param.mStepAtt;
			curState.trunkScal *=param.mTrunkSizeAtt;
			depth++;
			break;
		case ']':
			curState = stateStack.top();
			stateStack.pop();
			totalStepAtt /= param.mStepAtt;
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

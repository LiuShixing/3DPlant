#include "stdafx.h"
#include "LSystem.h"


LSparameter::LSparameter()
{
	mIterations = 3;
	mStepMin = 1.0f;
	mStepMax = 5.0f;
	mRotAngleMin = (XM_PI*5.0f) / 36.0f;
	mRotAngleMax = XM_PI / 3.0f;
	mTrunkSize = 1.0f;

	//默认规则
	std::vector<std::string> vs;
	std::string defaultRule("F[z+x-X][z-x-X][x+X]");
	vs.push_back(defaultRule);
	vs.push_back("F[z+x-X][z-x-X]");
	vs.push_back("F[z+x-X][x+X]");
	vs.push_back("F[z-x-X][x+X]");
	vs.push_back("F[z+x-X]");
	mRules['X'] = vs;
	mStart = 'X';

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
	float tmin = mStepMin - att > 0.0f ? mStepMin - att : mStepMin;
	float tmax = mStepMax - att > 0.0f ? mStepMax - att : mStepMin;
	return tmin + (tmax - tmin) * x;
}

float LSparameter::GetRandomAngle()
{
	float x = (float)rand() / (float)(RAND_MAX + 1);
	return mRotAngleMin + (mRotAngleMax - mRotAngleMin) * x;
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

	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;

	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	vertexs.push_back(orinVer);

	//步长衰减
	float stepDelta = 0.5f;
	float stepAtt = 0.0f;

	State curState = orinState;
	std::stack<State> stateStack; std::cout << std::endl;
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
								
					XMVECTOR NPOS = OPOS + V*param.GetRandomStep(stepAtt);
					XMStoreFloat3(&newState.pos, NPOS);

					Vertex::PosColor newVer;
					newVer.pos = newState.pos;
					newVer.color = reinterpret_cast<const float*>(&Colors::Green);
					newState.verIndiex = vertexs.size();

					indices.push_back(curState.verIndiex);
					indices.push_back(newState.verIndiex);

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
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'y':
		{
					float tmpa = param.GetRandomAngle();
					float angle = plantStr[++i] == '+' ? tmpa : -tmpa;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationY(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'z':
		{
					float tmpa = param.GetRandomAngle();
					float angle = plantStr[++i] == '+' ? tmpa : -tmpa;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationZ(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case '[':
			stateStack.push(curState);	
			stepAtt += stepDelta;
			break;
		case ']':
			curState = stateStack.top();
			stateStack.pop();
			stepAtt -= stepDelta;
			break;
		default:	
			break;
		}
	}
}

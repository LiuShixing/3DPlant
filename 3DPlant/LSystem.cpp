#include "stdafx.h"
#include "LSystem.h"


LSparameter::LSparameter()
{
	mIterations = 3;
	mStepMin = 1.0f;
	mStepMax = 5.0f;
	mRotAngleMin = (XM_PI*5.0f) / 36.0f;
	mRotAngleMax = XM_PI / 3.0f;
	mTrunkSize = 0.1f;

	//Ĭ�Ϲ���
	std::vector<std::string> vs;
	std::string defaultRule("F[z+x-X][z-x-X][x+X]");
//	std::string defaultRule("F[z+X][X]");
	vs.push_back(defaultRule);
//	vs.push_back("F[z+x-X][z-x-X]");
//	vs.push_back("F[z+x-X][x+X]");
//	vs.push_back("F[z-x-X][x+X]");
//	vs.push_back("F[z+x-X]");
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
	mTrunks.clear();
	mLeaves.clear();

	//���������ַ���
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

	//���ó�ʼ״̬
	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;

	//�����ʼ����
	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	vertexs.push_back(orinVer);

	//����˥��
	float stepDelta = 0.5f;
	float stepAtt = 0.0f;

	//for trunk size
	float trunkScal = 1.0f;
	float trunkScalFact = 0.8f;

	//��¼���
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
					
					float step = param.GetRandomStep(stepAtt);
					XMVECTOR NPOS = OPOS + V*step;
					XMStoreFloat3(&newState.pos, NPOS);

					Vertex::PosColor newVer;
					newVer.pos = newState.pos;
					newVer.color = reinterpret_cast<const float*>(&Colors::Green);
					newState.verIndiex = vertexs.size();

					indices.push_back(curState.verIndiex);
					indices.push_back(newState.verIndiex);

					//--trunk--ÿǰ��һ������һ������
					Trunk trunk;
					trunk.pos = curState.pos;

					//�������ɵ�������ת�ᣬ��ת�Ƕ�
					XMVECTOR Y = XMVector3Normalize(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));//������ʼ��������
					XMVECTOR DOT = XMVector3Dot(V, Y);
					XMFLOAT3 DotTmp;
					XMStoreFloat3(&DotTmp, DOT);
					float dot = DotTmp.x;
					if (fabs(dot - 1.0f) < 0.000001f)
					{//���շ������ʼ������ͬ
						trunk.rotAxis = XMFLOAT3(0.0f, 0.1f, 0.0f);
						trunk.angle = 0.0f;
					}
					else
					{
						trunk.angle = acosf(dot);
						XMVECTOR DIR = XMVector3Normalize(XMVector3Cross(Y, V));
						XMStoreFloat3(&trunk.rotAxis, DIR);
					}
					

					//�����������źͳ�������
					trunk.sizeScal = trunkScal;
					trunk.scalY = step / param.mStepMax;
					mTrunks.push_back(trunk);

					//����Ҷ��
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

						//����Ҷ��
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);
						mLeaves.push_back(leave1);

						Leave leave2;
						leave2.pos = newState.pos;
						leave2.rotAxis = trunk.rotAxis;
						leave2.angle = trunk.angle;
						leave2.scal = param.mLeaveSize;

						//����Ҷ��
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
			trunkScal = trunkScal*trunkScalFact;
			depth++;
			break;
		case ']':
			curState = stateStack.top();
			stateStack.pop();
			stepAtt -= stepDelta;
			trunkScal = trunkScal/trunkScalFact;
			depth--;
			break;
		default:	
			break;
		}
	}
	std::cout <<"indices.size "<< indices.size() << std::endl;
	std::cout << "mTrunks.size " << mTrunks.size() << std::endl;
}

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

	//Ĭ�Ϲ���
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
	//������keyΪ�󲿵Ĺ��򼯺�
	std::map<char, std::vector<RuleRight> >::iterator it = mRules.find(key);
	if (it != mRules.end())
	{//�ҵ�����

		//ʹ�����̶��㷨���ӹ��򼯺�ѡ��һ������

		//������ʱ�ֵ���ܺ�
		float total = 0.0f;
		for (int i = 0; i < (it->second).size(); i++)
			total += (it->second)[i].probability;

		//���ѡ��ֵֹͣ
		float x = (float)rand() / (float)(RAND_MAX + 1);
		float stop = x*total-0.1f;

		//����ת���̡����ۼӸ���ֱ������ֵֹͣ
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
		//����ѡ�й�����Ҳ��ַ���
		return (it->second)[index].production;
	}
	//û�ҵ����򣬷���key����
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

	//�������ɵ�������ת�ᣬ��ת�Ƕ�
	XMVECTOR Y = XMVector3Normalize(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));//������ʼ��������
	XMVECTOR DOT = XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&curState.v)), Y);
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
		XMVECTOR DIR = XMVector3Normalize(XMVector3Cross(Y, XMVector3Normalize(XMLoadFloat3(&curState.v))));
		XMStoreFloat3(&trunk.rotAxis, DIR);
	}

	//�����������źͳ�������
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
		//����Ҷ��
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
		//֦�˵�Ҷ��
		leaf.pos = newState.pos;
		leaf.rotY = rotY[i % 2][j % 2];
		leaves.push_back(leaf);
	}
	else if (mParam.mLeafOrder == 1)
	{
		//����Ҷ��
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

			//�����Ƭ
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
		//����Ҷ��
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

	//���������ַ���
	std::string Lstring = GenerateLstring();

	//���ó�ʼ״̬
	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;
	orinState.trunkScal = 1.0f;

	//�����ʼ����
	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	plantData.mVertexs.push_back(orinVer);

	//����˥��
	float totalStepAtt = 1.0f;

	//��¼���
	int depth = 1;

	//��L�ַ���ĩβ��]
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

					//--trunk--ÿǰ��һ������һ������
					GenerateTrunk(plantData.mTrunks, curState, step);

					//����Ҷ��
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

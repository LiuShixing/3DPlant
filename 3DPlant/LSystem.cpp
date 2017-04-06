#include "stdafx.h"
#include "LSystem.h"


LSystem::LSystem() :mIterations(6), ma(0.0f), md(0.0f)
{
	mProduction = "F[xyzP]xyzP";
}


LSystem::~LSystem()
{
}

void LSystem::CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, int iterations)
{
	vertexs.clear();
	indices.clear();

	//mProduction = "Fz-[[X]z+X]z+F[z+FX]z-X";
	mProduction = "F[z+x-X][z-x-X][x+X]";
	std::string plantStr = mProduction;
	while (--iterations)
	{
		std::string tmpstr = "";
		for (int i = 0; i < plantStr.size(); i++)
		{
			char ch = plantStr[i];
			if (ch == 'X')
			{
				tmpstr += mProduction;
			}
			else
			{
				tmpstr += ch;
			}
		}
		plantStr = tmpstr;
	}
//	std::cout << plantStr << std::endl;


	std::queue<State> stateQue;
	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;

	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	vertexs.push_back(orinVer);

	stateQue.push(orinState);


	md = 2.0f;
	ma = (XM_PI*5.0f) / 36.0f;

	//	plantStr = "F[z+x-F][z-x-F]x+F";
	int count = 0;
	float stepDelta = 0.5f;
	State curState = orinState;
	std::stack<State> stateStack; std::cout << std::endl;
	for (int i = 0; i < plantStr.size(); i++)
	{
		
	//	std::cout << plantStr[i] << std::endl;;
		switch (plantStr[i])
		{
		case 'F':
		{
			//		std::cout << "F" << std::endl;
					State newState;
					newState.v = curState.v;
					XMVECTOR OPOS = XMLoadFloat3(&curState.pos);
					XMVECTOR V = XMVector3Normalize(XMLoadFloat3(&curState.v));

					
					
					XMVECTOR NPOS = OPOS + V*md;
					XMStoreFloat3(&newState.pos, NPOS);
				//	std::cout << newState.pos.x << " " << newState.pos.y << " " << newState.pos.z << std::endl;
					count++;

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
				//	std::cout << "x" << std::endl;
					float angle = plantStr[++i] == '+' ? ma : -ma;

					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationX(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}

		case 'y':
		{
				//	std::cout << "y" << std::endl;
					float angle = plantStr[++i] == '+' ? ma : -ma;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationY(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'z':
		{
				//	std::cout << "z" << std::endl;
					float angle = plantStr[++i] == '+' ? ma : -ma;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationZ(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case '[':
		//	std::cout << "[" << std::endl;
			stateStack.push(curState);
			
			md -= stepDelta;
			md = md > 0 ? md : stepDelta;
			break;
		case ']':
		//	std::cout << "]" << std::endl;
			curState = stateStack.top();
			stateStack.pop();
			md += stepDelta;
			break;
		default:
			
			break;
		}
	}
	//indices.erase(indices.begin(), indices.begin() + 86);
/*
	int i = 0;
	while (i<indices.size())
	{
		std::cout << i<<" "<<indices[i] << " " << indices[i+1] << std::endl;
		i += 2;
 	}*/
	std::cout << "vertex count = " << vertexs.size() << std::endl;
	std::cout << "indices count = " << indices.size() << std::endl;
	std::cout <<"count = "<<count<< std::endl;

	
}
/*
void LSystem::CreatePlant(std::vector<Vertex::PosColor>& vertexs, std::vector<UINT>& indices, int iterations)
{
	vertexs.clear();
	indices.clear();

	mProduction = "F[z+x-F][z-x-F]x+F";
	//mProduction = "F[z++F";
	std::string plantStr = mProduction;
	while (--iterations)
	{
		std::string tmpstr = "";
		for (int i = 0; i < plantStr.size(); i++)
		{
			char ch = plantStr[i];
			if (ch == 'F')
			{
				tmpstr += mProduction;
			}
			else
			{
				tmpstr += ch;
			}
		}
		plantStr = tmpstr;
	}
	std::cout << plantStr << std::endl;


	std::queue<State> stateQue;
	State orinState;
	orinState.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	orinState.v = XMFLOAT3(0.0f, 1.0f, 0.0f);
	orinState.verIndiex = 0;

	Vertex::PosColor orinVer;
	orinVer.pos = orinState.pos;
	orinVer.color = reinterpret_cast<const float*>(&Colors::Green);
	vertexs.push_back(orinVer);

	stateQue.push(orinState);
	int	n = iterations;

	md = 2.0f;
	ma = XM_PI / 6;

//	plantStr = "F[z+x-F][z-x-F]x+F";
	
	State curState = orinState;
	std::stack<State> stateStack;
	for (int i = 0; i < plantStr.size(); i++)
	{
		switch (plantStr[i])
		{
		case 'F':
		{std::cout << "F" << std::endl;
					State newState;
					newState.v = curState.v;
					XMVECTOR OPOS = XMLoadFloat3(&curState.pos);
					XMVECTOR V = XMVector3Normalize(XMLoadFloat3(&curState.v));

					std::cout << curState.v.x << " "<<curState.v.y << " " << curState.v.z << std::endl;
					XMVECTOR NPOS = OPOS + V*md;
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
					std::cout << "x" << std::endl;
            		float angle = plantStr[++i] == '+' ? ma : -ma;
			
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationX(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
				
		case 'y':
		{
					std::cout << "y" << std::endl;
					float angle = plantStr[++i] == '+' ? ma : -ma;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationY(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case 'z':
		{
					std::cout << "z" << std::endl;
					float angle = plantStr[++i] == '+' ? ma : -ma;
					XMVECTOR OldV = XMLoadFloat3(&curState.v);
					XMVECTOR NewV = XMVector3Normalize(XMVector3Transform(OldV, XMMatrixRotationZ(angle)));
					XMStoreFloat3(&curState.v, NewV);
					break;
		}
		case '[':
			stateStack.push(curState);
			break;
		case ']':
			curState = stateStack.top();
			stateStack.pop();
			break;
		default:
			break;
		}
	}
	
}
*/
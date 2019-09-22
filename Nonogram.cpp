#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream> 
#include <conio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <set>
#include <vector>


using namespace std;
#define SIZE 100

struct Node
{
	int iRow;
	int iCol;
	double dValue;
};

bool NodeValueCompare(Node a, Node b)
{
	return a.dValue > b.dValue;
}

class NonoFill
{
	public:
		int lstInfo[51][13];
		int iInfos[51];
		int iCheck[51][13];
		int iState[26][26];
		set<int> PGNewNode;
		vector<Node> ChoiceNode;
	
		NonoFill();
		bool Propagate();
		void FP();
		void backtracking();
		string Paint(int,int,int);
		string Paint0(int,int,int);
		string Paint1(int,int,int);
		string Merge(string,string);
		bool Fix(int,int,int);
		bool Fix0(int,int,int);
		bool Fix1(int,int,int);
		void reset();
		double ChoiceValue(int,int);
};

int main()
{
	char cLine[SIZE];
	fstream fInputFile;
	NonoFill NonoSol;

	int iSolution[26][26] = {0};
	
	clock_t tStart, tEnd;
	
	fInputFile.open("Nonogram_Input_Data.txt",ios::in);
	int iQues = 0;
	int iListNum = 0;
	int iCount = 0;
	int iInfo = 0;
	int iColMax = 0;
	int iRowMax = 0;
	
	double dTotalTime = 0;
	
//		read File
	while(iQues <= 999)
    {
    	fInputFile.getline(cLine,sizeof(cLine),'\n');
        if(cLine[0] == '$')
        {
        	iQues++;
        	iListNum = 1;
			iCount = 1;
			NonoSol.reset();
		}
		if(iListNum > 50)
			continue;
		char * cGet;
		while(iListNum <= 50)
		{
			fInputFile.getline(cLine,sizeof(cLine),'\n');
			cGet = strtok(cLine, "	");
			while(cGet != NULL)
			{
				string sGet = cGet;
				stringstream get(sGet);
				get >> iInfo;
				NonoSol.lstInfo[iListNum][iCount] = iInfo;
				NonoSol.iCheck[iListNum][iCount] = NonoSol.iCheck[iListNum][iCount-1] + iInfo + 1;
				cGet = strtok(NULL,  "	");	
				iCount++;
			}
			NonoSol.iInfos[iListNum] = iCount-1;
			if(iCount-1 > iColMax && iListNum <= 25)
				iColMax = iCount-1;
			else if(iCount-1 > iRowMax && iListNum > 25)
				iRowMax = iCount-1;
			iListNum++;
			iCount = 1;
		}
		tStart = clock();
		
//------Fill state----------------
		NonoSol.Propagate();
		NonoSol.FP();
		NonoSol.backtracking();
		
		
		tEnd = clock();
//------output File---------------
		ofstream outFile;
		stringstream ssName;
		string sFileName;
		
		ssName.str(""); 
		ssName << "Solution/Nonogram_Solution_" << iQues << ".txt"; 
		sFileName = ssName.str(); 
		outFile.open(sFileName.c_str(),ios::out);
		
		for(int i = 1;i <= iColMax;i++)
		{
			outFile << setw(iRowMax*4+1) << "";
			for(int j = 1;j <= 25;j++)
			{
				if(NonoSol.lstInfo[j][i] != 0)
					outFile << setw(4) << NonoSol.lstInfo[j][i];
				else
					outFile << setw(4) << "";
			}
			outFile << endl;
		}
		outFile << setw(iRowMax*4 + 101) << "\\----------------------------------------------------------------------------------------------------" << endl;
		
		for(int i = 26;i <= 50;i++)
		{
			for(int j = 1;j <= iRowMax;j++)
			{
				if(NonoSol.lstInfo[i][j] != 0)
					outFile << setw(4) << NonoSol.lstInfo[i][j];
				else
					outFile << setw(4) << "";
			}
			outFile << "|";
			for(int c = 1;c <= 25;c++)
			{
				if(NonoSol.iState[c][i-25] == 1)
					outFile << setw(4) << "1";
				else if(NonoSol.iState[c][i-25] == 0)
					outFile << setw(4) << "0";
				else
					outFile << setw(4) << "U";
			}
			outFile << endl;
		}
		outFile.close();
		
		dTotalTime += (tEnd - tStart) / (double)(CLOCKS_PER_SEC);
		if(dTotalTime < 60)
			cout << setw(3) << iQues << "  Cost: " << setiosflags(ios::fixed) << setprecision(3) << setw(7) << (tEnd - tStart) / (double)(CLOCKS_PER_SEC) << " s,    Total: " << setw(13) << dTotalTime << " s" <<  endl;
    	else
    		cout << setw(3) << iQues << "  Cost: " << setiosflags(ios::fixed) << setprecision(3) << setw(7) << (tEnd - tStart) / (double)(CLOCKS_PER_SEC) << " s,    Total: " << setw(3) << (int)dTotalTime / 60 << " m " << setw(7) << dTotalTime - ((int)dTotalTime / 60 * 60) << " s"  <<  endl;
	}
	cout << "Finish" << endl;

	string sCommand = "";
	cout << "Enter '0' to exit" <<endl;
    while(sCommand != "0")
    {
    	sCommand = getch();
	}
    return 0;
}

NonoFill::NonoFill()
{
	for(int i = 1;i < 51;i++)
	{
		iInfos[i] = 0;
		iCheck[i][0] = -1;
		for(int j = 0;j < 13;j++)
		{
			lstInfo[i][j] = 0;
		}
	}
	for(int c = 1;c < 26;c++)
	{
		for(int r = 1;r < 26;r++)
			iState[c][r] = 2;
	}
}

string NonoFill::Paint(int i,int j,int iNum)
{
	if(i == 0)
		return "";
	string sResult = "";
	bool bF0 = Fix0(i,j,iNum);
	bool bF1 = Fix1(i,j,iNum);
	if(bF0 && !bF1)
		sResult = Paint0(i,j,iNum);
	else if(bF1 && !bF0)
		sResult = Paint1(i,j,iNum);
	else
		sResult = Merge(Paint0(i,j,iNum),Paint1(i,j,iNum));
	return sResult;
}

string NonoFill::Paint0(int i,int j,int iNum)
{
	string sResult = Paint(i-1,j,iNum) + "0";
	return sResult;
}

string NonoFill::Paint1(int i,int j,int iNum)
{
	string sResult = "";
	for(int count = 0;count < lstInfo[iNum][j];count++)
	{
		sResult = sResult + "1";
	}
	int iNew = i-lstInfo[iNum][j]-1;
	if(i > lstInfo[iNum][j])
		sResult = Paint(iNew,j-1,iNum) + "0" + sResult;
	return sResult;
}

string NonoFill::Merge(string s1,string s2)
{
	string sResult = "";
	for(int i = 0;i < s1.length();i++)
	{
		if(s1[i] == s2[i])
			sResult = sResult + s1[i];
		else
			sResult = sResult + "2";
	}
	return sResult;
}

bool NonoFill::Fix(int i,int j,int iNum)
{
	if(i == 0 && j== 0)
		return true;
	else if(i == 0 && j >= 1)
		return false;
	else if(i < iCheck[iNum][j])
		return false;
	else
		return (Fix0(i,j,iNum) || Fix1(i,j,iNum));
}

bool NonoFill::Fix0(int i,int j,int iNum)
{
	if(iNum <= 25)
	{
		if(iState[iNum][i] != 1)
		{
			return Fix(i-1,j,iNum);
		}
	}	
	else if(iNum > 25)
	{
		if(iState[i][iNum - 25] != 1)
		{
			return Fix(i-1,j,iNum);
		}
	}
	return false;
}

bool NonoFill::Fix1(int i,int j,int iNum)
{
	if(j >= 1 && i >= lstInfo[iNum][j])
	{
		for(int count = 0;count < lstInfo[iNum][j];count++)
		{
			if(iNum <= 25)
			{
				if(iState[iNum][i-count] == 0)
				{
					return false;
				}
			}
			else if(iNum > 25)
			{
				if(iState[i-count][iNum - 25] == 0)
				{
					return false;
				}
			}
		}
		if(i > lstInfo[iNum][j])
		{
			if(iNum <= 25)
			{
				if(iState[iNum][i - lstInfo[iNum][j]] != 1)
				{
					return Fix(i - lstInfo[iNum][j] - 1, j-1,iNum);
				}
				else
				{
					return false;
				}
			}
			else if(iNum > 25)
			{
				if(iState[i - lstInfo[iNum][j]][iNum - 25] != 1)
				{
					return Fix(i - lstInfo[iNum][j] - 1, j-1,iNum);
				}
				else
				{
					return false;
				}
			}
		}
		else if(i == lstInfo[iNum][j])
		{
			return Fix(0, j-1,iNum);
		}
	}
	return false;
}

bool NonoFill::Propagate()
{
	int iSolve = 0;
	set<int> siChange;
	set<int>::iterator itChange;
	string L[51] = "2222222222222222222222222";
	L[0] = "";
	int iNum = 1;
	int iLoop = 0;
	
	PGNewNode.clear();
	while(iSolve < 50)
	{
		if(iNum > 50)
			iNum = 1;
		if(L[iNum].find('2') < 0 || L[iNum].find('2') > 25)
		{
			iNum++;
			continue;
		}
		if(iLoop > 50) break;
		if(!Fix(25,iInfos[iNum],iNum))
		{
			//cout << "Not Fix:" << iNum << endl;
			return false;
		}
		L[iNum] = Paint(25,iInfos[iNum],iNum);
		
		for(int i = 1;i <= 25;i++)
		{
			if(((int)L[iNum][i-1] - 48) != 2)
			{
				if(iNum <= 25)		//col
				{
					if(iState[iNum][i] != ((int)L[iNum][i-1] - 48))
					{
						iState[iNum][i] = (int)L[iNum][i-1] - 48;
						if(siChange.count(i+25) == 0)
							siChange.insert(i+25);
						iLoop = 0;
						PGNewNode.insert(iNum*100+i);
					}
				}
				else if(iNum > 25)	//row
				{
					if(iState[i][iNum-25] != ((int)L[iNum][i-1] - 48))
					{
						iState[i][iNum-25] = (int)L[iNum][i-1] - 48;
						if(siChange.count(i) == 0)
							siChange.insert(i);
						iLoop = 0;
						PGNewNode.insert(i*100+iNum-25);
					}
				}
			}
		}
		if(L[iNum].find('2') < 0 || L[iNum].find('2') > 25)
		{
			//cout << L[iNum] << "/" << iSolve << "/" << iNum << endl;
			iSolve++;
		}
		if(siChange.size() == 0)
			iNum++;
		else
		{
//			for(int i = 1;i <= 50;i++)
//			{
//				if(siChange.count(i) == 1)
//				{
//					iNum = i;
//					siChange.erase(i);
//					break;
//				}
//			}
			itChange = siChange.begin();
			iNum = *itChange;
			siChange.erase(*itChange);
		}
		iLoop++;
	}
	return true;
}

void NonoFill::FP()
{
	bool bPG0 = false;
	bool bPG1 = false;
	bool bLoop = true;
	set<int> siPG0;
	set<int> siPG1;
	set<int>::iterator itNode;
	
	Node Unode;
	int oriState[26][26];
	int s1State[26][26];
	int iNodes0 = 0;
	int iNodes1 = 0;
	
	ChoiceNode.clear();
	memcpy(oriState, iState, sizeof(iState));	//save original state
	for(int c = 1;c <= 25;c++)
	{
		for(int r = 1;r <= 25;r++)
		{
			if(iState[c][r] == 2)
			{
				iState[c][r] = 1;
				bPG1 = Propagate();
				siPG1 = PGNewNode;
				memcpy(s1State, iState, sizeof(iState));
				memcpy(iState, oriState, sizeof(iState));
				iState[c][r] = 0;
				bPG0 = Propagate();
				siPG0 = PGNewNode;
			
				if(bPG1 && !bPG0)
				{
					memcpy(iState, s1State, sizeof(iState));
					memcpy(oriState, iState, sizeof(iState));
				}
				else if(bPG0 && !bPG1)
				{
					memcpy(oriState, iState, sizeof(iState));
				}
				else if(!bPG1 && !bPG0)
				{
						cout << "CONFLICT" << endl;
						return;
				}
				else
				{
					iNodes1 = siPG1.size();
					iNodes0 = siPG0.size();
					Unode.dValue = ChoiceValue(iNodes1, iNodes0);
					Unode.iCol = c;
					Unode.iRow = r;
					ChoiceNode.push_back(Unode);

					if(siPG0.size() < siPG1.size())
					{
						for(itNode = siPG0.begin();itNode != siPG0.end();itNode++)
						{
							if(siPG1.count(*itNode) == 1)
							{
								int col = *itNode / 100;
								int row = *itNode % 100;
								if(iState[col][row] == s1State[col][row])
									oriState[col][row] = iState[col][row];
							}
						}
					}
					else
					{
						for(itNode = siPG1.begin();itNode != siPG1.end();itNode++)
						{
							if(siPG0.count(*itNode) == 1)
							{
								int col = *itNode / 100;
								int row = *itNode % 100;
								if(iState[col][row] == s1State[col][row])
									oriState[col][row] = iState[col][row];
							}
						}
					}
					memcpy(iState, oriState, sizeof(iState));
				}
				//cout << c << "/" << r << ":" << iState[c][r] << endl;
			}
		}
	}
//	int uCount = 0;
//	for(int i = 1;i <= 25;i++)
//	{
//		for(int j = 1;j <= 25;j++)
//		{
//			if(iState[i][j] == 2)
//				uCount++;
//		}
//	}
	//cout << "FP Total Fill: " << 256 - uCount << endl;
}

void NonoFill::backtracking()
{
	bool bPG0 = false;
	bool bPG1 = false;
	bool bLoop = true;
	int iSPCount = 0;
	int iPG0Nodes = 0;
	int iPG1Nodes = 0;
	
	int oriState[26][26];
	int s1State[26][26];
	int spCase[625][26][26];
	vector<int> spNode;
	
	memcpy(oriState, iState, sizeof(iState));	//save original state
	int iChoiceNum = 0;
	int c = 1;
	int r = 1;
	sort(ChoiceNode.begin(), ChoiceNode.end(), NodeValueCompare);
	
	while(bLoop)
	{
		c = ChoiceNode[iChoiceNum].iCol;
		r = ChoiceNode[iChoiceNum].iRow;
		if(iState[c][r] == 2)
		{
			iState[c][r] = 1;
			bPG1 = Propagate();
			memcpy(s1State, iState, sizeof(iState));
			iPG1Nodes = PGNewNode.size();
			
			memcpy(iState, oriState, sizeof(iState));
			iState[c][r] = 0;
			bPG0 = Propagate();
			iPG0Nodes = PGNewNode.size();
		
			if(bPG1 && !bPG0)
			{
				memcpy(iState, s1State, sizeof(iState));
				memcpy(oriState, iState, sizeof(iState));
			}
			else if(bPG0 && !bPG1)
			{
				memcpy(oriState, iState, sizeof(iState));
			}
			else if(!bPG1 && !bPG0)
			{
				if(iSPCount > 0)
				{
					memcpy(iState, spCase[iSPCount], sizeof(iState));
					memcpy(oriState, iState, sizeof(iState));
					iChoiceNum = spNode[iSPCount-1];
					spNode.pop_back();
					iSPCount--;
				}
				else
				{
					cout << "CONFLICT" << endl;
					return;
				}
			}
			else
			{
				iSPCount++;
				if(iPG1Nodes >= iPG0Nodes)
				{
					memcpy(spCase[iSPCount], iState, sizeof(iState));
					memcpy(iState, s1State, sizeof(iState));
					memcpy(oriState, iState, sizeof(iState));
				}
				else
				{
					memcpy(spCase[iSPCount], s1State, sizeof(iState));
					memcpy(oriState, iState, sizeof(iState));
				}
				spNode.push_back(iChoiceNum);
			}
			//cout << c << "/" << r << ":" << iState[c][r] << endl;
		}
		if(iChoiceNum < ChoiceNode.size()-1)
		{
			iChoiceNum++;
		}
		else
		{
			bLoop = false;
		}
	}
}

void NonoFill::reset()
{
	for(int i = 1;i < 51;i++)
	{
		iInfos[i] = 0;
		for(int j = 0;j < 13;j++)
		{
			lstInfo[i][j] = 0;
		}
	}
	for(int c = 0;c < 26;c++)
	{
		for(int r = 0;r < 26;r++)
			iState[c][r] = 2;
	}
}

double NonoFill::ChoiceValue(int Gone, int Gzero)
{
	/*-----Sum---------*/
//	return (Gone+Gzero);
	/*-----Min---------*/
//	if(Gone > Gzero)
//		return Gzero;
//	else
//		return Gone;
	/*-----Max---------*/
//	if(Gone < Gzero)
//		return Gzero;
//	else
//		return Gone;
	/*-----Mul---------*/
//	return ((Gone + 1) * (Gzero + 1));
	

	int Vmin;
	int Vmax;
	if(Gone > Gzero)
	{
		Vmin = Gzero;
		Vmax = Gone;
	}
	else
	{
		Vmin = Gone;
		Vmax = Gzero;
	}
	/*-----sqrt--------*/
//	return (Vmin + sqrt(Vmax / (Vmin + 1)));
	/*-----MinLogm-----*/
//	return (Vmin + ((log(1 + Gzero) + 1) * (log(1 + Gone) + 1)));
	/*-----MinLogd-----*/
	return (Vmin + fabs(log(1 + Gzero) - log(1 + Gone)));
}

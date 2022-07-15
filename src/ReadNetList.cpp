#include <iostream>
#include <fstream>
#include "type_define.h"
#include "ReadNetList.h"

using namespace std;
void ReadNetList(	const struct  InputParameter InputPara, 
					vector<vector<struct BumpAxis>> &netList,
					vector<int>& netGroupIndex){
    std::ifstream netListFile;
	netListFile.open(InputPara.netList, std::ios::in);
	
	if (!netListFile.is_open()) {
		std::cout << "NetList Open Fail!!!" << std::endl;
		cout << "please check netlist file and use the format: -n netListFileName" << endl;
		return;
	}

	vector<struct BumpAxis> tmpNet(3);
	int tmpNetGroupIndex;
	// tmpNet.resize(3);
	int index = -1;
	tmpNet[2].TOB_x = -1;
	while(!netListFile.eof()){
		netListFile >> tmpNet[0].dir >> tmpNet[0].TOB_x >> tmpNet[0].TOB_y >> tmpNet[0].bumpgrid_x >> tmpNet[0].bumpgrid_y;
		netListFile >> tmpNet[1].dir >> tmpNet[1].TOB_x >> tmpNet[1].TOB_y >> tmpNet[1].bumpgrid_x >> tmpNet[1].bumpgrid_y;
		netListFile >> tmpNetGroupIndex;

		if(tmpNet[1].TOB_x == -1){
			//ground
		}
		else if(tmpNet[1].TOB_x == -2){
			//power
		}
		else{
			//same wireList
			if(tmpNet[0].TOB_x == tmpNet[2].TOB_x && tmpNet[0].TOB_y == tmpNet[2].TOB_y && tmpNet[0].bumpgrid_x == tmpNet[2].bumpgrid_x && tmpNet[0].bumpgrid_y == tmpNet[2].bumpgrid_y){
				netList[index].push_back(tmpNet[1]);
			}
			else{//diff wireList
				index++;

				//netlist
				netList.resize(index + 1);
				netList[index].push_back(tmpNet[0]);
				netList[index].push_back(tmpNet[1]);
				
				//Sync lines index
				netGroupIndex.push_back(tmpNetGroupIndex);
			}
		}
		tmpNet[2] = tmpNet[0];
	}
}

/*void ChangeNetList(vector<vector<struct BumpAxis>>& netList,vector<chiplet> &chip)
{
	for (int i = 0; i < netList.size(); i++)
	{
		for (int j = 0; j < netList[i].size(); j++)
		{
			int a = netList[i][j].chip_x * 4 + netList[i][j].chip_y;
			netList[i][j].chip_x = chip[a].point.x;
			netList[i][j].chip_y = chip[a].point.y;
		}
		
	}
	
}*/
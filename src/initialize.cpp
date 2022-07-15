#include "initialize.h"
#include "ReadNetList.h"

//test
using namespace std;

void initializeNetList(
	const struct  InputParameter InputPara,
	const struct RoutingParameter setting, 
	vector<vector<struct BumpAxis>>& netList,
	vector<int>& netGroupIndex
	) {
	int netindex = 0;
	netList.resize(setting.bump_grid_width * setting.bump_grid_width);
	ReadNetList(InputPara, netList, netGroupIndex);
}

void initializeChipCoordinate(const struct RoutingParameter setting,
							  array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
							  array<vector<vector<struct Tob>>, 2>& toblist,
							  vector<vector<struct BumpAxis>>& netList) {
	int i = 0, j = 0, k = 0, dir = 0, x = 0, y = 0, index = 0;
	
	for (i = 0; i < netList.size(); i++) {// first fix the chip coordinate, in the final spec, chip coordinate should read from placement
		for (j = 0; j < netList[i].size(); j++) {
			dir = netList[i][j].dir;
			x = netList[i][j].TOB_x;
			y = netList[i][j].TOB_y;
			for (k = 0; k < setting.channel_width; k++) {
				trackdetail[dir][x][y][k].bumpgrid = true;
			}
		}
	}

	//initialize toblist
	for (dir = 0; dir < 2; dir++) {
		toblist[dir].resize(setting.channel_grid_width);
		for (x = 0; x < setting.channel_grid_width; x++) {
			toblist[dir][x].resize(setting.channel_grid_height);
			for (y = 0; y < setting.channel_grid_height; y++) {
				if (trackdetail[dir][x][y][0].bumpgrid) {
					toblist[dir][x][y].hori.resize(setting.channel_width);
					toblist[dir][x][y].vert.resize(setting.channel_width);
					toblist[dir][x][y].trac.resize(setting.channel_width);
					for (index = 0; index < setting.channel_width; index++) {
						toblist[dir][x][y].hori[index] = -1;
						toblist[dir][x][y].vert[index] = -1;
						toblist[dir][x][y].trac[index] = -1;
					}
				}
			}
		}
	}
}

void initializeTrack(const struct RoutingParameter setting,
					 array<vector<vector<vector<struct TrackDetail>>>, 2>& trackdetail) {
	// initialize track detail --------- [x(0) or y(1) direction][x axis][y axis][track index]
	int dir, x, y, z;
	//x axis trackdetail[0]
	// dir = 0;
	for(dir = 0; dir < 2; dir++){ //
		trackdetail[dir].resize(setting.channel_grid_width);
		for (x = 0; x < setting.channel_grid_width; x++) {
			trackdetail[dir][x].resize(setting.channel_grid_height);
			for (y = 0; y < setting.channel_grid_height; y++) {
				trackdetail[dir][x][y].resize(setting.channel_width);
				for(int index = 0; index < setting.channel_width; index++){
					trackdetail[dir][x][y][index].bumpgrid = false;
					if(dir == 0){
						trackdetail[dir][x][y][index].xaxis = true;
					} else {
						trackdetail[dir][x][y][index].xaxis = false;
					}

					trackdetail[dir][x][y][index].channel_x = x;
					trackdetail[dir][x][y][index].channel_y = y;
					trackdetail[dir][x][y][index].trackIndex = index;

					trackdetail[dir][x][y][index].cb = -1;
					trackdetail[dir][x][y][index].linkedbump = NULL;
					trackdetail[dir][x][y][index].cost = -1;
					trackdetail[dir][x][y][index].father = NULL;
					trackdetail[dir][x][y][index].intree = false;
					trackdetail[dir][x][y][index].searched = false;


					// cout << dir << " " << x << " " << y << "  " << trackdetail[dir][x][y][index].bumpgrid << endl;

					// trackdetail[dir][x][y][index].sb[0] = true;
					// trackdetail[dir][x][y][index].sb[1] = true;
				}
			}
		}
	}
}


void initializeSetting(struct RoutingParameter& setting) {
	
	//TOB
	setting.bump_grid_height = 16;
	setting.bump_grid_width = 8;

	//COB
	setting.cob_grid_height = 13;
	setting.cob_grid_width = 9;

	setting.cob_width = 128;
	setting.cob_cellNum = 16;
	setting.cob_cellWidth =setting.cob_width / setting.cob_cellNum;

	setting.channel_grid_height = setting.cob_grid_height;
	setting.channel_grid_width = setting.cob_grid_width;

	setting.channel_width = 128;
	
	setting.cobWilton =true;
}
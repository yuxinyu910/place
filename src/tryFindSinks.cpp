#include "tryFindSinks.h"
#include "findSyncNet.h"
bool checkallsinks(vector<bool> netcheck) {
	for (int i = 0; i < netcheck.size(); i++) {
		if (netcheck[i] == false) return false;
	}
	return true;
}

void pushtracktobump(vector<struct GeneralAxis>& tmpList,
					 const struct RoutingParameter setting,
					 struct TrackDetail& currenttrack,
					 array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
					 array<vector<vector<struct Tob>>, 2> & toblist,
					 vector<struct BumpAxis>& net,
					 vector<bool>& netcheck,
					 int maxStep,
					 vector<struct GeneralAxis>& tmpTree,
					 int currentindex) {
	//father is bump  do nothing
	//father is track , search sink , is ture? put cb true
	int tmpTreefatherindex = tmpTree[currentindex].tmpTreefather;
	if (tmpTree[tmpTreefatherindex].istrack) {
		//father is not bump  search sink
		if (currenttrack.bumpgrid) {
			// can connect to bump   1. define chip 2.define y
			//chip seq and channel seq same
			int curChip_dir = 0;
			if (currenttrack.xaxis) {
				curChip_dir = 0;
			}
			else {
				curChip_dir = 1;
			}
			int curChip_x = currenttrack.channel_x;
			int curChip_y = currenttrack.channel_y;
			for (int i = 1; i < net.size(); i++) {
				bool connect2Bump = net[i].TOB_x == curChip_x && net[i].TOB_y == curChip_y && curChip_dir == net[i].dir;
				//bool can2Bump = currenttrack.cbenabled && currenttrack.cb < 0;//current date strcut only support one track connect to one bummp
				//if (connect2Bump && net[i].cost < 0) {220409 xl edit
				if (connect2Bump && (maxStep == -1 || maxStep <= (tmpTree[currentindex].step + 1))) {
					int verttrackindex = currenttrack.trackIndex;
					int horitrackindex = -1;
					int the2M2index = -1;
					int bumpnum = net[i].bumpgrid_x + net[i].bumpgrid_y * setting.bump_grid_width;
					if (bumpnum < setting.channel_width / 2) {
						if (verttrackindex >= setting.channel_width / 2) {
							the2M2index = verttrackindex;
							verttrackindex -= setting.channel_width / 2;
							horitrackindex = verttrackindex / setting.bump_grid_width + net[i].bumpgrid_y * setting.bump_grid_width;
						}
						else {
							the2M2index = verttrackindex + setting.channel_width / 2;
							horitrackindex = verttrackindex / setting.bump_grid_width + net[i].bumpgrid_y * setting.bump_grid_width;
						}
					}
					else {
						if (verttrackindex < setting.channel_width / 2) {
							the2M2index = verttrackindex;
							verttrackindex += setting.channel_width / 2;
							horitrackindex = the2M2index / setting.bump_grid_width + net[i].bumpgrid_y * setting.bump_grid_width;
						}
						else {
							the2M2index = verttrackindex - setting.channel_width / 2;
							horitrackindex = the2M2index / setting.bump_grid_width + net[i].bumpgrid_y * setting.bump_grid_width;
						}
					}
					if (toblist[curChip_dir][curChip_x][curChip_y].trac[currenttrack.trackIndex] == -1 &&
						toblist[curChip_dir][curChip_x][curChip_y].trac[the2M2index] != currenttrack.trackIndex &&
						toblist[curChip_dir][curChip_x][curChip_y].vert[verttrackindex] == -1 &&
						toblist[curChip_dir][curChip_x][curChip_y].hori[horitrackindex] == -1){

						netcheck[i] = true;
						struct GeneralAxis tmp;
						tmp.bump = &net[i];
						tmp.isbump = true;
						tmp.istrack = false;
						tmp.track = NULL;
						tmp.tmpTreefather = currentindex;
						tmp.step = tmpTree[currentindex].step + 1;
						tmp.tob_hori = net[i].bumpgrid_x;
						tmp.tob_vert = horitrackindex;
						tmp.tob_trac = verttrackindex;

						tmpList.push_back(tmp);
						return;

					}
				}
			}
		}
	}
}

void pushtracktotrack(vector<struct GeneralAxis>& tmpList,
	const struct RoutingParameter setting,
	struct TrackDetail& currenttrack,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	vector<struct GeneralAxis>& tmpTree,
	int currentindex) {
	int tmpTreefatherindex = tmpTree[currentindex].tmpTreefather;
	int width = setting.cob_cellWidth;
	if (tmpTree[tmpTreefatherindex].isbump) {
		//father is bump, push two side tracks
		struct GeneralAxis tmpnode;
		tmpnode.isbump = false;
		tmpnode.bump = NULL;
		tmpnode.istrack = true;
		tmpnode.step = tmpTree[currentindex].step + 1;
		tmpnode.tmpTreefather = currentindex;
		if (currenttrack.xaxis) {
			//x axis track
			int x = currenttrack.channel_x;
			int y = currenttrack.channel_y;
			int index = currenttrack.trackIndex;

			// if the track isn't at the right border ,find right
			if (x < setting.channel_grid_width - 1) {
				// left to right
				if (x + 1 < setting.channel_grid_width) {
					int ltr = index;
					tmpnode.track = &trackdetail[0][x + 1][y][ltr];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}


				// left to top
				if (y + 1 < setting.channel_grid_height && x + 1 < setting.channel_grid_width) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex =(width - wiltonindex) % width;
					int ltt = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ltt = index;
					}
					tmpnode.track = &trackdetail[1][x][y + 1][ltt];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}

				}

				// left to bottom
				if (y > 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (wiltonindex - 1 + width) % width;
					int ltb = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ltb = index;
					}
					tmpnode.track = &trackdetail[1][x][y][ltb];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}
			}

			// if the track isn't at the left border ,find left

			// right to left
			if (x > 0) {
				if (x - 1 >= 0) {
					int rtl = index;
					tmpnode.track = &trackdetail[0][x - 1][y][rtl];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//right to bottom
				if (x - 1 >= 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (2 * width - wiltonindex - 2) % width;
					int rtb = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						rtb = index;
					}
					tmpnode.track = &trackdetail[1][x - 1][y][rtb];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//right to top
				if (x - 1 >= 0 && y + 1 < setting.channel_grid_height) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (wiltonindex - 1 + width) % width;
					int rtt = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						rtt = index;
					}
					tmpnode.track = &trackdetail[1][x - 1][y + 1][rtt];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}
			}
		}
		else {

			//y axis track
			int dir = 1;
			int x = currenttrack.channel_x;
			int y = currenttrack.channel_y;
			int index = currenttrack.trackIndex;

			//bottom to top
			if (y + 1 < setting.channel_grid_height) {
				int btt = index;
				tmpnode.track = &trackdetail[1][x][y + 1][btt];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}
			}

			//bottom to left
			int cobUnitIndex = index % 64 % 8;
			int rowindex = index % 64 / width;
			int wiltonindex = (rowindex + 1) % width;
			wiltonindex = width - 1 - wiltonindex;
			int btl = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
			if (!setting.cobWilton) {
				btl = index;
			}
			tmpnode.track = &trackdetail[0][x][y][btl];
			if (!tmpnode.track->intree && !tmpnode.track->searched) {
				tmpnode.track->searched = true;
				tmpList.push_back(tmpnode);
			}

			//bottom to right 
			if (x + 1 < setting.channel_grid_width) {
				int cobUnitIndex = index % 64 % 8;
				int rowindex = index % 64 / width;
				int wiltonindex = (2 * width - rowindex - 2) % width;
				wiltonindex = width - 1 - wiltonindex;
				int btr = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
				if (!setting.cobWilton) {
					btr = index;
				}
				tmpnode.track = &trackdetail[0][x + 1][y][btr];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}

			}

			//top to bottom
			if (y > 1) {
				int ttb = index;
				tmpnode.track = &trackdetail[1][x][y - 1][ttb];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}
			}

			//top to left
			if (y > 0) {
				int cobUnitIndex = index % 64 % 8;
				int rowindex = index % 64 / width;
				int wiltonindex = (width - rowindex) % width;
				wiltonindex = width - 1 - wiltonindex;
				int ttl = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
				if (!setting.cobWilton) {
					ttl = index;
				}
				tmpnode.track = &trackdetail[0][x][y - 1][ttl];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}
			}

			//top to right
			if (x + 1 < setting.channel_grid_width && y > 0) {
				int cobUnitIndex = index % 64 % 8;
				int rowindex = index % 64 / width;
				int wiltonindex = (rowindex + 1) % width;
				wiltonindex = width - 1 - wiltonindex;
				int ttr = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
				if (!setting.cobWilton) {
					ttr = index;
				}
				tmpnode.track = &trackdetail[0][x + 1][y - 1][ttr];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}
			}

		}
	}
	else if (tmpTree[tmpTreefatherindex].istrack) {
		//father is track, push the other side tracks
		struct GeneralAxis tmpnode;
		tmpnode.isbump = false;
		tmpnode.bump = NULL;
		tmpnode.istrack = true;
		tmpnode.step = tmpTree[currentindex].step + 1;
		tmpnode.tmpTreefather = currentindex;
		if (currenttrack.xaxis) {
			//x axis track
			int dir = 0;
			int x = currenttrack.channel_x;
			int y = currenttrack.channel_y;
			int index = currenttrack.trackIndex;
			if (tmpTree[tmpTreefatherindex].track->channel_x < x) {
				//find right tracks
				// left to right
				if (x + 1 < setting.channel_grid_width) {
					int ltr = index;

					tmpnode.track = &trackdetail[0][x + 1][y][ltr];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}


				// left to top
				if (y + 1 < setting.channel_grid_height && x + 1 < setting.channel_grid_width) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (width - wiltonindex) % width;
					int ltt = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ltt = index;
					}
					tmpnode.track = &trackdetail[1][x][y + 1][ltt];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				// left to bottom
				if (x + 1 < setting.channel_grid_width && y > 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (wiltonindex - 1 + width) % width;
					int ltb = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ltb = index;
					}
					tmpnode.track = &trackdetail[1][x][y][ltb];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}
			}
			else {
				//find left tracks
				// right to left
				if (x - 1 >= 0) {
					int rtl = index;
					tmpnode.track = &trackdetail[0][x - 1][y][rtl];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//right to bottom
				if (x - 1 >= 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (2 * width - wiltonindex - 2) % width;
					int rtb = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						rtb = index;
					}
					tmpnode.track = &trackdetail[1][x - 1][y][rtb];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//right to top
				if (x - 1 >= 0 && y + 1 < setting.channel_grid_height) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = width - 1 - rowindex;
					wiltonindex = (wiltonindex - 1 + width) % width;
					int rtt = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						rtt = index;
					}
					tmpnode.track = &trackdetail[1][x - 1][y + 1][rtt];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}
			}
		}
		else {
			//y axis track
			int dir = 1;
			int x = currenttrack.channel_x;
			int y = currenttrack.channel_y;
			int index = currenttrack.trackIndex;
			if (tmpTree[tmpTreefatherindex].track->channel_y < y) {
				//find top tracks
				//bottom to top
				if (y + 1 < setting.channel_grid_height) {
					int btt = index;
					tmpnode.track = &trackdetail[1][x][y + 1][btt];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//bottom to left
				int cobUnitIndex = index % 64 % 8;
				int rowindex = index % 64 / width;
				int wiltonindex = (rowindex + 1) % width;
				wiltonindex = width - 1 - wiltonindex;
				int btl = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
				if (!setting.cobWilton) {
					btl = index;
				}
				tmpnode.track = &trackdetail[0][x][y][btl];
				if (!tmpnode.track->intree && !tmpnode.track->searched) {
					tmpnode.track->searched = true;
					tmpList.push_back(tmpnode);
				}

				//bottom to right 
				if (x + 1 < setting.channel_grid_width) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = (2 * width - rowindex - 2) % width;
					wiltonindex = width - 1 - wiltonindex;
					int btr = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						btr = index;
					}
					tmpnode.track = &trackdetail[0][x + 1][y][btr];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}
			}
			else {
				//find bottom tracks
				//top to bottom
				if (y > 1) {
					int ttb = index;
					tmpnode.track = &trackdetail[1][x][y - 1][ttb];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//top to left
				if (y > 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = (width - rowindex) % width;
					wiltonindex = width - 1 - wiltonindex;
					int ttl = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ttl = index;
					}
					tmpnode.track = &trackdetail[0][x][y - 1][ttl];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				//top to right
				if (x + 1 < setting.channel_grid_width && y > 0) {
					int cobUnitIndex = index % 64 % 8;
					int rowindex = index % 64 / width;
					int wiltonindex = (rowindex + 1) % width;
					wiltonindex = width - 1 - wiltonindex;
					int ttr = cobUnitIndex + wiltonindex * width + int(index / 64) * 64;
					if (!setting.cobWilton) {
						ttr = index;
					}
					tmpnode.track = &trackdetail[0][x + 1][y - 1][ttr];
					if (!tmpnode.track->intree && !tmpnode.track->searched) {
						tmpnode.track->searched = true;
						tmpList.push_back(tmpnode);
					}
				}

				
			}
		}
	}
}

void pushbumptotrack(vector<struct GeneralAxis>& tmpList,
					 const struct RoutingParameter setting,
					 struct BumpAxis& currentbump,
					 array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
					 array<vector<vector<struct Tob>>, 2> & toblist,
					 vector<struct GeneralAxis>& tmpTree,
					 int currentindex) {
	//candidate track (dir,x, y)
	int dir = currentbump.dir;
	int x = currentbump.TOB_x;
	int y = currentbump.TOB_y;
	int bumpnum = y * setting.bump_grid_width + x;
	int horizon = 0,vertical = 0, outtrack = 0;
	for (horizon = currentbump.bumpgrid_y * setting.bump_grid_width; horizon < (currentbump.bumpgrid_y + 1) * setting.bump_grid_width; horizon++) {
		if (toblist[dir][x][y].hori[horizon] == -1) {
			int vertgroup = horizon % setting.bump_grid_width;
			if (horizon >= setting.channel_width / 2) {
				vertgroup += 8;
			}
			for (vertical = vertgroup * setting.bump_grid_width; vertical < (vertgroup + 1) * setting.bump_grid_width; vertical++) {
				if (toblist[dir][x][y].vert[vertical] == -1) {
					//one selection in 2*2 block
					outtrack = vertical;
					struct TrackDetail& candidatetrack = trackdetail[dir][x][y][outtrack];
					if (toblist[dir][x][y].trac[outtrack] == -1 && !candidatetrack.intree && !candidatetrack.searched) {

						//push to the tmpList
						struct GeneralAxis tmp;
						tmp.bump = NULL;
						tmp.isbump = false;
						tmp.istrack = true;
						candidatetrack.searched = true;
						tmp.track = &candidatetrack;
						tmp.step = tmpTree[currentindex].step + 1;
						tmp.tmpTreefather = currentindex;
						tmp.tob_hori = currentbump.bumpgrid_x;
						tmp.tob_vert = horizon;
						tmp.tob_trac = vertical;
						
						tmpList.push_back(tmp);

					}

					// the other selection in 2*2 block
					if (outtrack < setting.channel_width / 2) {
						outtrack += setting.channel_width / 2;
					}
					else {
						outtrack -= setting.channel_width / 2;
					}
					struct TrackDetail& candidatetrack2 = trackdetail[dir][x][y][outtrack];
					if (toblist[dir][x][y].trac[outtrack] == -1 && !candidatetrack2.intree && !candidatetrack2.searched) {

						//push to the tmpList
						struct GeneralAxis tmp;
						tmp.bump = NULL;
						tmp.isbump = false;
						tmp.istrack = true;
						candidatetrack2.searched = true;
						tmp.track = &candidatetrack2;
						tmp.step = tmpTree[currentindex].step + 1;
						tmp.tmpTreefather = currentindex;
						tmp.tob_hori = currentbump.bumpgrid_x;
						tmp.tob_vert = horizon;
						tmp.tob_trac = vertical;

						tmpList.push_back(tmp);

					}
				}
			}
		}
	}


}


vector<struct GeneralAxis> findAdjacentPoint(const struct RoutingParameter setting,
	int currentindex,
	vector<struct GeneralAxis>& tmpTree,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<bool>& netcheck,
	vector<struct BumpAxis>& net,
	int maxStep) {
	struct GeneralAxis currentPoint = tmpTree[currentindex];
	vector<struct GeneralAxis> tmpList;

	if (currentPoint.isbump) {
		struct BumpAxis& currentbump = *currentPoint.bump;
		//push top tracks
		pushbumptotrack(tmpList, setting, currentbump, trackdetail, toblist, tmpTree, currentindex);
	}
	else if (currentPoint.istrack) {
		struct TrackDetail& currenttrack = *currentPoint.track;
		pushtracktotrack(tmpList, setting, currenttrack, trackdetail, tmpTree, currentindex);
		pushtracktobump(tmpList, setting, currenttrack, trackdetail, toblist, net, netcheck, maxStep, tmpTree, currentindex);
	}

	return tmpList;
}

bool tryFindSinks(const struct RoutingParameter setting,
	vector<struct BumpAxis>& net,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<struct GeneralAxis>& tmpTree,
	int maxStep) {
	//use netcheck to check whether all sinks are found
	vector<bool> netcheck(net.size(), false);
	netcheck[0] = true;
	//net store the bump to connect
	//tmpTree store the tmp Routing tree
	int i = 0;
	if (maxStep > -1) {
		i = tmpTree.size() - 1;
	}
	bool allSinksFound = false;
	while (i < tmpTree.size()) {

		//check if tmpTree is too longer 
		if (maxStep != -1 && tmpTree[i].istrack && (tmpTree[i].step > 2 * maxStep)) {
			return false;
		}

		vector<struct GeneralAxis> tmpList;
		if (maxStep >= 0) {
			//find synchronized net
			tmpList = findSyncNet(setting, i, tmpTree, trackdetail, toblist, netcheck, net, maxStep);
		}
		else if (maxStep == -1) {
			tmpList = findAdjacentPoint(setting, i, tmpTree, trackdetail, toblist, netcheck, net, maxStep);
		}

		// push tmpList to tmpTree
		int j;
		for (j = 0; j < tmpList.size(); j++) {
			if (tmpList[j].isbump) {
				tmpList[j].bump->tmpTreeindex = tmpTree.size();
			}
			tmpTree.push_back(tmpList[j]);
		}

		//check if all sinks are reached
		allSinksFound = checkallsinks(netcheck);

		if (allSinksFound) {
			break;
		}

		i++;
	}
	return allSinksFound;
}
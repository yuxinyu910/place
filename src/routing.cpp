#include "routing.h"
#include "tryFindSinks.h"
#include <map>
#include <algorithm>

void outputPrint(const struct RoutingParameter setting, vector<vector<struct GeneralAxis>>& routeTree) {
	int i;
	int j;
	using namespace std;
	for (i = 0; i < routeTree.size(); i++) {
		cout << "route Tree  " << i << endl;
		for (j = 0; j < routeTree[i].size(); j++) {
			struct GeneralAxis node = routeTree[i][j];
			if (node.isbump) {
				cout << "bump (" << node.bump->dir << " " << node.bump->TOB_x << " " << node.bump->TOB_y << " " << node.bump->bumpgrid_x << " " << node.bump->bumpgrid_y << ") " ;
				cout << "---- h" << node.bump->tob_vert << "----- v" << node.bump->tob_trac << endl;
			}
			else if (node.istrack) {
				cout << "track\t";
				if (node.track->xaxis) {
					cout << "Horizontal";
				}
				else{
					cout << "vertical";
				}
				cout<< "\t\t" << node.track->channel_x << "\t" << node.track->channel_y << "\t" << node.track->trackIndex << endl;
			}
		}
		cout << endl;
	}
	int sumstep = 0;
	for (i = 0; i < routeTree.size(); i++) {
	cout << "net " << i << " " << routeTree[i].size() - 1 << " step" << endl;
	sumstep += routeTree[i].size() - 1;
	}
	cout << "Total " << sumstep << " steps" << endl;
}

bool routingCheck() {
	return true;
}

void setPathInTree(const struct RoutingParameter setting,
				   array<vector<vector<struct Tob>>, 2> & toblist,
				   vector<struct GeneralAxis>& tmpTree, int j,
				   vector<struct GeneralAxis>& routeTree) {
	int i;
	for (i = 0; i < routeTree.size(); i++) {
		if (routeTree[i].istrack) {
			if (tmpTree[j].istrack) {
				if (routeTree[i].track->xaxis == tmpTree[j].track->xaxis &&
					routeTree[i].track->channel_x == tmpTree[j].track->channel_x &&
					routeTree[i].track->channel_y == tmpTree[j].track->channel_y &&
					routeTree[i].track->trackIndex == tmpTree[j].track->trackIndex) {
					return;
				}
			}
		}
		else if (routeTree[i].isbump) {
			if (tmpTree[j].isbump) {
				if (tmpTree[j].bump->TOB_x == routeTree[i].bump->TOB_x &&
					tmpTree[j].bump->TOB_y == routeTree[i].bump->TOB_y &&
					tmpTree[j].bump->bumpgrid_x == routeTree[i].bump->bumpgrid_x &&
					tmpTree[j].bump->bumpgrid_y == routeTree[i].bump->bumpgrid_y &&
					tmpTree[j].bump->dir == routeTree[i].bump->dir) {
					return;
				}
			}
		}
	}
	if (j >= 0) {
		if (tmpTree[j].isbump) {
			tmpTree[j].bump->step = tmpTree[j].step;
			if (tmpTree[j].tmpTreefather >= 0 && tmpTree[tmpTree[j].tmpTreefather].istrack) {
				tmpTree[tmpTree[j].tmpTreefather].track->cb = tmpTree[j].bump->bumpgrid_x + tmpTree[j].bump->bumpgrid_y * setting.bump_grid_width;
				tmpTree[tmpTree[j].tmpTreefather].track->linkedbump = tmpTree[j].bump;
				tmpTree[j].bump->tob_hori = tmpTree[j].tob_hori;
				tmpTree[j].bump->tob_vert = tmpTree[j].tob_vert;
				tmpTree[j].bump->tob_trac = tmpTree[j].tob_trac;
				tmpTree[j].bump->linkedtrack = tmpTree[tmpTree[j].tmpTreefather].track;

				int dir = tmpTree[j].bump->dir;
				int x = tmpTree[j].bump->TOB_x;
				int y = tmpTree[j].bump->TOB_y;
				toblist[dir][x][y].hori[tmpTree[j].tob_vert] = tmpTree[j].tob_hori;
				toblist[dir][x][y].vert[tmpTree[j].tob_trac] = tmpTree[j].tob_vert;
				toblist[dir][x][y].trac[tmpTree[tmpTree[j].tmpTreefather].track->trackIndex] = tmpTree[j].tob_trac;
			}
		}
		else if (tmpTree[j].istrack) {
			tmpTree[j].track->step = tmpTree[j].step;
			tmpTree[j].track->intree = true;
			if (tmpTree[j].tmpTreefather >= 0 && tmpTree[tmpTree[j].tmpTreefather].istrack) {
				tmpTree[j].track->father = tmpTree[tmpTree[j].tmpTreefather].track;
			}
			else if (tmpTree[j].tmpTreefather >= 0 && tmpTree[tmpTree[j].tmpTreefather].isbump) {
				tmpTree[j].track->father = NULL;
				tmpTree[j].track->linkedbump = tmpTree[tmpTree[j].tmpTreefather].bump;
				tmpTree[j].track->cb = tmpTree[tmpTree[j].tmpTreefather].bump->bumpgrid_x + tmpTree[tmpTree[j].tmpTreefather].bump->bumpgrid_y * setting.bump_grid_width;
				tmpTree[tmpTree[j].tmpTreefather].bump->linkedtrack = tmpTree[j].track;
				tmpTree[tmpTree[j].tmpTreefather].bump->tob_hori = tmpTree[j].tob_hori;
				tmpTree[tmpTree[j].tmpTreefather].bump->tob_vert = tmpTree[j].tob_vert;
				tmpTree[tmpTree[j].tmpTreefather].bump->tob_trac = tmpTree[j].tob_trac;
				int dir = tmpTree[tmpTree[j].tmpTreefather].bump->dir;
				int x = tmpTree[tmpTree[j].tmpTreefather].bump->TOB_x;
				int y = tmpTree[tmpTree[j].tmpTreefather].bump->TOB_y;
				toblist[dir][x][y].hori[tmpTree[j].tob_vert] = tmpTree[j].tob_hori;
				toblist[dir][x][y].vert[tmpTree[j].tob_trac] = tmpTree[j].tob_vert;
				toblist[dir][x][y].trac[tmpTree[j].track->trackIndex] = tmpTree[j].tob_trac;
			}
		}
		setPathInTree(setting, toblist, tmpTree, tmpTree[j].tmpTreefather, routeTree);
		routeTree.push_back(tmpTree[j]);
	}
}


void getRouteTree(const struct RoutingParameter setting,
	vector<struct BumpAxis>& net,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<struct GeneralAxis>& tmpTree,
	vector<struct GeneralAxis>& routeTree) {

	int i, j;
	for (i = 1; i < net.size(); i++) {
		j = net[i].tmpTreeindex;
		setPathInTree(setting, toblist, tmpTree, j, routeTree);
	}
	for (i = 0; i < tmpTree.size(); i++) {
		if (tmpTree[i].istrack) {
			tmpTree[i].track->searched = false;
		}
	}
}

void ripupRouteTree(const struct RoutingParameter setting,
	vector<struct GeneralAxis>& routeTree,
	array<vector<vector<struct Tob>>, 2> & toblist) {
	int i;
	int ripUp = routeTree.size() * RipUpRatio;
	for (i = ripUp; i < routeTree.size(); i++) {
		if (routeTree[i].istrack) {
			routeTree[i].track->bumpgrid = -1;
			routeTree[i].track->cb = -1;
			routeTree[i].track->cost = -1;
			routeTree[i].track->father = NULL;
			routeTree[i].track->intree = false;
			routeTree[i].track->searched = false;
			routeTree[i].track->step = -1;
			routeTree[i].track->linkedbump = NULL;
		}
		else if (routeTree[i].isbump) {
			routeTree[i].bump->cost = -1;
			routeTree[i].bump->tmpTreeindex = -1;
			routeTree[i].bump->step = -1;
			toblist[routeTree[i].bump->dir][routeTree[i].bump->TOB_x][routeTree[i].bump->TOB_y].hori[routeTree[i].bump->tob_vert] = -1;
			toblist[routeTree[i].bump->dir][routeTree[i].bump->TOB_x][routeTree[i].bump->TOB_y].vert[routeTree[i].bump->tob_trac] = -1;
			toblist[routeTree[i].bump->dir][routeTree[i].bump->TOB_x][routeTree[i].bump->TOB_y].trac[routeTree[i].bump->linkedtrack->trackIndex] = -1;
			routeTree[i].bump->linkedtrack = NULL;
			routeTree[i].bump->tob_hori = -1;
			routeTree[i].bump->tob_vert = -1;
			routeTree[i].bump->tob_trac = -1;
		}
	}
	routeTree.resize(0);
}

void getHistoryRoutingTree(const struct RoutingParameter setting,
	vector<struct GeneralAxis>& tmpTree,
	vector<vector<struct GeneralAxis>>& routeTree,
	vector<vector<struct BumpAxis>>& netList,
	int iter,
	int netindex,
	int maxStep) {

	if (maxStep > -1) {
		// get history tree
		tmpTree = routeTree[netindex];
		int i;
		int ripUp = tmpTree.size() * RipUpRatio;
		tmpTree.resize(ripUp);
		for (i = 1; i < ripUp; i++) {
			tmpTree[i].tmpTreefather = i - 1;
		}
	}
	else {
		tmpTree.resize(1);
		struct GeneralAxis tmpAxis;
		tmpAxis.isbump = true;
		tmpAxis.istrack = false;
		tmpAxis.bump = &netList[netindex][0];
		tmpAxis.bump->linkedtrack = NULL;
		tmpAxis.bump->tob_hori = -1;
		tmpAxis.bump->tob_trac = -1;
		tmpAxis.bump->tob_vert = -1;
		tmpAxis.bump->cost = 0;
		tmpAxis.bump->tmpTreeindex = 0;
		tmpAxis.tmpTreefather = -1;
		tmpAxis.step = 0;
		tmpTree[0] = tmpAxis;
	}
}

bool startRouting(const struct RoutingParameter setting,
				  array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
				  array<vector<vector<struct Tob>>, 2> & toblist,
				  vector<vector<struct BumpAxis>>& netList,
				  vector<int>& netGroupIndex,
				  vector<int>& netGroupPriority,
				  vector<vector<struct GeneralAxis>>& routeTree) {
	//debug
	int successnet = 0, failednet = 0;
	int iter = 0;
	bool checkflag = false, netdone = false;
	vector<int> netGroupMaxStep;
	netGroupMaxStep.resize(netGroupPriority.size(), -1);
	routeTree.resize(netList.size());

	while (1) {// max iteration number
		iter++;
		int groupi = 0;
		while (groupi < netGroupPriority.size()) {
			//choose group index based on priority
			int groupindex = netGroupPriority[groupi];
			cout << "route group " << groupindex << endl;
			//get the maxStep in the group
			int maxStep = -1;
			if (groupindex >= 0) {
				maxStep = netGroupMaxStep[groupindex];
			}
			else if (groupindex == -1) {
				maxStep = -1;
			}

			//record current max step in this iteration
			int currentMaxStep = maxStep;
			bool checkSyncStep = true;

			//loop the nets
			for (int i = 0; i < netList.size(); i++) {
				//find the net in this group
				if (netGroupIndex[i] == groupindex) {
					vector<struct GeneralAxis> tmpTree;
					// get history routing tree
					getHistoryRoutingTree(setting, tmpTree, routeTree, netList, iter, i, maxStep);

					bool rerouted = false;

					//if group history maxStep == -1 , find any path
					if (maxStep == -1) {
						netdone = false;
						netdone = tryFindSinks(setting, netList[i], trackdetail, toblist, tmpTree, maxStep);
						rerouted = true;
					}
					//if group has history maxStep, reroute to find a path longer than or equal to maxStep
					else if (maxStep > 0 && (netList[i].back().step != maxStep)) {
						ripupRouteTree(setting, routeTree[i], toblist);
						netdone = false;
						netdone = tryFindSinks(setting, netList[i], trackdetail, toblist, tmpTree, maxStep);
						rerouted = true;
					}

					// return false if no path is found with no constrain
					if (maxStep == -1 && !netdone) {//This net cannot be routed
						failednet++;
						cout << "net " << i << " failed " << endl;
						//return false;
					}

					//if net is find, get the step number
					int netStep = 0;
					if (netdone && rerouted) {
						successnet++;
						cout << "net " << i << " success " << endl;
						getRouteTree(setting, netList[i], trackdetail, toblist, tmpTree, routeTree[i]);
						netStep = int(netList[i].back().step);
						cout << "net " << i << " route " << netStep << " step\ttmpTree " << tmpTree.size() << endl;
						//update the maxStep in the group and check if all nets are synchronized
						if (netStep > currentMaxStep) {
							if (currentMaxStep > 0) {
								checkSyncStep = false;
							}
							currentMaxStep = netStep;
						}
					}

					//	updateCongestion();//channel_x[x][y] = c%

					//	updateHistoryCongestion();

					//	updateDelay();
				}

			}

			//if all nets are synchronized, continue to next group
			if (checkSyncStep || groupindex == -1) {
				groupi++;
				cout << "success in " << currentMaxStep << endl;
			}
			//if not, reroute this group again
			else if (!checkSyncStep) {
				netGroupMaxStep[groupindex] = currentMaxStep;
				cout << "failed in " << maxStep << endl;
			}
		}

		if (routingCheck()) {// check if all nets have been routed
			checkflag = true;
			break;
		}
		if (iter > MAXIter) {
			checkflag = false;
			break;
		}
	}
	cout << "success " << successnet << "\tfailed " << failednet << endl;
	if (setting.cobWilton) {

		cout << "wilton " <<endl;
	}
	else {

		cout << "disjoint " << endl;
	}
	return checkflag;
}

bool cmpNetListSinks(pair<int, int> a, pair<int,int> b) {
	return a.second > b.second;
}
void sortNetList(	vector<vector<struct BumpAxis>>& netList,
					vector<int>& netGroupIndex,
					vector<int>& netGroupOrder) {
	//more sinks group first
	int index = netGroupIndex[0];
	map<int, int> netGroupSink;
	for(int i = 0; i < netGroupIndex.size(); i++){
		//this group
		if(index == netGroupIndex[i]){
			netGroupSink[netGroupIndex[i]] += netList[i].size();
		//next group
		} else {	
		//add group info to netGroupSum
		//cout next group
			index = netGroupIndex[i];
			netGroupSink[netGroupIndex[i]] = netList[i].size();
		}
	}

	vector<pair<int, int>> tmp_order;
	for (auto it = netGroupSink.begin(); it != netGroupSink.end(); it++) {
		tmp_order.push_back(make_pair(it -> first, it -> second));
	}

	sort(tmp_order.begin(), tmp_order.end(), cmpNetListSinks);
	
	for(auto it = tmp_order.begin(); it != tmp_order.end(); it++){
		if(it -> first != -1){
			netGroupOrder.push_back(it -> first);
		}
	}
	// asyn last
	netGroupOrder.push_back(-1);
}

void routing(const struct RoutingParameter setting,
	array<vector<vector<vector<struct TrackDetail>>>, 2>& trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<vector<struct BumpAxis>>& netList,
	vector<int>& netGroupIndex) {

	//getTimingConstrain();//timing constrain

	//routingInitialize();

	// TODO:route high speed net first
	vector<int> netGroupOrder;
	sortNetList(netList, netGroupIndex, netGroupOrder);
	// for(int i = 0; i < netGroupOrder.size(); i++) cout << netGroupOrder[i] << endl;

	vector<vector<struct GeneralAxis>> routeTree;
	bool flag = startRouting(setting, trackdetail, toblist, netList, netGroupIndex, netGroupOrder, routeTree);

	outputPrint(setting, routeTree);

}
#pragma once
#include "type_define.h"
vector<struct GeneralAxis> findSyncNet(const struct RoutingParameter setting,
	int currentindex,
	vector<struct GeneralAxis>& tmpTree,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<bool>& netcheck,
	vector<struct BumpAxis>& net,
	int maxStep);
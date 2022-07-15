#pragma once
#include "type_define.h"

bool tryFindSinks(const struct RoutingParameter setting,
	vector<struct BumpAxis>& net,
	array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<struct GeneralAxis>& tmpTree,
	int maxStep);
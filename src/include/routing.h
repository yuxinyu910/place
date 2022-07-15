#pragma once
#include <cstdio>
#include <vector>
#include <array>
#include <iostream>
#include "type_define.h"

#define MAXIter 1

void sortNetList(vector<vector<struct BumpAxis>>& netList);

void routing(const struct RoutingParameter setting,
	array<vector<vector<vector<struct TrackDetail>>>, 2>& trackdetail,
	array<vector<vector<struct Tob>>, 2> & toblist,
	vector<vector<struct BumpAxis>>& netList,
	vector<int>& netGroupIndex);
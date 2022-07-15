#pragma once
#include <cstdio>
#include <vector>
#include <array>
#include <iostream>
#include "type_define.h"

void initializeTrack(const struct RoutingParameter setting,
					 array<vector<vector<vector<struct TrackDetail>>>, 2>& trackdetail);
void initializeSetting(struct RoutingParameter& setting);

void initializeChipCoordinate(const struct RoutingParameter setting,
							  array<vector<vector<vector<struct TrackDetail>>>, 2> & trackdetail,
							  array<vector<vector<struct Tob>>, 2> & toblist,
							  vector<vector<struct BumpAxis>>& netList);
// void initializeNetList(const struct RoutingParameter setting, vector<vector<struct BumpAxis>>& netList);
void initializeNetList(const struct  InputParameter, const struct RoutingParameter setting, vector<vector<struct BumpAxis>>& netList, vector<int>& netGroupIndex);
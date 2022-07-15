#ifndef ReadNetList_H
#define ReadNetList_H
    
    void ReadNetList(const struct  InputParameter, vector<vector<struct BumpAxis>> &netList, vector<int>& netGroupIndex);
    void ChangeNetList(vector<vector<struct BumpAxis>>& netList,vector<chiplet> &chip);
    void ReadPara( int argc, char const *argv[], struct InputParameter& inputPara);
#endif
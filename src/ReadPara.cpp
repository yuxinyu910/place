#include <iostream>
#include <type_define.h>
#include "ReadNetList.h"

using namespace std;
void ReadPara(  int argc, char const *argv[], struct InputParameter& inputPara){

    for(int i = 0; i < argc; i++){
        string tmpArgv = argv[i];
        if(tmpArgv == "-n"){
            inputPara.netList = argv[++i];
        }
    }

    //netList err
    if(inputPara.netList == "null"){
        cout << "please check netlist file and use the format: -n netListFileName" << endl;
    }
}


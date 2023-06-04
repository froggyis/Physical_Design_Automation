#pragma once
#include "../Structure/Structure.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Parser
{
    public:
        void readAux(string filename);
        void readNode();
        void readPl();
        void readScl();
        string prefix;

    private:
        string file_node_name, file_pl_name, file_scl_name, testcase;
       
        int MaxDisplacement;

};

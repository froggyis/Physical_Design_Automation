#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "myObject.hpp"
using namespace  std;

//  define global parameters
int CS_WIDTH = 7100;
int CS_HEIGHT = 6600;
int M3_WIDTH =  440;
int M3_SPACING = 310;
int M4_WIDTH =  1000;
int M4_SPACING = 490;

int CS_X1_TO_DRAIN = 1260;
int CS_Y1_TO_DRAIN = 4100;
string CS_LIB_NAME = "MSBCS";
string VIA34_LIB_NAME = "Via34";

void write_def(string file_name, Die die,  vector<Component*> component_list,  vector<SpecialNet*> sepcialnet_list);


int main(int argc, char **argv)
{
    int numCS = (stoi(argv[1]));
    int sqrt_numCS = sqrt(numCS);
    // ##### Step 1: create die boundary #####
    string design_name = "CS_APR";
    int die_x1 = 0;
    int die_y1 = 0;
    int die_x2 = CS_WIDTH * (sqrt_numCS*2) + M3_SPACING * ((sqrt_numCS+1)*(sqrt_numCS*2)-1) + M3_WIDTH *(sqrt_numCS) *(sqrt_numCS*2);
    int die_y2 = CS_HEIGHT * (sqrt_numCS*2) + M4_SPACING * (((sqrt_numCS/2+1)*(sqrt_numCS*2)-1)) + M4_WIDTH * ((sqrt_numCS/2)*(sqrt_numCS*2));
 
    auto die = Die(design_name, die_x1, die_y1, die_x2, die_y2);

    // ##### Step 2: create CS array #####
    int Dx = CS_WIDTH + M3_SPACING * (sqrt_numCS+1) + M3_WIDTH * (sqrt_numCS);
    int Dy = CS_HEIGHT + M4_SPACING * (sqrt_numCS/2+1) + M4_WIDTH * (sqrt_numCS/2);
    int offy = M4_SPACING * (sqrt_numCS/2 +1 -1) + M4_WIDTH * (sqrt_numCS/2);

 
    vector <vector<Component*>> cs_array;
    cs_array.resize(sqrt_numCS*2, vector<Component*>(sqrt_numCS*2));
    for (int i = 0 ; i<sqrt_numCS*2 ; i++)
    {
        for(int j =0; j<sqrt_numCS*2 ; j++)
        {
            string cs_lib_name = CS_LIB_NAME;
            string cs_instance_name = "Transistor" + to_string(i * sqrt_numCS*2 + j);
            int x = i*Dx;
            int y = j*Dy + offy;
            Component *tmp = new Component(cs_lib_name, cs_instance_name, x, y);
            cs_array[i][j] = tmp;
        }
    }


    // ##### Step 3: create vertical ME3 #####
    // # ME3 nets
    Dx = CS_WIDTH + M3_SPACING;
    int Px = M3_WIDTH + M3_SPACING;
    vector<vector<SpecialNet*>> ME3_specialnet;
    ME3_specialnet.resize(sqrt_numCS*2, vector<SpecialNet*>(sqrt_numCS));
    for (int i = 0 ; i<sqrt_numCS*2; i++)
    {
        for(int j = 0 ; j<sqrt_numCS ; j++)
        {
            string inst_name = "Metal3_" + to_string(i * sqrt_numCS + j);
            string layer = "ME3";
            int x1 = cs_array[i][0]->_x + Dx + j * Px;
            int x2 = x1 + M3_WIDTH;
            int y1 = 0;
            int y2 = die_y2;
            SpecialNet *tmp = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME3_specialnet[i][j] = tmp;
        }
    }
    // ##### Step 4: create ME4 drain #####
    // # ME4 drains
    vector<vector<SpecialNet*>> ME4_specialnet_drain;
    ME4_specialnet_drain.resize(sqrt_numCS*2, vector<SpecialNet*>(sqrt_numCS*2));

    for(int i = 0 ; i<sqrt_numCS ; i++)
        for(int j = 0 ; j<sqrt_numCS ; j++)
        {
            string layer = "ME4";
            string inst_name = "Metal4_drain_" + to_string(i * sqrt_numCS + j + 0 * numCS);
            int x1 = cs_array[i][j]->_x + CS_X1_TO_DRAIN;
            int x2 = ME3_specialnet[i][j]->_x2;
            int y1 = cs_array[i][j]->_y + CS_Y1_TO_DRAIN;
            int y2 = y1 + M4_WIDTH;
            SpecialNet *drain_1 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[i][j] = drain_1;

            inst_name = "Metal4_drain_" + to_string(i * sqrt_numCS + j + 1 * numCS);
            x1 = cs_array[sqrt_numCS*2-1-i][j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[sqrt_numCS*2-1-i][j]->_x2;
            y1 = cs_array[sqrt_numCS*2-1-i][j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_2 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[sqrt_numCS*2-1-i][j] = drain_2;

            inst_name = "Metal4_drain_" + to_string(i * sqrt_numCS + j + 2 * numCS);
            x1 = cs_array[i][sqrt_numCS*2-1-j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[i][j]->_x2;
            y1 = cs_array[i][sqrt_numCS*2-1-j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_3 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[i][sqrt_numCS*2-1-j] = drain_3;

            inst_name = "Metal4_drain_" + to_string(i * sqrt_numCS + j + 3 * numCS);
            x1 = cs_array[sqrt_numCS*2-1-i][sqrt_numCS*2-1-j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[sqrt_numCS*2-1-i][j]->_x2;
            y1 = cs_array[i][sqrt_numCS*2-1-j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_4 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[sqrt_numCS*2-1-i][sqrt_numCS*2-1-j] = drain_4;
        }
    
    
   

    // ##### Step 5: create ME4 port #####
    // # ME4 ports
    vector<vector<SpecialNet*>> ME4_specialnet_port;

    ME4_specialnet_port.resize(sqrt_numCS*2, vector<SpecialNet*>(sqrt_numCS/2));
    Dy = CS_HEIGHT + M4_SPACING*(sqrt_numCS/2+1) + M4_WIDTH*(sqrt_numCS/2);


    for(int i = 0 ; i<sqrt_numCS*2 ; i++)
    {
        for(int j = 0 ; j<sqrt_numCS/2 ; j++)
        {
            string inst_name = "Metal4_port_" + to_string(i*sqrt_numCS/2+j);
            string layer = "ME4";
            int x1 = 0;
            int x2 = die_x2;
            int y1 = i * Dy + j * (M4_WIDTH + M4_SPACING);
            int y2 = y1 + M4_WIDTH;
            auto *ME4_tmp = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_port[i][j] = ME4_tmp;
        }

    }

    // ##### Step 6: create Via34 from ME4 drain #####
    // # drain to ME3
    // Via34_drain2ME3 = [[Component for j in range(4)] for i in range(4)]
    vector<vector<Component*>> Via34_drain2ME3;
    Via34_drain2ME3.resize(sqrt_numCS*2, vector<Component*>(sqrt_numCS*2));

    for(int i = 0 ; i<sqrt_numCS ; i++)
    {
        for(int j = 0 ; j<sqrt_numCS ; j++)
        {
            string lib_name = VIA34_LIB_NAME;
            // # left bottom corner units
            string inst_name = "Via34_drain2ME3_" + to_string(i * sqrt_numCS + j + 0 * numCS);
            int x = ME3_specialnet[i][j]->_x1;
            int y = cs_array[i][j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_1 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[i][j] = Via34_1;

            inst_name = "Via34_drain2ME3_" + to_string(i * sqrt_numCS + j + 1 * numCS);
            x = ME3_specialnet[sqrt_numCS*2-1-i][j]->_x1;
            y = cs_array[sqrt_numCS*2-1-i][j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_2 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[sqrt_numCS*2-1-i][j] = Via34_2;
      
            inst_name = "Via34_drain2ME3_" + to_string(i * sqrt_numCS + j + 2 * numCS);
            x = ME3_specialnet[i][j]->_x1;
            y = cs_array[i][sqrt_numCS*2-1-j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_3 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[i][sqrt_numCS*2-1-j] = Via34_3;

            inst_name = "Via34_drain2ME3_" + to_string(i * sqrt_numCS + j + 3 * numCS);
            x = ME3_specialnet[sqrt_numCS*2-1-i][j]->_x1;
            y = cs_array[sqrt_numCS*2-1-i][sqrt_numCS*2-1-j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_4 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[sqrt_numCS*2-1-i][sqrt_numCS*2-1-j] = Via34_4;

        }
    }

    // // ##### Step 7: create Via34 to ME4 port #####
    // // # port to ME4
    // // # Precaution:
    // // # 1. create a port list 'Via34_port2ME3' to contain port 'Component'
    // // # 2. lib_name = VIA34_LIB_NAME
    // // # 3. inst_name = 'Via34_port2ME3_'
    // // # TODO
    // // Via34_port2ME3 = [[Component for j in range(2)] for i in range(4)]
    string lib_name = VIA34_LIB_NAME;

    vector<vector<Component*> > Via34_port2ME3;
    Via34_port2ME3.resize(numCS, vector<Component*>(2));

    // /////////////////////(0, 0)////////////////////
    // string inst_name = "Via34_port2ME3_" + to_string(0);
    // int x = Via34_drain2ME3[0][0]->_x;
    // int y = ME4_specialnet_port[0][0]->_y1;
    // Via34_port2ME3[0][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(1);
    // x = Via34_drain2ME3[7][0]->_x;
    // y = ME4_specialnet_port[0][0]->_y1;;
    // Via34_port2ME3[0][1] = new Component(lib_name, inst_name, x, y);
    // /////////////////////(0, 1)///////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(2);
    // x = Via34_drain2ME3[1][0]->_x;
    // y = ME4_specialnet_port[0][1]->_y1;;
    // Via34_port2ME3[1][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(3);
    // x = Via34_drain2ME3[6][0]->_x;
    // y = ME4_specialnet_port[0][1]->_y1;;
    // Via34_port2ME3[1][1] = new Component(lib_name, inst_name, x, y);
    // /////////////////////(1, 0)///////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(4);
    // x = Via34_drain2ME3[2][0]->_x;
    // y = ME4_specialnet_port[1][0]->_y1;;
    // Via34_port2ME3[2][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(5);
    // x = Via34_drain2ME3[5][0]->_x;
    // y = ME4_specialnet_port[1][0]->_y1;;
    // Via34_port2ME3[2][1] = new Component(lib_name, inst_name, x, y);
    // //////////////////////(1, 1)/////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(6);
    // x = Via34_drain2ME3[3][0]->_x;
    // y = ME4_specialnet_port[1][1]->_y1;;
    // Via34_port2ME3[3][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(7);
    // x = Via34_drain2ME3[4][0]->_x;
    // y = ME4_specialnet_port[1][1]->_y1;;
    // Via34_port2ME3[3][1] = new Component(lib_name, inst_name, x, y);
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(8);
    // x = Via34_drain2ME3[0][1]->_x;
    // y = ME4_specialnet_port[2][0]->_y1;;
    // Via34_port2ME3[4][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(9);
    // x = Via34_drain2ME3[7][1]->_x;
    // y = ME4_specialnet_port[2][0]->_y1;;
    // Via34_port2ME3[4][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(10);
    // x = Via34_drain2ME3[1][1]->_x;
    // y = ME4_specialnet_port[2][1]->_y1;;
    // Via34_port2ME3[5][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(11);
    // x = Via34_drain2ME3[6][1]->_x;
    // y = ME4_specialnet_port[2][1]->_y1;;
    // Via34_port2ME3[5][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(12);
    // x = Via34_drain2ME3[2][1]->_x;
    // y = ME4_specialnet_port[3][0]->_y1;;
    // Via34_port2ME3[6][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(13);
    // x = Via34_drain2ME3[5][1]->_x;
    // y = ME4_specialnet_port[3][0]->_y1;;
    // Via34_port2ME3[6][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(14);
    // x = Via34_drain2ME3[3][1]->_x;
    // y = ME4_specialnet_port[3][1]->_y1;;
    // Via34_port2ME3[7][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(15);
    // x = Via34_drain2ME3[4][1]->_x;
    // y = ME4_specialnet_port[3][1]->_y1;;
    // Via34_port2ME3[7][1] = new Component(lib_name, inst_name, x, y);
    // // ////////////////////////////////////////////////////////////////
    // // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(16);
    // x = Via34_drain2ME3[0][2]->_x;
    // y = ME4_specialnet_port[4][0]->_y1;;
    // Via34_port2ME3[8][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(17);
    // x = Via34_drain2ME3[7][2]->_x;
    // y = ME4_specialnet_port[4][0]->_y1;;
    // Via34_port2ME3[8][1] = new Component(lib_name, inst_name, x, y);
    // //////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(18);
    // x = Via34_drain2ME3[1][2]->_x;
    // y = ME4_specialnet_port[4][1]->_y1;;
    // Via34_port2ME3[9][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(19);
    // x = Via34_drain2ME3[6][2]->_x;
    // y = ME4_specialnet_port[4][1]->_y1;;
    // Via34_port2ME3[9][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(20);
    // x = Via34_drain2ME3[2][2]->_x;
    // y = ME4_specialnet_port[5][0]->_y1;;
    // Via34_port2ME3[10][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(21);
    // x = Via34_drain2ME3[5][2]->_x;
    // y = ME4_specialnet_port[5][0]->_y1;;
    // Via34_port2ME3[10][1] = new Component(lib_name, inst_name, x, y);
    // // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(22);
    // x = Via34_drain2ME3[3][2]->_x;
    // y = ME4_specialnet_port[5][1]->_y1;;
    // Via34_port2ME3[11][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(23);
    // x = Via34_drain2ME3[4][2]->_x;
    // y = ME4_specialnet_port[5][1]->_y1;;
    // Via34_port2ME3[11][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(24);
    // x = Via34_drain2ME3[0][3]->_x;
    // y = ME4_specialnet_port[6][0]->_y1;;
    // Via34_port2ME3[12][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(25);
    // x = Via34_drain2ME3[7][3]->_x;
    // y = ME4_specialnet_port[6][0]->_y1;
    // Via34_port2ME3[12][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(26);
    // x = Via34_drain2ME3[1][3]->_x;
    // y = ME4_specialnet_port[6][1]->_y1;;//13
    // Via34_port2ME3[13][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(27);
    // x = Via34_drain2ME3[6][3]->_x;
    // y = ME4_specialnet_port[6][1]->_y1;;//13
    // Via34_port2ME3[13][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(28);
    // x = Via34_drain2ME3[2][3]->_x;
    // y = ME4_specialnet_port[7][0]->_y1;;//14
    // Via34_port2ME3[14][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(29);
    // x = Via34_drain2ME3[5][3]->_x;
    // y = ME4_specialnet_port[7][0]->_y1;;//14
    // Via34_port2ME3[14][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // inst_name = "Via34_port2ME3_" + to_string(30);
    // x = Via34_drain2ME3[3][3]->_x;
    // y = ME4_specialnet_port[7][1]->_y1;;//15
    // Via34_port2ME3[15][0] = new Component(lib_name, inst_name, x, y);
    // inst_name = "Via34_port2ME3_" + to_string(31);
    // x = Via34_drain2ME3[4][3]->_x;
    // y = ME4_specialnet_port[7][1]->_y1;;//15
    // Via34_port2ME3[15][1] = new Component(lib_name, inst_name, x, y);

    //0 1 2 3  0 1 2 3
    //7 6 5 4  7 6 5 4
    int count = 0;
    for(int i = 0 ; i<sqrt_numCS*2 ; i++)
        for(int j = 0 ; j<sqrt_numCS/2 ; j++)
        {
            string inst_name = "Via34_port2ME3_" + to_string(i*sqrt_numCS+j*2);
            // int x = Via34_drain2ME3[i%2*sqrt_numCS/2+j][i%sqrt_numCS]->_x;
            int x = Via34_drain2ME3[count][i%sqrt_numCS]->_x;

            int y = ME4_specialnet_port[i][j]->_y1;
            Via34_port2ME3[i*sqrt_numCS/2+j][0] = new Component(lib_name, inst_name, x, y);

            inst_name = "Via34_port2ME3_" + to_string(i*sqrt_numCS+j*2+1);
            x = Via34_drain2ME3[(sqrt_numCS*2-1)-count][i%sqrt_numCS]->_x;
            y = ME4_specialnet_port[i][j]->_y1;
            Via34_port2ME3[i*sqrt_numCS/2+j][1] = new Component(lib_name, inst_name, x, y);

            if(count == sqrt_numCS-1)count=0;
            else count++;

        }

    
    // # write info to def file
    vector<Component*>component_list;
    for(int i = 0 ; i<sqrt_numCS*2 ; i++)
        for(int j = 0 ; j<sqrt_numCS*2 ; j++)
            component_list.emplace_back(cs_array[i][j]);


    // # 4. add 'Via34_port2ME3' component to 'component_list'
    // # TODO
    for(int i = 0 ; i <numCS ; i++)
        for(int j = 0 ; j<2 ; j++)
            component_list.emplace_back(Via34_port2ME3[i][j]);



    vector<SpecialNet*> specialnet_list;
    for(int i = 0 ; i <sqrt_numCS*2 ; i++)
        for(int j = 0 ; j<sqrt_numCS ; j++)
            specialnet_list.emplace_back(ME3_specialnet[i][j]);


    for(int i = 0 ; i <sqrt_numCS*2 ; i++)
        for(int j = 0 ; j<sqrt_numCS*2 ; j++)
            specialnet_list.emplace_back(ME4_specialnet_drain[i][j]);

    for(int i = 0 ; i <sqrt_numCS*2 ; i++)
        for(int j =0 ; j<sqrt_numCS/2 ; j++)
            specialnet_list.emplace_back(ME4_specialnet_port[i][j]);

    for(int i = 0 ; i <sqrt_numCS*2 ; i++)
        for(int j = 0 ; j<sqrt_numCS*2 ; j++)
            component_list.emplace_back(Via34_drain2ME3[i][j]);



    string output_CS = to_string(numCS);
    string file_name = "../output/CS_"+output_CS+".def";
    file_name = argv[2];
    
    write_def(file_name, die, component_list, specialnet_list);
    
}

void write_def(string file_name, Die die,  vector<Component*> component_list,  vector<SpecialNet*> sepcialnet_list)
{
    ofstream fout;

    fout.open(file_name);
    fout<<"VERSION 5.6 ;\n";
    fout<<"DIVIDERCHAR \"/\" ;\n";
    fout<<"BUSBITCHARS \"[]\" ;\n";
    fout<<"DESIGN "<<die.design_name<<" ;\n\n";
    fout<<"UNITS DISTANCE MICRONS 1000 ;\n\n";
    fout<<"PROPERTYDEFINITIONS\n";
    fout<<"  COMPONENTPIN text STRING ;\n";
    fout<<"END PROPERTYDEFINITIONS\n\n";
    
 
    fout<<"DIEAREA ( " << die._x1 <<" "<<die._y1<< " ) "<< "( "<<die._x2<<" "<<die._y2<<" )"<<" ;\n\n";
    fout<<"COMPONENTS "<<component_list.size()<<" ;\n";
    for(auto component : component_list)
    {
        fout<<"- "<<component->inst_name<<" "<<component->lib_name<<"\n";
        fout<<"  + PLACED ( "<<component->_x<<" "<<component->_y<<" ) N ;\n";
    }
    fout<<"END COMPONENTS\n\n";

    int x, x1, x2, y, y1, y2;
    fout<<"SPECIALNETS "<<sepcialnet_list.size()<<" ;\n";
    for(auto specialnet : sepcialnet_list)
    {
        if(specialnet->layer =="ME3")
        {
            string name = specialnet->inst_name;
            // cout<<"_x1 : "<<specialnet->_x1<<" _x2 : "<<specialnet->_x2<<endl;
            x = (specialnet->_x1 + specialnet->_x2) / 2;
            y1 = specialnet->_y1;
            y2 = specialnet->_y2;
            fout<<" - "<<name<<"\n";
            fout<<"  + ROUTED ME3 440 "<<"( "<<x<<" "<<y1<< " ) "<<"( * "<<y2<<" ) ; \n";
        }

        else if(specialnet->layer =="ME4")
        {
            string name = specialnet->inst_name;
            x1 = specialnet->_x1;
            x2 = specialnet->_x2;
            y = (specialnet->_y1 + specialnet->_y2) / 2;
            fout<<" - "<<name<<"\n";
            fout<<"  + ROUTED ME4 1000 "<<"( "<<x1<<" "<<y<< " ) "<<"( "<<x2<<" * ) ; \n";
        }
  
    }
    fout<<"END SPECIALNETS\n\n";
    fout<<"END DESIGN\n";


}





    // int y = ME4_specialnet_port[0][0]->_y1;
    // Via34_port2ME3[0][0] = new Component(lib_name, inst_name, x, y);
  
    // y = ME4_specialnet_port[0][0]->_y1;;
    // Via34_port2ME3[0][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
 
    // y = ME4_specialnet_port[0][1]->_y1;;
    // Via34_port2ME3[1][0] = new Component(lib_name, inst_name, x, y);
 
    // y = ME4_specialnet_port[0][1]->_y1;;
    // Via34_port2ME3[1][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////

    // y = ME4_specialnet_port[1][0]->_y1;;
    // Via34_port2ME3[2][0] = new Component(lib_name, inst_name, x, y);
  
    // y = ME4_specialnet_port[1][0]->_y1;;
    // Via34_port2ME3[2][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
 
    // y = ME4_specialnet_port[1][1]->_y1;;
    // Via34_port2ME3[3][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[1][1]->_y1;;
    // Via34_port2ME3[3][1] = new Component(lib_name, inst_name, x, y);
    // //////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////
 
    // y = ME4_specialnet_port[2][0]->_y1;;
    // Via34_port2ME3[4][0] = new Component(lib_name, inst_name, x, y);
 
    // y = ME4_specialnet_port[2][0]->_y1;;
    // Via34_port2ME3[4][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////

    // y = ME4_specialnet_port[2][1]->_y1;;
    // Via34_port2ME3[5][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[2][1]->_y1;;
    // Via34_port2ME3[5][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////

    // y = ME4_specialnet_port[3][0]->_y1;;
    // Via34_port2ME3[6][0] = new Component(lib_name, inst_name, x, y);
 
    // y = ME4_specialnet_port[3][0]->_y1;;
    // Via34_port2ME3[6][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
  
    // y = ME4_specialnet_port[3][1]->_y1;;
    // Via34_port2ME3[7][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[3][1]->_y1;;
    // Via34_port2ME3[7][1] = new Component(lib_name, inst_name, x, y);
    // // ////////////////////////////////////////////////////////////////
    // // ////////////////////////////////////////////////////////////////

    // y = ME4_specialnet_port[4][0]->_y1;;
    // Via34_port2ME3[8][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[4][0]->_y1;;
    // Via34_port2ME3[8][1] = new Component(lib_name, inst_name, x, y);
    // //////////////////////////////////////////////////////////////
   
    // y = ME4_specialnet_port[4][1]->_y1;;
    // Via34_port2ME3[9][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[4][1]->_y1;;
    // Via34_port2ME3[9][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////

    // y = ME4_specialnet_port[5][0]->_y1;;
    // Via34_port2ME3[10][0] = new Component(lib_name, inst_name, x, y);

    // y = ME4_specialnet_port[5][0]->_y1;;
    // Via34_port2ME3[10][1] = new Component(lib_name, inst_name, x, y);
    // // ////////////////////////////////////////////////////////////////
  
    // y = ME4_specialnet_port[5][1]->_y1;;
    // Via34_port2ME3[11][0] = new Component(lib_name, inst_name, x, y);
 
    // y = ME4_specialnet_port[5][1]->_y1;;
    // Via34_port2ME3[11][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////
 
    // y = ME4_specialnet_port[6][0]->_y1;;
    // Via34_port2ME3[12][0] = new Component(lib_name, inst_name, x, y);
  
    // y = ME4_specialnet_port[6][0]->_y1;;
    // Via34_port2ME3[12][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
 
    // y = ME4_specialnet_port[6][1]->_y1;;
    // Via34_port2ME3[13][0] = new Component(lib_name, inst_name, x, y);
 
    // y = ME4_specialnet_port[6][1]->_y1;;
    // Via34_port2ME3[13][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
  
    // y = ME4_specialnet_port[7][0]->_y1;;
    // Via34_port2ME3[14][0] = new Component(lib_name, inst_name, x, y);
   
    // y = ME4_specialnet_port[7][0]->_y1;;
    // Via34_port2ME3[14][1] = new Component(lib_name, inst_name, x, y);
    // ////////////////////////////////////////////////////////////////
   
    // y = ME4_specialnet_port[7][1]->_y1;;
    // Via34_port2ME3[15][0] = new Component(lib_name, inst_name, x, y);
  
    // y = ME4_specialnet_port[7][1]->_y1;;
    // Via34_port2ME3[15][1] = new Component(lib_name, inst_name, x, y);
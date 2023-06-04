#pragma once
#include <vector>
#include <chrono>
#include "../Struct/Struct.hpp"
using namespace std;


extern vector<HardBlock*> HBList;
extern unordered_map<string, HardBlock*> HBTable;
 
class Floorplanner
{ 
  public:
    vector<TreeNode*> hbNodeList, cutNodeList;
    void InitNPE(vector<int>& NPE);//C
    bool Skewed(vector<int>& NPE, int idx);//C
    bool Ballot(vector<int>& NPE, int i);//C
    vector<int> Perturb(vector<int> NPE, int M);//M3 NC
    TreeNode* ConstructTree(vector<int>& NPE);//NC
    void Placement(TreeNode* node, int shapeIdx, int new_x, int new_y);//NC
    int GetCost_Place(vector<int>& NPE, bool const & forWL);//WC
    void SA(double epsilon, double r, int k, bool forWL, 
    vector<int>& curNPE, vector<int>& bestNPE,  chrono::high_resolution_clock::time_point begin);//WC
    
  
    int RegionOutline;
    Floorplanner()
    {
      for(auto hb : HBTable)
      {
        // HardBlock* cur_hb = HBList[i];
        TreeNode* hbNode = new TreeNode(0, hb.second);
        hbNode->shape = { {hb.second->width, hb.second->height, 0, 0}, {hb.second->height, hb.second->width, 1, 1}};
        hbNodeList.emplace_back(hbNode);
        TreeNode* cutNode = new TreeNode(-1);
        cutNodeList.emplace_back(cutNode);
      }

  

    };
   

};


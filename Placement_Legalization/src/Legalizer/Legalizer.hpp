#pragma once
#include "../Structure/Structure.hpp"

class Legalizer
{
    void CutRow();
    int GetRow(Cell  *cell);
    void UpdateEQW(Cluster* cluster, Cell* cell, int &trialWeight, double &trialQ, int &trialWidth);
    void AddCell(Cluster* cluster, Cell* cell, double optimalX);
    void AddCluster(Cluster* prevCluster, Cluster* cluster);

    int GetSubRow(Row  *row, Cell  *cell);
    int FakePlace(Row  *row, Cell *cell);
    void PlaceRow(Row  *row, int  &subRowIdx, Cell *cell);

    double GetCost(Cell  *cell);
    void Cluster2Cell();
    void GetDisplacement();
    Cluster* collapse(Cluster* cluster, SubRow* subRow);
    void Test_collapse(Cluster* cluster, SubRow* subRow, int final);

    void VisitDownRow(Cell *cell, int &BestRowIndex, double &BestCost, int &BestSubIndex, int DownIdx);
    void VisitUpRow(Cell *cell, int &BestRowIndex, double &BestCost, int &BestSubIndex, int UpIdx);




public:
  
    void Abacus();
   ;
};
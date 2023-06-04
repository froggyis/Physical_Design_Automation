#include "Legalizer.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>
#include <iostream>
using namespace std;

extern vector<Cell *> cells, terminals;
extern vector<Row *> rows;



void Legalizer::UpdateEQW(Cluster* cluster, Cell* cell, int &trialWeight, double &trialQ, int &trialWidth)
{

    trialWeight = cluster->weight + cell->weight;
    trialQ = cluster->q + cell->weight * (cell->x - cluster->width);
    trialWidth = cluster->width + cell->width;

}

void Legalizer::AddCell(Cluster* cluster, Cell* cell, double legalX)
{
  cluster->CellInCluster.emplace_back(cell);
  cluster->weight += cell->weight;
  cluster->q += cell->weight * (legalX - cluster->width);
  cluster->width += cell->width; 
}

void Legalizer::AddCluster(Cluster* prevCluster, Cluster* cluster)
{
    //use insert and copy will change the result, interestion.
    // prevCluster->CellInCluster.insert(prevCluster->CellInCluster.end(), cluster->CellInCluster.begin(), cluster->CellInCluster.end());

    copy (cluster->CellInCluster.begin(), cluster->CellInCluster.end(), back_inserter(prevCluster->CellInCluster));
    prevCluster->weight += cluster->weight;
    prevCluster->q += cluster->q - cluster->weight * prevCluster->width; 
    prevCluster->width += cluster->width;
}

void Legalizer::CutRow()
{    
    int terminal_minX, terminal_maxX;
    for(auto terminal : terminals)
    {
        terminal_minX = terminal->x;
        terminal_maxX = terminal_minX + terminal->width;
        for(auto row : rows)
        {
            if(row->coor_y + row->height <= terminal->y || row->coor_y >= (terminal->y + terminal->height) )continue;
            auto CurSubRow = row->SubRows.back();
            if(CurSubRow->min < terminal_minX)//if we meet a terminal we need to update subrow_max
            {
                if(CurSubRow->max > terminal_maxX)
                {
                    row->SubRows.emplace_back(new SubRow(terminal_maxX, CurSubRow->max));
                    CurSubRow->min = CurSubRow->min;
                    CurSubRow->max = terminal_minX;
                    CurSubRow->load = CurSubRow->max - CurSubRow->min;

                }
                CurSubRow->max = terminal_minX;
                if(CurSubRow->max <= terminal_maxX)
                CurSubRow->max = terminal_minX;
            }  
            else
            {
                if(CurSubRow->max > terminal_maxX)//if there's still space , we keep cutting subrow
                {    
                    CurSubRow->min = terminal_maxX;
                    CurSubRow->max = CurSubRow->max;
                    CurSubRow->load = CurSubRow->max - CurSubRow->min;

                
                }
                else
                    row->SubRows.pop_back();
            }
        }
    }

}

int Legalizer::GetRow(Cell  *cell)
{
    double y_diff = numeric_limits<double>::max();
    int row_number = rows.size() - 1 ;
    for(int i = 0 ; i<rows.size() ; i++)
    {
        if(fabs(cell->y - rows[i]->coor_y) < y_diff)
        {
            y_diff = fabs(cell->y - rows[i]->coor_y);
            row_number = i;
        }
        else break;
    }
    return row_number;
}

int Legalizer::GetSubRow(Row  *row, Cell  *cell)
{
    auto  &SubRows = row->SubRows;
    for (size_t i = 0; i < SubRows.size(); ++i)
    {
        auto subRow = row->SubRows[i];
        if (cell->x >= subRow->max)// if cell can't fit in current subrow then just continue the loop
            continue;

        if (cell->x >= subRow->min && cell->width <= subRow->load)return i;
        
        
        else if(i>0)
        {
            auto left_dif = abs(cell->x + cell->width - row->SubRows[i-1]->max);
            auto right_dif = abs(cell->x - subRow->min);

            if (left_dif < right_dif && cell->width <= row->SubRows[i - 1]->load)
                    return i - 1;
            if (cell->width <= subRow->load)
                return i;
        
        }
    }

    return -1;
}

int Legalizer::FakePlace(Row  *row, Cell *cell)
{
    // For Fakeplace function is different from paper
    // We pretend to place cell on a row to get it's cost
    // Unlike paper's pseudo I think this function is more implement friendly
    // We don't need to insert/remove cluster until we run the place function
  
    //get the corresponding row/subrow id
    // auto row = rows[Row_index];

    // int subRowIdx = GetSubRow(row, cell);
    int SubRowIndex = GetSubRow(row, cell);

    if (SubRowIndex == -1)
    {
        cell->legalX = cell->legalY = numeric_limits<double>::max();
        return -1;
    }

    auto  &subRow = row->SubRows[SubRowIndex];
    double legalX = cell->x;

    // check whether cell can fit with the boundary
    if (cell->x < subRow->min)
        legalX = subRow->min;
    else if (cell->x > subRow->max - cell->width)
        legalX = subRow->max - cell->width;

    auto cluster = subRow->LastCluster;//same as paper pick the last cluster

    if (cluster == nullptr || cluster->x + cluster->width <= legalX)//if this is new cell or cluster's width > cell's GP
    {
        cell->legalX = legalX;

    }

    else
    {

        int trialE ;double trialQ ;int trialW;
        UpdateEQW(cluster, cell, trialE, trialQ, trialW);
        cluster->test_e = trialE;cluster->test_q = trialQ; cluster->test_w = trialW;
        // double trialX;
        while (true)
        {
            //if we can set instance to fit with the boundary
            Test_collapse(cluster, subRow, 0);
            
            auto  &prevCluster = cluster->Predecessor;
    
            if (prevCluster != nullptr && prevCluster->x + prevCluster->width > cluster->test_x)//if overlap with pre cluster then combine them
            {
                cluster->test_e = prevCluster->weight + cluster->test_e;
                cluster->test_q = prevCluster->q + cluster->test_q - cluster->test_e * prevCluster->width;
                cluster->test_w = prevCluster->width + cluster->test_w;

                cluster = prevCluster;
            }
            else
            {
                break;
            }
        }
        cell->legalX = cluster->test_x + cluster->test_w - cell->width;

    }
    cell->legalY = row->coor_y;
    return SubRowIndex;
}

void Legalizer::PlaceRow(Row  *row, int  &SubRowIndex, Cell *cell)
{
    // auto subRow = rows[Row_index]->SubRows[SubRowIndex];
    auto subRow = row->SubRows[SubRowIndex];
    subRow->load -= cell->width;

    double legalX = cell->x;
    if (cell->x < subRow->min)legalX = subRow->min;
    else if (cell->x > subRow->max - cell->width) legalX = subRow->max - cell->width;

    auto cluster = subRow->LastCluster;
    if (cluster == nullptr || cluster->x + cluster->width <= legalX)
    {
        cluster = new Cluster(legalX, 0.0, subRow->LastCluster, 0, 0);
		AddCell(cluster, cell, legalX);
        subRow->LastCluster = cluster;

    }
    else
    {
        AddCell(cluster, cell, legalX);
        subRow->LastCluster = collapse(cluster, subRow);;
    }
}
void Legalizer::Test_collapse(Cluster* cluster, SubRow* subRow, int final)
{
    if(final ==1)
    {
        cluster->x = cluster->q / cluster->weight;
        if(cluster->x < subRow->min)
        {
            cluster->x = subRow->min;
        }
        if(cluster->x > subRow->max - cluster->width)
        {
            cluster->x = subRow->max - cluster->width;
        }
    }
    else
    {
    
        cluster->test_x = cluster->test_q / cluster->test_e;
        if(cluster->test_x < subRow->min)
            cluster->test_x = subRow->min;
        if(cluster->test_x > subRow->max - cluster->test_w)
            cluster->test_x = subRow->max - cluster->test_w;
    }
}


Cluster* Legalizer::collapse(Cluster* cluster, SubRow* subRow)
{
    while(true)
    {
        Test_collapse(cluster, subRow, 1);
        auto prevCluster = cluster->Predecessor;
        if(prevCluster != nullptr && prevCluster->x + prevCluster->width > cluster->x)
        {
           
            AddCluster(prevCluster,  cluster);
            delete cluster;
            cluster = prevCluster;
        }
        else break;
    }
    return cluster;
}





double Legalizer::GetCost(Cell  *cell)
{

    double x_diff = cell->legalX - cell->x;
    double y_diff = cell->legalY - cell->y;
    return sqrt(x_diff * x_diff + y_diff * y_diff);
}

void Legalizer::Cluster2Cell()
{
    for (auto  &row : rows)
    {
        int siteWidth = row->width;
        for (auto  &subRow : row->SubRows)
        {
            auto cluster = subRow->LastCluster;
            while (cluster != nullptr)
            {
                double shiftX = cluster->x - subRow->min;
                cluster->x = floor(shiftX / siteWidth) * siteWidth + subRow->min;
             
                int legalX = cluster->x;
                for (auto &cell : cluster->CellInCluster)
                {
                    cell->legalX = legalX;
                    cell->legalY = row->coor_y;
                    legalX += cell->width;
                }
                cluster = cluster->Predecessor;//pop back the cluster
            }
        }
    }
}

void Legalizer::Abacus()
{
    CutRow();
    // I sort the cells array in parser
  
    for (auto  &cell : cells)
    {
        // paper claim we iterate through rows
        // I change to we look for th enearest row
        // and check which traverse up&down.
        int BestRowIndex = GetRow(cell);
        int BestSubIndex = FakePlace(rows[BestRowIndex], cell);
        double bestCost = GetCost(cell);
        size_t DownIdx = BestRowIndex, UpIdx = BestRowIndex;
        //due to paper's hint , we only need to traverse row up and donw
        //until the vertical displacemnt is larger than cost, then we can stop.
        VisitDownRow(cell, BestRowIndex, bestCost, BestSubIndex, DownIdx);
        VisitUpRow(cell, BestRowIndex, bestCost, BestSubIndex, UpIdx);
        // PlaceRow(BestRowIndex, BestSubIndex, cell);
        
        PlaceRow(rows[BestRowIndex], BestSubIndex, cell);

    }
    //Transform cluster position to cell positions
    for (auto  &row : rows)
    {
        int siteWidth = row->width;
        for (auto  &subRow : row->SubRows)
        {
            auto cluster = subRow->LastCluster;
            while (cluster != nullptr)
            {
                double shiftX = cluster->x - subRow->min;
                cluster->x = floor(shiftX / siteWidth) * siteWidth + subRow->min;
                int legalX = cluster->x;
                for (auto &cell : cluster->CellInCluster)
                {
                    cell->legalX = legalX;
                    cell->legalY = row->coor_y;
                    legalX += cell->width;
                }
                cluster = cluster->Predecessor;//pop back the cluster
            }
        }
    }
}





void Legalizer::VisitDownRow(Cell *cell, int &BestRowIndex, double &BestCost, int &BestSubIndex, int DownIdx)
{
    size_t diff;
    if(DownIdx>0)
    {
        do
        {
            diff= abs(cell->y - rows[DownIdx]->coor_y);
            DownIdx -= 1;
            int SubRowIndex = FakePlace(rows[DownIdx], cell);
            double cost = GetCost(cell);
            if (cost < BestCost)
            {
                BestRowIndex = DownIdx;
                BestSubIndex = SubRowIndex;
                BestCost = cost;
            }
        }while(DownIdx>0 && diff < BestCost);
    }
    else return;
}

void Legalizer::VisitUpRow(Cell *cell, int &BestRowIndex, double &BestCost, int &BestSubIndex,int UpIdx)
{
    size_t diff;
    if(UpIdx < rows.size() - 1)
    {
        do
        {
            diff= abs(cell->y - rows[UpIdx]->coor_y);
            UpIdx += 1;
            
            int SubRowIndex = FakePlace(rows[UpIdx], cell);
            double cost = GetCost(cell);
            if (cost < BestCost)
            {
                BestRowIndex = UpIdx;
                BestSubIndex = SubRowIndex;
                BestCost = cost;
            }
        }while(UpIdx < rows.size() - 1 && diff < BestCost);
    }
    else return;


}
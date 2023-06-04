#include "Legalizer/Legalizer.hpp"
#include "Parser/Parser.hpp"
#include "Structure/Structure.hpp"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

vector<Cell *> cells, terminals;
vector<Row *> rows;

void WriteResult(string filename);
void CalDisplacement();


int main(int argc, char **argv)
{
 
    srand (1);
    Parser *parser = new Parser();
    parser->readAux(argv[1]);
    parser->readNode();
    parser->readPl();
    parser->readScl();
    

    Legalizer *legalizer = new Legalizer();
    legalizer->Abacus();
   
    //calculate final result
  
    CalDisplacement();
    WriteResult(argv[2]);


    return 0;
}

void CalDisplacement()
{
    double TotalDis = 0;
    double MaxDis = 0;
    double diff_x, diff_y;
    double total_diff;
    for (auto &cell : cells)
    {
        diff_x = cell->legalX - cell->x;
        diff_y = cell->legalY - cell->y;
        total_diff = sqrt(diff_x * diff_x + diff_y * diff_y);
        TotalDis += total_diff;
        (MaxDis<total_diff) ? MaxDis = total_diff : MaxDis = MaxDis;
    }
    cout<<"Total displacement : "<<TotalDis<<endl;
    cout<<"Max displacement : "<<MaxDis<<endl;
}

void WriteResult(string filename)
{
    // string auxFilePath(filename_);
    // auto filename = auxFilePath.substr(auxFilePath.find_last_of('/') + 1,
    //                                    auxFilePath.find_last_of('.') - auxFilePath.find_last_of('/') - 1);
    
    // ofstream fout("../output/" + filename + ".result");
    ofstream fout(filename);

    for (auto &cell : cells)
    {
        fout << cell->name <<" "<< static_cast<int>(cell->legalX) <<" "<< static_cast<int>(cell->legalY) <<"\n";
    }

    for (auto &terminal : terminals)
    {
        fout << terminal->name <<" "<<static_cast<int>(terminal->x) <<" "<<static_cast<int>(terminal->y) <<"\n";
    }
}
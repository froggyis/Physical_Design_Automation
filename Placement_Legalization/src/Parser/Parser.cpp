#include "Parser.hpp"
#include <unordered_map>
#include <iostream>
#include <algorithm>
using namespace std;

extern vector<Cell *> cells, terminals;
extern vector<Row *> rows;
unordered_map<string, Cell *> TmpCellMap;

 
void Parser::readAux(string filename)
{
    vector<string> vect;
    string tmp = filename;

    size_t found = tmp.find_last_of("/\\");
    this->prefix = tmp.substr(0, found);
    cout <<this->prefix <<endl;

    stringstream ss(filename);

    while( ss.good() )
    {
        string substr;
        getline( ss, substr, '/' );
        vect.emplace_back( substr );
    }
    this->testcase = vect[2];// my vector contains ["..", "testcase", "ibm01", "ibm01.aux"]

    ifstream in_aux(filename);
    string identifier, comma, node, pl, scl;
    int MaxDisplacement;
    while(in_aux>>identifier)
    {
        if(identifier == "RowBasedPlacement")
        {
            in_aux>>comma>>node>>pl>>scl;
            this->file_node_name = node;
            this->file_pl_name = pl;
            this->file_scl_name = scl;
        }
        else
        {
            in_aux>>comma>>MaxDisplacement;
            this->MaxDisplacement = MaxDisplacement;
        }

    }
    
    // cout<<this->file_node_name<<endl;
    // cout<<this->file_pl_name<<endl;
    // cout<<this->file_scl_name<<endl;

    
}


void Parser::readNode()
{
    string input = this->prefix + "/" + this->file_node_name;
    ifstream fin(input);
    string s, line;
    int nodeCnt, terminalCnt;
    string name, terminal;
    int width, height;
    while(getline(fin, s)) 
    {
        if(s.empty()) break; 
        stringstream ss(s);
        string identifier, _;
        while(ss >> identifier) 
        {
            if(identifier == "NumNodes")ss >> _ >> nodeCnt;
            
            else ss >> _ >> terminalCnt;
        }
    }
     while(getline(fin, line))
    {
        
        size_t found1 = line.find("terminal");
        if (found1 != line.npos) //if terminal is found
        {
            stringstream sss(line);
            sss>>name>>width>>height>>terminal;
            // cout<<name<<", "<<width<<", "<<height<<endl;
            Cell *cell = new Cell(name, width, height);
            TmpCellMap[name] = cell;
            terminals.emplace_back(cell);
        }
        else//if terminal is not found
        {

            stringstream ssss(line);
            ssss>>name>>width>>height;
            Cell *cell = new Cell(name, width, height);
            TmpCellMap[name] = cell;
            cells.emplace_back(cell);
      
        }
    }
       
}



void Parser::readPl()
{
    string input = this->prefix + "/" + this->file_pl_name;

    ifstream in_pl(input);
    string name, tmp, line;
    double x, y;

    while(getline(in_pl, line))
    {
        
        size_t found1 = line.find("FIXED");
        if (found1 != line.npos) //if FIXED is found
        {
            stringstream ss(line);
            ss>>name>>x>>y;
            TmpCellMap[name]->x = x;
            TmpCellMap[name]->y = y; 
        }
        else//if FIXED is NOT found
        {
            stringstream sss(line);
            sss>>name>>x>>y;
            TmpCellMap[name]->x = x;
            TmpCellMap[name]->y = y; 
        }
    }
    //we sort terminals by its X location
    sort(terminals.begin(), terminals.end(), [&](auto &a, auto &b){ return a->x < b->x; });
    sort(cells.begin(), cells.end(), [&](auto &a, auto &b){ return a->x < b->x; });

    
}

void Parser::readScl()
{
    string input = this->prefix + "/" + this->file_scl_name;

    ifstream in_scl(input);
    string identifier, tmp,_;
    int NumRows ;
    getline(in_scl, identifier);
    stringstream ss(identifier);
    ss>>tmp;
    if(tmp == "NumRows")ss>>_>>NumRows;

    for(int i = 0 ; i<NumRows; i++)
    {   
        int Coordinate, Height, Sitewidth, NumSites, SubrowOrigin;
        while(in_scl>>identifier)
        {
            if(identifier=="Coordinate")in_scl>>_>>Coordinate;
            else if(identifier=="Height")in_scl>>_>>Height;
            else if(identifier=="Sitewidth")in_scl>>_>>Sitewidth;
            else if(identifier=="NumSites")in_scl>>_>>NumSites;
            else if(identifier=="SubrowOrigin")in_scl>>_>>SubrowOrigin;
            else if(identifier=="End")
            {
                Row *row = new Row(Sitewidth, Height, Coordinate);
                SubRow *tmp_subrow = new SubRow(SubrowOrigin, SubrowOrigin+Sitewidth*NumSites);
                row->SubRows.emplace_back(tmp_subrow);
                rows.emplace_back(row);
            }

        }

    }
    
}

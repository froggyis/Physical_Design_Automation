#pragma once
#include <string>
#include <vector>
using namespace std;

struct Cell
{
    string name;
    int width, height, weight;
    double x, y;
    double legalX, legalY;

    Cell(string  &name, int  &width, int  &height)
        : name(name), width(width), height(height), weight(width), x(0), y(0), legalX(0), legalY(0) {}
};

struct SubRow;
struct Row
{
    int width, height;
    int coor_y;
    std::vector<SubRow *> SubRows;

    Row(int  &width, int  &height, int  &coor_y)
        : width(width), height(height), coor_y(coor_y) {}
};

struct Cluster;
struct SubRow
{
    int min, max;
    int load;
    Cluster *LastCluster;

    SubRow(int const &min, int const &max)
        : min(min), max(max), load(max - min), LastCluster(nullptr) {}
 
};

struct Cluster
{
    double x, q;
    int width, weight;
    double test_e, test_q, test_w, test_x;
    vector<Cell *> CellInCluster;
    Cluster *Predecessor;

    Cluster(double const &x, double const &q, Cluster *Predecessor, int const &weight,  int const &width)
        : x(x), q(q), width(width), weight(weight), Predecessor(Predecessor) {}
};

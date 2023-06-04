using namespace std;
#include <string>
#include <iostream>

class Die
{
    public :
        string design_name;
        int _x1, _y1, _x2, _y2;

    Die(string design_name, int x1, int y1, int x2, int y2)
    :design_name(design_name), _x1(x1), _y1(y1), _x2(x2), _y2(y2) {}
    
};

class Component
{
    public:
        string lib_name, inst_name;
        int _x, _y;

    Component(string lib_name, string inst_name, int x, int y)
    :lib_name(lib_name), inst_name(inst_name), _x(x), _y(y){}
    
};

class SpecialNet
{
    public:
        string inst_name, layer;
        int _x1, _y1, _x2, _y2;

    SpecialNet(string inst_name, string layer, int x1, int y1, int x2, int y2)
    :inst_name(inst_name),layer(layer), _x1(x1), _y1(y1), _x2(x2), _y2(y2) {}
    
};

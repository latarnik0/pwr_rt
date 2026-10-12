#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

struct Point
{
    float x,y;
    int id;
};

void readFromFile(std::ifstream& file, std::vector<Point>& points, int N, float M)
{
    std::ifstream file("example_input.txt");
    std::string line;
    
    if(!file.is_open())
    {
        std::cerr<<"cannot open file\n";
    }

    getline(file, line);
    N = stoi(line);
    getline(file, line);
    M = stof(line);
    
    Point p;
    while(getline(file, line))
    {
        std::string idS, xS, yS;
        std::stringstream ss(line);
        ss >> idS >> xS >> yS;

        p.id = stoi(idS);
        p.x = stof(xS);
        p.y = stof(yS);

        points.push_back(p);
        p=Point();
    }
    file.close();
}

int main()
{
    return 0;
}
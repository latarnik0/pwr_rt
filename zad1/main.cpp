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

void readFromFile(std::vector<Point>& points, int& N, float& M)
{
    std::ifstream file("/home/latarnik3/pwr_rt/zad1/example_input.txt");
    std::string line;
    
    if(!file.is_open())
    {
        std::cerr << "cannot open file\n";
        return;
    }

    if (getline(file, line)) N = stoi(line);
    if (getline(file, line)) M = stof(line);
    
    Point p;
    while(getline(file, line))
    {
        if (line.empty()) continue;
        std::string idS, xS, yS;
        std::stringstream ss(line);

        if (ss >> idS >> xS >> yS) 
        {
            p.id = stoi(idS);
            p.x = stof(xS);
            p.y = stof(yS);

            points.push_back(p);
            p = Point();
        }
    }
    file.close();
}

int main()
{
    std::vector<Point> points = {};
    int N = 0;
    float M = 0.0f;
    readFromFile(points, N, M);
    std::cout<<points.size();

    return 0;
}
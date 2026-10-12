#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

struct Point
{
    float x,y;
    int id;
    std::vector<int> distancesFromOtherPoints;
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

    // there are only 12 records in the example file - brute force can be used
    // calculate distances from every point to every other point
    float temp=0.0;
    for(int i=0; i<points.size(); ++i)
    {
        for(int j=0; j<points.size(); ++j)
        {
            if(i == j)
            {
                // 0 indicates current point
                points[i].distancesFromOtherPoints.push_back(0);
                continue;
            }

            temp = std::sqrt(std::pow(points[i].x - points[j].x, 2.0f)
                + std::pow(points[i].y - points[j].y, 2.0f));

            if(temp < M)
            {
                // 2 indicates that this point is too close to point i
                points[i].distancesFromOtherPoints.push_back(2);
            }
            else
            {
                // 1 indicates that this point is out of merging zone
                points[i].distancesFromOtherPoints.push_back(1);
            }
        }
    }
    return 0;
}
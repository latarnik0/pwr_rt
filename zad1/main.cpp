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

void calculateDistances(std::vector<Point>& points, float& M)
{
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
}

// every distancesFromOtherPoints vector of every point creates a square matrix like this (example not real output):
// [0, 1, 1, 1]
// [1, 0, 2, 1] - this is supposed to merge with the one below
// [1, 2, 0, 1]
// [1, 1, 1, 0]

// We look at the points as graph so the matrix above is adjacency matrix of this graph
// In traditional adjacency matrix where there is no connection between the nodes we type 0
// here, when distance from the points is >M we type 1 (non-merging zone)
// We need to traverse this matrix in order to find which points are connected (distance < M, merging zone)
// In order to avoid creation of the same multiple points (for example point 1 with point 2 and point 2 with point 1) 
// we need to remember which points were already visited

int main()
{
    std::vector<Point> points = {};
    int N = 0;
    float M = 0.0f;
    
    readFromFile(points, N, M);
    calculateDistances(points, M);
    
    return 0;
}
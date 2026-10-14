#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// ##################   ZADANIE 1.1   ##################
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

// We can look at the points as graph so, the matrix above is adjacency matrix of this graph
// In traditional adjacency matrix where there is no connection between the nodes we type 0
// Here, when distance from the points is >M we type 1 (non-merging zone)
// We need to traverse this matrix in order to find which points are connected (distance < M, merging zone)
// In order to avoid creation of the same multiple points (for example point 1 with point 2 and point 2 with point 1) 
// we need to remember which points were already visited
// In order to achieve this we can use DFS algorithm

// EDGE CASE 1: i assume that 0 means point is calculating distance from itself but it is possible
// that there are going to be points with the same (x,y) coordinates but different ID's - TODO

// This function checks for points that are close enough to be merged and saves them to local vector toBeMerged
// This vector is build for every point so we need to collect all of these distinct toBeMerged vectors (those with .size() > 1)
void dfs(int startingPoint, std::vector<Point>& points, std::vector<bool>& visited, std::vector<int>& toBeMerged)
{
    // set current point as visited
    visited[startingPoint] = true;
    // add current point to the current merging group
    toBeMerged.push_back(startingPoint + 1);

    // for every point of the graph
    for(int i=0; i<points.size(); ++i)
    {
        if(visited[i] || startingPoint == i)
        {
            continue;
        }
        
        // if distance for startingPoint to the i-th point is 2  
        if(points[startingPoint].distancesFromOtherPoints[i] == 2)
        {
            // recursively visit the i-th point and all its valid neighbors
            dfs(i, points, visited, toBeMerged);
        }
    }
}

// This function collects these distinct toBeMerged vectors mentioned above
std::vector<std::vector<int>> collectAllToBeMergedVectors(std::vector<Point>& points)
{
    std::vector<bool> visited(points.size(), false);
    std::vector<std::vector<int>> result;

    for(int i=0; i<points.size(); ++i)
    {
        if(visited[i])
        {
            continue;
        }

        std::vector<int> toBeMerged;
        dfs(i, points, visited, toBeMerged);

        // Check if a point has other points in <M proximity
        // If not, it is a single point, we do not include it
        if(toBeMerged.size() > 1)
        {
            result.push_back(toBeMerged);
        }
    }
    return result;
}

// This function does the actual merging of points (calculating average (x,y) coordinates and giving them new ID's) 
std::vector<Point> merge(const std::vector<Point>& points, const std::vector<std::vector<int>>& result)
{
    std::vector<Point> mergedPoints;
    const int startingId = points.size();

    for(const std::vector<int>& group : result)
    {
        float xav = 0.0f;
        float yav = 0.0f;
        const int groupSize = group.size();

        for(int memberId : group)
        {
            const int index = memberId - 1;
            xav += points[index].x;
            yav += points[index].y;
        }

        Point mergedPoint;
        mergedPoint.x = xav / groupSize;
        mergedPoint.y = yav / groupSize;
        mergedPoint.id = startingId + mergedPoints.size() + 1;
        mergedPoints.push_back(mergedPoint);
        mergedPoint = Point();
    }
    return mergedPoints;
}

// ##################   ZADANIE 1.2   ##################

// First we find the extreme points (those with: minX, maxX, minY, maxY coordinates)
// We need them because in determining the shortest path that wraps all points those extreme points 
// are going to be always on the outline - they define boundries
// Next we can connect them and discard all points that are inside this figure - they will always be inside
// so we shouldnt take them into considiration
// Finally we can use Jarvis march algorithm to find remaining points that do not fit into this area, so
// they need to make the outline giving us the shortest possible path to wrap all of the points

struct Extremes
{
    Point minX;
    Point maxX;
    Point minY;
    Point maxY;
};

// This function is simple search for min/max in a vector
Extremes findExtremePoints(const std::vector<Point>& points)
{
    if(points.empty())
    {
        return {}; 
    }

    Extremes e = {points[0], points[0], points[0], points[0]};
    for(const auto& p : points)
    {
        if(p.x < e.minX.x) e.minX = p;
        if(p.x > e.maxX.x) e.maxX = p;
        if(p.y < e.minY.y) e.minY = p;
        if(p.y > e.maxY.y) e.maxY = p;
    }
    return e;
}

// This function is a helper to determine if a point P is to the left or to the right from AB line
// It is essential in order to filter out points that will be inside the outline (they will be to the right of a vertex)
// and to include all other points to build an actual shortest path (they will be to the left of a vertex)
float crossProduct(const Point& A, const Point& B, const Point& P)
{
    return (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
}

// This function checks if a point P is inside 2D figure created from extremes
bool isInside(const Point& A, const Point& B, const Point& C, const Point& D, const Point& P)
{
    // If the point P is inside it will be to the right of every vertex of this quadrilateral
    return crossProduct(A, B, P) >= 0 &&
           crossProduct(B, C, P) >= 0 &&
           crossProduct(C, D, P) >= 0 &&
           crossProduct(D, A, P) >= 0;
}

// This function is an actual filter of the points that won't be taken into considiration in Jarvis march algorithm 
std::vector<Point> filterOutInsidePoints(const std::vector<Point>& points, const Extremes& e)
{
    std::vector<Point> filteredPoints;
    filteredPoints.reserve(points.size());
    Point A = e.minX;
    Point B = e.minY;
    Point C = e.maxX;
    Point D = e.maxY;

    for(const auto& p : points)
    {
        // Extremes are making the outline so we include them
        if(p.id == A.id || p.id == B.id || p.id == C.id || p.id == D.id)
        {
            filteredPoints.push_back(p);
            continue;
        }

        // If a point is NOT inside, we include it
        if(!isInside(A, B, C, D, p))
        {
            filteredPoints.push_back(p);
        }
    }
    return filteredPoints;
}

// This function is a helper that addresses the EDGE CASE 2 mentioned below
float calcDist(const Point& p1, const Point& p2)
{
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// This function is a standard Jarvis march algorithm
std::vector<Point> jarvisMarch(const std::vector<Point>& filteredPoints)
{
    std::vector<Point> outline;
    
    // The starting point is a point with smallest x coordinate
    // EDGE CASE 1: there could be multiple points with the same smallest X coordinate.
    // In that case we look at smallest Y coordinate. It could be that there are points with the same
    // coordinates but different ID's like in part one of this task, but here it does not matter
    // because we operate in a 2D setup. This edge case will be fixed so it wont be a concern
    int startingPoint = 0;
    
    // We start from the first next point 
    for(int i=1; i<filteredPoints.size(); ++i)
    {
        if(filteredPoints[i].x < filteredPoints[startingPoint].x)
        {
            startingPoint = i;
        }
        if(filteredPoints[i].x == filteredPoints[startingPoint].x && filteredPoints[i].y < filteredPoints[startingPoint].y)
        {
            startingPoint = i;
        }
    }

    int p = startingPoint;
    int q = 0;

    // Start of the march 
    do
    {
        // Add current point to the outline
        outline.push_back(filteredPoints[p]);

        // We set 'q' as the next candidate to be included in the outline.
        // Zmieniono na modulo, aby uniknąć wyjścia poza zakres tablicy dla ostatniego elementu.
        q = (p + 1) % filteredPoints.size();

        for(int i=0; i<filteredPoints.size(); ++i)
        {
            if (i == p) continue;

            // We check the orientation of point 'i' relative to the directed line created by points pq 
            // to see if it makes a more extreme turn than q
            float cross = crossProduct(filteredPoints[p], filteredPoints[q], filteredPoints[i]);
            
            // If this is true then we found point that is further away to the left
            // It becomes the next candidate
            if(cross < 0)
            {
                q = i;
            }
            // EDGE CASE 2: it could be that points are in the straight line
            // In that case we choose the further one (further from point p) 
            else if(cross == 0.0f)
            {
                if(calcDist(filteredPoints[p], filteredPoints[i]) > calcDist(filteredPoints[p], filteredPoints[q]))
                {
                    q = i;
                }
            }
        }

        p = q;
    
    // The loop ends when we reach the beginning
    } while (p != startingPoint);

    return outline;
}

// This function is a wrapper for task 1.2
std::vector<Point> getShortestOutline(const std::vector<Point>& points)
{
    // First step: find the extremes
    Extremes e = findExtremePoints(points);

    // Second step: filter out the inside points
    std::vector<Point> filteredPoints = filterOutInsidePoints(points, e);

    // Third step: determine the outline from filtered points
    std::vector<Point> outline = jarvisMarch(filteredPoints);

    return outline;
}

// This function calculates the length of the outline
float calculateOutlineLength(std::vector<Point>& outline)
{
    float totalLength = 0.0f;
    for (int i=0; i<outline.size(); ++i)
    {
        const Point& p1 = outline[i];
        const Point& p2 = outline[(i + 1) % outline.size()]; 

        totalLength += calcDist(p1, p2);
    }
    return totalLength;
}

// These 2 functions below replace points that are going to be merged with the merged ones
// as for the task 1.2 the input should be vector with new points
// This is not optimal solution in terms of runtime and memory allocation 
// but for 12 records it is acceptable
bool isGoingToBeMerged(int pointId, const std::vector<std::vector<int>>& result)
{
    for(int i=0; i<result.size(); ++i)
    {
        for(int j=0; j<result[i].size(); ++j)
        {
            if(result[i][j] == pointId)
            {
                return true;
            }
        }
    }
    return false;
}

void updateOriginalPoints(std::vector<Point>& points, const std::vector<std::vector<int>>& result, const std::vector<Point>& mergedPoints)
{
    std::vector<Point> newPoints;
    newPoints.reserve(points.size());

    // In new vector include only those points which are not going to be merged
    for(int i=0; i<points.size(); ++i)
    {
        if(!isGoingToBeMerged(points[i].id, result))
        {
            newPoints.push_back(points[i]);
        }
    }

    // Add new points created from merging
    for(int i=0; i<mergedPoints.size(); ++i)
    {
        newPoints.push_back(mergedPoints[i]);
    }
    points = newPoints;
}

int main()
{
    std::vector<Point> points = {};
    int N = 0;
    float M = 0.0f;
    
    // ZAD 1.1
    readFromFile(points, N, M);
    calculateDistances(points, M);
    std::vector<std::vector<int>> result = collectAllToBeMergedVectors(points);
    std::vector<Point> mergedPoints = merge(points, result);
    updateOriginalPoints(points, result, mergedPoints);

    // ZAD 1.2
    std::vector<Point> outline = getShortestOutline(points);
    float lengthOfOutline = calculateOutlineLength(outline);

    return 0;
}
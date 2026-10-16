#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// ścieżka do pliku
std::string PATH_TO_TASK1 = "example_input.txt";
// ##################   ZADANIE 1.1   ##################

// Aby wyznaczyć punkty kwalifikujące się do połączenia obliczę odległość każdego punktu do każdego innego punktu
// Decyduje się na rozwiązanie brute force z powodu małej liczby rekordów w pliku

struct Point
{
    float x,y;
    int id;
    std::vector<int> distancesFromOtherPoints;
};

// Funkcja czytania danych z pliku
void readFromFile(std::vector<Point>& points, int& N, float& M)
{
    std::ifstream file(PATH_TO_TASK1);
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

// Funkcja obliczania odległości każdego punktu do każdego innego punktu
void calculateDistances(std::vector<Point>& points, float& M)
{
    float temp=0.0;
    for(int i=0; i<points.size(); ++i)
    {
        for(int j=0; j<points.size(); ++j)
        {
            if(i == j)
            {
                // 0 będzie oznaczać samego siebie (jako punkt)
                points[i].distancesFromOtherPoints.push_back(0);
                continue;
            }

            // standardowy wzór na odległość dwóch punktów 2D
            temp = std::sqrt(std::pow(points[i].x - points[j].x, 2.0f)
                + std::pow(points[i].y - points[j].y, 2.0f));

            if(temp < M)
            {
                // 2 będzie oznaczać że punkt 'i' znajduje się w "strefie łączenia"
                points[i].distancesFromOtherPoints.push_back(2);
            }
            else
            {
                // 1 będzie oznaczać że punkt jest dostatecznie daleko i nie będziemy go łączyć
                points[i].distancesFromOtherPoints.push_back(1);
            }
        }
    }
}

// każdy wektor distancesFromOtherPoints każdego punktu tworzy taką macierz kwadratową (przykład):
// [0, 1, 1, 1]
// [1, 0, 2, 1]
// [1, 2, 0, 1]
// [1, 1, 1, 0]
// widzimy z powyższej macierzy że istnieją 2 punkty do scalenia

// Punkty można traktować jako graf, zatem powyższa macierz jest macierzą sąsiedztwa tego grafu
// W tradycyjnej, matematycznej macierzy sąsiedztwa w miejscach gdzie nie ma połączenia między wezłami wpisujemy 0
// Tutaj gdy odległość między punktami jest większa od M wpisujemy 1 (dostatecznie daleko - nie scalamy)
// Gdy odległość jest <M wpisujemy 2 (trzeba scalić)
// Musimy przeszukać tą macierz aby ustalić które punkty są zbyt blisko (odległość < M, scalamy)
// Aby uniknąć wielokrotnego tworzenia tych samych nowych punktów (na przykład punktu 1 z punktem 2 daje punkt 3 
// oraz punktu 2 z punktem 1 co też da punkt 3), musimy pamiętać które punkty zostały już odwiedzone
// Użyje do tego algorytmu DFS

// Ta funkcja to standardowy algorytm DFS. Odwiedza każdy punkt i sprawdza czy dany punkt leży w strefie scalania
// Następnie zapisuje grupy punktów które powinny być połączone ze sobą do wektora toBeMerged
// Funkcja ta na początku dodaje aktualny punkt do wektora więc jeśli punkt nie ma bliskich sąsiadów
// ten wektor będzie miał tylko ten jeden punkt. Należy to wziąć pod uwagę później
// Wektor toBeMerged jest budowany dla każdego punktu zatem trzeba będzie pozbierać te wektory (patrz funkcję niżej)
void dfs(int startingPoint, std::vector<Point>& points, std::vector<bool>& visited, std::vector<int>& toBeMerged)
{
    // aktualny punkt jako odwiedzony
    visited[startingPoint] = true;
    // dodaj aktualny punkt do obecnej grupy do scalenia
    toBeMerged.push_back(startingPoint);

    // dla każdego punktu w grafie
    for(int i=0; i<points.size(); ++i)
    {
        if(visited[i] || startingPoint == i)
        {
            // jeśli już tu byliśmy przejdz do nastepnego punktu
            continue;
        }
        
        // jeśli i-ty punkt nie byl odwiedzany i leży w strefie scalania (wartość 2)
        if(points[startingPoint].distancesFromOtherPoints[i] == 2)
        {
            // rekurencyjnie odwiedź i-ty punkt i wszystkich jego sąsiadów
            // musimy sprawdzać wszystkich sąsiadów aby nie dochodziło do duplikacji
            dfs(i, points, visited, toBeMerged);
        }
    }
}

// Ta funkcja zbiera wszystkie grupy punktów do scalenia (wektory toBeMerged)
std::vector<std::vector<int>> collectAllToBeMergedVectors(std::vector<Point>& points)
{
    // na początku żaden punkt nie jest odwiedzony
    std::vector<bool> visited(points.size(), false);
    std::vector<std::vector<int>> result;

    // wywołujemy dfs dla każdego punktu w grafie
    for(int i=0; i<points.size(); ++i)
    {
        if(visited[i])
        {
            continue;
        }

        std::vector<int> toBeMerged;
        dfs(i, points, visited, toBeMerged);

        // sprawdź czy wektor ma więcej niż 1 element
        // jeśli nie, pomijamy go ponieważ to pojedynczy punkt bez bliskich sąsiadów
        if(toBeMerged.size() > 1)
        {
            result.push_back(toBeMerged);
        }
    }
    return result;
}

// Ta funkcja scala grupy punktów które są zwracane z funkcji powyżej, liczy średnie (x,y) oraz nadaje punktom nowe ID
std::vector<Point> merge(const std::vector<Point>& points, const std::vector<std::vector<int>>& result)
{
    std::vector<Point> mergedPoints;
    const int startingId = points.size();

    // dla każdej grupy do scalenia przechodzimy przez każdy punkt
    for(const std::vector<int>& group : result)
    {
        float xav = 0.0f;
        float yav = 0.0f;
        const int groupSize = group.size();

        for(int memberId : group)
        {
            xav += points[memberId].x;
            yav += points[memberId].y;
        }

        // tworzymy nowy punkt, dodajemy go do wektora nowych punktow, czyścimy strukturę mergedPoint przed kolejną iteracją
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

// Do tego zadania moje rozumowanie jest nastepujące:
// Najpierw znajdujemy punkty skrajne w każdym kierunku (minX, maxX, minY, maxY)
// Są one potrzebne, ponieważ przy wyznaczaniu najkrótszej ścieżki obejmującej wszystkie punkty
// te zawsze muszą się znaleźć na tej ścieżce - wyznaczają one granice
// Następnie możemy je połączyć ze sobą i odrzucić wszystkie punkty znajdujące się wewnątrz tak powstałego czworokąta –
// skoro zawsze będą one w środku, nie musimy brać ich pod uwagę
// Na koniec możemy użyć algorytmu Jarvisa, aby znaleźć pozostałe punkty, które nie mieszczą się w w tej figurze,
// a zatem muszą tworzyć ścieżkę
// To podejście zapewni znalezienie najkrótszej możliwej ścieżki obejmującej wszystkie punkty

struct Extremes
{
    Point minX;
    Point maxX;
    Point minY;
    Point maxY;
};

// Ta funkcja szuka punktow ekstremalnych
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

// Ta funkcja to helper aby sprawdzić czy punkt P znajduje sie po lewej czy po prawej stronie odcinka stworzonego
// z dwóch innych punktów AB
// Ta funkcja jest potrzebna przy odrzucaniu punktów które znajdą się w środku (po połączeniu skrajnych punktów)
// wtedy każdy wierzchołek tak powstałej figury 2D będzie miał te punkty po swojej prawej stronie 
// (albo lewej, ja zakładam konwencję że figura jest tworzona zgodnie z ruchem wskazówek zegara)
// oraz do wyznaczania najkrótkszej ścieżki (wtedy szukamy punktów najbardziej na lewo)
float crossProduct(const Point& A, const Point& B, const Point& P)
{
    return (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
}

// Ta funkcja sprawdza czy punkt znajduje się wewnątrz figury powstałej z połączenia punktow skrajnych
bool isInside(const Point& A, const Point& B, const Point& C, const Point& D, const Point& P)
{
    // Tutaj logika opisana przy funkcji wyżej - iloczyn wektorowy>=0 -> punkt znajduje się po prawej czyli w środku
    return crossProduct(A, B, P) >= 0 &&
           crossProduct(B, C, P) >= 0 &&
           crossProduct(C, D, P) >= 0 &&
           crossProduct(D, A, P) >= 0;
}

// Ta funkcja filtruje punkty i odrzuca te które znajdują się w środku. Wynik tej funkcji będzie wejściem algorytmu Jarvisa 
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
        // Punkty skrajne należa do ścieżki
        if(p.id == A.id || p.id == B.id || p.id == C.id || p.id == D.id)
        {
            filteredPoints.push_back(p);
            continue;
        }

        // Jeśli punkt nie jest w środku przechodzi filtr
        if(!isInside(A, B, C, D, p))
        {
            filteredPoints.push_back(p);
        }
    }
    return filteredPoints;
}

// Ta funkcja to helper rozwiązujący EDGE CASE podany poniżej
float calcDist(const Point& p1, const Point& p2)
{
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Ta funkcja to algorytm Jarvisa
std::vector<Point> jarvisMarch(const std::vector<Point>& filteredPoints)
{
    std::vector<Point> outline;
    int startingPoint = 0;
    
    // szukamy punktu najbardziej na lewo
    // bierzemy pierwszy punkt (o indeksie = 0) i zaczynamy porównywać kolejne 
    for(int i=1; i<filteredPoints.size(); ++i)
    {
        if(filteredPoints[i].x < filteredPoints[startingPoint].x)
        {
            startingPoint = i;
        }
        // może zdarzyć się tak że mamy 2 punkty o takiej samej współrzędnej x, wtedy bierzemy ten z mniejszym y
        // nie muszę się tu martwić o przypadek gdzie punkty będą takie same ponieważ wejściem do tego zadania
        // jest wektor punktów scalonych a jeśli odległość 2 punktów byłaby 0.0 to zostalyby one scalone
        if(filteredPoints[i].x == filteredPoints[startingPoint].x && filteredPoints[i].y < filteredPoints[startingPoint].y)
        {
            startingPoint = i;
        }
    }

    int p = startingPoint;
    int q = 0;

    // Start wlaściwego algorytmu 
    do
    {
        // dodaj punkt do otoczki
        outline.push_back(filteredPoints[p]);

        // 'q' to następny kandydat do włączenia do otoczki
        // 'q' musi byc różne od 'p'. Dodałem modulo aby nie wystąpiło wyjście poza zakres
        q = (p + 1) % filteredPoints.size();

        for(int i=0; i<filteredPoints.size(); ++i)
        {
            if (i == p) continue;

            // sprawdzamy czy punkt 'i' jest po prawo czy po lewo od prostej stworzonej przez 'p' i 'q'
            float cross = crossProduct(filteredPoints[p], filteredPoints[q], filteredPoints[i]);
            
            // jeśli tak to znaczy że punkt 'i' jest bardziej na lewo zatem staje się on obecnym kandydatem
            if(cross < 0)
            {
                q = i;
            }
            // EDGE CASE: może się zdarzyć tak że dwa punkty będą w prostej linii (iloczyn wektorowy = 0.0)
            // wtedy wybieramy punkt który jest dalej
            else if(cross == 0.0f)
            {
                if(calcDist(filteredPoints[p], filteredPoints[i]) > calcDist(filteredPoints[p], filteredPoints[q]))
                {
                    q = i;
                }
            }
        }

        p = q;
    
    // Algorytm kończy się gdy wróciliśmy do punktu wyjścia
    } while (p != startingPoint);

    return outline;
}

// Ta funkcja to wrapper do zadania 1.2
std::vector<Point> getShortestOutline(const std::vector<Point>& points)
{
    // znajdz punkty skrajne
    Extremes e = findExtremePoints(points);

    // pozbądź się punktów które napewno nie będą tworzyć otoczki
    std::vector<Point> filteredPoints = filterOutInsidePoints(points, e);

    // stwórz ścieżkę z pozostalych punktów
    std::vector<Point> outline = jarvisMarch(filteredPoints);

    return outline;
}

// Ta funkcja liczy faktyczną długość ścieżki
float calculateOutlineLength(std::vector<Point>& outline)
{
    float totalLength = 0.0f;
    for (int i=0; i<outline.size(); ++i)
    {
        const Point& p1 = outline[i];
        const Point& p2 = outline[(i + 1) % outline.size()]; // modulo aby nie wychodzic poza zakres

        totalLength += calcDist(p1, p2);
    }
    return totalLength;
}

// Te dwie funkcje poniżej usuwają punkty które zostaly scalone i dodają nowo utworzone punkty
// Ten proces nie jest zoptymalizowany ale dla 12 rekordów zostawiam to w takiej formie
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

    for(int i=0; i<points.size(); ++i)
    {
        if(!isGoingToBeMerged(points[i].id, result))
        {
            newPoints.push_back(points[i]);
        }
    }

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

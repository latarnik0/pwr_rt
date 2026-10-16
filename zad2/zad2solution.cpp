#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>

const std::string PATH_TO_FILE = "/home/latarnik3/pwr_rt/zad2/car_position.csv";

struct Position
{
    // Przechowujemy x i y jako string poniewaz nie przeprowadzamy zadnych operacji matematycznych
    std::string x;
    std::string y; 
};

// Wczytanie danych z nowego wygenerowanego w Matlabie pliku do hash mapy gdzie kluczem jest czas - patrz dokument PDF
std::unordered_map<std::string, Position> loadTrajectory(const std::string& trajectory)
{
    std::unordered_map<std::string, Position> data;
    
    // Zarezerwowanie miejsca z gó©y
    // Przy takiej liczbie rekordów przyspieszy ładowanie
    data.reserve(35000); 
    
    std::ifstream file(trajectory);
    std::string line, t, x, y;
    
    if(!file.is_open())
    {
        std::cerr << "Nie mozna otworzyc pliku\n";
        return data;
    }

    // pominiecie naglowka
    getline(file, line); 

    while(getline(file, line))
    {
        std::stringstream ss(line);
        
        if(getline(ss, t, ',') && getline(ss, x, ',') && getline(ss, y, ','))
        {
            data[t] = {x, y};
        }
    }
    return data;
}

int main() 
{
    std::unordered_map<std::string, Position> trajectory = loadTrajectory(PATH_TO_FILE);
    std::string T;
    
    while(true)
    {
        std::cout << "Podaj T: ";
        std::cin>>T;
            
        auto pos = trajectory.find(T);
            
        if(pos != trajectory.end())
        {
            std::cout<<pos->second.x<<" "<<pos->second.y<<"\n";
        } 
        else 
        {
            std::cout<<"T z poza zakresu\n";
        }
    }
    return 0;
}
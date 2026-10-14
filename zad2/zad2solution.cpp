#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

// To zadanie wymaga identyfikacji i korekcji błędu sensor1 tak aby jego pomiary w ogóle były użyteczne
// W pomiarach sensor2 nie występuje żadne przesunięcie, pomiary dobrze oddają kształt toru. Problemem jest ich częstotliwość
// Dla pomiarow sensor2 należy wykonać interpolację

struct Position
{
    double x;
    double y;
};

// Wczytuje dane z plików do mapy gdzie kluczem jest czas
std::map<double, Position> readSensorReadings(std::ifstream& sensor)
{
    std::string line;
    std::map<double, Position> sensorReadings;
    
    if(!sensor.is_open()) return sensorReadings; 
    
    while(std::getline(sensor, line))
    {
        std::string timestampS, xS, yS;
        std::stringstream ss(line);

        if(std::getline(ss, timestampS, ',') && 
           std::getline(ss, xS, ',') && 
           std::getline(ss, yS, ',')) 
        {
            sensorReadings[std::stod(timestampS)] = {std::stod(xS), std::stod(yS)};
        }
    }
    return sensorReadings;
}

int main ()
{
    std::ifstream sensor1("/home/latarnik3/pwr_rt/zad2/sensor1.csv");
    std::ifstream sensor2("/home/latarnik3/pwr_rt/zad2/sensor2.csv");

    std::string T;
    std::cout<<"Podaj T: ";
    std::cin>>T;

    return 0;
}
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>

// Do tego zadania użyje hash mapy aby mieć szybki dostęp do wyszukiwania rekordów
// Kluczem mapy będzie timestamp ponieważ analiza danych z plików pokazuje że jest on unikatowy dla każdego odczytu
// Wartością klucza będzie wektor stringów. Dwa odczyty x,y. Wybieram typ string ponieważ zadanie nie wymaga
// żadnych operacji matematycznych, jedynie wyszukiwanie rekordów

// Użyję dwóch niezależnych map po jednej dla każdego czujnika aby móc stwierdzic z którego czujnika pochodzi dany odczyt

// Funkcja czytająca plik .csv i wrzucająca kazdy odczyt do mapy
std::unordered_map<std::string, std::vector<std::string>> readSensorReadings(std::ifstream& sensor)
{
    std::string line;
    std::unordered_map<std::string, std::vector<std::string>> sensorReadings;
    if(!sensor.is_open())
    {
        std::cerr << "cannot open file\n";
    }
    while(std::getline(sensor, line))
    {
        // Bierzemy najpierw całą linię 
        std::string timestampS, xS, yS;
        std::stringstream ss(line);

        // Dopiero teraz tniemy ją na tokeny używając przecinka jako separatora
        if(std::getline(ss, timestampS, ',') && 
           std::getline(ss, xS, ',') && 
           std::getline(ss, yS, ',')) 
        {
            // Wrzucamy odczyt do mapy
            sensorReadings.insert({timestampS, {xS, yS}});
        }
    }
    sensor.close();
    return sensorReadings;
}

// Funkcja znajdywania rekordu z podanym kluczem T
void findRecord(const std::unordered_map<std::string, std::vector<std::string>>& sensorReadings, 
    const std::string& T, const std::string& sensorName)
{
    auto record = sensorReadings.find(T);
    if(record != sensorReadings.end())
    {
        std::cout<<sensorName<<": "<<record->second[0]<<" "<<record->second[1]<<'\n';
    }    
    else
    {
        std::cout<<sensorName<<": Brak odczytu dla T = " << T << '\n';
    }
}

int main ()
{
    std::ifstream sensor1("/home/latarnik3/pwr_rt/zad2/sensor1.csv");
    std::ifstream sensor2("/home/latarnik3/pwr_rt/zad2/sensor2.csv");

    std::unordered_map<std::string, std::vector<std::string>> sensor1Readings = readSensorReadings(sensor1);
    std::unordered_map<std::string, std::vector<std::string>> sensor2Readings = readSensorReadings(sensor2);

    std::string T;
    std::cout<<"Podaj T: ";
    std::cin>>T;
    
    findRecord(sensor1Readings, T, "Sensor 1");
    findRecord(sensor2Readings, T, "Sensor 2");
    return 0;
}
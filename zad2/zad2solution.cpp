#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <mutex>

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
        std::string timestampS, xS, yS;
        std::stringstream ss(line);

        if(std::getline(ss, timestampS, ',') && 
           std::getline(ss, xS, ',') && 
           std::getline(ss, yS, ',')) 
        {
            sensorReadings.insert({timestampS, {xS, yS}});
        }
    }
    sensor.close();
    return sensorReadings;
}

void findRecord(std::unordered_map<std::string, std::vector<std::string>>& sensorReadings, std::string T)
{
    auto record = sensorReadings.find(T);
    if(record != sensorReadings.end())
    {
        for(int i=0; i<2; ++i)
        {
            std::cout<<record->second[i]<<" ";
        }
        return;
    }    
    else
    {
        std::cout<<"Podane T jest z poza zakresu!\n";
        return;
    }
}

int main ()
{
    std::ifstream sensor1("/home/latarnik3/pwr_rt/zad2/sensor1.csv");
    std::unordered_map<std::string, std::vector<std::string>> sensorReadings = readSensorReadings(sensor1);

    std::string T;
    std::cout<<"Podaj T: ";
    std::cin>>T;
    findRecord(sensorReadings, T);
    return 0;
}
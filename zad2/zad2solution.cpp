#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

struct sensorData 
{
    std::vector<double> t, x, y;
};

// Funkcja wczytująca dane z pliku
sensorData readSensorReadings(const std::string& sensor)
{
    sensorData data;
    std::ifstream file(sensor);
    if(!file.is_open())
    {
        std::cerr << "Błąd otwarcia pliku\n";
        return data;
    }
    
    std::string line, val;
    std::getline(file, line); // Pominięcie nagłówka danych
    
    while(std::getline(file, line))
    {
        std::stringstream ss(line);
        double t, x, y;
        if (std::getline(ss, val, ',')) t = std::stod(val);
        if (std::getline(ss, val, ',')) x = std::stod(val);
        if (std::getline(ss, val, ',')) y = std::stod(val);
        data.t.push_back(t);
        data.x.push_back(x);
        data.y.push_back(y);
    }
    return data;
}

int main() {
    sensorData sensor1 = readSensorReadings("/home/latarnik3/pwr_rt/zad2/sensor1.csv");
    sensorData sensor2 = readSensorReadings("/home/latarnik3/pwr_rt/zad2/sensor2.csv");

    std::vector<double> errorX, errorY, timestampSensor2;
    return 0;
}
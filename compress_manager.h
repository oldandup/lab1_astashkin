#ifndef COMPRESS_MANAGER_H
#define COMPRESS_MANAGER_H

#include "structs.h"
#include "logger.h"
#include <vector>
#include <sstream>

using namespace std;

class CompressManager {
private:
    vector<Compress> stations;
    int& nextId;
    Logger& logger;

public:
    CompressManager(int& id, Logger& log) : nextId(id), logger(log) {}

    void Add(const Compress& station) {
        Compress newStation = station;
        newStation.id = nextId++;
        stations.push_back(newStation);
        
        stringstream ss;
        ss << "ADDED CS - ID: " << newStation.id << ", Name: " << newStation.name 
           << ", Workshops: " << newStation.workshop_count 
           << ", Working: " << newStation.workshop_working
           << ", Class: " << newStation.classification 
           << ", Active: " << (newStation.working ? "Yes" : "No");
        logger.Log(ss.str());
    }

    Compress* FindById(int id) {
        for (auto& station : stations) {
            if (station.id == id) return &station;
        }
        return nullptr;
    }

    bool Delete(int id) {
        for (size_t i = 0; i < stations.size(); i++) {
            if (stations[i].id == id) {
                stringstream ss;
                ss << "DELETED CS - ID: " << stations[i].id << ", Name: " << stations[i].name 
                   << ", Workshops: " << stations[i].workshop_count 
                   << ", Working: " << stations[i].workshop_working;
                logger.Log(ss.str());
                stations.erase(stations.begin() + i);
                return true;
            }
        }
        return false;
    }

    vector<Compress>& GetAll() { return stations; }
    const vector<Compress>& GetAll() const { return stations; }
    void Clear() { stations.clear(); }
};

#endif

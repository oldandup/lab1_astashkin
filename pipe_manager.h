#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include "structs.h"
#include "logger.h"
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

class PipeManager {
private:
    vector<Pipe> pipes;
    int& nextId;
    Logger& logger;

public:
    PipeManager(int& id, Logger& log) : nextId(id), logger(log) {}

    void Add(const Pipe& pipe) {
        Pipe newPipe = pipe;
        newPipe.id = nextId++;
        pipes.push_back(newPipe);
        
        stringstream ss;
        ss << "ADDED PIPE - ID: " << newPipe.id << ", KM Mark: " << newPipe.km_mark 
           << ", Length: " << fixed << setprecision(2) << newPipe.length << " km"
           << ", Diameter: " << newPipe.diametr << " mm"
           << ", On repair: " << (newPipe.repair ? "Yes" : "No");
        logger.Log(ss.str());
    }

    Pipe* FindById(int id) {
        for (auto& pipe : pipes) {
            if (pipe.id == id) return &pipe;
        }
        return nullptr;
    }

    bool Delete(int id) {
        for (size_t i = 0; i < pipes.size(); i++) {
            if (pipes[i].id == id) {
                stringstream ss;
                ss << "DELETED PIPE - ID: " << pipes[i].id << ", KM Mark: " << pipes[i].km_mark 
                   << ", Length: " << fixed << setprecision(2) << pipes[i].length << " km"
                   << ", Diameter: " << pipes[i].diametr << " mm";
                logger.Log(ss.str());
                pipes.erase(pipes.begin() + i);
                return true;
            }
        }
        return false;
    }

    vector<Pipe>& GetAll() { return pipes; }
    const vector<Pipe>& GetAll() const { return pipes; }
    void Clear() { pipes.clear(); }
};

#endif

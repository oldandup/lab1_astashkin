#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include "structs.h"
#include "generic_manager.h"
#include <iomanip>
#include <sstream>

using namespace std;

class PipeManager : public GenericManager<Pipe> {
public:
    PipeManager(int& id, Logger& log) : GenericManager<Pipe>(id, log) {}

private:
    void OnAdd(const Pipe& pipe) override {
        stringstream ss;
        ss << "ADDED PIPE - ID: " << pipe.id << ", KM Mark: " << pipe.km_mark 
           << ", Length: " << fixed << setprecision(2) << pipe.length << " km"
           << ", Diameter: " << pipe.diametr << " mm"
           << ", On repair: " << (pipe.repair ? "Yes" : "No");
        logger.Log(ss.str());
    }

    void OnDelete(const Pipe& pipe) override {
        stringstream ss;
        ss << "DELETED PIPE - ID: " << pipe.id << ", KM Mark: " << pipe.km_mark 
           << ", Length: " << fixed << setprecision(2) << pipe.length << " km"
           << ", Diameter: " << pipe.diametr << " mm";
        logger.Log(ss.str());
    }
};

#endif

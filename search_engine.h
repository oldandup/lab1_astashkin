#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "structs.h"
#include "logger.h"
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

class SearchEngine {
private:
    Logger& logger;

public:
    SearchEngine(Logger& log) : logger(log) {}

    vector<Pipe> SearchPipesById(const vector<Pipe>& pipes, int id) {
        vector<Pipe> results;
        for (const auto& pipe : pipes) {
            if (pipe.id == id) {
                results.push_back(pipe);
                break;
            }
        }
        logger.Log("SEARCH PIPE BY ID - ID: " + to_string(id) + (results.empty() ? " - No results" : " - Found"));
        return results;
    }

    vector<Pipe> SearchPipesByKmMark(const vector<Pipe>& pipes, const string& kmMark) {
        vector<Pipe> results;
        for (const auto& pipe : pipes) {
            if (pipe.km_mark.find(kmMark) != string::npos) {
                results.push_back(pipe);
            }
        }
        stringstream ss;
        ss << "SEARCH PIPE BY KM MARK - Query: '" << kmMark << "' - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Pipe> SearchPipesByDiameter(const vector<Pipe>& pipes, int diameter) {
        vector<Pipe> results;
        for (const auto& pipe : pipes) {
            if (pipe.diametr == diameter) {
                results.push_back(pipe);
            }
        }
        stringstream ss;
        ss << "SEARCH PIPE BY DIAMETER - Diameter: " << diameter << " mm - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Pipe> SearchPipesByRepair(const vector<Pipe>& pipes, bool repair) {
        vector<Pipe> results;
        for (const auto& pipe : pipes) {
            if (pipe.repair == repair) {
                results.push_back(pipe);
            }
        }
        stringstream ss;
        ss << "SEARCH PIPE BY REPAIR STATUS - Status: " << (repair ? "On repair" : "Not on repair")
           << " - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Pipe> SearchPipesByLength(const vector<Pipe>& pipes, double minLength, double maxLength) {
        vector<Pipe> results;
        for (const auto& pipe : pipes) {
            if (pipe.length >= minLength && pipe.length <= maxLength) {
                results.push_back(pipe);
            }
        }
        stringstream ss;
        ss << "SEARCH PIPE BY LENGTH - Range: " << fixed << setprecision(2) 
           << minLength << "-" << maxLength << " km - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Compress> SearchCompressById(const vector<Compress>& stations, int id) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.id == id) {
                results.push_back(station);
                break;
            }
        }
        logger.Log("SEARCH CS BY ID - ID: " + to_string(id) + (results.empty() ? " - No results" : " - Found"));
        return results;
    }

    vector<Compress> SearchCompressByName(const vector<Compress>& stations, const string& name) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.name.find(name) != string::npos) {
                results.push_back(station);
            }
        }
        stringstream ss;
        ss << "SEARCH CS BY NAME - Query: '" << name << "' - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Compress> SearchCompressByClassification(const vector<Compress>& stations, const string& classification) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.classification.find(classification) != string::npos) {
                results.push_back(station);
            }
        }
        stringstream ss;
        ss << "SEARCH CS BY CLASSIFICATION - Query: '" << classification << "' - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Compress> SearchCompressByStatus(const vector<Compress>& stations, bool working) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.working == working) {
                results.push_back(station);
            }
        }
        stringstream ss;
        ss << "SEARCH CS BY STATUS - Status: " << (working ? "Working" : "Not working")
           << " - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Compress> SearchCompressByWorkshopPercentage(const vector<Compress>& stations, double minPercent, double maxPercent) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.workshop_count > 0) {
                double percentage = (double)station.workshop_working / station.workshop_count * 100;
                if (percentage >= minPercent && percentage <= maxPercent) {
                    results.push_back(station);
                }
            }
        }
        stringstream ss;
        ss << "SEARCH CS BY WORKSHOP PERCENTAGE - Range: " << fixed << setprecision(1)
           << minPercent << "%-" << maxPercent << "% - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }

    vector<Compress> SearchCompressByWorkshopCount(const vector<Compress>& stations, int minCount, int maxCount) {
        vector<Compress> results;
        for (const auto& station : stations) {
            if (station.workshop_working >= minCount && station.workshop_working <= maxCount) {
                results.push_back(station);
            }
        }
        stringstream ss;
        ss << "SEARCH CS BY WORKING WORKSHOPS - Range: " << minCount << "-" << maxCount 
           << " - Found: " << results.size();
        logger.Log(ss.str());
        return results;
    }
};

#endif

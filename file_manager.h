#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "pipe_manager.h"
#include "compress_manager.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class FileManager {
private:
    Logger& logger;
    string backupFile;

public:
    FileManager(Logger& log, const string& filename = "data_backup.txt") 
        : logger(log), backupFile(filename) {}

    void SaveAllData(const PipeManager& pipeManager, const CompressManager& compressManager, const string& customFilename = "") {
        string filename = customFilename.empty() ? backupFile : customFilename;
        
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file for saving data.\n";
            logger.Log("ERROR: Failed to save all data - file open error");
            return;
        }

        file << "===== DATA BACKUP =====\n";
        file << "Backup time: " << logger.GetCurrentDateTime() << "\n";
        file << "======================================\n\n";

        SavePipes(file, pipeManager.GetAll());
        SaveCompress(file, compressManager.GetAll());

        file.close();
        
        cout << "All data saved successfully to " << filename << "\n";
        stringstream ss;
        ss << "SAVED ALL DATA - Pipes: " << pipeManager.GetAll().size() << ", CS: " 
           << compressManager.GetAll().size() << " exported to " << filename;
        logger.Log(ss.str());
    }

    void LoadAllData(PipeManager& pipeManager, CompressManager& compressManager, 
                     int& nextPipeId, int& nextCompressId, const string& customFilename = "") {
        string filename = customFilename.empty() ? backupFile : customFilename;
        
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open " << filename << ". File not found.\n";
            logger.Log("ERROR: Failed to load data - file not found: " + filename);
            return;
        }

        pipeManager.Clear();
        compressManager.Clear();

        string line;
        int loadedPipes = 0;
        int loadedStations = 0;
        int maxPipeId = 0;
        int maxStationId = 0;

        bool inPipeSection = false;
        bool inStationSection = false;

        Pipe currentPipe = {};
        Compress currentStation = {};
        bool inPipe = false;
        bool inStation = false;

        while (getline(file, line)) {
            if (line.find("===== PIPES DATA =====") != string::npos) {
                inPipeSection = true;
                inStationSection = false;
                continue;
            }
            if (line.find("===== COMPRESSOR STATIONS DATA =====") != string::npos) {
                if (inPipe && currentPipe.id > 0) {
                    pipeManager.Add(currentPipe);
                    maxPipeId = max(maxPipeId, currentPipe.id);
                    loadedPipes++;
                    inPipe = false;
                }
                inPipeSection = false;
                inStationSection = true;
                continue;
            }

            if (line.empty() || line.find("=====") != string::npos || 
                line.find("Total") != string::npos || line.find("------") != string::npos) {
                continue;
            }

            try {
                if (inPipeSection && line.find("~~~") != string::npos) {
                    if (inPipe && currentPipe.id > 0) {
                        pipeManager.Add(currentPipe);
                        maxPipeId = max(maxPipeId, currentPipe.id);
                        loadedPipes++;
                        inPipe = false;
                    }
                    continue;
                }

                if (inStationSection && line.find("~~~") != string::npos) {
                    if (inStation && currentStation.id > 0) {
                        compressManager.Add(currentStation);
                        maxStationId = max(maxStationId, currentStation.id);
                        loadedStations++;
                        inStation = false;
                    }
                    continue;
                }

                if (inPipeSection) {
                    ParsePipeLine(line, currentPipe, inPipe);
                } else if (inStationSection) {
                    ParseCompressLine(line, currentStation, inStation);
                }
            } catch (...) {
                logger.Log("WARNING: Skipped invalid line while loading data");
            }
        }

        // Add last items if exist
        if (inPipe && currentPipe.id > 0) {
            pipeManager.Add(currentPipe);
            maxPipeId = max(maxPipeId, currentPipe.id);
            loadedPipes++;
        }
        if (inStation && currentStation.id > 0) {
            compressManager.Add(currentStation);
            maxStationId = max(maxStationId, currentStation.id);
            loadedStations++;
        }

        file.close();
        
        nextPipeId = maxPipeId + 1;
        nextCompressId = maxStationId + 1;
        
        cout << "All data loaded successfully!\n";
        cout << "Pipes loaded: " << loadedPipes << "\n";
        cout << "CS loaded: " << loadedStations << "\n";
        
        stringstream ss;
        ss << "LOADED ALL DATA - Pipes: " << loadedPipes << ", CS: " << loadedStations 
           << " imported from " << filename;
        logger.Log(ss.str());
    }

private:
    void SavePipes(ofstream& file, const vector<Pipe>& pipes) {
        file << "===== PIPES DATA =====\n";
        file << "Total pipes: " << pipes.size() << "\n";
        file << "--------------------------------------\n\n";
        
        for (const auto& pipe : pipes) {
            file << "ID: " << pipe.id << "\n";
            file << "KM Mark: " << pipe.km_mark << "\n";
            file << "Length (km): " << fixed << setprecision(2) << pipe.length << "\n";
            file << "Diameter (mm): " << pipe.diametr << "\n";
            file << "On repair: " << (pipe.repair ? "Yes" : "No") << "\n";
            file << "~~~\n\n";
        }
    }

    void SaveCompress(ofstream& file, const vector<Compress>& stations) {
        file << "\n===== COMPRESSOR STATIONS DATA =====\n";
        file << "Total stations: " << stations.size() << "\n";
        file << "--------------------------------------\n\n";
        
        for (const auto& station : stations) {
            file << "ID: " << station.id << "\n";
            file << "Name: " << station.name << "\n";
            file << "Workshops: " << station.workshop_count << "\n";
            file << "Working: " << station.workshop_working << "\n";
            file << "Classification: " << station.classification << "\n";
            file << "Active: " << (station.working ? "Yes" : "No") << "\n";
            file << "~~~\n\n";
        }
    }

    void ParsePipeLine(const string& line, Pipe& pipe, bool& inPipe) {
        if (line.find("ID: ") == 0) {
            pipe.id = stoi(line.substr(4));
            inPipe = true;
        }
        else if (line.find("KM Mark: ") == 0 && inPipe) {
            pipe.km_mark = line.substr(9);
        }
        else if (line.find("Length (km): ") == 0 && inPipe) {
            pipe.length = stod(line.substr(13));
        }
        else if (line.find("Diameter (mm): ") == 0 && inPipe) {
            pipe.diametr = stoi(line.substr(15));
        }
        else if (line.find("On repair: ") == 0 && inPipe) {
            pipe.repair = (line.substr(11) == "Yes");
        }
    }

    void ParseCompressLine(const string& line, Compress& station, bool& inStation) {
        if (line.find("ID: ") == 0) {
            station.id = stoi(line.substr(4));
            inStation = true;
        }
        else if (line.find("Name: ") == 0 && inStation) {
            station.name = line.substr(6);
        }
        else if (line.find("Workshops: ") == 0 && inStation) {
            station.workshop_count = stoi(line.substr(11));
        }
        else if (line.find("Working: ") == 0 && inStation) {
            station.workshop_working = stoi(line.substr(9));
        }
        else if (line.find("Classification: ") == 0 && inStation) {
            station.classification = line.substr(16);
        }
        else if (line.find("Active: ") == 0 && inStation) {
            station.working = (line.substr(8) == "Yes");
        }
    }
};

#endif
#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <ctime>
#include <functional>

using namespace std;

struct Pipe {
    int id;
    string km_mark;           // километровая отметка (название)
    double length;            // длина в км
    int diametr;              // диаметр в мм
    bool repair;              // признак "в ремонте"
};

struct Compress {
    int id;
    string name;              // название
    int workshop_count;       // количество цехов
    int workshop_working;     // количество цехов в работе
    string classification;    // класс станции
    bool working;             // статус работы
};

// Logger class for centralized logging
class Logger {
private:
    string logFile;

public:
    Logger(const string& filename = "operations_log.txt") : logFile(filename) {}

    string GetCurrentDateTime() const {
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

    void Log(const string& action) {
        ofstream file(logFile, ios::app);
        if (file.is_open()) {
            file << "[" << GetCurrentDateTime() << "] " << action << "\n";
            file.close();
        }
    }

    void ViewLogs() const {
        ifstream file(logFile);
        if (!file.is_open()) {
            cout << "\nNo log file found yet.\n";
            return;
        }

        cout << "\n===== Operation Logs =====\n";
        string line;
        int lineCount = 0;
        while (getline(file, line)) {
            cout << line << "\n";
            lineCount++;
        }
        file.close();
        
        if (lineCount == 0) {
            cout << "No operations logged yet.\n";
        }
    }
};

// Manager classes for Pipes and Compress Stations
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

// FileManager for data persistence
class FileManager {
private:
    Logger& logger;
    string backupFile;

public:
    FileManager(Logger& log, const string& filename = "data_backup.txt") 
        : logger(log), backupFile(filename) {}

    void SaveAllData(const PipeManager& pipeManager, const CompressManager& compressManager) {
        ofstream file(backupFile);
        if (!file.is_open()) {
            cout << "Error: Could not open file for saving data.\n";
            logger.Log("ERROR: Failed to save all data - file open error");
            return;
        }

        file << "===== DATA BACKUP =====\n";
        file << "Backup time: " << logger.GetCurrentDateTime() << "\n";
        file << "======================================\n\n";

        // Save Pipes
        SavePipes(file, pipeManager.GetAll());

        // Save Compress Stations
        SaveCompress(file, compressManager.GetAll());

        file.close();
        
        cout << "All data saved successfully to " << backupFile << "\n";
        stringstream ss;
        ss << "SAVED ALL DATA - Pipes: " << pipeManager.GetAll().size() << ", CS: " 
           << compressManager.GetAll().size() << " exported to " << backupFile;
        logger.Log(ss.str());
    }

    void LoadAllData(PipeManager& pipeManager, CompressManager& compressManager, 
                     int& nextPipeId, int& nextCompressId) {
        ifstream file(backupFile);
        if (!file.is_open()) {
            cout << "Error: Could not open " << backupFile << ". File not found.\n";
            logger.Log("ERROR: Failed to load data - file not found");
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
           << " imported from " << backupFile;
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

// Search class for finding items by various criteria
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
};

// UI Controller class
class UIController {
private:
    PipeManager& pipeManager;
    CompressManager& compressManager;
    Logger& logger;
    FileManager& fileManager;
    SearchEngine searchEngine;

public:
    UIController(PipeManager& pm, CompressManager& cm, Logger& log, FileManager& fm)
        : pipeManager(pm), compressManager(cm), logger(log), fileManager(fm), searchEngine(log) {}

    void AddPipe() {
        Pipe pipe = {};
        cout << "\nPipe parameters:\n";
        cout << "Enter KM mark (name): ";
        cin.ignore();
        getline(cin, pipe.km_mark);

        cout << "Enter length (km): ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add pipe - invalid length");
            return;
        }

        cout << "Enter diameter (mm): ";
        cin >> pipe.diametr;
        if (cin.fail() || pipe.diametr <= 0) {
            cout << "Error: Invalid diameter.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add pipe - invalid diameter");
            return;
        }

        cout << "On repair? (0 - no, 1 - yes): ";
        cin >> pipe.repair;
        if (cin.fail()) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add pipe - invalid repair status");
            return;
        }

        pipeManager.Add(pipe);
        cout << "Pipe added successfully!\n";
    }

    void AddCompress() {
        Compress station = {};
        cout << "\nCS parameters:\n";
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, station.name);

        cout << "Enter workshop quantity: ";
        cin >> station.workshop_count;
        if (cin.fail() || station.workshop_count < 0) {
            cout << "Error: Invalid quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add CS - invalid workshop quantity");
            return;
        }

        cout << "Enter workshops in work: ";
        cin >> station.workshop_working;
        if (cin.fail() || station.workshop_working < 0 || station.workshop_working > station.workshop_count) {
            cout << "Error: Invalid working quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add CS - invalid working quantity");
            return;
        }

        cout << "Enter classification: ";
        cin.ignore();
        getline(cin, station.classification);

        cout << "Is CS working? (0 - no, 1 - yes): ";
        cin >> station.working;
        if (cin.fail()) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add CS - invalid working status");
            return;
        }

        compressManager.Add(station);
        cout << "CS added successfully!\n";
    }

    void ViewAllPipes() {
        const auto& pipes = pipeManager.GetAll();
        if (pipes.empty()) {
            cout << "\nNo pipes available.\n";
            return;
        }
        cout << "\n===== All Pipes =====\n";
        for (const auto& pipe : pipes) {
            cout << "ID: " << pipe.id << " | KM: " << pipe.km_mark
                 << " | Length: " << fixed << setprecision(2) << pipe.length << " km"
                 << " | Diameter: " << pipe.diametr << " mm"
                 << " | On repair: " << (pipe.repair ? "Yes" : "No") << "\n";
        }
        logger.Log("VIEWED ALL PIPES - Total: " + to_string(pipes.size()));
    }

    void ViewAllCompress() {
        const auto& stations = compressManager.GetAll();
        if (stations.empty()) {
            cout << "\nNo CS available.\n";
            return;
        }
        cout << "\n===== All CS =====\n";
        for (const auto& station : stations) {
            cout << "ID: " << station.id << " | Name: " << station.name
                 << " | Workshops: " << station.workshop_count
                 << " | Working: " << station.workshop_working
                 << " | Class: " << station.classification
                 << " | Active: " << (station.working ? "Yes" : "No") << "\n";
        }
        logger.Log("VIEWED ALL CS - Total: " + to_string(stations.size()));
    }

    void EditPipe() {
        ViewAllPipes();
        int id;
        cout << "\nEnter pipe ID to edit: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to edit pipe - invalid ID");
            return;
        }

        Pipe* pipe = pipeManager.FindById(id);
        if (!pipe) {
            cout << "Error: Pipe not found.\n";
            logger.Log("ERROR: Pipe not found - ID: " + to_string(id));
            return;
        }

        logger.Log("EDIT PIPE STARTED - ID: " + to_string(id) + ", Old Name: " + pipe->km_mark);
        EditPipeFields(*pipe);
        logger.Log("EDIT PIPE COMPLETED - ID: " + to_string(id));
    }

    void EditCompress() {
        ViewAllCompress();
        int id;
        cout << "\nEnter CS ID to edit: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to edit CS - invalid ID");
            return;
        }

        Compress* station = compressManager.FindById(id);
        if (!station) {
            cout << "Error: CS not found.\n";
            logger.Log("ERROR: CS not found - ID: " + to_string(id));
            return;
        }

        logger.Log("EDIT CS STARTED - ID: " + to_string(id) + ", Old Name: " + station->name);
        EditCompressFields(*station);
        logger.Log("EDIT CS COMPLETED - ID: " + to_string(id));
    }

    void DeletePipe() {
        ViewAllPipes();
        int id;
        cout << "\nEnter pipe ID to delete: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to delete pipe - invalid ID");
            return;
        }

        if (pipeManager.Delete(id)) {
            cout << "Pipe deleted successfully!\n";
        } else {
            cout << "Error: Pipe not found.\n";
            logger.Log("ERROR: Pipe not found for deletion - ID: " + to_string(id));
        }
    }

    void DeleteCompress() {
        ViewAllCompress();
        int id;
        cout << "\nEnter CS ID to delete: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to delete CS - invalid ID");
            return;
        }

        if (compressManager.Delete(id)) {
            cout << "CS deleted successfully!\n";
        } else {
            cout << "Error: CS not found.\n";
            logger.Log("ERROR: CS not found for deletion - ID: " + to_string(id));
        }
    }

    void SaveData() {
        fileManager.SaveAllData(pipeManager, compressManager);
    }

    void LoadData(int& nextPipeId, int& nextCompressId) {
        fileManager.LoadAllData(pipeManager, compressManager, nextPipeId, nextCompressId);
    }

    void ViewLogs() {
        logger.ViewLogs();
    }

    void SearchPipes() {
        const auto& pipes = pipeManager.GetAll();
        if (pipes.empty()) {
            cout << "\nNo pipes available.\n";
            return;
        }

        int choice;
        while (true) {
            cout << "\n===== Pipe Search Menu =====\n";
            cout << "1. Search by ID\n";
            cout << "2. Search by KM Mark\n";
            cout << "3. Search by Diameter\n";
            cout << "4. Search by Repair Status\n";
            cout << "5. Back to Main Menu\n";
            cout << "Choose search criteria: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Invalid input.\n";
                continue;
            }

            switch (choice) {
            case 1:
                SearchPipesById();
                break;
            case 2:
                SearchPipesByKmMark();
                break;
            case 3:
                SearchPipesByDiameter();
                break;
            case 4:
                SearchPipesByRepair();
                break;
            case 5:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void SearchCompress() {
        const auto& stations = compressManager.GetAll();
        if (stations.empty()) {
            cout << "\nNo CS available.\n";
            return;
        }

        int choice;
        while (true) {
            cout << "\n===== CS Search Menu =====\n";
            cout << "1. Search by ID\n";
            cout << "2. Search by Name\n";
            cout << "3. Search by Classification\n";
            cout << "4. Search by Working Status\n";
            cout << "5. Back to Main Menu\n";
            cout << "Choose search criteria: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Invalid input.\n";
                continue;
            }

            switch (choice) {
            case 1:
                SearchCompressById();
                break;
            case 2:
                SearchCompressByName();
                break;
            case 3:
                SearchCompressByClassification();
                break;
            case 4:
                SearchCompressByStatus();
                break;
            case 5:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }

private:
    void EditPipeFields(Pipe& pipe) {
        cout << "\nEditing pipe: " << pipe.km_mark << "\n";
        cout << "Enter new KM mark: ";
        cin.ignore();
        getline(cin, pipe.km_mark);

        cout << "Enter new length (km): ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter new diameter (mm): ";
        cin >> pipe.diametr;
        if (cin.fail() || pipe.diametr <= 0) {
            cout << "Error: Invalid diameter.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "On repair? (0 - no, 1 - yes): ";
        cin >> pipe.repair;
        if (cin.fail()) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Pipe updated successfully!\n";
    }

    void EditCompressFields(Compress& station) {
        cout << "\nEditing CS: " << station.name << "\n";
        cout << "Enter new name: ";
        cin.ignore();
        getline(cin, station.name);

        cout << "Enter new workshop quantity: ";
        cin >> station.workshop_count;
        if (cin.fail() || station.workshop_count < 0) {
            cout << "Error: Invalid quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter workshops in work: ";
        cin >> station.workshop_working;
        if (cin.fail() || station.workshop_working < 0 || station.workshop_working > station.workshop_count) {
            cout << "Error: Invalid working quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter new classification: ";
        cin.ignore();
        getline(cin, station.classification);

        cout << "Is CS working? (0 - no, 1 - yes): ";
        cin >> station.working;
        if (cin.fail()) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "CS updated successfully!\n";
    }

    void DisplayPipes(const vector<Pipe>& pipes) {
        cout << "\n===== Search Results =====\n";
        for (size_t i = 0; i < pipes.size(); i++) {
            cout << "[" << i << "] ID: " << pipes[i].id << " | KM: " << pipes[i].km_mark
                 << " | Length: " << fixed << setprecision(2) << pipes[i].length << " km"
                 << " | Diameter: " << pipes[i].diametr << " mm"
                 << " | On repair: " << (pipes[i].repair ? "Yes" : "No") << "\n";
        }
    }

    void DisplayCompress(const vector<Compress>& stations) {
        cout << "\n===== Search Results =====\n";
        for (size_t i = 0; i < stations.size(); i++) {
            cout << "[" << i << "] ID: " << stations[i].id << " | Name: " << stations[i].name
                 << " | Workshops: " << stations[i].workshop_count
                 << " | Working: " << stations[i].workshop_working
                 << " | Class: " << stations[i].classification
                 << " | Active: " << (stations[i].working ? "Yes" : "No") << "\n";
        }
    }

    void SearchPipesById() {
        int id;
        cout << "\nEnter pipe ID to search: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid input.\n";
            return;
        }

        auto results = searchEngine.SearchPipesById(pipeManager.GetAll(), id);
        if (results.empty()) {
            cout << "No pipes found with ID: " << id << "\n";
            return;
        }

        DisplayPipes(results);
    }

    void SearchPipesByKmMark() {
        string kmMark;
        cout << "\nEnter KM mark to search: ";
        cin.ignore();
        getline(cin, kmMark);

        auto results = searchEngine.SearchPipesByKmMark(pipeManager.GetAll(), kmMark);
        if (results.empty()) {
            cout << "No pipes found with KM mark containing: " << kmMark << "\n";
            return;
        }

        DisplayPipes(results);
    }

    void SearchPipesByDiameter() {
        int diameter;
        cout << "\nEnter diameter (mm) to search: ";
        cin >> diameter;
        if (cin.fail() || diameter <= 0) {
            cout << "Error: Invalid diameter.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchPipesByDiameter(pipeManager.GetAll(), diameter);
        if (results.empty()) {
            cout << "No pipes found with diameter: " << diameter << " mm\n";
            return;
        }

        DisplayPipes(results);
    }

    void SearchPipesByRepair() {
        int repair;
        cout << "\nSearch for pipes on repair? (0 - no, 1 - yes): ";
        cin >> repair;
        if (cin.fail() || (repair != 0 && repair != 1)) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchPipesByRepair(pipeManager.GetAll(), repair != 0);
        if (results.empty()) {
            cout << "No pipes found with repair status: " << (repair ? "Yes" : "No") << "\n";
            return;
        }

        DisplayPipes(results);
    }

    void SearchCompressById() {
        int id;
        cout << "\nEnter CS ID to search: ";
        cin >> id;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid input.\n";
            return;
        }

        auto results = searchEngine.SearchCompressById(compressManager.GetAll(), id);
        if (results.empty()) {
            cout << "No CS found with ID: " << id << "\n";
            return;
        }

        DisplayCompress(results);
    }

    void SearchCompressByName() {
        string name;
        cout << "\nEnter CS name to search: ";
        cin.ignore();
        getline(cin, name);

        auto results = searchEngine.SearchCompressByName(compressManager.GetAll(), name);
        if (results.empty()) {
            cout << "No CS found with name containing: " << name << "\n";
            return;
        }

        DisplayCompress(results);
    }

    void SearchCompressByClassification() {
        string classification;
        cout << "\nEnter classification to search: ";
        cin.ignore();
        getline(cin, classification);

        auto results = searchEngine.SearchCompressByClassification(compressManager.GetAll(), classification);
        if (results.empty()) {
            cout << "No CS found with classification containing: " << classification << "\n";
            return;
        }

        DisplayCompress(results);
    }

    void SearchCompressByStatus() {
        int working;
        cout << "\nSearch for CS working? (0 - no, 1 - yes): ";
        cin >> working;
        if (cin.fail() || (working != 0 && working != 1)) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchCompressByStatus(compressManager.GetAll(), working != 0);
        if (results.empty()) {
            cout << "No CS found with working status: " << (working ? "Yes" : "No") << "\n";
            return;
        }

        DisplayCompress(results);
    }
};

// Main application
class Application {
private:
    Logger logger;
    int nextPipeId = 1;
    int nextCompressId = 1;
    PipeManager pipeManager;
    CompressManager compressManager;
    FileManager fileManager;
    UIController ui;

public:
    Application() 
        : pipeManager(nextPipeId, logger),
          compressManager(nextCompressId, logger),
          fileManager(logger),
          ui(pipeManager, compressManager, logger, fileManager) {
        logger.Log("APPLICATION STARTED");
    }

    ~Application() {
        logger.Log("APPLICATION CLOSED");
    }

    void Run() {
        int choice;
        while (true) {
            cout << "\n===== Main Menu =====\n";
            cout << "1. Add pipe\n";
            cout << "2. Edit pipe\n";
            cout << "3. Delete pipe\n";
            cout << "4. View all pipes\n";
            cout << "5. Search pipes\n";
            cout << "6. Add CS\n";
            cout << "7. Edit CS\n";
            cout << "8. Delete CS\n";
            cout << "9. View all CS\n";
            cout << "10. Search CS\n";
            cout << "11. Save all data to file\n";
            cout << "12. Load all data from file\n";
            cout << "13. View Operation Logs\n";
            cout << "14. Exit\n";
            cout << "Choose an option: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Invalid input.\n";
                continue;
            }

            switch (choice) {
            case 1: ui.AddPipe(); break;
            case 2: ui.EditPipe(); break;
            case 3: ui.DeletePipe(); break;
            case 4: ui.ViewAllPipes(); break;
            case 5: ui.SearchPipes(); break;
            case 6: ui.AddCompress(); break;
            case 7: ui.EditCompress(); break;
            case 8: ui.DeleteCompress(); break;
            case 9: ui.ViewAllCompress(); break;
            case 10: ui.SearchCompress(); break;
            case 11: ui.SaveData(); break;
            case 12: ui.LoadData(nextPipeId, nextCompressId); break;
            case 13: ui.ViewLogs(); break;
            case 14: return;
            default: cout << "Invalid option.\n";
            }
        }
    }
};

int main() {
    Application app;
    app.Run();
    return 0;
}

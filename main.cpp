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
    string name;
    double length;
    double diametr;
    bool repair;
};

struct Compress {
    int id;
    string name;
    int count;
    int count_working;
    string classification;
    bool working;
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
        ss << "ADDED PIPE - ID: " << newPipe.id << ", Name: " << newPipe.name 
           << ", Length: " << fixed << setprecision(2) << newPipe.length 
           << ", Diameter: " << newPipe.diametr 
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
                ss << "DELETED PIPE - ID: " << pipes[i].id << ", Name: " << pipes[i].name 
                   << ", Length: " << fixed << setprecision(2) << pipes[i].length 
                   << ", Diameter: " << pipes[i].diametr;
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
           << ", Qty: " << newStation.count << ", Working: " << newStation.count_working 
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
                   << ", Qty: " << stations[i].count << ", Working: " << stations[i].count_working;
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
            file << "Name: " << pipe.name << "\n";
            file << "Length: " << fixed << setprecision(2) << pipe.length << "\n";
            file << "Diameter: " << pipe.diametr << "\n";
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
            file << "Quantity: " << station.count << "\n";
            file << "Working: " << station.count_working << "\n";
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
        else if (line.find("Name: ") == 0 && inPipe) {
            pipe.name = line.substr(6);
        }
        else if (line.find("Length: ") == 0 && inPipe) {
            pipe.length = stod(line.substr(8));
        }
        else if (line.find("Diameter: ") == 0 && inPipe) {
            pipe.diametr = stod(line.substr(10));
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
        else if (line.find("Quantity: ") == 0 && inStation) {
            station.count = stoi(line.substr(10));
        }
        else if (line.find("Working: ") == 0 && inStation) {
            station.count_working = stoi(line.substr(9));
        }
        else if (line.find("Classification: ") == 0 && inStation) {
            station.classification = line.substr(16);
        }
        else if (line.find("Active: ") == 0 && inStation) {
            station.working = (line.substr(8) == "Yes");
        }
    }
};

// UI Controller class
class UIController {
private:
    PipeManager& pipeManager;
    CompressManager& compressManager;
    Logger& logger;
    FileManager& fileManager;

public:
    UIController(PipeManager& pm, CompressManager& cm, Logger& log, FileManager& fm)
        : pipeManager(pm), compressManager(cm), logger(log), fileManager(fm) {}

    void AddPipe() {
        Pipe pipe = {};
        cout << "\nPipe parameters:\n";
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, pipe.name);

        cout << "Enter length: ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add pipe - invalid length");
            return;
        }

        cout << "Enter diameter: ";
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

        cout << "Enter quantity: ";
        cin >> station.count;
        if (cin.fail() || station.count < 0) {
            cout << "Error: Invalid quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            logger.Log("ERROR: Failed to add CS - invalid quantity");
            return;
        }

        cout << "Enter quantity on working: ";
        cin >> station.count_working;
        if (cin.fail() || station.count_working < 0 || station.count_working > station.count) {
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
            cout << "ID: " << pipe.id << " | Name: " << pipe.name
                 << " | Length: " << fixed << setprecision(2) << pipe.length
                 << " | Diameter: " << pipe.diametr
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
                 << " | Qty: " << station.count
                 << " | Working: " << station.count_working
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

        logger.Log("EDIT PIPE STARTED - ID: " + to_string(id) + ", Old Name: " + pipe->name);
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

private:
    void EditPipeFields(Pipe& pipe) {
        cout << "\nEditing pipe: " << pipe.name << "\n";
        cout << "Enter new name: ";
        cin.ignore();
        getline(cin, pipe.name);

        cout << "Enter new length: ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter new diameter: ";
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

        cout << "Enter new quantity: ";
        cin >> station.count;
        if (cin.fail() || station.count < 0) {
            cout << "Error: Invalid quantity.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter quantity on working: ";
        cin >> station.count_working;
        if (cin.fail() || station.count_working < 0 || station.count_working > station.count) {
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
            cout << "5. Add CS\n";
            cout << "6. Edit CS\n";
            cout << "7. Delete CS\n";
            cout << "8. View all CS\n";
            cout << "9. Save all data to file\n";
            cout << "10. Load all data from file\n";
            cout << "11. View Operation Logs\n";
            cout << "12. Exit\n";
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
            case 5: ui.AddCompress(); break;
            case 6: ui.EditCompress(); break;
            case 7: ui.DeleteCompress(); break;
            case 8: ui.ViewAllCompress(); break;
            case 9: ui.SaveData(); break;
            case 10: ui.LoadData(nextPipeId, nextCompressId); break;
            case 11: ui.ViewLogs(); break;
            case 12: return;
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

#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <ctime>

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

int nextPipeId = 1;
int nextCompressId = 1;

// Forward declarations
void EditPipeField(Pipe& pipe);
void BatchEditPipes(vector<Pipe>& pipes, const vector<Pipe>& searchResults);
void SearchPipe(vector<Pipe>& pipes);
void SaveAllData(const vector<Pipe>& pipes, const vector<Compress>& stations);
void LoadAllData(vector<Pipe>& pipes, vector<Compress>& stations);

string GetCurrentDateTime() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

void LogAction(const string& action) {
    ofstream logFile("operations_log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[" << GetCurrentDateTime() << "] " << action << "\n";
        logFile.close();
    }
}

void ViewLogs() {
    ifstream logFile("operations_log.txt");
    if (!logFile.is_open()) {
        cout << "\nNo log file found yet.\n";
        return;
    }

    cout << "\n===== Operation Logs =====\n";
    string line;
    int lineCount = 0;
    while (getline(logFile, line)) {
        cout << line << "\n";
        lineCount++;
    }
    logFile.close();
    
    if (lineCount == 0) {
        cout << "No operations logged yet.\n";
    }
}

void SaveAllData(const vector<Pipe>& pipes, const vector<Compress>& stations) {
    ofstream file("data_backup.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open file for saving data.\n";
        LogAction("ERROR: Failed to save all data - file open error");
        return;
    }

    file << "===== DATA BACKUP =====\n";
    file << "Backup time: " << GetCurrentDateTime() << "\n";
    file << "======================================\n\n";

    // Save Pipes
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

    file << "\n===== COMPRESSOR STATIONS DATA =====\n";
    file << "Total stations: " << stations.size() << "\n";
    file << "--------------------------------------\n\n";
    
    // Save Compress Stations
    for (const auto& station : stations) {
        file << "ID: " << station.id << "\n";
        file << "Name: " << station.name << "\n";
        file << "Quantity: " << station.count << "\n";
        file << "Working: " << station.count_working << "\n";
        file << "Classification: " << station.classification << "\n";
        file << "Active: " << (station.working ? "Yes" : "No") << "\n";
        file << "~~~\n\n";
    }

    file.close();
    
    cout << "All data saved successfully to data_backup.txt\n";
    stringstream ss;
    ss << "SAVED ALL DATA - Pipes: " << pipes.size() << ", CS: " << stations.size() 
       << " exported to data_backup.txt";
    LogAction(ss.str());
}

void LoadAllData(vector<Pipe>& pipes, vector<Compress>& stations) {
    ifstream file("data_backup.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open data_backup.txt. File not found.\n";
        LogAction("ERROR: Failed to load data - file not found");
        return;
    }

    pipes.clear();
    stations.clear();
    
    string line;
    int loadedPipes = 0;
    int loadedStations = 0;
    int maxPipeId = 0;
    int maxStationId = 0;
    
    Pipe currentPipe;
    Compress currentStation;
    bool inPipeSection = false;
    bool inStationSection = false;
    bool inPipe = false;
    bool inStation = false;

    while (getline(file, line)) {
        // Check section headers
        if (line.find("===== PIPES DATA =====") != string::npos) {
            inPipeSection = true;
            inStationSection = false;
            continue;
        }
        if (line.find("===== COMPRESSOR STATIONS DATA =====") != string::npos) {
            if (inPipe && currentPipe.id > 0) {
                pipes.push_back(currentPipe);
                maxPipeId = max(maxPipeId, currentPipe.id);
                loadedPipes++;
                inPipe = false;
            }
            inPipeSection = false;
            inStationSection = true;
            continue;
        }

        if (line.empty() || line.find("Data backup") != string::npos || 
            line.find("Total") != string::npos || line.find("------") != string::npos ||
            line.find("Backup time") != string::npos || line.find("======") != string::npos) {
            continue;
        }

        try {
            if (inPipeSection && line.find("~~~") != string::npos) {
                if (inPipe && currentPipe.id > 0) {
                    pipes.push_back(currentPipe);
                    maxPipeId = max(maxPipeId, currentPipe.id);
                    loadedPipes++;
                    inPipe = false;
                }
                continue;
            }

            if (inStationSection && line.find("~~~") != string::npos) {
                if (inStation && currentStation.id > 0) {
                    stations.push_back(currentStation);
                    maxStationId = max(maxStationId, currentStation.id);
                    loadedStations++;
                    inStation = false;
                }
                continue;
            }

            if (inPipeSection) {
                if (line.find("ID: ") == 0) {
                    if (inPipe && currentPipe.id > 0) {
                        pipes.push_back(currentPipe);
                        maxPipeId = max(maxPipeId, currentPipe.id);
                        loadedPipes++;
                    }
                    currentPipe.id = stoi(line.substr(4));
                    inPipe = true;
                }
                else if (line.find("Name: ") == 0 && inPipe) {
                    currentPipe.name = line.substr(6);
                }
                else if (line.find("Length: ") == 0 && inPipe) {
                    currentPipe.length = stod(line.substr(8));
                }
                else if (line.find("Diameter: ") == 0 && inPipe) {
                    currentPipe.diametr = stod(line.substr(10));
                }
                else if (line.find("On repair: ") == 0 && inPipe) {
                    string status = line.substr(11);
                    currentPipe.repair = (status == "Yes");
                }
            }
            else if (inStationSection) {
                if (line.find("ID: ") == 0) {
                    if (inStation && currentStation.id > 0) {
                        stations.push_back(currentStation);
                        maxStationId = max(maxStationId, currentStation.id);
                        loadedStations++;
                    }
                    currentStation.id = stoi(line.substr(4));
                    inStation = true;
                }
                else if (line.find("Name: ") == 0 && inStation) {
                    currentStation.name = line.substr(6);
                }
                else if (line.find("Quantity: ") == 0 && inStation) {
                    currentStation.count = stoi(line.substr(10));
                }
                else if (line.find("Working: ") == 0 && inStation) {
                    currentStation.count_working = stoi(line.substr(9));
                }
                else if (line.find("Classification: ") == 0 && inStation) {
                    currentStation.classification = line.substr(16);
                }
                else if (line.find("Active: ") == 0 && inStation) {
                    string status = line.substr(8);
                    currentStation.working = (status == "Yes");
                }
            }
        } catch (...) {
            LogAction("WARNING: Skipped invalid line while loading data");
        }
    }

    // Add last items if exist
    if (inPipe && currentPipe.id > 0) {
        pipes.push_back(currentPipe);
        maxPipeId = max(maxPipeId, currentPipe.id);
        loadedPipes++;
    }
    if (inStation && currentStation.id > 0) {
        stations.push_back(currentStation);
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
       << " imported from data_backup.txt";
    LogAction(ss.str());
}

void AddPipe(vector<Pipe>& pipes) {
    Pipe newPipe;
    newPipe.id = nextPipeId++;
    cout << "\nPipe parameters:\n";
    cout << "Enter name: ";
    cin.ignore();
    getline(cin, newPipe.name);

    cout << "Enter length: ";
    cin >> newPipe.length;
    if (cin.fail() || newPipe.length <= 0) {
        cout << "Error: Invalid length. Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add pipe - invalid length");
        return;
    }

    cout << "Enter diameter: ";
    cin >> newPipe.diametr;
    if (cin.fail() || newPipe.diametr <= 0) {
        cout << "Error: Invalid diameter. Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add pipe - invalid diameter");
        return;
    }

    cout << "On repair? (0 - no, 1 - yes): ";
    cin >> newPipe.repair;
    if (cin.fail()) {
        cout << "Error: Invalid input. Please enter 0 or 1.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add pipe - invalid repair status");
        return;
    }
    pipes.push_back(newPipe);
    cout << "Pipe added successfully! (ID: " << newPipe.id << ")\n";
    
    stringstream ss;
    ss << "ADDED PIPE - ID: " << newPipe.id << ", Name: " << newPipe.name 
       << ", Length: " << fixed << setprecision(2) << newPipe.length 
       << ", Diameter: " << newPipe.diametr 
       << ", On repair: " << (newPipe.repair ? "Yes" : "No");
    LogAction(ss.str());
}

void AddCompress(vector<Compress>& stations) {
    Compress newStation;
    newStation.id = nextCompressId++;
    cout << "\nCS parameters:\n";
    cout << "Enter name: ";
    cin.ignore();
    getline(cin, newStation.name);

    cout << "Enter quantity: ";
    cin >> newStation.count;
    if (cin.fail() || newStation.count < 0) {
        cout << "Error: Invalid input. Please enter a non-negative number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add CS - invalid quantity");
        return;
    }

    cout << "Enter quantity on working: ";
    cin >> newStation.count_working;
    if (cin.fail() || newStation.count_working < 0 || newStation.count_working > newStation.count) {
        cout << "Error: Invalid input. Please enter a non-negative number less than or equal to quantity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add CS - invalid working quantity");
        return;
    }

    cout << "Enter classification: ";
    cin.ignore();
    getline(cin, newStation.classification);

    cout << "Is CS working? (0 - no, 1 - yes): ";
    cin >> newStation.working;
    if (cin.fail()) {
        cout << "Error: Invalid input. Please enter 0 or 1.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Failed to add CS - invalid working status");
        return;
    }
    stations.push_back(newStation);
    cout << "CS added successfully! (ID: " << newStation.id << ")\n";
    
    stringstream ss;
    ss << "ADDED CS - ID: " << newStation.id << ", Name: " << newStation.name 
       << ", Qty: " << newStation.count << ", Working: " << newStation.count_working 
       << ", Class: " << newStation.classification 
       << ", Active: " << (newStation.working ? "Yes" : "No");
    LogAction(ss.str());
}

void ViewAllPipes(const vector<Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "\nNo pipes available.\n";
        return;
    }
    cout << "\n===== All Pipes =====\n";
    for (size_t i = 0; i < pipes.size(); i++) {
        cout << "ID: " << pipes[i].id << " | Name: " << pipes[i].name
             << " | Length: " << fixed << setprecision(2) << pipes[i].length
             << " | Diameter: " << pipes[i].diametr
             << " | On repair: " << (pipes[i].repair ? "Yes" : "No") << "\n";
    }
    LogAction("VIEWED ALL PIPES - Total: " + to_string(pipes.size()));
}

void ViewAllCompress(const vector<Compress>& stations) {
    if (stations.empty()) {
        cout << "\nNo CS available.\n";
        return;
    }
    cout << "\n===== All CS =====\n";
    for (size_t i = 0; i < stations.size(); i++) {
        cout << "ID: " << stations[i].id << " | Name: " << stations[i].name
             << " | Qty: " << stations[i].count
             << " | Working: " << stations[i].count_working
             << " | Class: " << stations[i].classification
             << " | Active: " << (stations[i].working ? "Yes" : "No") << "\n";
    }
    LogAction("VIEWED ALL CS - Total: " + to_string(stations.size()));
}

void EditPipe(vector<Pipe>& pipes) {
    ViewAllPipes(pipes);
    if (pipes.empty()) return;

    int id;
    cout << "\nEnter pipe ID to edit: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Failed to edit pipe - invalid ID input");
        return;
    }

    for (auto& pipe : pipes) {
        if (pipe.id == id) {
            cout << "\nEditing pipe: " << pipe.name << "\n";
            
            stringstream ss;
            ss << "EDIT PIPE STARTED - ID: " << pipe.id << ", Old Name: " << pipe.name;
            LogAction(ss.str());
            
            cout << "Enter new name: ";
            cin.ignore();
            getline(cin, pipe.name);

            cout << "Enter new length: ";
            cin >> pipe.length;
            if (cin.fail() || pipe.length <= 0) {
                cout << "Error: Invalid length.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                LogAction("ERROR: Edit pipe cancelled - invalid length");
                return;
            }

            cout << "Enter new diameter: ";
            cin >> pipe.diametr;
            if (cin.fail() || pipe.diametr <= 0) {
                cout << "Error: Invalid diameter.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                LogAction("ERROR: Edit pipe cancelled - invalid diameter");
                return;
            }

            cout << "On repair? (0 - no, 1 - yes): ";
            cin >> pipe.repair;
            if (cin.fail()) {
                cout << "Error: Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                LogAction("ERROR: Edit pipe cancelled - invalid repair status");
                return;
            }
            cout << "Pipe updated successfully!\n";
            
            stringstream ss2;
            ss2 << "EDIT PIPE COMPLETED - ID: " << pipe.id << ", New Name: " << pipe.name 
                << ", Length: " << fixed << setprecision(2) << pipe.length 
                << ", Diameter: " << pipe.diametr;
            LogAction(ss2.str());
            return;
        }
    }
    cout << "Error: Pipe with ID " << id << " not found.\n";
    LogAction("ERROR: Pipe not found - ID: " + to_string(id));
}

void EditCompress(vector<Compress>& stations) {
    ViewAllCompress(stations);
    if (stations.empty()) return;

    int id;
    cout << "\nEnter CS ID to edit: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Failed to edit CS - invalid ID input");
        return;
    }

    for (auto& station : stations) {
        if (station.id == id) {
            cout << "\nEditing CS: " << station.name << "\n";
            
            stringstream ss;
            ss << "EDIT CS STARTED - ID: " << station.id << ", Old Name: " << station.name;
            LogAction(ss.str());
            
            cout << "Enter new name: ";
            cin.ignore();
            getline(cin, station.name);

            cout << "Enter new quantity: ";
            cin >> station.count;
            if (cin.fail() || station.count < 0) {
                cout << "Error: Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                LogAction("ERROR: Edit CS cancelled - invalid quantity");
                return;
            }

            cout << "Enter quantity on working: ";
            cin >> station.count_working;
            if (cin.fail() || station.count_working < 0 || station.count_working > station.count) {
                cout << "Error: Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                LogAction("ERROR: Edit CS cancelled - invalid working quantity");
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
                LogAction("ERROR: Edit CS cancelled - invalid working status");
                return;
            }
            cout << "CS updated successfully!\n";
            
            stringstream ss2;
            ss2 << "EDIT CS COMPLETED - ID: " << station.id << ", New Name: " << station.name 
                << ", Qty: " << station.count << ", Working: " << station.count_working;
            LogAction(ss2.str());
            return;
        }
    }
    cout << "Error: CS with ID " << id << " not found.\n";
    LogAction("ERROR: CS not found - ID: " + to_string(id));
}

void DeletePipe(vector<Pipe>& pipes) {
    ViewAllPipes(pipes);
    if (pipes.empty()) return;

    int id;
    cout << "\nEnter pipe ID to delete: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Failed to delete pipe - invalid ID input");
        return;
    }

    for (size_t i = 0; i < pipes.size(); i++) {
        if (pipes[i].id == id) {
            stringstream ss;
            ss << "DELETED PIPE - ID: " << pipes[i].id << ", Name: " << pipes[i].name 
               << ", Length: " << fixed << setprecision(2) << pipes[i].length 
               << ", Diameter: " << pipes[i].diametr;
            LogAction(ss.str());
            
            pipes.erase(pipes.begin() + i);
            cout << "Pipe deleted successfully!\n";
            return;
        }
    }
    cout << "Error: Pipe with ID " << id << " not found.\n";
    LogAction("ERROR: Pipe not found for deletion - ID: " + to_string(id));
}

void DeleteCompress(vector<Compress>& stations) {
    ViewAllCompress(stations);
    if (stations.empty()) return;

    int id;
    cout << "\nEnter CS ID to delete: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Failed to delete CS - invalid ID input");
        return;
    }

    for (size_t i = 0; i < stations.size(); i++) {
        if (stations[i].id == id) {
            stringstream ss;
            ss << "DELETED CS - ID: " << stations[i].id << ", Name: " << stations[i].name 
               << ", Qty: " << stations[i].count << ", Working: " << stations[i].count_working;
            LogAction(ss.str());
            
            stations.erase(stations.begin() + i);
            cout << "CS deleted successfully!\n";
            return;
        }
    }
    cout << "Error: CS with ID " << id << " not found.\n";
    LogAction("ERROR: CS not found for deletion - ID: " + to_string(id));
}

void SearchPipeById(vector<Pipe>& pipes, const vector<Pipe>& allPipes) {
    int id;
    cout << "\nEnter pipe ID to search: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Search pipe by ID - invalid input");
        return;
    }

    LogAction("SEARCH PIPE BY ID - ID: " + to_string(id));
    
    vector<Pipe> results;
    for (const auto& pipe : allPipes) {
        if (pipe.id == id) {
            results.push_back(pipe);
            cout << "\n===== Search Result =====\n";
            cout << "ID: " << pipe.id << " | Name: " << pipe.name
                 << " | Length: " << fixed << setprecision(2) << pipe.length
                 << " | Diameter: " << pipe.diametr
                 << " | On repair: " << (pipe.repair ? "Yes" : "No") << "\n";
            break;
        }
    }
    if (results.empty()) {
        cout << "Error: Pipe with ID " << id << " not found.\n";
        LogAction("SEARCH PIPE BY ID - No results found, ID: " + to_string(id));
        return;
    }

    int choice;
    cout << "\n1. Edit this pipe\n";
    cout << "2. Back\n";
    cout << "Choose: ";
    cin >> choice;
    if (choice == 1) {
        BatchEditPipes(pipes, results);
    }
}

void SearchPipeByName(vector<Pipe>& pipes, const vector<Pipe>& allPipes) {
    string name;
    cout << "\nEnter pipe name to search: ";
    cin.ignore();
    getline(cin, name);

    stringstream logss;
    logss << "SEARCH PIPE BY NAME - Query: '" << name << "'";
    LogAction(logss.str());

    vector<Pipe> results;
    for (const auto& pipe : allPipes) {
        if (pipe.name.find(name) != string::npos) {
            results.push_back(pipe);
        }
    }

    if (results.empty()) {
        cout << "No pipes found with name containing: " << name << "\n";
        LogAction("SEARCH PIPE BY NAME - No results found");
        return;
    }

    cout << "\n===== Search Results =====\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "[" << i << "] ID: " << results[i].id << " | Name: " << results[i].name
             << " | Length: " << fixed << setprecision(2) << results[i].length
             << " | Diameter: " << results[i].diametr
             << " | On repair: " << (results[i].repair ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH PIPE BY NAME - Found " << results.size() << " results";
    LogAction(logss2.str());

    int choice;
    cout << "\n1. Batch edit results\n";
    cout << "2. Back\n";
    cout << "Choose: ";
    cin >> choice;
    if (choice == 1) {
        BatchEditPipes(pipes, results);
    }
}

void SearchPipeByDiameter(vector<Pipe>& pipes, const vector<Pipe>& allPipes) {
    double diameter;
    cout << "\nEnter diameter to search: ";
    cin >> diameter;
    if (cin.fail() || diameter <= 0) {
        cout << "Error: Invalid diameter.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Search pipe by diameter - invalid input");
        return;
    }

    stringstream logss;
    logss << "SEARCH PIPE BY DIAMETER - Diameter: " << fixed << setprecision(2) << diameter;
    LogAction(logss.str());

    vector<Pipe> results;
    for (const auto& pipe : allPipes) {
        if (pipe.diametr == diameter) {
            results.push_back(pipe);
        }
    }

    if (results.empty()) {
        cout << "No pipes found with diameter: " << diameter << "\n";
        LogAction("SEARCH PIPE BY DIAMETER - No results found");
        return;
    }

    cout << "\n===== Search Results (Diameter = " << diameter << ") =====\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "[" << i << "] ID: " << results[i].id << " | Name: " << results[i].name
             << " | Length: " << fixed << setprecision(2) << results[i].length
             << " | Diameter: " << results[i].diametr
             << " | On repair: " << (results[i].repair ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH PIPE BY DIAMETER - Found " << results.size() << " results";
    LogAction(logss2.str());

    int choice;
    cout << "\n1. Batch edit results\n";
    cout << "2. Back\n";
    cout << "Choose: ";
    cin >> choice;
    if (choice == 1) {
        BatchEditPipes(pipes, results);
    }
}

void SearchPipeByRepair(vector<Pipe>& pipes, const vector<Pipe>& allPipes) {
    int repair;
    cout << "\nSearch for pipes on repair? (0 - no, 1 - yes): ";
    cin >> repair;
    if (cin.fail() || (repair != 0 && repair != 1)) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Search pipe by repair - invalid input");
        return;
    }

    stringstream logss;
    logss << "SEARCH PIPE BY REPAIR STATUS - Status: " << (repair ? "On repair" : "Not on repair");
    LogAction(logss.str());

    vector<Pipe> results;
    for (const auto& pipe : allPipes) {
        if (pipe.repair == (bool)repair) {
            results.push_back(pipe);
        }
    }

    if (results.empty()) {
        cout << "No pipes found with repair status: " << (repair ? "Yes" : "No") << "\n";
        LogAction("SEARCH PIPE BY REPAIR STATUS - No results found");
        return;
    }

    cout << "\n===== Search Results (Repair = " << (repair ? "Yes" : "No") << ") =====\n";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "[" << i << "] ID: " << results[i].id << " | Name: " << results[i].name
             << " | Length: " << fixed << setprecision(2) << results[i].length
             << " | Diameter: " << results[i].diametr
             << " | On repair: " << (results[i].repair ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH PIPE BY REPAIR STATUS - Found " << results.size() << " results";
    LogAction(logss2.str());

    int choice;
    cout << "\n1. Batch edit results\n";
    cout << "2. Back\n";
    cout << "Choose: ";
    cin >> choice;
    if (choice == 1) {
        BatchEditPipes(pipes, results);
    }
}

void EditPipeField(Pipe& pipe) {
    int choice;
    cout << "\nWhat to edit?\n";
    cout << "1. Name\n";
    cout << "2. Length\n";
    cout << "3. Diameter\n";
    cout << "4. Repair Status\n";
    cout << "Choose field: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Edit pipe field - invalid choice");
        return;
    }

    switch (choice) {
    case 1: {
        cout << "Enter new name: ";
        cin.ignore();
        getline(cin, pipe.name);
        break;
    }
    case 2: {
        cout << "Enter new length: ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            LogAction("ERROR: Edit pipe field - invalid length");
            return;
        }
        break;
    }
    case 3: {
        cout << "Enter new diameter: ";
        cin >> pipe.diametr;
        if (cin.fail() || pipe.diametr <= 0) {
            cout << "Error: Invalid diameter.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            LogAction("ERROR: Edit pipe field - invalid diameter");
            return;
        }
        break;
    }
    case 4: {
        cout << "On repair? (0 - no, 1 - yes): ";
        cin >> pipe.repair;
        if (cin.fail()) {
            cout << "Error: Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            LogAction("ERROR: Edit pipe field - invalid repair status");
            return;
        }
        break;
    }
    default:
        cout << "Invalid option.\n";
        LogAction("ERROR: Edit pipe field - invalid option");
        break;
    }
}

void BatchEditPipes(vector<Pipe>& pipes, const vector<Pipe>& searchResults) {
    if (searchResults.empty()) {
        cout << "\nNo pipes to edit.\n";
        return;
    }

    int choice;
    cout << "\nBatch Edit Options:\n";
    cout << "1. Edit all found pipes\n";
    cout << "2. Select pipes to edit individually\n";
    cout << "Choose option: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Batch edit - invalid choice");
        return;
    }

    if (choice == 1) {
        cout << "\nEditing all " << searchResults.size() << " pipes...\n";
        LogAction("BATCH EDIT ALL - Editing " + to_string(searchResults.size()) + " pipes");
        
        for (auto& foundPipe : pipes) {
            for (const auto& searchPipe : searchResults) {
                if (foundPipe.id == searchPipe.id) {
                    EditPipeField(foundPipe);
                    break;
                }
            }
        }
        cout << "All pipes updated successfully!\n";
        LogAction("BATCH EDIT ALL - Completed");
    }
    else if (choice == 2) {
        cout << "\n===== Select Pipes to Edit =====\n";
        for (size_t i = 0; i < searchResults.size(); i++) {
            cout << "[" << i << "] ID: " << searchResults[i].id << " | Name: " << searchResults[i].name
                 << " | Diameter: " << searchResults[i].diametr << "\n";
        }

        string input;
        cout << "\nEnter pipe indices to edit (comma-separated, e.g., 0,2,4) or 'all': ";
        cin.ignore();
        getline(cin, input);

        vector<int> indicesToEdit;
        if (input == "all") {
            for (size_t i = 0; i < searchResults.size(); i++) {
                indicesToEdit.push_back(i);
            }
        } else {
            stringstream ss(input);
            string token;
            while (getline(ss, token, ',')) {
                try {
                    int idx = stoi(token);
                    if (idx >= 0 && idx < (int)searchResults.size()) {
                        indicesToEdit.push_back(idx);
                    }
                } catch (...) {
                    cout << "Warning: Invalid index '" << token << "' skipped.\n";
                }
            }
        }

        if (indicesToEdit.empty()) {
            cout << "No valid indices provided.\n";
            LogAction("BATCH EDIT INDIVIDUAL - No valid indices");
            return;
        }

        stringstream logss;
        logss << "BATCH EDIT INDIVIDUAL - Editing " << indicesToEdit.size() << " pipes";
        LogAction(logss.str());

        for (int idx : indicesToEdit) {
            cout << "\n--- Editing: " << searchResults[idx].name << " (ID: " << searchResults[idx].id << ") ---\n";
            for (auto& pipe : pipes) {
                if (pipe.id == searchResults[idx].id) {
                    EditPipeField(pipe);
                    break;
                }
            }
        }
        cout << "\nSelected pipes updated successfully!\n";
        LogAction("BATCH EDIT INDIVIDUAL - Completed");
    } else {
        cout << "Invalid option.\n";
        LogAction("ERROR: Batch edit - invalid option");
    }
}

void SearchPipe(vector<Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "\nNo pipes available.\n";
        return;
    }

    int choice;
    while (true) {
        cout << "\n===== Pipe Search Menu =====\n";
        cout << "1. Search by ID\n";
        cout << "2. Search by Name\n";
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
            SearchPipeById(pipes, pipes);
            break;
        case 2:
            SearchPipeByName(pipes, pipes);
            break;
        case 3:
            SearchPipeByDiameter(pipes, pipes);
            break;
        case 4:
            SearchPipeByRepair(pipes, pipes);
            break;
        case 5:
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
    }
}

void SearchCompressById(const vector<Compress>& stations) {
    int id;
    cout << "\nEnter CS ID to search: ";
    cin >> id;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Invalid input.\n";
        LogAction("ERROR: Search CS by ID - invalid input");
        return;
    }

    LogAction("SEARCH CS BY ID - ID: " + to_string(id));

    bool found = false;
    for (const auto& station : stations) {
        if (station.id == id) {
            cout << "\n===== Search Result =====\n";
            cout << "ID: " << station.id << " | Name: " << station.name
                 << " | Qty: " << station.count
                 << " | Working: " << station.count_working
                 << " | Class: " << station.classification
                 << " | Active: " << (station.working ? "Yes" : "No") << "\n";
            found = true;
            LogAction("SEARCH CS BY ID - Result found");
            break;
        }
    }
    if (!found) {
        cout << "Error: CS with ID " << id << " not found.\n";
        LogAction("SEARCH CS BY ID - No results found");
    }
}

void SearchCompressByName(const vector<Compress>& stations) {
    string name;
    cout << "\nEnter CS name to search: ";
    cin.ignore();
    getline(cin, name);

    stringstream logss;
    logss << "SEARCH CS BY NAME - Query: '" << name << "'";
    LogAction(logss.str());

    vector<Compress> results;
    for (const auto& station : stations) {
        if (station.name.find(name) != string::npos) {
            results.push_back(station);
        }
    }

    if (results.empty()) {
        cout << "No CS found with name containing: " << name << "\n";
        LogAction("SEARCH CS BY NAME - No results found");
        return;
    }

    cout << "\n===== Search Results =====\n";
    for (const auto& station : results) {
        cout << "ID: " << station.id << " | Name: " << station.name
             << " | Qty: " << station.count
             << " | Working: " << station.count_working
             << " | Class: " << station.classification
             << " | Active: " << (station.working ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH CS BY NAME - Found " << results.size() << " results";
    LogAction(logss2.str());
}

void SearchCompressByClassification(const vector<Compress>& stations) {
    string classification;
    cout << "\nEnter classification to search: ";
    cin.ignore();
    getline(cin, classification);

    stringstream logss;
    logss << "SEARCH CS BY CLASSIFICATION - Query: '" << classification << "'";
    LogAction(logss.str());

    vector<Compress> results;
    for (const auto& station : stations) {
        if (station.classification.find(classification) != string::npos) {
            results.push_back(station);
        }
    }

    if (results.empty()) {
        cout << "No CS found with classification containing: " << classification << "\n";
        LogAction("SEARCH CS BY CLASSIFICATION - No results found");
        return;
    }

    cout << "\n===== Search Results =====\n";
    for (const auto& station : results) {
        cout << "ID: " << station.id << " | Name: " << station.name
             << " | Qty: " << station.count
             << " | Working: " << station.count_working
             << " | Class: " << station.classification
             << " | Active: " << (station.working ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH CS BY CLASSIFICATION - Found " << results.size() << " results";
    LogAction(logss2.str());
}

void SearchCompressByStatus(const vector<Compress>& stations) {
    int working;
    cout << "\nSearch for CS working? (0 - no, 1 - yes): ";
    cin >> working;
    if (cin.fail() || (working != 0 && working != 1)) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        LogAction("ERROR: Search CS by status - invalid input");
        return;
    }

    stringstream logss;
    logss << "SEARCH CS BY STATUS - Status: " << (working ? "Working" : "Not working");
    LogAction(logss.str());

    vector<Compress> results;
    for (const auto& station : stations) {
        if (station.working == (bool)working) {
            results.push_back(station);
        }
    }

    if (results.empty()) {
        cout << "No CS found with working status: " << (working ? "Yes" : "No") << "\n";
        LogAction("SEARCH CS BY STATUS - No results found");
        return;
    }

    cout << "\n===== Search Results (Working = " << (working ? "Yes" : "No") << ") =====\n";
    for (const auto& station : results) {
        cout << "ID: " << station.id << " | Name: " << station.name
             << " | Qty: " << station.count
             << " | Working: " << station.count_working
             << " | Class: " << station.classification
             << " | Active: " << (station.working ? "Yes" : "No") << "\n";
    }

    stringstream logss2;
    logss2 << "SEARCH CS BY STATUS - Found " << results.size() << " results";
    LogAction(logss2.str());
}

void SearchCompress(const vector<Compress>& stations) {
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
            SearchCompressById(stations);
            break;
        case 2:
            SearchCompressByName(stations);
            break;
        case 3:
            SearchCompressByClassification(stations);
            break;
        case 4:
            SearchCompressByStatus(stations);
            break;
        case 5:
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
    }
}

void Menu(vector<Pipe>& pipes, vector<Compress>& stations) {
    LogAction("APPLICATION STARTED");
    
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
        case 1:
            AddPipe(pipes);
            break;
        case 2:
            EditPipe(pipes);
            break;
        case 3:
            DeletePipe(pipes);
            break;
        case 4:
            ViewAllPipes(pipes);
            break;
        case 5:
            SearchPipe(pipes);
            break;
        case 6:
            AddCompress(stations);
            break;
        case 7:
            EditCompress(stations);
            break;
        case 8:
            DeleteCompress(stations);
            break;
        case 9:
            ViewAllCompress(stations);
            break;
        case 10:
            SearchCompress(stations);
            break;
        case 11:
            SaveAllData(pipes, stations);
            break;
        case 12:
            LoadAllData(pipes, stations);
            break;
        case 13:
            ViewLogs();
            break;
        case 14:
            LogAction("APPLICATION CLOSED");
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
    }
}

int main() {
    vector<Pipe> pipes;
    vector<Compress> stations;

    Menu(pipes, stations);

    return 0;
}

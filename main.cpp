#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <vector>

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
        return;
    }

    cout << "Enter diameter: ";
    cin >> newPipe.diametr;
    if (cin.fail() || newPipe.diametr <= 0) {
        cout << "Error: Invalid diameter. Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "On repair? (0 - no, 1 - yes): ";
    cin >> newPipe.repair;
    if (cin.fail()) {
        cout << "Error: Invalid input. Please enter 0 or 1.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    pipes.push_back(newPipe);
    cout << "Pipe added successfully! (ID: " << newPipe.id << ")\n";
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
        return;
    }

    cout << "Enter quantity on working: ";
    cin >> newStation.count_working;
    if (cin.fail() || newStation.count_working < 0 || newStation.count_working > newStation.count) {
        cout << "Error: Invalid input. Please enter a non-negative number less than or equal to quantity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
        return;
    }
    stations.push_back(newStation);
    cout << "CS added successfully! (ID: " << newStation.id << ")\n";
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
        return;
    }

    for (auto& pipe : pipes) {
        if (pipe.id == id) {
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
            return;
        }
    }
    cout << "Error: Pipe with ID " << id << " not found.\n";
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
        return;
    }

    for (auto& station : stations) {
        if (station.id == id) {
            cout << "\nEditing CS: " << station.name << "\n";
            cout << "Enter new name: ";
            cin.ignore();
            getline(cin, station.name);

            cout << "Enter new quantity: ";
            cin >> station.count;
            if (cin.fail() || station.count < 0) {
                cout << "Error: Invalid input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }

            cout << "Enter quantity on working: ";
            cin >> station.count_working;
            if (cin.fail() || station.count_working < 0 || station.count_working > station.count) {
                cout << "Error: Invalid input.\n";
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
            return;
        }
    }
    cout << "Error: CS with ID " << id << " not found.\n";
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
        return;
    }

    for (size_t i = 0; i < pipes.size(); i++) {
        if (pipes[i].id == id) {
            pipes.erase(pipes.begin() + i);
            cout << "Pipe deleted successfully!\n";
            return;
        }
    }
    cout << "Error: Pipe with ID " << id << " not found.\n";
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
        return;
    }

    for (size_t i = 0; i < stations.size(); i++) {
        if (stations[i].id == id) {
            stations.erase(stations.begin() + i);
            cout << "CS deleted successfully!\n";
            return;
        }
    }
    cout << "Error: CS with ID " << id << " not found.\n";
}

void Menu(vector<Pipe>& pipes, vector<Compress>& stations) {
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
        cout << "9. Exit\n";
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
            AddCompress(stations);
            break;
        case 6:
            EditCompress(stations);
            break;
        case 7:
            DeleteCompress(stations);
            break;
        case 8:
            ViewAllCompress(stations);
            break;
        case 9:
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

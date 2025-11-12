#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

struct Pipe {
    string name;
    double length;
    double diametr;
    bool repair;
};

struct Compress {
    string name;
    int count;
    int count_working;
    string classification;
    bool working;
};

void AddPipe(vector<Pipe>& pipes) {
    Pipe newPipe;
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
    cout << "Pipe added successfully!\n";
}

void AddCompress(vector<Compress>& stations) {
    Compress newStation;
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
    cout << "CS added successfully!\n";
}

void ViewAllPipes(const vector<Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "\nNo pipes available.\n";
        return;
    }
    cout << "\n===== All Pipes =====\n";
    for (size_t i = 0; i < pipes.size(); i++) {
        cout << "[" << i << "] Name: " << pipes[i].name
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
        cout << "[" << i << "] Name: " << stations[i].name
             << " | Qty: " << stations[i].count
             << " | Working: " << stations[i].count_working
             << " | Class: " << stations[i].classification
             << " | Active: " << (stations[i].working ? "Yes" : "No") << "\n";
    }
}

void EditPipe(vector<Pipe>& pipes) {
    ViewAllPipes(pipes);
    if (pipes.empty()) return;

    int index;
    cout << "\nEnter pipe index to edit: ";
    cin >> index;
    if (cin.fail() || index < 0 || index >= (int)pipes.size()) {
        cout << "Error: Invalid index.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "\nEditing pipe: " << pipes[index].name << "\n";
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, pipes[index].name);

    cout << "Enter new length: ";
    cin >> pipes[index].length;
    if (cin.fail() || pipes[index].length <= 0) {
        cout << "Error: Invalid length.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "Enter new diameter: ";
    cin >> pipes[index].diametr;
    if (cin.fail() || pipes[index].diametr <= 0) {
        cout << "Error: Invalid diameter.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "On repair? (0 - no, 1 - yes): ";
    cin >> pipes[index].repair;
    if (cin.fail()) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "Pipe updated successfully!\n";
}

void EditCompress(vector<Compress>& stations) {
    ViewAllCompress(stations);
    if (stations.empty()) return;

    int index;
    cout << "\nEnter CS index to edit: ";
    cin >> index;
    if (cin.fail() || index < 0 || index >= (int)stations.size()) {
        cout << "Error: Invalid index.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "\nEditing CS: " << stations[index].name << "\n";
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, stations[index].name);

    cout << "Enter new quantity: ";
    cin >> stations[index].count;
    if (cin.fail() || stations[index].count < 0) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "Enter quantity on working: ";
    cin >> stations[index].count_working;
    if (cin.fail() || stations[index].count_working < 0 || stations[index].count_working > stations[index].count) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "Enter new classification: ";
    cin.ignore();
    getline(cin, stations[index].classification);

    cout << "Is CS working? (0 - no, 1 - yes): ";
    cin >> stations[index].working;
    if (cin.fail()) {
        cout << "Error: Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "CS updated successfully!\n";
}

void DeletePipe(vector<Pipe>& pipes) {
    ViewAllPipes(pipes);
    if (pipes.empty()) return;

    int index;
    cout << "\nEnter pipe index to delete: ";
    cin >> index;
    if (cin.fail() || index < 0 || index >= (int)pipes.size()) {
        cout << "Error: Invalid index.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    pipes.erase(pipes.begin() + index);
    cout << "Pipe deleted successfully!\n";
}

void DeleteCompress(vector<Compress>& stations) {
    ViewAllCompress(stations);
    if (stations.empty()) return;

    int index;
    cout << "\nEnter CS index to delete: ";
    cin >> index;
    if (cin.fail() || index < 0 || index >= (int)stations.size()) {
        cout << "Error: Invalid index.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    stations.erase(stations.begin() + index);
    cout << "CS deleted successfully!\n";
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

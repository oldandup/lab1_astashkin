#include <iostream>
#include <string>

using namespace std;

struct Pipe {
    string name;
    float length;
    int diametr;
    bool repair;
};

struct Compress {
    string name;
    int count;
    int count_working;
    string classification;
    bool working;
};

void Menu(Pipe t, Compress c) {
    while (true)
    {
        cout << "\nChoose type:\n 1. Pipe \n 2. Compress\n 3. Show All\n";
        int type;
        cin >> type;
        if (cin.fail()) {
            cout << "Error!" << endl;
            cout << "Write correct number (1-3)" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        switch (type)
        {
        case 1:
            cout << "Choose option:\n 1. Add pipe \n 2. Pipe lenght\n 3. Pipe diametr\n 4. Repairing\n";
            int option;
            cin >> option;
            if (cin.fail()) {
                cout << "Error!" << endl;
                cout << "Write correct number (1-4)" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            switch (option)
            {
            case 1:
                cout << "Enter pipe name:";
                cin >> t.name;
                break;
            case 2:
                cout << "Enter pipe lenght (km): ";
                cin >> t.length;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write correct number" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            case 3:
                cout << "Enter pipe diametr (mm): ";
                cin >> t.diametr;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write correct number" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            case 4:
                cout << "Is pipe on repair? (0 - no, 1 - yes)";
                cin >> t.repair;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write 0 or 1" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            }
            break;
        case 2:
            cout << "Choose option:\n 1. Add CS \n 2. Quantity of CS \n 3. Quantity working\n 4. Classification\n 5. Working\n";
            int option2;
            cin >> option2;
            if (cin.fail()) {
                cout << "Error!" << endl;
                cout << "Write correct number (1-5)" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            switch (option2)
            {
            case 1:
                cout << "Enter CS name: ";
                cin >> c.name;
                break;
            case 2:
                cout << "Enter quantity of enterprises: ";
                cin >> c.count;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write correct number" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            case 3:
                cout << "Enter quantity of working enterprises: ";
                cin >> c.count_working;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write correct number" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            case 4:
                cout << "Enter classification: ";
                cin >> c.classification;
                break;
            case 5:
                cout << "Is CS working? (0 - no, 1 - yes) ";
                cin >> c.working;
                if (cin.fail()) {
                    cout << "Error!" << endl;
                    cout << "Write 0 or 1" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                break;
            }
            break;
        case 3:
            cout << "\nPipe parameters: \n" << "Name: " << t.name << "\nLength: " << t.length << "\nDiametr: " << t.diametr << "\nOn repair: " << t.repair;
            cout << "\n\nCS parameters: \n" << "Name: " << c.name << "\nQuantity: " << c.count << "\nOn working: " << c.count_working << "\nClassification: " << c.classification << "\nWorking : " << c.working;
            break;
        }
    };
};

int main()
{
    Pipe truba;
    Compress ceh;
    Menu(truba, ceh);
}

#include <iostream>
#include <limits>
#include <string>
#include <iomanip> // Для std::fixed и std::setprecision

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

void Menu(Pipe& t, Compress& c) { // Передача по ссылке
    int choice;
    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add pipe\n";
        cout << "2. Add CS\n";
        cout << "3. View objects\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "Pipe parameters:\n";
            cout << "Enter name: ";
            cin.ignore(); // Очищаем буфер перед getline
            getline(cin, t.name);


            cout << "Enter length: ";
            cin >> t.length;
            if (cin.fail() || t.length <= 0) { // Проверка на ошибку и валидация
                cout << "Error: Invalid length. Please enter a positive number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Enter diameter: "; // Исправлено "diametr" на "diameter"
            cin >> t.diametr;
            if (cin.fail() || t.diametr <= 0) { // Проверка на ошибку и валидация
                cout << "Error: Invalid diameter. Please enter a positive number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "On repair? (0 - no, 1 - yes) ";
            cin >> t.repair;
            if (cin.fail()) {
                cout << "Error: Invalid input. Please enter 0 or 1.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            break;
        }
        case 2: {
            cout << "CS parameters:\n";
            cout << "Enter name: ";
            cin.ignore(); // Очищаем буфер перед getline
            getline(cin, c.name);

            cout << "Enter quantity: ";
            cin >> c.count;
            if (cin.fail() || c.count < 0) {
                cout << "Error: Invalid input. Please enter a non-negative number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Enter quantity on working: ";
            cin >> c.count_working;
            if (cin.fail() || c.count_working < 0 || c.count_working > c.count) {
                cout << "Error: Invalid input. Please enter a non-negative number less than or equal to quantity.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Enter classification: ";
            cin.ignore(); // Очищаем буфер перед getline
            getline(cin, c.classification);

            cout << "Is CS working? (0 - no, 1 - yes) ";
            cin >> c.working;
            if (cin.fail()) {
                cout << "Error: Invalid input. Please enter 0 or 1.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            break;
        }
        case 3:
             cout << "\nPipe parameters:\n"
                  << "Name: " << t.name << "\n"
                  << "Length: " << fixed << setprecision(2) << t.length << "\n" // Форматированный вывод с двумя знаками после запятой
                  << "Diameter: " << t.diametr << "\n" // Исправлено "Diametr" на "Diameter"
                  << "On repair: " << (t.repair ? "Yes" : "No") << "\n";

             cout << "\nCS parameters:\n"
                  << "Name: " << c.name << "\n"
                  << "Quantity: " << c.count << "\n"
                  << "On working: " << c.count_working << "\n"
                  << "Classification: " << c.classification << "\n"
                  << "Working: " << (c.working ? "Yes" : "No") << "\n";
            break;
        case 4:
            return;
        default:
            cout << "Invalid option. Please try again.\n";
            break;
        }
    };
};

int main()
{
    Pipe truba;
    Compress ceh;

    // Инициализация значений по умолчанию
    truba.name = "Unknown";
    truba.length = 0.0;
    truba.diametr = 0.0;
    truba.repair = false;

    ceh.name = "Unknown";
    ceh.count = 0;
    ceh.count_working = 0;
    ceh.classification = "Unknown";
    ceh.working = false;


    Menu(truba, ceh);
}

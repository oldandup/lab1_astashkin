#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "pipe_manager.h"
#include "compress_manager.h"
#include "file_manager.h"
#include "search_engine.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

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
        string filename;
        cout << "\nEnter filename to save (or press Enter for default 'data_backup.txt'): ";
        cin.ignore();
        getline(cin, filename);

        if (filename.empty()) {
            fileManager.SaveAllData(pipeManager, compressManager);
        } else {
            if (filename.find('.') == string::npos) {
                filename += ".txt";
            }
            fileManager.SaveAllData(pipeManager, compressManager, filename);
        }
    }

    void LoadData(int& nextPipeId, int& nextCompressId) {
        string filename;
        cout << "\nEnter filename to load (or press Enter for default 'data_backup.txt'): ";
        cin.ignore();
        getline(cin, filename);

        if (filename.empty()) {
            fileManager.LoadAllData(pipeManager, compressManager, nextPipeId, nextCompressId);
        } else {
            if (filename.find('.') == string::npos) {
                filename += ".txt";
            }
            fileManager.LoadAllData(pipeManager, compressManager, nextPipeId, nextCompressId, filename);
        }
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
            cout << "5. Search by Length Range\n";
            cout << "6. Back to Main Menu\n";
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
                SearchPipesByLength();
                break;
            case 6:
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
            cout << "5. Search by Working Workshops Count\n";
            cout << "6. Search by Workshop Percentage\n";
            cout << "7. Back to Main Menu\n";
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
                SearchCompressByWorkshopCount();
                break;
            case 6:
                SearchCompressByPercentage();
                break;
            case 7:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void EditPipeById() {
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
            logger.Log("ERROR: Pipe not found for editing - ID: " + to_string(id));
            return;
        }

        logger.Log("EDIT PIPE STARTED - ID: " + to_string(id) + ", Old Name: " + pipe->km_mark);
        EditPipeFields(*pipe);
        logger.Log("EDIT PIPE COMPLETED - ID: " + to_string(id));
    }

    void EditCompressById() {
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
            logger.Log("ERROR: CS not found for editing - ID: " + to_string(id));
            return;
        }

        logger.Log("EDIT CS STARTED - ID: " + to_string(id) + ", Old Name: " + station->name);
        EditCompressFields(*station);
        logger.Log("EDIT CS COMPLETED - ID: " + to_string(id));
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

    void DisplayPipesWithEditOption(const vector<Pipe>& pipes) {
        cout << "\n===== Search Results =====\n";
        for (size_t i = 0; i < pipes.size(); i++) {
            cout << "[" << i << "] ID: " << pipes[i].id << " | KM: " << pipes[i].km_mark
                 << " | Length: " << fixed << setprecision(2) << pipes[i].length << " km"
                 << " | Diameter: " << pipes[i].diametr << " mm"
                 << " | On repair: " << (pipes[i].repair ? "Yes" : "No") << "\n";
        }
        cout << "\nWould you like to edit any of these results? (0 - no, 1 - yes): ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            EditSearchResultsPipes(pipes);
        }
    }

    void DisplayCompressWithEditOption(const vector<Compress>& stations) {
        cout << "\n===== Search Results =====\n";
        for (size_t i = 0; i < stations.size(); i++) {
            cout << "[" << i << "] ID: " << stations[i].id << " | Name: " << stations[i].name
                 << " | Workshops: " << stations[i].workshop_count
                 << " | Working: " << stations[i].workshop_working
                 << " | Class: " << stations[i].classification
                 << " | Active: " << (stations[i].working ? "Yes" : "No") << "\n";
        }
        cout << "\nWould you like to edit any of these results? (0 - no, 1 - yes): ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            EditSearchResultsCompress(stations);
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

        DisplayPipesWithEditOption(results);
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

        DisplayPipesWithEditOption(results);
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

        DisplayPipesWithEditOption(results);
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

        DisplayPipesWithEditOption(results);
    }

    void SearchPipesByLength() {
        double minLength, maxLength;
        cout << "\nEnter minimum length (km): ";
        cin >> minLength;
        if (cin.fail() || minLength < 0) {
            cout << "Error: Invalid length.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter maximum length (km): ";
        cin >> maxLength;
        if (cin.fail() || maxLength < 0 || maxLength < minLength) {
            cout << "Error: Invalid length range.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchPipesByLength(pipeManager.GetAll(), minLength, maxLength);
        if (results.empty()) {
            cout << "No pipes found with length between " << fixed << setprecision(2)
                 << minLength << " and " << maxLength << " km\n";
            return;
        }

        DisplayPipesWithEditOption(results);
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

        DisplayCompressWithEditOption(results);
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

        DisplayCompressWithEditOption(results);
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

        DisplayCompressWithEditOption(results);
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

        DisplayCompressWithEditOption(results);
    }

    void SearchCompressByWorkshopCount() {
        int minCount, maxCount;
        cout << "\nEnter minimum working workshops: ";
        cin >> minCount;
        if (cin.fail() || minCount < 0) {
            cout << "Error: Invalid count.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter maximum working workshops: ";
        cin >> maxCount;
        if (cin.fail() || maxCount < 0 || maxCount < minCount) {
            cout << "Error: Invalid count range.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchCompressByWorkshopCount(compressManager.GetAll(), minCount, maxCount);
        if (results.empty()) {
            cout << "No CS found with working workshops between " << minCount << " and " << maxCount << "\n";
            return;
        }

        DisplayCompressWithEditOption(results);
    }

    void SearchCompressByPercentage() {
        double minPercent, maxPercent;
        cout << "\nEnter minimum working percentage (0-100): ";
        cin >> minPercent;
        if (cin.fail() || minPercent < 0 || minPercent > 100) {
            cout << "Error: Invalid percentage.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        cout << "Enter maximum working percentage (0-100): ";
        cin >> maxPercent;
        if (cin.fail() || maxPercent < 0 || maxPercent > 100 || maxPercent < minPercent) {
            cout << "Error: Invalid percentage range.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        auto results = searchEngine.SearchCompressByWorkshopPercentage(compressManager.GetAll(), minPercent, maxPercent);
        if (results.empty()) {
            cout << "No CS found with working percentage between " << fixed << setprecision(1)
                 << minPercent << "% and " << maxPercent << "%\n";
            return;
        }

        DisplayCompressWithEditOption(results);
    }

    void EditSearchResultsPipes(const vector<Pipe>& searchResults) {
        if (searchResults.empty()) {
            cout << "No results to edit.\n";
            return;
        }

        int choice;
        while (true) {
            cout << "\n===== Edit Search Results =====\n";
            cout << "1. Edit all results\n";
            cout << "2. Edit specific pipe by index\n";
            cout << "3. Back to Search Menu\n";
            cout << "Choose option: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Invalid input.\n";
                continue;
            }

            switch (choice) {
            case 1:
                EditAllPipeResults(searchResults);
                break;
            case 2:
                EditSpecificPipeResult(searchResults);
                break;
            case 3:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void EditSearchResultsCompress(const vector<Compress>& searchResults) {
        if (searchResults.empty()) {
            cout << "No results to edit.\n";
            return;
        }

        int choice;
        while (true) {
            cout << "\n===== Edit Search Results =====\n";
            cout << "1. Edit all results\n";
            cout << "2. Edit specific CS by index\n";
            cout << "3. Back to Search Menu\n";
            cout << "Choose option: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Invalid input.\n";
                continue;
            }

            switch (choice) {
            case 1:
                EditAllCompressResults(searchResults);
                break;
            case 2:
                EditSpecificCompressResult(searchResults);
                break;
            case 3:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void EditAllPipeResults(const vector<Pipe>& searchResults) {
        cout << "\nYou are about to edit all " << searchResults.size() << " pipes.\n";
        cout << "Confirm? (0 - no, 1 - yes): ";
        int confirm;
        cin >> confirm;
        if (cin.fail() || confirm != 1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Operation cancelled.\n";
            return;
        }

        for (const auto& result : searchResults) {
            Pipe* pipe = pipeManager.FindById(result.id);
            if (pipe) {
                cout << "\n--- Editing Pipe ID: " << pipe->id << " (KM: " << pipe->km_mark << ") ---\n";
                logger.Log("BATCH EDIT PIPE STARTED - ID: " + to_string(pipe->id));
                EditPipeFields(*pipe);
                logger.Log("BATCH EDIT PIPE COMPLETED - ID: " + to_string(pipe->id));
            }
        }
        cout << "\nBatch edit completed!\n";
    }

    void EditAllCompressResults(const vector<Compress>& searchResults) {
        cout << "\nYou are about to edit all " << searchResults.size() << " CS.\n";
        cout << "Confirm? (0 - no, 1 - yes): ";
        int confirm;
        cin >> confirm;
        if (cin.fail() || confirm != 1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Operation cancelled.\n";
            return;
        }

        for (const auto& result : searchResults) {
            Compress* station = compressManager.FindById(result.id);
            if (station) {
                cout << "\n--- Editing CS ID: " << station->id << " (Name: " << station->name << ") ---\n";
                logger.Log("BATCH EDIT CS STARTED - ID: " + to_string(station->id));
                EditCompressFields(*station);
                logger.Log("BATCH EDIT CS COMPLETED - ID: " + to_string(station->id));
            }
        }
        cout << "\nBatch edit completed!\n";
    }

    void EditSpecificPipeResult(const vector<Pipe>& searchResults) {
        int index;
        cout << "\nEnter index of pipe to edit (0-" << (searchResults.size() - 1) << "): ";
        cin >> index;
        if (cin.fail() || index < 0 || index >= (int)searchResults.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid index.\n";
            return;
        }

        Pipe* pipe = pipeManager.FindById(searchResults[index].id);
        if (!pipe) {
            cout << "Error: Pipe not found.\n";
            return;
        }

        logger.Log("EDIT PIPE FROM SEARCH - ID: " + to_string(pipe->id) + ", Old Name: " + pipe->km_mark);
        EditPipeFields(*pipe);
        logger.Log("EDIT PIPE FROM SEARCH COMPLETED - ID: " + to_string(pipe->id));
    }

    void EditSpecificCompressResult(const vector<Compress>& searchResults) {
        int index;
        cout << "\nEnter index of CS to edit (0-" << (searchResults.size() - 1) << "): ";
        cin >> index;
        if (cin.fail() || index < 0 || index >= (int)searchResults.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid index.\n";
            return;
        }

        Compress* station = compressManager.FindById(searchResults[index].id);
        if (!station) {
            cout << "Error: CS not found.\n";
            return;
        }

        logger.Log("EDIT CS FROM SEARCH - ID: " + to_string(station->id) + ", Old Name: " + station->name);
        EditCompressFields(*station);
        logger.Log("EDIT CS FROM SEARCH COMPLETED - ID: " + to_string(station->id));
    }
};

#endif

#include <iostream>
#include <string>
using namespace std;

struct Pipe {
    string name;
    float legth;
    int diametr;
    bool repair;
};
struct Comp {
    string name;
    int workshops;
    string classes;
};

int main(){
    cout << "Hello world!" << endl;
    Pipe truba;
    truba.name = "SVOBODA";
    cout << truba.name;
    return 0;
}
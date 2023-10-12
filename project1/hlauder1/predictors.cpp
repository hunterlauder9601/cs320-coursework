#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include "predictorTypes.cpp"
using namespace std;

int main(int argc, char *argv[]) {
    if(argc!=3){
        cout<< "Use command format: ./predictors <inputfile> <outputfile>" <<endl;
        return 0;
    }

    auto* p1 = new predictor1();
    auto* p2 = new predictor2();
    auto* p3 = new predictor3();
    auto* p4 = new predictor4();
    auto* p5 = new predictor5();
    auto* p6 = new predictor6();
    auto* p7 = new predictor7();

    unsigned long long address;
    string behavior, line;
    unsigned long long target;
    bool curBehavior;
    ifstream inFile;
    inFile.open(argv[1]);
    if (inFile.fail()) {
        cerr << "Could not find file" << endl;
    }
    // The following loop will read a line at a time
    while(getline(inFile, line)) {
        stringstream s(line);
        s >> std::hex >> address >> behavior >> std::hex >> target;

        if(behavior == "T") {
            curBehavior = true;
        } else {
            curBehavior = false;
        }

//        cout << "P1:" << endl;
        p1->update(curBehavior);
//        cout << "P2:" << endl;
        p2->update(curBehavior);
//        cout << "P3:" << endl;
        p3->predictUpdate(curBehavior, address);
//        cout << "P4:" << endl;
        p4->predict(address);
        p4->updateTables(curBehavior, address);
//        cout << "P5:" << endl;
        p5->predict(address);
        p5->update(curBehavior, address);
//        cout << "P6:" << endl;
        p6->predict(address);
        p6->update(curBehavior, address);
//        cout << "P7:" << endl;
        p7->predict(curBehavior, address);
        p7->update(curBehavior, address, target);
    }
    ofstream out(argv[2]);
    cout.rdbuf(out.rdbuf()); //redirects cout to out.txt file
    p1->print();
    p2->print();
    p3->print();
    p4->print();
    p5->print();
    p6->print();
    p7->print();
    return 0;
}
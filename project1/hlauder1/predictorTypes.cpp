#include <iostream>

using namespace std;
//Always Taken
class predictor1{
    private:
        int correct = 0;
        int total = 0;
    public:
        void update(bool result) {
            if(result) {
                correct++;
                total++;
            } else {
                total++;
            }
        }
        void print() const {
            cout << correct << "," << total << ";" << endl;
        }
};
//Always Non-Taken
class predictor2{
    private:
        int correct = 0;
        int total = 0;
    public:
        void update(bool result) {
            if(!result) {
                correct++;
                total++;
            } else {
                total++;
            }
        }
        void print() const {
            cout << correct << "," << total << ";" << endl;
        }
};
//Bimodal Predictor with a single bit of history
class predictor3 {
private:
    bool entries16[16];
    bool entries32[32];
    bool entries128[128];
    bool entries256[256];
    bool entries512[512];
    bool entries1024[1024];
    bool entries2048[2048];
    int correct16, total16, correct32, total32, correct128, total128, correct256, total256, correct512, total512,
            correct1024, total1024, correct2048, total2048 = 0;
public:
    predictor3() {
        for (bool &i: entries16) {
            i = true;
        }
        for (bool &i: entries32) {
            i = true;
        }
        for (bool &i: entries128) {
            i = true;
        }
        for (bool &i: entries256) {
            i = true;
        }
        for (bool &i: entries512) {
            i = true;
        }
        for (bool &i: entries1024) {
            i = true;
        }
        for (bool &i: entries2048) {
            i = true;
        }
    }
    bool getPrediction(unsigned long long address) {
        return entries512[address%512]; //for BTB
    }

    void predictUpdate(bool result, unsigned long long address) {
        if (entries16[address % 16] == result) {
            correct16++;
            total16++;
        } else {
            entries16[address % 16] = result;
            total16++;
        }
        if (entries32[address % 32] == result) {
            correct32++;
            total32++;
        } else {
            entries32[address % 32] = result;
            total32++;
        }
        if (entries128[address % 128] == result) {
            correct128++;
            total128++;
        } else {
            entries128[address % 128] = result;
            total128++;
        }
        if (entries256[address % 256] == result) {
            correct256++;
            total256++;
        } else {
            entries256[address % 256] = result;
            total256++;
        }
        if (entries512[address % 512] == result) {
            correct512++;
            total512++;
        } else {
            entries512[address % 512] = result;
            total512++;
        }
        if (entries1024[address % 1024] == result) {
            correct1024++;
            total1024++;
        } else {
            entries1024[address % 1024] = result;
            total1024++;
        }
        if (entries2048[address % 2048] == result) {
            correct2048++;
            total2048++;
        } else {
            entries2048[address % 2048] = result;
            total2048++;
        }
    }

    void print() const {
        std::cout << correct16 << "," << total16 << "; ";
        std::cout << correct32 << "," << total32 << "; ";
        std::cout << correct128 << "," << total128 << "; ";
        std::cout << correct256 << "," << total256 << "; ";
        std::cout << correct512 << "," << total512 << "; ";
        std::cout << correct1024 << "," << total1024 << "; ";
        std::cout << correct2048 << "," << total2048 << ";" << std::endl;
    }
};
//Bimodal Predictor with 2-bit saturating counters
class predictor4{
private:
    bool bitsToPrediction[7];
    int entries16[16];
    int entries32[32];
    int entries128[128];
    int entries256[256];
    int entries512[512];
    int entries1024[1024];
    int entries2048[2048];
    int correct16, total16, correct32, total32, correct128, total128, correct256, total256, correct512, total512, correct1024, total1024, correct2048, total2048=0;
public:
    //set all to strongly taken
    predictor4() {
        for (int& i : entries16) {
            i = 3;
        }
        for(int& i : entries32){
            i = 3;
        }
        for(int& i : entries128){
            i = 3;
        }
        for(int& i : entries256){
            i = 3;
        }
        for(int& i : entries512){
            i = 3;
        }
        for(int& i : entries1024){
            i = 3;
        }
        for(int& i : entries2048){
            i = 3;
        }
    }

    void predict(unsigned long long address){
        //00/01 = NT and 10/11 = T
        bitsToPrediction[0]= entries16[address % 16] > 1;
        bitsToPrediction[1]= entries32[address % 32] > 1;
        bitsToPrediction[2]= entries128[address % 128] > 1;
        bitsToPrediction[3]= entries256[address % 256] > 1;
        bitsToPrediction[4]= entries512[address % 512] > 1;
        bitsToPrediction[5]= entries1024[address % 1024] > 1;
        bitsToPrediction[6]= entries2048[address % 2048] > 1;
    }
    void updateTables(bool result, unsigned long long address){
        if (bitsToPrediction[0] == result) {
            correct16++;
            total16++;
            if (bitsToPrediction[0] && (entries16[address % 16] < 3)) { // T is correct
                entries16[address % 16] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[0] && (entries16[address % 16] > 0)) { // NT is correct
                entries16[address % 16] -= 1;//shift towards strongly not taken
            }
        } else {
            total16++;
            if (!bitsToPrediction[0] && (entries16[address % 16] < 3)) { //NT is incorrect
                entries16[address % 16] += 1;
            } else if (bitsToPrediction[0] && (entries16[address % 16] > 0)) { //T is incorrect
                entries16[address % 16] -= 1;
            }
        }
        if (bitsToPrediction[1] == result) {
            correct32++;
            total32++;
            if (bitsToPrediction[1] && (entries32[address % 32] < 3)) { // T is correct
                entries32[address % 32] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[1] && (entries32[address % 32] > 0)) { // NT is correct
                entries32[address % 32] -= 1;//shift towards strongly not taken
            }
        } else {
            total32++;
            if (!bitsToPrediction[1] && (entries32[address % 32] < 3)) { //NT is incorrect
                entries32[address % 32] += 1;
            } else if (bitsToPrediction[1] && (entries32[address % 32] > 0)) { //T is incorrect
                entries32[address % 32] -= 1;
            }
        }
        if (bitsToPrediction[2] == result) {
            correct128++;
            total128++;
            if (bitsToPrediction[2] && (entries128[address % 128] < 3)) { // T is correct
                entries128[address % 128] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[2] && (entries128[address % 128] > 0)) { // NT is correct
                entries128[address % 128] -= 1;//shift towards strongly not taken
            }
        } else {
            total128++;
            if (!bitsToPrediction[2] && (entries128[address % 128] < 3)) { //NT is incorrect
                entries128[address % 128] += 1;
            } else if (bitsToPrediction[2] && (entries128[address % 128] > 0)) { //T is incorrect
                entries128[address % 128] -= 1;
            }
        }
        if (bitsToPrediction[3] == result) {
            correct256++;
            total256++;
            if (bitsToPrediction[3] && (entries256[address % 256] < 3)) { // T is correct
                entries256[address % 256] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[3] && (entries256[address % 256] > 0)) { // NT is correct
                entries256[address % 256] -= 1;//shift towards strongly not taken
            }
        } else {
            total256++;
            if (!bitsToPrediction[3] && (entries256[address % 256] < 3)) { //NT is incorrect
                entries256[address % 256] += 1;
            } else if (bitsToPrediction[3] && (entries256[address % 256] > 0)) { //T is incorrect
                entries256[address % 256] -= 1;
            }
        }
        if (bitsToPrediction[4] == result) {
            correct512++;
            total512++;
            if (bitsToPrediction[4] && (entries512[address % 512] < 3)) { // T is correct
                entries512[address % 512] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[4] && (entries512[address % 512] > 0)) { // NT is correct
                entries512[address % 512] -= 1;//shift towards strongly not taken
            }
        } else {
            total512++;
            if (!bitsToPrediction[4] && (entries512[address % 512] < 3)) { //NT is incorrect
                entries512[address % 512] += 1;
            } else if (bitsToPrediction[4] && (entries512[address % 512] > 0)) { //T is incorrect
                entries512[address % 512] -= 1;
            }
        }
        if (bitsToPrediction[5] == result) {
            correct1024++;
            total1024++;
            if (bitsToPrediction[5] && (entries1024[address % 1024] < 3)) { // T is correct
                entries1024[address % 1024] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[5] && (entries1024[address % 1024] > 0)) { // NT is correct
                entries1024[address % 1024] -= 1;//shift towards strongly not taken
            }
        } else {
            total1024++;
            if (!bitsToPrediction[5] && (entries1024[address % 1024] < 3)) { //NT is incorrect
                entries1024[address % 1024] += 1;
            } else if (bitsToPrediction[5] && (entries1024[address % 1024] > 0)) { //T is incorrect
                entries1024[address % 1024] -= 1;
            }
        }
        if (bitsToPrediction[6] == result) {
            correct2048++;
            total2048++;
            if (bitsToPrediction[6] && (entries2048[address % 2048] < 3)) { // T is correct
                entries2048[address % 2048] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[6] && (entries2048[address % 2048] > 0)) { // NT is correct
                entries2048[address % 2048] -= 1;//shift towards strongly not taken
            }
        } else {
            total2048++;
            if (!bitsToPrediction[6] && (entries2048[address % 2048] < 3)) { //NT is incorrect
                entries2048[address % 2048] += 1;
            } else if (bitsToPrediction[6] && (entries2048[address % 2048] > 0)) { //T is incorrect
                entries2048[address % 2048] -= 1;
            }
        }
    }
    void print() const{
        std::cout<< correct16 <<"," <<total16 << "; ";
        std::cout<< correct32 <<"," <<total32 << "; ";
        std::cout<< correct128 << ","<<total128 << "; ";
        std::cout<< correct256<< ","<< total256<< "; ";
        std::cout<< correct512<< ","<< total512<< "; ";
        std::cout<< correct1024<< ","<< total1024<< "; ";
        std::cout<< correct2048<< ","<< total2048<< ";" <<std::endl;
    }
    bool getPrediction() {
        return bitsToPrediction[6];
    }
};
//Gshare predictor
class predictor5{
private:
    bool bitsToPrediction[9];
    int entries3b[2048];
    int entries4b[2048];
    int entries5b[2048];
    int entries6b[2048];
    int entries7b[2048];
    int entries8b[2048];
    int entries9b[2048];
    int entries10b[2048];
    int entries11b[2048];
    unsigned int correct3b,total3b, correct4b,total4b, correct5b, total5b, correct6b, total6b, correct7b, total7b, correct8b, total8b, correct9b,total9b, correct10b, total10b, correct11b, total11b = 0;
    unsigned int GHR3, GHR4, GHR5, GHR6, GHR7, GHR8, GHR9, GHR10, GHR11 = 0;
public:
    predictor5(){
        for(int i=0; i < (sizeof(entries3b)/sizeof(int)); i++){
            entries3b[i]=3;
            entries4b[i]=3;
            entries5b[i]=3;
            entries6b[i]=3;
            entries7b[i]=3;
            entries8b[i]=3;
            entries9b[i]=3;
            entries10b[i]=3;
            entries11b[i]=3;
        }
    }
    void predict(unsigned long long addr){
        //set GHRs to have x number of bits
        GHR3= GHR3&0b111;
        GHR4= GHR4&0b1111;
        GHR5= GHR5&0b11111;
        GHR6= GHR6&0b111111;
        GHR7= GHR7&0b1111111;
        GHR8= GHR8&0b11111111;
        GHR9= GHR9&0b111111111;
        GHR10= GHR10&0b1111111111;
        GHR11= GHR11&0b11111111111;
        //index = address xor GHR
        bitsToPrediction[0]= entries3b[(GHR3 ^ (addr % 2048))] > 1;
        bitsToPrediction[1]= entries4b[GHR4 ^ (addr % 2048)] > 1;
        bitsToPrediction[2]= entries5b[GHR5 ^ (addr % 2048)] > 1;
        bitsToPrediction[3]= entries6b[GHR6 ^ (addr % 2048)] > 1;
        bitsToPrediction[4]= entries7b[GHR7 ^ (addr % 2048)] > 1;
        bitsToPrediction[5]= entries8b[GHR8 ^ (addr % 2048)] > 1;
        bitsToPrediction[6]= entries9b[GHR9 ^ (addr % 2048)] > 1;
        bitsToPrediction[7]= entries10b[GHR10 ^ (addr % 2048)] > 1;
        bitsToPrediction[8]= entries11b[GHR11 ^ (addr % 2048)] > 1;
    }
    void update(bool result, unsigned long long addr){
        if (bitsToPrediction[0] == result) {
            total3b++;
            correct3b++;
            if (bitsToPrediction[0] && (entries3b[(GHR3 ^ (addr % 2048))] < 3)) { // T is correct
                entries3b[(GHR3 ^ (addr % 2048))] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[0] && (entries3b[(GHR3 ^ (addr % 2048))] > 0)) { // NT is correct
                entries3b[(GHR3 ^ (addr % 2048))] -= 1;//shift towards strongly not taken
            }
        } else {
            total3b++;
            if (bitsToPrediction[0] && (entries3b[(GHR3 ^ (addr % 2048))] > 0)) { // T is incorrect
                entries3b[(GHR3 ^ (addr % 2048))] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[0] && (entries3b[(GHR3 ^ (addr % 2048))] < 3)) { // NT is incorrect
                entries3b[(GHR3 ^ (addr % 2048))] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[1] == result) {
            total4b++;
            correct4b++;
            if (bitsToPrediction[1] && (entries4b[GHR4 ^ (addr % 2048)] < 3)) { // T is correct
                entries4b[GHR4 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[1] && (entries4b[GHR4 ^ (addr % 2048)] > 0)) { // NT is correct
                entries4b[GHR4 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total4b++;
            if (bitsToPrediction[1] && (entries4b[GHR4 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries4b[GHR4 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[1] && (entries4b[GHR4 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries4b[GHR4 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[2] == result) {
            total5b++;
            correct5b++;
            if (bitsToPrediction[2] && (entries5b[GHR5 ^ (addr % 2048)] < 3)) { // T is correct
                entries5b[GHR5 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[2] && (entries5b[GHR5 ^ (addr % 2048)] > 0)) { // NT is correct
                entries5b[GHR5 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total5b++;
            if (bitsToPrediction[2] && (entries5b[GHR5 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries5b[GHR5 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[2] && (entries5b[GHR5 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries5b[GHR5 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[3] == result) {
            total6b++;
            correct6b++;
            if (bitsToPrediction[3] && (entries6b[GHR6 ^ (addr % 2048)] < 3)) { // T is correct
                entries6b[GHR6 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[3] && (entries6b[GHR6 ^ (addr % 2048)] > 0)) { // NT is correct
                entries6b[GHR6 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total6b++;
            if (bitsToPrediction[3] && (entries6b[GHR6 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries6b[GHR6 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[3] && (entries6b[GHR6 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries6b[GHR6 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[4] == result) {
            total7b++;
            correct7b++;
            if (bitsToPrediction[4] && (entries7b[GHR7 ^ (addr % 2048)] < 3)) { // T is correct
                entries7b[GHR7 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[4] && (entries7b[GHR7 ^ (addr % 2048)] > 0)) { // NT is correct
                entries7b[GHR7 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total7b++;
            if (bitsToPrediction[4] && (entries7b[GHR7 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries7b[GHR7 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[4] && (entries7b[GHR7 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries7b[GHR7 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[5] == result) {
            total8b++;
            correct8b++;
            if (bitsToPrediction[5] && (entries8b[GHR8 ^ (addr % 2048)] < 3)) { // T is correct
                entries8b[GHR8 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[5] && (entries8b[GHR8 ^ (addr % 2048)] > 0)) { // NT is correct
                entries8b[GHR8 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total8b++;
            if (bitsToPrediction[5] && (entries8b[GHR8 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries8b[GHR8 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[5] && (entries8b[GHR8 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries8b[GHR8 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[6] == result) {
            total9b++;
            correct9b++;
            if (bitsToPrediction[6] && (entries9b[GHR9 ^ (addr % 2048)] < 3)) { // T is correct
                entries9b[GHR9 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[6] && (entries9b[GHR9 ^ (addr % 2048)] > 0)) { // NT is correct
                entries9b[GHR9 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total9b++;
            if (bitsToPrediction[6] && (entries9b[GHR9 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries9b[GHR9 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[6] && (entries9b[GHR9 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries9b[GHR9 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[7] == result) {
            total10b++;
            correct10b++;
            if (bitsToPrediction[7] && (entries10b[GHR10 ^ (addr % 2048)] < 3)) { // T is correct
                entries10b[GHR10 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[7] && (entries10b[GHR10 ^ (addr % 2048)] > 0)) { // NT is correct
                entries10b[GHR10 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total10b++;
            if (bitsToPrediction[7] && (entries10b[GHR10 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries10b[GHR10 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[7] && (entries10b[GHR10 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries10b[GHR10 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }
        if (bitsToPrediction[8] == result) {
            total11b++;
            correct11b++;
            if (bitsToPrediction[8] && (entries11b[GHR11 ^ (addr % 2048)] < 3)) { // T is correct
                entries11b[GHR11 ^ (addr % 2048)] += 1;//shift towards strongly taken
            } else if (!bitsToPrediction[8] && (entries11b[GHR11 ^ (addr % 2048)] > 0)) { // NT is correct
                entries11b[GHR11 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            }
        } else {
            total11b++;
            if (bitsToPrediction[8] && (entries11b[GHR11 ^ (addr % 2048)] > 0)) { // T is incorrect
                entries11b[GHR11 ^ (addr % 2048)] -= 1;//shift towards strongly not taken
            } else if (!bitsToPrediction[8] && (entries11b[GHR11 ^ (addr % 2048)] < 3)) { // NT is incorrect
                entries11b[GHR11 ^ (addr % 2048)] += 1;//shift towards strongly taken
            }
        }

        //GHR UPDATE
        if(result){
            GHR3= (GHR3<<1) + 1;
            GHR4= (GHR4<<1) + 1;
            GHR5= (GHR5<<1) + 1;
            GHR6= (GHR6<<1) + 1;
            GHR7= (GHR7<<1) + 1;
            GHR8= (GHR8<<1) + 1;
            GHR9= (GHR9<<1) + 1;
            GHR10= (GHR10<<1) + 1;
            GHR11= (GHR11<<1) + 1;
        } else {
            GHR3= GHR3<<1;
            GHR4= GHR4<<1;
            GHR5= GHR5<<1;
            GHR6= GHR6<<1;
            GHR7= GHR7<<1;
            GHR8= GHR8<<1;
            GHR9= GHR9<<1;
            GHR10= GHR10<<1;
            GHR11= GHR11<<1;
        }
    }
    void print() const {
        std::cout<< correct3b <<","<< total3b<<"; ";
        std::cout<< correct4b <<","<< total4b<<"; ";
        std::cout<< correct5b <<","<< total5b<<"; ";
        std::cout<< correct6b <<","<< total6b<<"; ";
        std::cout<< correct7b <<","<< total7b<<"; ";
        std::cout<< correct8b <<","<< total8b<<"; ";
        std::cout<< correct9b <<","<< total9b<<"; ";
        std::cout<< correct10b <<","<< total10b<<"; ";
        std::cout<< correct11b <<","<< total11b<<";" <<std::endl;
    }

    bool getPrediction() {
        return bitsToPrediction[8];
    }
};
//Tournament Predictor
class predictor6{
private:
    int selector[2048] = {0};
    predictor4* bimodal;
    predictor5* gshare;
    bool prediction;
    bool bimodal_prediction;
    bool gshare_prediction;
    int correct, total=0;
public:
    predictor6() {
        bimodal= new predictor4();
        gshare= new predictor5();
    }
    ~predictor6() {
        delete bimodal;
        delete gshare;
    }
    void predict(unsigned long long addr){
        bimodal->predict(addr);
        gshare->predict(addr);
        bimodal_prediction= bimodal->getPrediction();
        gshare_prediction= gshare->getPrediction(); //11 bit table
        //00,01 = gShare; 10,11 = bimodal
        prediction= selector[addr % 2048] > 1 ? bimodal_prediction : gshare_prediction;
    }
    void update(bool result, unsigned long long addr){
        gshare->update(result, addr);
        bimodal->updateTables(result, addr);
        if(prediction == result){
            correct++; total++;
        } else {
            total++;
        }
        if(bimodal_prediction==gshare_prediction){
            return; //both right or both wrong so do nothing
        } else if(result==gshare_prediction && (selector[addr % 2048] > 0)){ // gShare right and bimodal wrong
            selector[addr % 2048]-=1;
        } else if(result==bimodal_prediction && (selector[addr % 2048] < 3)){ // bimodal right and gShare wrong
            selector[addr % 2048]+=1;
        }
    }
    void print() const{
        std::cout<< correct<< ","<<total<< ";"<< std::endl;
    }
};
//Branch Target Buffer
class predictor7{
private:
    predictor3* bimodal;
    unsigned long long targetBuffer[512];
    int correct, total = 0;
    bool bimodal_prediction;
    unsigned long long targetPrediction;
public:
    predictor7(){
        bimodal= new predictor3();
        for(unsigned long long & i : targetBuffer){
            i=0;
        }
    }
    ~predictor7(){
        delete bimodal;
    }
    void predict(bool result, unsigned long long addr){
        bimodal_prediction= bimodal->getPrediction(addr);
        if(bimodal_prediction){
            total++;
            targetPrediction= targetBuffer[addr % 512]; //since T prediction, get predicted target
        } else {
            targetPrediction= addr + 4; // NT prediction, increment PC once
        }

    }
    void update(bool result, unsigned long long addr, unsigned long long target){
        bimodal->predictUpdate(result, addr);
        if(result){
            targetBuffer[addr % 512] = target; //update with expected target if T is correct
        }
        if(bimodal_prediction && (targetPrediction == target)) {
            correct++;
        }
    }
    void print() const{
        std::cout << correct << "," << total << ";" << std::endl;
    }
};

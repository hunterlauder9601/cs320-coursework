#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

class directMapping {
    private:
        int hit = 0;
        int access = 0;
        int cacheSize;
        array<long long, 1024> cache; //1024 entries for largest cache of 32 KB
    public:
        directMapping(int s) {
            cache.fill(-1); //-1 marks invalid entry/garbage
            cacheSize = s;
        }
        void directMap(unsigned long long addr){
            int numCacheBlocks = cacheSize / 32; //cache block size = 32 bytes
            long long blockAddress = addr >> 5; //shift out byte offset bits
            long long tag = blockAddress >> int(log2(numCacheBlocks)); //gets tag by shifting out index bits
            int blockIndex = blockAddress % numCacheBlocks;
            if (cache[blockIndex] == -1){ //cold start miss
                cache[blockIndex] = tag;
            } else if (cache[blockIndex] == tag){ //hit
                hit++;
            } else { //miss
                cache[blockIndex] = tag;
            }
            access++;
        }
        void print() const {
            if(cacheSize == 32768) {
                cout << hit << "," << access << ";" << endl;
            } else {
                cout << hit << "," << access << "; ";
            }
        }
};
class setAssociativeMapping {
private:
    int hit = 0;
    int access = 0;
    int counter = 0;
    int ways;
    array<long long, 512> cache; //512 entries for cache of 16 KB
    array<int, 512> accesses;
public:
    setAssociativeMapping(int inWays) {
        cache.fill(-1); //-1 marks invalid entry/garbage
        accesses.fill(0);
        ways = inWays;
    }
    void setAssociativeLRU(unsigned long long addr){
        int sets = 512 / ways; //(16KB / 32) / ways
        long long blockAddress = addr >> 5; //shift out byte offset bits
        long long tag = blockAddress >> int(log2(sets)); //gets tag by shifting out index bits
        int setIndex = blockAddress % sets;
        int cacheIndex = setIndex * ways;
        bool present = false; //cache block containing addr is present in cache
        int LRU = cacheIndex;
        access++;
        for (int i = 0; i < ways && !present; i++){
            if (cache[cacheIndex] == -1){ //cold start miss
                cache[cacheIndex] = tag;
                accesses[cacheIndex] = access;
                present = true;
            } else if (cache[cacheIndex] == tag){ //hit
                accesses[cacheIndex] = access;
                present = true;
                hit++;
            }
            if (accesses[cacheIndex] < accesses[LRU]){
                LRU = cacheIndex; // new LRU block
            }
            cacheIndex++; //next way if not present in cache yet
        }
        if (!present){ //conflict miss
            cache[LRU] = tag;
            accesses[LRU] = access; //new most recently used
        }
    }
    void setAssociativeNoAllocOnWriteMiss(unsigned long long addr, bool isLoad) {
        int sets = 512 / ways; //(16KB / 32) / ways
        long long blockAddress = addr >> 5; //shift out byte offset bits
        long long tag = blockAddress >> int(log2(sets)); //gets tag by shifting out index bits
        int setIndex = blockAddress % sets;
        int cacheIndex = setIndex * ways;
        bool present = false; //cache block containing addr is present in cache
        int LRU = cacheIndex;
        access++;
        for (int i = 0; i < ways && !present; i++){
            if (isLoad && cache[cacheIndex] == -1){ //has to be cold start miss and a load instruction
                cache[cacheIndex] = tag;
                accesses[cacheIndex] = access;
                present = true;
            } else if (cache[cacheIndex] == tag){ //hit
                accesses[cacheIndex] = access;
                present = true;
                hit++;
            }
            if (accesses[cacheIndex] < accesses[LRU]){
                LRU = cacheIndex; // new LRU block
            }
            cacheIndex++; //next way if not present in cache yet
        }
        if (!present){ //conflict miss
            if (isLoad){ //has to be a load instruction
                cache[LRU] = tag;
                accesses[LRU] = access; //new most recently used
            }
        }
    }
    void setAssociativePrefetch(unsigned long long addr){
        int sets = 512 / ways; //(16KB / 32) / ways
        long long blockAddress = addr >> 5; //shift out byte offset bits
        long long tag = blockAddress >> int(log2(sets)); //gets tag by shifting out index bits
        int setIndex = blockAddress % sets;
        int cacheIndex = setIndex * ways;
        bool present = false; //cache block containing addr is present in cache
        int LRU = cacheIndex;
        access++;
        counter++; //counts accesses + prefetch accesses
        for (int i = 0; i < ways && !present; i++){
            if (cache[cacheIndex] == -1){ //cold start miss
                cache[cacheIndex] = tag;
                accesses[cacheIndex] = counter;
                present = true;
            }
            else if (cache[cacheIndex] == tag){ //hit
                hit++;
                accesses[cacheIndex] = counter;
                present = true;
            }
            if (accesses[cacheIndex] < accesses[LRU]){
                LRU = cacheIndex;
            }
            cacheIndex++; //next way if not present in cache yet
        }
        if (!present){ //conflict miss
            cache[LRU] = tag; //replaced
            accesses[LRU] = counter; //new most recently used
        }
        //prefetch
        blockAddress++; //next line
        tag = blockAddress >> (int(log2(sets)));
        setIndex = blockAddress % sets;
        cacheIndex = setIndex * ways;
        present = false;
        LRU = cacheIndex;
        counter++;
        for (int i = 0; i < ways && !present; i++){
            if (cache[cacheIndex] == -1){ //cold start miss
                cache[cacheIndex] = tag;
                accesses[cacheIndex] = counter;
                present = true;
            }
            else if (cache[cacheIndex] == tag){ //hit
                accesses[cacheIndex] = counter;
                present = true;
            }
            if (accesses[cacheIndex] < accesses[LRU]){
                LRU = cacheIndex;
            }
            cacheIndex++; //next way if not present in cache yet
        }
        if(!present) { //conflict miss
            cache[LRU] = tag; //replaced
            accesses[LRU] = counter; //new most recently used
        }
    }
    void setAssociativePrefetchOnMiss(unsigned long long addr){
        int sets = 512 / ways; //(16KB / 32) / ways
        long long blockAddress = addr >> 5; //shift out byte offset bits
        long long tag = blockAddress >> int(log2(sets)); //gets tag by shifting out index bits
        int setIndex = blockAddress % sets;
        int cacheIndex = setIndex * ways;
        bool present = false; //cache block containing addr is present in cache
        bool miss = false;
        int LRU = cacheIndex;
        access++;
        counter++; //counts accesses + prefetch accesses
        for (int i = 0; i < ways && !present; i++){
            if (cache[cacheIndex] == -1){ //cold start miss
                cache[cacheIndex] = tag;
                accesses[cacheIndex] = counter;
                present = true;
                miss = true;
            }
            else if (cache[cacheIndex] == tag){ //hit
                hit++;
                accesses[cacheIndex] = counter;
                present = true;
            }
            if (accesses[cacheIndex] < accesses[LRU]){
                LRU = cacheIndex;
            }
            cacheIndex++; //next way if not present in cache yet
        }
        if (!present){ //conflict miss
            cache[LRU] = tag; //replaced
            accesses[LRU] = counter; //new most recently used
            miss = true;
        }
        //prefetch
        if(miss) { //prefetch only if it was a miss
            blockAddress++; //next line
            tag = blockAddress >> (int(log2(sets)));
            setIndex = blockAddress % sets;
            cacheIndex = setIndex * ways;
            present = false;
            LRU = cacheIndex;
            counter++;
            for (int i = 0; i < ways && !present; i++){
                if (cache[cacheIndex] == -1){ //cold start miss
                    cache[cacheIndex] = tag;
                    accesses[cacheIndex] = counter;
                    present = true;
                }
                else if (cache[cacheIndex] == tag){ //hit
                    accesses[cacheIndex] = counter;
                    present = true;
                }
                if (accesses[cacheIndex] < accesses[LRU]){
                    LRU = cacheIndex;
                }
                cacheIndex++; //next way if not present in cache yet
            }
            if(!present) { //conflict miss
                cache[LRU] = tag; //replaced
                accesses[LRU] = counter; //new most recently used
            }
        }
    }
    void print() const {
        if(ways == 16) {
            cout << hit << "," << access << ";" << endl;
        } else {
            cout << hit << "," << access << "; ";
        }
    }
};
class fullyAssociativeMapping {
private:
    int hit = 0;
    int access = 0;
    array<long long, 512> cache; //1024 entries for largest cache of 16 KB
    array<int, 512> accesses;
    array<int, 511> hotCold; //binary tree, #bits = (num of ways - 1) num of sets

public:
    fullyAssociativeMapping() {
        cache.fill(-1); //-1 marks invalid entry/garbage
        accesses.fill(0);
        hotCold.fill(0);
    }
    void fullyAssociativeLRU(unsigned long long addr){
        long long tag = addr >> 5; //5 leftmost bits reserved for byte offset
        bool present = false;
        int LRU = 0;
        access++;
        for (unsigned int i = 0; i < cache.size() && !present; i++){ //iteratively goes through each cache line
            if (cache[i] == -1) { //cold start miss
                cache[i] = tag;
                accesses[i] = access;
                present = true;
            } else if (cache[i] == tag) { //hit
                hit++;
                accesses[i] = access;
                present = true;
            }
            if (accesses[i] < accesses[LRU]) {
                LRU = i; //new LRU block
            }
        }
        if (!present) { //conflict miss
            cache[LRU] = tag;
            accesses[LRU] = access;
        }
    }
    void fullyAssociativePLRU(unsigned long long addr){
        long long tag = addr >> 5; //5 leftmost bits reserved for byte offset
        int treeIndex;
        bool present = false;
        access++;
        for (unsigned int i = 0; i < cache.size(); i++) { //iteratively goes through each cache line
            if (tag == cache[i]) {
                hit++;
                treeIndex = i + cache.size() - 1; //translate cache index, i, to tree index
                while (treeIndex != 0) {
                    if (treeIndex % 2 == 0) { //right child node
                        treeIndex = (treeIndex - 2) / 2; //get parent node
                        hotCold[treeIndex] = 0; //left side is now cold
                    }
                    else { //left child node
                        treeIndex = (treeIndex - 1) / 2; //get parent node
                        hotCold[treeIndex] = 1; //right side is now cold
                    }
                }
                present = true;
                break;
            }
        }
        if (!present) { //tag not in cache
            treeIndex = 0; //init to root node
            int level = 0; //init to root level
            while (level < 9) { //stop iteration before reaching leaf nodes
                level++;
                if (hotCold[treeIndex] == 0) { // if current node is 0
                    hotCold[treeIndex] = 1; //set it to 1 for future traversals since cold side is going to flip after placement
                    treeIndex = 2 * treeIndex + 1; //set index to left child node
                }
                else {
                    hotCold[treeIndex] = 0; //set it to 0 for future traversals since cold side is going to flip after placement
                    treeIndex = 2 * treeIndex + 2; //set index to right child node
                }
            }
            cache[treeIndex - cache.size() + 1] = tag; //translation to cache index where replacement occurs
        }
    }
    void print() const {
        cout << hit << "," << access << ";" << endl;
    }
};
int main(int argc, char* argv[]){
    if(argc!=3){
        cout<< "Use command format: ./cache-sim <inputfile> <outputfile>" <<endl;
        return -1;
    }

    auto* direct1KB = new directMapping(1024);
    auto* direct4KB = new directMapping(4096);
    auto* direct16KB = new directMapping(16384);
    auto* direct32KB = new directMapping(32768);

    auto* setAssociative2ways = new setAssociativeMapping(2);
    auto* setAssociative4ways = new setAssociativeMapping(4);
    auto* setAssociative8ways = new setAssociativeMapping(8);
    auto* setAssociative16ways = new setAssociativeMapping(16);

    auto* fullyAssociativeLRU = new fullyAssociativeMapping();
    auto* fullyAssociativepLRU = new fullyAssociativeMapping();

    auto* setAssociativeNoAllocOnWriteMiss_2 = new setAssociativeMapping(2);
    auto* setAssociativeNoAllocOnWriteMiss_4 = new setAssociativeMapping(4);
    auto* setAssociativeNoAllocOnWriteMiss_8 = new setAssociativeMapping(8);
    auto* setAssociativeNoAllocOnWriteMiss_16 = new setAssociativeMapping(16);

    auto* setAssociativePrefetch_2 = new setAssociativeMapping(2);
    auto* setAssociativePrefetch_4 = new setAssociativeMapping(4);
    auto* setAssociativePrefetch_8 = new setAssociativeMapping(8);
    auto* setAssociativePrefetch_16 = new setAssociativeMapping(16);

    auto* setAssociativePrefetchOnMiss_2 = new setAssociativeMapping(2);
    auto* setAssociativePrefetchOnMiss_4 = new setAssociativeMapping(4);
    auto* setAssociativePrefetchOnMiss_8 = new setAssociativeMapping(8);
    auto* setAssociativePrefetchOnMiss_16 = new setAssociativeMapping(16);


    unsigned long long addr;
    string instrType, line;
    ifstream inFile;
    bool isLoad;
    inFile.open(argv[1]);
    if (inFile.fail()) {
        cerr << "Could not find file" << endl;
    }
    // The following loop will read a line at a time
    while(getline(inFile, line)) {
        stringstream s(line);
        s >> instrType >> hex >> addr;

        if(instrType == "L") {
            isLoad = true;
        } else {
            isLoad = false;
        }

        direct1KB->directMap(addr);
        direct4KB->directMap(addr);
        direct16KB->directMap(addr);
        direct32KB->directMap(addr);

        setAssociative2ways->setAssociativeLRU(addr);
        setAssociative4ways->setAssociativeLRU(addr);
        setAssociative8ways->setAssociativeLRU(addr);
        setAssociative16ways->setAssociativeLRU(addr);

        fullyAssociativeLRU->fullyAssociativeLRU(addr);
        fullyAssociativepLRU->fullyAssociativePLRU(addr);

        setAssociativeNoAllocOnWriteMiss_2->setAssociativeNoAllocOnWriteMiss(addr, isLoad);
        setAssociativeNoAllocOnWriteMiss_4->setAssociativeNoAllocOnWriteMiss(addr, isLoad);
        setAssociativeNoAllocOnWriteMiss_8->setAssociativeNoAllocOnWriteMiss(addr, isLoad);
        setAssociativeNoAllocOnWriteMiss_16->setAssociativeNoAllocOnWriteMiss(addr, isLoad);

        setAssociativePrefetch_2->setAssociativePrefetch(addr);
        setAssociativePrefetch_4->setAssociativePrefetch(addr);
        setAssociativePrefetch_8->setAssociativePrefetch(addr);
        setAssociativePrefetch_16->setAssociativePrefetch(addr);

        setAssociativePrefetchOnMiss_2->setAssociativePrefetchOnMiss(addr);
        setAssociativePrefetchOnMiss_4->setAssociativePrefetchOnMiss(addr);
        setAssociativePrefetchOnMiss_8->setAssociativePrefetchOnMiss(addr);
        setAssociativePrefetchOnMiss_16->setAssociativePrefetchOnMiss(addr);

    }
    ofstream out(argv[2]);
    cout.rdbuf(out.rdbuf()); //redirects output to file
    direct1KB->print();
    direct4KB->print();
    direct16KB->print();
    direct32KB->print();

    setAssociative2ways->print();
    setAssociative4ways->print();
    setAssociative8ways->print();
    setAssociative16ways->print();

    fullyAssociativeLRU->print();
    fullyAssociativepLRU->print();

    setAssociativeNoAllocOnWriteMiss_2->print();
    setAssociativeNoAllocOnWriteMiss_4->print();
    setAssociativeNoAllocOnWriteMiss_8->print();
    setAssociativeNoAllocOnWriteMiss_16->print();

    setAssociativePrefetch_2->print();
    setAssociativePrefetch_4->print();
    setAssociativePrefetch_8->print();
    setAssociativePrefetch_16->print();

    setAssociativePrefetchOnMiss_2->print();
    setAssociativePrefetchOnMiss_4->print();
    setAssociativePrefetchOnMiss_8->print();
    setAssociativePrefetchOnMiss_16->print();

    return 0;
}

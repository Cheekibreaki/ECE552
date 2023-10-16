#include "predictor.h"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <cassert>

void printBinary(UINT32 num, int print_num) {
    for (int i = print_num-1; i >= 0; i--) {  // Assuming a 32-bit integer
        printf("%d", (num & (1 << i)) ? 1 : 0);
        if (i % 4 == 0 && i != 0) {  // Optional: Add space every 4 bits for better readability
            printf(" ");
        }
    }
    printf("\n");
}

/////////////////////////////////////////////////////////////
// 2bitsat
/////////////////////////////////////////////////////////////
#define TAGSIZE 12
#define BPTROW 4096 // 2-bit Branch Perdictor Table Size

static int _2bitPerdT[BPTROW]; // 2-bit Branch Perdictor Table
// NN: 0 Strong Not Taken
// NT: 1 Weak Not Taken (Use as Initial Value)
// TN: 2 Weak Taken
// TT: 3 Strong Taken

void InitPredictor_2bitsat() {
  for(int i=0; i< BPTROW; i++){
    _2bitPerdT[i] = 1;
  }
}

void _12bitMask(UINT32 & PC){
  PC = PC & ((UINT32)BPTROW-1);
}

bool GetPrediction_2bitsat(UINT32 PC) {
  _12bitMask(PC);
  if(_2bitPerdT[PC] > 1)
    return TAKEN;
  else
    return NOT_TAKEN;
}

void UpdatePredictor_2bitsat(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  _12bitMask(PC);

  if(resolveDir == NOT_TAKEN && predDir == TAKEN){
    _2bitPerdT[PC]--;
  }
  if(resolveDir == TAKEN && predDir == NOT_TAKEN){
    _2bitPerdT[PC]++;
  }
  if(resolveDir == TAKEN && predDir == TAKEN && _2bitPerdT[PC] < 3){
    _2bitPerdT[PC]++;
  }
  if(resolveDir == NOT_TAKEN && predDir == NOT_TAKEN && _2bitPerdT[PC] > 0){
    _2bitPerdT[PC]--;
  }
}

/////////////////////////////////////////////////////////////
// 2level
/////////////////////////////////////////////////////////////
#undef BPTROW

#define BHTTAGSIZE 9
#define BHTSIZE 512 // 2^9
#define BHRSIZE 6

#define BPTTAGSIZE 3
#define BPTSIZE 8 // 2^3
#define BPTROW 64 // 2^6

static int _6bitBHT[BHTSIZE]; // use only first 6 bit, inialize with NNNNNN
static int _2bitBPT[BPTSIZE][BPTROW]; // use only first 2 bit

void _12bitTagSplit(UINT32 PC, UINT32 & _9bitTag, UINT32 & _3bitTag){
  _3bitTag = PC & ((1 << BPTTAGSIZE) - 1);
  _9bitTag = (PC >> BPTTAGSIZE) & ((1 << BHTTAGSIZE) - 1);
}

void InitPredictor_2level() { 
  for(int i=0; i< BPTSIZE; i++){
    for(int j=0; j< BPTROW; j++){
      _2bitBPT[i][j] = 1;
    }
  }
}

bool GetPrediction_2level(UINT32 PC) {
  UINT32 _9bitTag, _3bitTag;
  _12bitTagSplit(PC, _9bitTag, _3bitTag);

  if(_2bitBPT[_3bitTag][_6bitBHT[_9bitTag]] > 1)
    return TAKEN;
  else
    return NOT_TAKEN;
}

void UpdatePredictor_2level(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  UINT32 _9bitTag, _3bitTag;
  _12bitTagSplit(PC, _9bitTag, _3bitTag);

  int* _2bitPtr = &(_2bitBPT[_3bitTag][_6bitBHT[_9bitTag]]);

  if(resolveDir == NOT_TAKEN && predDir == TAKEN){
    (*_2bitPtr)--;
  }
  if(resolveDir == TAKEN && predDir == NOT_TAKEN){
    (*_2bitPtr)++;
  }
  if(resolveDir == TAKEN && predDir == TAKEN && *_2bitPtr < 3){
    (*_2bitPtr)++;
  }
  if(resolveDir == NOT_TAKEN && predDir == NOT_TAKEN && *_2bitPtr > 0){
    (*_2bitPtr)--;
  }

  _6bitBHT[_9bitTag] = (_6bitBHT[_9bitTag] << 1);
  _6bitBHT[_9bitTag] = _6bitBHT[_9bitTag] & (BPTROW - 1);
  _6bitBHT[_9bitTag] |= (UINT32)resolveDir;


  return;
}

/////////////////////////////////////////////////////////////
// openend
/////////////////////////////////////////////////////////////
#define GHRPATCH 32
#define GHRMAXHIST 256
#define PBTSIZE 8

typedef uint32_t HASHVAL;

static HASHVAL HashAddress[PBTSIZE] = {0};                        // Connection between GHT and BPT
static int HistoryLength [PBTSIZE] = {0,2,4,8,16,32,64,128};      // Take short OR long History to hash
static int AddressLength [PBTSIZE] = {11,11,12,12,13,13,14,14};   // Length of address for each subtable
static int NBitCounter [PBTSIZE] = {5,5,5,5,5,5,5,5};

/*           Global History Table              */
struct GHR {
  int history_counter;
  int max_history;
  std::vector<HASHVAL> data;

  GHR(){
      history_counter = 0;
      max_history = GHRMAXHIST;
      int count = GHRMAXHIST / GHRPATCH;
      for(int i = 0; i < count; i++){
        data.push_back(0);
      }
  }
  void get_GHRAddr(HASHVAL PC/*, path history */){
    for (int i=0; i<PBTSIZE; i++){
      HashAddress[i] = get_Hash(AddressLength[i], PC, HistoryLength[i]);
    }
  }
  void update_GHR(bool resolveDir){
    int max = history_counter / GHRPATCH + (history_counter % GHRPATCH > 0);

    for(int lastBit = resolveDir, tmpBit, i = 0; i < max; i++){
      tmpBit = lastBit;
      lastBit = (data[i] >> (GHRPATCH-1)) & 1;
      data[i] = (data[i] << 1) | tmpBit;
    }

    if(history_counter < max_history)
      history_counter++;
  }

  HASHVAL get_HistoryFold(int n, int historyLength){
    if(historyLength < n){
      return data[0] & ((1 << n) - 1);
    }
    HASHVAL res = 0;

    int patch = historyLength / GHRPATCH; // assume the modulo is always 0
    for (int i = 0; i< patch; i++){
      res ^= data[i];
    }
    //based on assumption, don't know if this thing is needed or not
    // res ^= data[patch - 1] | ((1 << historyLength % GHRPATCH) - 1);
    return res % (1<<n);
  }
  HASHVAL get_Hash(int n, HASHVAL PC, int historyLength /*, path history*/){
    // fold History Address into n bit
    HASHVAL historyFold = get_HistoryFold(n, historyLength);

    // get n least siginificant bit from PC
    HASHVAL PCFold = PC & ((1 << n) - 1);

    // get 3n bit composed with least siginaficant bit of PC, ghr & path history
    return PCFold ^ historyFold;
  }
};

/*           Branch Prediction Table              */
struct Counter {
  int data;
  const int initialized_value = 0;
  int saturate;
  
  Counter(){
    data = 0;
    saturate = 0;
  }
  Counter(int counter_bitsize){
    data = initialized_value;
    saturate = (1<<counter_bitsize);
  }
  void increment_Counter(){
    if(data<saturate - 1)
      data++;
  }
  void decrement_Counter(){
    if(data>-saturate)
      data--;
  }
};

struct Subtable {
  int historyLength; // For Debugging Propose
  int rowNum;
  int counter_bitsize;
  std::vector<Counter> data; // Dynamically allocated data array

  Subtable(){
    historyLength = 0;
    rowNum = 0;
    counter_bitsize = 0;
  }

  Subtable(int _historyLen, int _rowNum, int _counter_bitsize) :
    historyLength(_historyLen), rowNum(_rowNum),
    counter_bitsize(_counter_bitsize){
      int count = 1 << rowNum;
      for(int i = 0; i < count; i++){
        data.push_back(Counter(_counter_bitsize));
      }
  }
};

struct PBT{
  int theta = PBTSIZE;
  int sigma = 0;
  std::vector<Subtable> PBT_table;

  PBT() {
    for(int i=0;i<PBTSIZE;i++){
      PBT_table.push_back(Subtable(HistoryLength[i],AddressLength[i],NBitCounter[i]));
    }
  }
  void update_PBT(bool resolveDir, bool predDir){
    if(predDir != resolveDir || abs(sigma) <= theta){
      for(int i = 0; i<PBTSIZE; i++){
        Subtable& sub_table = PBT_table[i];
        HASHVAL hashIndex = HashAddress[i];
        assert(hashIndex >= 0 && hashIndex < sub_table.data.size());
        Counter& bit_counter = sub_table.data[hashIndex];

        if(resolveDir)
          bit_counter.increment_Counter();
        else
          bit_counter.decrement_Counter();
      }
    }
  }
  int get_NT_T(){ 
    int i;
    sigma = 0;
    for(i = 0; i<PBTSIZE; i++){
      Subtable& sub_table = PBT_table[i];
      HASHVAL hashIndex = HashAddress[i];
      assert(hashIndex >= 0 && hashIndex < sub_table.data.size());
      Counter& prediction = sub_table.data[hashIndex];

      sigma += prediction.data;
    }
    sigma += PBTSIZE / 2;

    if(sigma >= 0)
      return TAKEN;
    else
      return NOT_TAKEN;
  }
};
struct GEHL {
  GHR ghr;
  PBT pbt;

  GEHL(){
    pbt = PBT();
    ghr = GHR();
  }
};

static GEHL gehlObject;

void InitPredictor_openend() {
  gehlObject = GEHL();
}

bool GetPrediction_openend(UINT32 PC) {
  gehlObject.ghr.get_GHRAddr(PC);
  return gehlObject.pbt.get_NT_T();
}

void UpdatePredictor_openend(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget) {
  // Value related with PC already stored in HashAddress[], no need to passin
  gehlObject.ghr.update_GHR(resolveDir);
  gehlObject.pbt.update_PBT(resolveDir, predDir);
}

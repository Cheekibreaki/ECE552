#include "predictor.h"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <cassert>
#include <string.h>

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
#define PHROFFSET 0

typedef uint32_t HASHVAL; 

static HASHVAL HashAddress[PBTSIZE] = {0};                        // Connection between GHT and BPT
static int HistoryLength [PBTSIZE] = {0,4,8,16,32,64,128,256};      // Take short OR long History to hash
static int AddressLength [PBTSIZE] = {6,7,8,10,11,12,13,13};   // Length of address for each subtable
static int NBitCounter [PBTSIZE] = {4,4,5,5,5,5,5,5};

// Dynamic History Length Tag
// #define FREQ 8
// static HASHVAL GHTag[1024] = {0};
// static int LongHistory [8] = {0,256,160,8,192,32,384,128};
// static int ShortHistory[8] = {0,256,4,8,16,32,64,128};

// Path History Register bit length
#define PHRSIZE 16

/*           Global History Table              */
struct GHR {
  int history_counter; // set to 0 initially, used to update GHR
  int max_history; // GHR length, this case 256 bits
  std::vector<HASHVAL> data; // Our GHR is 256 bits, we use a vector to represent it, with each vector value type HASHVAL which is 32bit 
  uint16_t phr; // Path history register

  //constructor
  GHR(){
      history_counter = 0;
      max_history = GHRMAXHIST;
      int count = GHRMAXHIST / GHRPATCH;
      for(int i = 0; i < count; i++){
        data.push_back(0);
      }

      phr = 0;
  }

  //Based on PC, Find the Index address for 8 tables though hashing
  void get_GHRAddr(HASHVAL PC){
    for (int i=0; i<PBTSIZE; i++){
      HashAddress[i] = get_Hash(AddressLength[i], PC, HistoryLength[i]);
    }
  }

  //Update GHR
  void update_GHR_PHR(bool resolveDir, uint32_t PC){
    int max = history_counter / GHRPATCH + (history_counter % GHRPATCH > 0);

    for(int lastBit = resolveDir, tmpBit, i = 0; i < max; i++){
      tmpBit = lastBit;
      lastBit = (data[i] >> (GHRPATCH-1)) & 1;
      data[i] = (data[i] << 1) | tmpBit;
    }

    if(history_counter < max_history)
      history_counter++;

    // update phr
    phr <<= 3;
    phr |= ((PC << PHROFFSET) & 0b11); 
  }

  //To fold, XOR and then mod to index Address size n
  HASHVAL get_HistoryFold(int n, int historyLength){
    if(historyLength < n){
      return data[0] & ((1 << n) - 1);
    }
    HASHVAL res = 0;

    int patch = historyLength / GHRPATCH; // assume the modulo is always 0
    for (int i = 0; i< patch; i++){
      res ^= data[i];
    }
    return res % (1<<n);
  }

  //Based on PC, Path History Register and GHR, 
  //Fold GHR to index Address size n, and XOR with least n significant bits of PHR and GHR
  HASHVAL get_Hash(int n, HASHVAL PC, int historyLength){
    // fold History Address into n bit
    HASHVAL historyFold = get_HistoryFold(n, historyLength);

    // get n least siginificant bit from PC
    HASHVAL PCFold = PC & ((1 << n) - 1);

    if(n > PHRSIZE || n == 0)
      return PCFold ^ historyFold;
    else
      // get 3n bit composed with least siginaficant bit of PC, ghr & path history
      return PCFold ^ historyFold ^ (phr % (1<<n));
  }
};

/*           Branch Prediction Table              */
// Saturated Counter Implemtation
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

// Table T(i), include Row # of Saturated Counters
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

// Contains Multiple Table T(i), with Threshold Calculator
struct PBT{
  int theta = PBTSIZE;
  int sigma = 0;
  std::vector<Subtable> PBT_table;
  Counter* AdptThres;
  Counter* AC;

  PBT() {
    for(int i=0;i<PBTSIZE;i++){
      PBT_table.push_back(Subtable(HistoryLength[i],AddressLength[i],NBitCounter[i]));
    }

    AdptThres = new Counter(7);
    AC = new Counter(9);
  }
  void update_PBT(bool resolveDir, bool predDir, UINT32 PC){
    // Counter Update based on resolveDir and predDir
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

    // Dynamic Threshold
    if(predDir != resolveDir){
      AdptThres->increment_Counter();
      if(AdptThres->data == AdptThres->saturate - 1){
        theta++;
        AdptThres->data = 0;
      }
    }
    if(predDir == resolveDir && abs(sigma) <= theta){
      AdptThres->decrement_Counter();
      if(AdptThres->data == -AdptThres->saturate){
        theta--;
        AdptThres->data = 0;
      }
    }

    // Dyanmic History Length
    // if ((HashAddress[PBTSIZE - 1] % FREQ) != 0) return;
    // int access = HashAddress[PBTSIZE - 1] / FREQ;

    // if((predDir != resolveDir) && abs(sigma) <= theta){
    //   if((PC & 1) == GHTag[access])
    //     AC->increment_Counter();
    //   else{
    //     AC->decrement_Counter();
    //     AC->decrement_Counter();
    //     AC->decrement_Counter();
    //     AC->decrement_Counter();
    //   }
    //   if(AC->data == (AC->saturate)-1)
    //     memcpy(HistoryLength, LongHistory, sizeof(HistoryLength));
      
    //   if(AC->data == -(AC->saturate))
    //     memcpy(HistoryLength, ShortHistory, sizeof(HistoryLength));

    //   GHTag[access] = (PC & 1);
    // }
  }

  // Calculate the Prediction using all Table T(i) P = PBTSIZE/2 + SUM(Counter(i))
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
  gehlObject.ghr.update_GHR_PHR(resolveDir, PC);
  gehlObject.pbt.update_PBT(resolveDir, predDir, PC);
}

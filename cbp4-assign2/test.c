#include <iostream>
#include <vector>
#define GHRPATCH 32
#define GHRMAXHIST 256

struct GHR {
  int history_counter;
  int max_history;
  std::vector<uint32_t> DataList;

  GHR(){
      history_counter = 0;
      max_history = GHRMAXHIST;
  }

  void update_GHR(bool resolveDir){
    int max = history_counter / GHRPATCH + (history_counter % GHRPATCH > 0);
    while(DataList.size() < max)
      DataList.push_back(0);
    
    for(int lastBit = resolveDir, i = 0; i < max; i++){
      DataList[i] = (DataList[i] << 1) | lastBit;
      lastBit = DataList[i] & (1 << (GHRPATCH-1));
    }
    if(history_counter < max_history)
      history_counter++;
  }
  
  void printVectorBits() {
    for (int element : DataList) {
      for (int i = 0; i < sizeof(int) * GHRPATCH; i++) {
        int bit = (element >> (sizeof(int) * GHRPATCH - 1 - i)) & 1;
        std::cout << bit;
      }
      std::cout << ' ';
    }
    std::cout << std::endl;
  }
};

int get_HistoryFold(int n, int historyLength, GHR& ghr){
    if(historyLength < n){
      return ghr.DataList[0] & ((1 << n) - 1);
    }

    int res = 0;
    int patch = historyLength / n;
    for (int i =0; i< n; i++){
      res |= (ghr.DataList[(i * patch) / 32] >> ((i * patch) % 32) & 1) << i;
    }
    return res;
  }
// Define GHR, GHRMAXHIST, UINT32, and GHRPATCH here or include relevant headers.

// Define a function to test get_HistoryFold.
void test_get_HistoryFold() {
    // Create a GHR instance.
    GHR ghr;


    ghr.DataList.push_back(0b1);


    int historyLength = ghr.DataList.size() * GHRPATCH; // Calculate historyLength based on DataList size.
    int n = 11; 

    // Call get_HistoryFold to fold the GHR and store the result.
    int foldedHistory = get_HistoryFold(n, historyLength, ghr);

    // Print the folded history.
    std::cout << "Folded History: " << foldedHistory << std::endl;
    
    // Add assertions to verify the result.
    int expectedFoldedHistory = 0b00000001; // Replace with the expected result.
    if (foldedHistory == expectedFoldedHistory) {
        std::cout << "Test Passed!" << std::endl;
    } else {
        std::cout << "Test Failed!" << std::endl;
    }
}

int main() {
    // Call the test function.
    test_get_HistoryFold();

    return 0;
}
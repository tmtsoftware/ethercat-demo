using namespace std;

class Pdo {
 
  public: Pdo(string name, int index, int n_entries);

  public: string name;
  public: uint16_t index;
  public: unsigned int n_entries;
  public: vector<PdoEntry> pdoEntries;

};


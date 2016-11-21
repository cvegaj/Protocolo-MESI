#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct block{
  bool bValidBit = false;
  long lBlockTag = 0;
};

//dados los valores se calcula tamaño en bits del index y del offset necesarios en la función splitAddress
class cache{
  private:
    int iTagSize, iIndexSize;
    int iNumSets, iRand, iNumBlocks, iOffsetSize;
    long lAddress, lIndex, lTag, lIndexMask;
    std::vector<std::vector<block> > vBlocks;
  public:
    long long llBlockSize;
    int iAssociativity;
    long long llCacheSize;
    void splitAddress();
    bool readAddress(long);
    //Constructor con 3 parametros
    cache(int iAssoc,long long llCSize,long long llBSize){
      llBlockSize = llBSize;
      iAssociativity = iAssoc;
      llCacheSize = llCSize;
      iNumBlocks = llCSize/llBSize;
      iNumSets = iNumBlocks/iAssoc;
      iOffsetSize = int(log2(llBSize));
      iIndexSize = int(log2(iNumSets));
      iTagSize = 32 - iIndexSize - iOffsetSize;
      vBlocks.resize(iNumSets, std::vector<block>(iAssociativity) );
    }
    //Destructor >:O
    ~cache(){}
};

//para dividir la direccion en tag, index y offset
void cache::splitAddress(){
  lIndexMask = 0;
  for(int i = 0; i < iIndexSize; i++){
    lIndexMask <<= 1; //se crea una mascara para index
    lIndexMask |= 1; //indexMask or 1
  }
  lIndex = lAddress >> (iOffsetSize);
  lIndex = lIndex & lIndexMask;
  lTag = lAddress >> (iOffsetSize+iIndexSize);
}

//despues de saber en que set estoy de acuerdo al index
bool cache::readAddress(long lAddr){
  lAddress = lAddr;
  splitAddress();
  for(int i=0; i<iAssociativity; i++){ //para buscar la cantidad de bloques
    if (vBlocks[lIndex][i].bValidBit){
      if (vBlocks[lIndex][i].lBlockTag == lTag){
        return true;
      }
    }
  }
  iRand = rand() % iAssociativity;
  vBlocks[lIndex][iRand].bValidBit = true;
  vBlocks[lIndex][iRand].lBlockTag = lTag;

  return false;
}


//se reciben los parametros del programa
int main(int arcg, char* argv[]){

  //long long llParameter[3];
  //llParameter[0]=atoll(argv[1]);
  //llParameter[1]=atoll(argv[2]);
  //llParameter[2]=atoll(argv[3]);

  ifstream datos("aligned.trace");
	long lAddr;
	bool bHit;

	long long llTotalCPU0 = 0;
	long long llHitsCPU0 = 0;
	long long llMissesCPU0 = 0;
	double dMissRateCPU0;

        long long llTotalCPU1 = 0;
	long long llHitsCPU1 = 0;
	long long llMissesCPU1 = 0;
	double dMissRateCPU1;
  string line;

  //cantidad de accesos totales, hits y misses
  cache CacheL1(1,8000,16);
  cache CacheL2(1,64000,16);
  int CPU;
  CPU = 0;

  while ( getline (datos,line) ) {

    getline(datos,line);
    line.erase(line.begin()+8,line.end());
    istringstream(line) >> std::hex >> lAddr;

    if(CPU == 0 ) {
  	bHit = CacheL1.readAddress(lAddr);
    	if(bHit){
      	    llHitsCPU0++;
    	}
    	llTotalCPU0++;
	CPU = 1;
    }else{
	bHit = CacheL2.readAddress(lAddr);
    	if(bHit){
      	    llHitsCPU1++;
    	}
    	llTotalCPU1++;
        CPU = 0;
    }
  }
  cout << "Hits CPU0: " << llHitsCPU0 << endl;
  cout << "Total CPU0: " << llTotalCPU0 << endl;
  llMissesCPU0 = llTotalCPU0 - llHitsCPU0;
  dMissRateCPU0 = (double(llMissesCPU0)/double(llTotalCPU0))*100;
  cout << "Miss Rate CPU0: " << dMissRateCPU0 << endl;

  cout << "Hits CPU1: " << llHitsCPU1 << endl;
  cout << "Total CPU1: " << llTotalCPU1 << endl;
  llMissesCPU1 = llTotalCPU1 - llHitsCPU1;
  dMissRateCPU1 = (double(llMissesCPU1)/double(llTotalCPU1))*100;
  cout << "Miss Rate CPU1: " << dMissRateCPU1 << endl;
}

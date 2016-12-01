#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct block{
  char State = 'I'; //MESI
  bool bValidBit = false;
  long lBlockTag = 0;
};

//dados los valores se calcula tamaño en bits del index y del offset necesarios en la función splitAddress
class cache{
  private:
    int iTagSize, iIndexSize;
    int iNumSets, iRand, iNumBlocks, iOffsetSize;
    long lAddress, lIndex, lTag, lIndexMask;
    std::vector<block> vBlocks;
  public:
    long long llBlockSize;
    int iAssociativity;
    long long llCacheSize;
    void splitAddress();
    bool readAddress(long);
    char checkBlock(long);
    void setState(long, char);

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
      vBlocks.resize(iNumSets);
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

//lectura
bool cache::readAddress(long lAddr){
  lAddress = lAddr;
  splitAddress();
  if (vBlocks[lIndex].bValidBit){
        return true;
  }
  //Si se da MISS lo busca y reemplaza
  vBlocks[lIndex].bValidBit = true;
  vBlocks[lIndex].lBlockTag = lTag;
  return false;
}

//revisar estado del bloque cache
char cache::checkBlock(long lAddr){
  lAddress = lAddr;
  splitAddress();
  if(vBlocks[lIndex].bValidBit){
      return vBlocks[lIndex].State;
  }
  return 'I';
}


//Para cambiar estado del bloque
void cache::setState(long lAddr,char Stat){
  lAddress = lAddr;
  splitAddress();
  vBlocks[lIndex].State = Stat;
}

//***********************************************************

//se reciben los parametros del programa
int main(int arcg, char* argv[]){

  ifstream datos1("aligned.trace");
	long lAddr;
        char lAct;
	bool bHit;
        char Mode;
        long Counter = 0;

	long long llTotalCPU0 = 0;
	long long llHitsCPU0 = 0;
	long long llMissesCPU0 = 0;
	double dMissRateCPU0;
  string line;
  string line_copy;

  //cantidad de accesos totales, hits y misses
  cache CacheCPU0(1,8000,16);
  cache CacheCPU1(1,8000,16);
  cache CacheShared(1,64000,16);
  int CPU;
  CPU = 0;

  //Para ver cuantas lineas hay
  while ( getline (datos1,line) ) {

    getline(datos1,line);
    Counter = Counter+1;
  }

  ifstream datos("aligned.trace");
  while ( getline (datos,line) ) {

    getline(datos,line);
    line_copy = line;
    line.erase(line.begin()+8,line.end());
    istringstream(line) >> std::hex >> lAddr; //Obtiene direccion

    lAct = line_copy.at(line_copy.size()-1); //Obtiene si es lectura (L) o escritura (S)
    
    Counter = Counter-1;
    
    if(CPU == 0 ) {//CPU0
        if(lAct == 'L'){//Lectura
  	    bHit = CacheCPU0.readAddress(lAddr);
            if(!bHit){//Si hay Read MISS
                //cout << "*************************************" << endl;
                //cout << "CPU0 Read Miss" << endl;
                if(CacheCPU1.checkBlock(lAddr) == 'I'){//No está en CPU1
		    CacheCPU0.setState(lAddr,'E');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 No modificó otros Caches: " << endl;
		}
	        if(CacheCPU1.checkBlock(lAddr) == 'S'){
                    CacheCPU0.setState(lAddr,'S'); //Si está compartido poner CPU0 también en compartido
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 No modificó otros Caches: " << endl;
                }
                if(CacheCPU1.checkBlock(lAddr) == 'E'){
                    CacheCPU0.setState(lAddr,'S');//Si está exclusivo, poner ambos
                    CacheCPU1.setState(lAddr,'S');// en compartido
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU0 No modificó Shared" << endl;
                }
                if(CacheCPU1.checkBlock(lAddr) == 'M'){//Si está modificado, actualizar Cache compartido
                    CacheShared.setState(lAddr,'S');
                    CacheCPU0.setState(lAddr,'S');
                    CacheCPU1.setState(lAddr,'S');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }else{
		}
            }
            else{//Si hay Read HIT
                //cout << "*************************************" << endl;
                //cout << "CPU0 Read Hit" << endl;
                if(CacheCPU0.checkBlock(lAddr) == 'I'){
                    if(CacheCPU1.checkBlock(lAddr) != 'I'){
                        CacheCPU0.setState(lAddr,'S');
                        CacheCPU1.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está inválido o no está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 No modificó otros Caches: " << endl;
                    }
                }else{
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 No modificó otros Caches: " << endl;
                }
            }
        }else{//Escritura
            bHit = CacheCPU0.readAddress(lAddr);
            if(!bHit){//Si hay Write MISS
                //cout << "*************************************" << endl;
                //cout << "CPU0 Write Miss" << endl;
                if(CacheCPU1.checkBlock(lAddr) == 'I'){//No está en CPU1
		    CacheCPU0.setState(lAddr,'E');
                    CacheShared.setState(lAddr,'E');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    //cout << "CPU0 No modificó bloque CPU1 " << endl;
		}
	        if(CacheCPU1.checkBlock(lAddr) == 'S'){
                    CacheCPU0.setState(lAddr,'E'); 
                    CacheCPU1.setState(lAddr,'I');// Como era compartido y se modificó se invalida CPU1
                    CacheShared.setState(lAddr,'E');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }
                if(CacheCPU1.checkBlock(lAddr) == 'E'){
                    CacheCPU0.setState(lAddr,'E');
                    CacheCPU1.setState(lAddr,'E');
                    CacheCPU1.setState(lAddr,'E');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque Shared" << CacheShared.checkBlock(lAddr) << endl;
                }
                if(CacheCPU1.checkBlock(lAddr) == 'M'){//Si está modificado, actualizar Cache compartido
                    CacheShared.setState(lAddr,'S');
                    CacheCPU0.setState(lAddr,'S');
                    CacheCPU1.setState(lAddr,'S');
                    //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }else{
		}
            }
            else{//Si hay Write HIT
                //cout << "*************************************" << endl;
                //cout << "CPU0 Read Hit" << endl;
                if(CacheCPU0.checkBlock(lAddr) == 'E'){
                    if(CacheCPU1.checkBlock(lAddr) != 'I'){// Si está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        CacheCPU1.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU0 y no está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 No modificó bloque CPU1: " << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU0.checkBlock(lAddr) == 'M'){
                    if(CacheCPU1.checkBlock(lAddr) != 'I'){// Si está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        CacheCPU1.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU0 y no está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 No modificó bloque CPU1: " << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU0.checkBlock(lAddr) == 'S'){
                    if(CacheCPU1.checkBlock(lAddr) != 'I'){// Si está en CPU1
                        CacheCPU0.setState(lAddr,'M');
                        CacheCPU1.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'M');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU0 y no está en CPU1
                        CacheCPU0.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 No modificó bloque CPU1: " << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU0.checkBlock(lAddr) == 'I'){
                    if(CacheCPU1.checkBlock(lAddr) == 'I'){// Si no está en CPU1
                        CacheCPU0.setState(lAddr,'M');
                        CacheCPU1.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'M');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU1
                        CacheCPU0.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        //cout << "CPU0 Dejó bloque propio en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque CPU1 en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU0 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
            }    
        }
	CPU = 1;
    ///////////////////////////////////////////////////////////////////////////////////
    }else{//CPU1
        if(lAct == 'L'){//Lectura
  	    bHit = CacheCPU0.readAddress(lAddr);
            if(!bHit){//Si hay Read MISS
                //cout << "*************************************" << endl;
                //cout << "CPU1 Read Miss" << endl;
                if(CacheCPU0.checkBlock(lAddr) == 'I'){//No está en CPU1
		    CacheCPU1.setState(lAddr,'E');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 No modificó otros Caches: " << endl;
		}
	        if(CacheCPU0.checkBlock(lAddr) == 'S'){
                    CacheCPU1.setState(lAddr,'S'); //Si está compartido poner CPU0 también en compartido
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 No modificó otros Caches: " << endl;
                }
                if(CacheCPU0.checkBlock(lAddr) == 'E'){
                    CacheCPU1.setState(lAddr,'S');//Si está exclusivo, poner ambos
                    CacheCPU0.setState(lAddr,'S');// en compartido
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU1 No modificó Shared en: " << endl;
                }
                if(CacheCPU0.checkBlock(lAddr) == 'M'){//Si está modificado, actualizar Cache compartido
                    CacheShared.setState(lAddr,'S');
                    CacheCPU1.setState(lAddr,'S');
                    CacheCPU0.setState(lAddr,'S');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }
            }
            else{//Si hay Read HIT
                //cout << "*************************************" << endl;
                //cout << "CPU1 Read Hit" << endl;
                if(CacheCPU1.checkBlock(lAddr) == 'I'){
                    if(CacheCPU0.checkBlock(lAddr) != 'I'){
                        CacheCPU1.setState(lAddr,'S');
                        CacheCPU0.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está inválido o no está en CPU1
                        CacheCPU1.setState(lAddr,'E');

                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 No modificó otros Caches" << endl;
                    }
                }else{
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 No modificó otros Caches" << endl;
                }
            }
        }else{//Escritura
            bHit = CacheCPU1.readAddress(lAddr);
            if(!bHit){//Si hay Write MISS
                //cout << "*************************************" << endl;
                //cout << "CPU1 Write Miss" << endl;
                if(CacheCPU0.checkBlock(lAddr) == 'I'){//No está en CPU0
		    CacheCPU1.setState(lAddr,'E');
                    CacheShared.setState(lAddr,'E');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    //cout << "CPU1 No modificó bloque CPU0: " << endl;
		}
	        if(CacheCPU0.checkBlock(lAddr) == 'S'){
                    CacheCPU1.setState(lAddr,'E'); 
                    CacheCPU0.setState(lAddr,'I');// Como era compartido y se modificó se invalida CPU0
                    CacheShared.setState(lAddr,'E');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }
                if(CacheCPU0.checkBlock(lAddr) == 'E'){
                    CacheCPU1.setState(lAddr,'E');
                    CacheCPU0.setState(lAddr,'E');
                    CacheCPU0.setState(lAddr,'E');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque Shared" << CacheShared.checkBlock(lAddr) << endl;
                }
                if(CacheCPU0.checkBlock(lAddr) == 'M'){//Si está modificado, actualizar Cache compartido
                    CacheShared.setState(lAddr,'S');
                    CacheCPU1.setState(lAddr,'S');
                    CacheCPU0.setState(lAddr,'S');
                    //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                    //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                }else{
		}
            }
            else{//Si hay Write HIT
                //cout << "*************************************" << endl;
                //cout << "CPU1 Write Hit" << endl;
                if(CacheCPU1.checkBlock(lAddr) == 'E'){
                    if(CacheCPU0.checkBlock(lAddr) != 'I'){// Si está en CPU0
                        CacheCPU1.setState(lAddr,'E');
                        CacheCPU0.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU1 y no está en CPU0
                        CacheCPU1.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 No modificó bloque CPU0: " << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU1.checkBlock(lAddr) == 'M'){
                    if(CacheCPU0.checkBlock(lAddr) != 'I'){// Si está en CPU0
                        CacheCPU1.setState(lAddr,'E');
                        CacheCPU0.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU1 y no está en CPU0
                        CacheCPU1.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 No modificó bloque CPU0: " << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU1.checkBlock(lAddr) == 'S'){
                    if(CacheCPU0.checkBlock(lAddr) != 'I'){// Si está en CPU1
                        CacheCPU1.setState(lAddr,'M');
                        CacheCPU0.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'M');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU1 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU1 y no está en CPU0
                        CacheCPU1.setState(lAddr,'E');
                        CacheShared.setState(lAddr,'E');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 No modificó bloque CPU0: " << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
                if(CacheCPU1.checkBlock(lAddr) == 'I'){
                    if(CacheCPU0.checkBlock(lAddr) == 'I'){// Si no está en CPU1
                        CacheCPU1.setState(lAddr,'M');
                        CacheCPU0.setState(lAddr,'I');
                        CacheShared.setState(lAddr,'M');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU0 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;

                    }else{//Si está en CPU0
                        CacheCPU1.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        CacheShared.setState(lAddr,'S');
                        //cout << "CPU1 Dejó bloque propio en: " << CacheCPU1.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque CPU1 en: " << CacheCPU0.checkBlock(lAddr) << endl;
                        //cout << "CPU1 Dejó bloque Shared en: " << CacheShared.checkBlock(lAddr) << endl;
                    }
                }
            }    
        }
	CPU = 0;
    }
    if (Counter<20){
        cout << "Estado bloque cache CPU0: " << CacheCPU0.checkBlock(lAddr) << endl;
        cout << "Estado bloque cache CPU1: " << CacheCPU1.checkBlock(lAddr) << endl;
        cout << "Estado bloque cache Shared: " << CacheShared.checkBlock(lAddr) << endl;
    }
  }
}

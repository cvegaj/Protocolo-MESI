#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>

void eratostenes(){

	int size = 5000;
	int aiPrimos[size];		//aiPrimos: Array de enteros con tamano size. Se quiere que al final del algoritmo el array contenga solamente numeros primos.
	int count = 0;
							// En las casillas de numeros no-primos deberan haber ceros.
	for(int i=0; i<size ;i++){
		aiPrimos[i] = i;	//Se llena el array con numeros iguales a su indice. Ejemplo: en la casilla 5 del array hay un entero de valor 5.
	}
	aiPrimos[0] = 0;		//Las dos primeras casillas se llenan con ceros. Porque los numeros 0 y 1 se toman como no primos.
	aiPrimos[1] = 0;

	for(int k=2; k*k<=size; k++){			//Para cada valor de k entre 2 y la raiz cuadrada de Size.
		if(aiPrimos[k] == 0){					//Si la casilla k contiene un numero hasta el momento primo (es decir, diferente de cero) continua
		}
		else{
			for(int j=k*2; j<size; j=j+k){	//j aumenta de k en k, obteniendo los multiplos de k.
				aiPrimos[j]=0;				//Se pone en cero la casilla de j.
			}

		}
	}

	std::cout<< "--------IMPRIMIENDO ARRAY DE PRIMOS - SIN PARALELISMO------------" <<std::endl;
	for(int i=0; i<size; i++){
		if(aiPrimos[i]==0){					//Se imprimen los numeros primos. Es decir, aquellos elementos del array que no esten en cero.
		}
		else{
			std::cout<< aiPrimos[i] << " - ";
			count++;
		}
	}
	std::cout<<std::endl<<"Cantidad de números primos: "<< count <<std::endl;
	std::cout<<std::endl<<"*****FIN******" <<std::endl;
}

int main(){
	clock_t t_inicio, t_fin; //Variables para tomar el tiempo de ejecucion.
  double t_total;
  t_inicio = clock();			//Se guarda el tiempo inicial
	//eratostenes();
	eratostenes();
	t_fin = clock(); 				//Se guarda el tiempo final
	t_total = (double)(t_fin - t_inicio) / CLOCKS_PER_SEC;
	printf("tiempo: %f\n",t_total);
	return 0;
}

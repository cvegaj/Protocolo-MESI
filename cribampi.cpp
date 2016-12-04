#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include "mpi.h"

void eratostenes(int iCommID){

	int size = 5000;
	int aiPrimos[size];		//aiPrimos: Array de enteros con tamano size. Se quiere que al final del algoritmo el array contenga solamente numeros primos.
  int aiPrimos2[size];	//aiPrimos2: Segundo array que se utilizara para unir los resultados de cada proceso.
  int count = 0;				//Contador de numeros primos.
												// En las casillas de numeros no-primos deberan haber ceros.
	for(int i=0; i<size ;i++){
		aiPrimos[i] = i;	//Se llena el array con numeros iguales a su indice. Ejemplo: en la casilla 5 del array hay un entero de valor 5.
	}
	aiPrimos[0] = 0;		//Las dos primeras casillas se llenan con ceros. Porque los numeros 0 y 1 se toman como no primos.
	aiPrimos[1] = 0;
														//El lazo se divide en 2. Cada proceso ejecutara una de las partes del lazo.
  if(iCommID==0){						//El primer proceso:
  	for(int k=2; k<19; k++){			//Para cada valor de k entre 2 y 19 (numero escogido arbitrariamente)
  		if(aiPrimos[k] == 0){				//Si la casilla k contiene un numero hasta el momento primo (es decir, diferente de cero) continua
  		}
  		else{
  			for(int j=k*2; j<size; j=j+k){	//j aumenta de k en k, obteniendo los multiplos de k.
  				aiPrimos[j]=0;				//Se pone en cero la casilla de j.
  			}

  		}
  	}
  }
  if(iCommID==1){								//El segundo proceso:
  	for(int k=19; k*k<=size; k=k+2){			//Realiza lo mismo que el primero pero k entre 19 y la raiz cuadrada de Size
  		if(aiPrimos[k] == 0){			//Sabemos que los numeros pares no son primos. Por tanto k nunca es par. (Esto se logra con k=k+2 empezando en impar)
  		}														//Si la casilla k contiene un numero hasta el momento primo (es decir, diferente dse cero) continua
  		else{
  			for(int j=k*2; j<size; j=j+k){	//j aumenta de k en k, obteniendo los multiplos de k.
  				aiPrimos[j]=0;				//Se pone en cero la casilla de j.
  			}

  		}
  	}
  }

  MPI_Reduce(aiPrimos, aiPrimos2, size, MPI_INT, MPI_MIN,0,MPI_COMM_WORLD);
	//La funcion MPI_Reduce realiza una comparacion entre los arreglos de cada proceso y obtiene un unico array como resultado.
	//Al utilizar la opcion MPI_MIN, en el resultado se obtiene el valor minimo al comparar cada valor de los arrays de entrada.
	// Por ejemplo: Array del Proceso1 = [1,5,3,4];   Array del Proceso2 = [2,0,4,1]; 	Resultado = [1,0,3,1]

  if(iCommID==0){
  	std::cout<< "--------IMPRIMIENDO ARRAY DE PRIMOS - CON PARALELISMO------------" <<std::endl;
  	for(int i=0; i<size; i++){
  		if(aiPrimos2[i]==0){					//Se imprimen los numeros primos. Es decir, aquellos elementos del array que no esten en cero.
			}
			else{
				std::cout<< aiPrimos2[i] << " - ";
  			count++;
  		}
  	}
  	std::cout<<std::endl<<"Cantidad de números primos: "<< count <<std::endl;
  	std::cout<<std::endl<<"*****FIN******" <<std::endl;
  }
}

int main(){
	// Inicializar MPI
  MPI_Init(NULL, NULL);
  //Obtener el ID del proceso (Communication ID)
  int iCommID;
  MPI_Comm_rank(MPI_COMM_WORLD, &iCommID);

  clock_t t_inicio, t_fin; //Variables para tomar el tiempo de ejecucion.
  double t_total;
  t_inicio = clock();			//Se guarda el tiempo inicial

	eratostenes(iCommID);
	t_fin = clock(); 				//Se guarda el tiempo final
	t_total = (double)(t_fin - t_inicio) / CLOCKS_PER_SEC;
	if(iCommID==0){		//El tiempo total es la resta de Tiempo Final - Tiempo Inicial
		printf("tiempo: %f\n",t_total);
	}
  //Finalizar MPI
  MPI_Finalize();
	return 0;
}

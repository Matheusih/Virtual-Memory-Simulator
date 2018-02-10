#include "memvirt.h"
#include "simplegrade.h"
#include <stdio.h>


#define WS(a, b, c) \
if (a){ \
	isEqual(a->avg_ws, b, c); \
} \
else isNotNull(a, c);

#define PFRATE(a, b, c) \
if (a){ \
	isNear(a->total_pf_rate, b, 1, c); \
} \
else isNotNull(a, c);


void FREE(struct result * a){
	if (a){ 
		if (a->refs) 
			free(a->refs); 
		if (a->pfs)
			free(a->pfs); 
		if (a->pf_rate)
			free(a->pf_rate);
		free(a);
	}
}



void test_param(){
	struct result * res;
/* 
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
*/
	WHEN("Tenho apenas um processo e um frame");

	res = memvirt(1,1,"small.txt",1);
	THEN("Deve falhar, mínimo de dois frames");
	isNull(res, 1);
	FREE(res);

	WHEN("Tenho dois processos e três frames");

	res = memvirt(2,3,"small.txt",1);
	THEN("Deve falhar, mínimo de dois frames");
	isNull(res, 1);
	FREE(res);
}


void test_small(){
	struct result * res;
/* 
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
0 0
*/
	WHEN("Tenho apenas um processo, dois frame e uma página");

	res = memvirt(1,2,"small.txt",1);
	THEN("Deve ter working set de 1");
	WS(res, 1, 1);
	THEN("Somente o primeiro acesso é pf");
	PFRATE(res, 0.1, 1);
	FREE(res);
}

void test_small2(){
	struct result * res;
/* 
0 0
0 1
0 2
0 3
0 4
0 5
0 6
0 7
0 8
0 9
*/

	WHEN("Tenho apenas um processo, dois frame e 10 páginas sendo acessadas em sequencia");

	res = memvirt(1,2,"small2.txt",1);
	THEN("Todo acesso é pf");
	PFRATE(res, 1, 1);
	FREE(res);
}

void test_small3(){
	struct result * res;
/* 
0 0 pf
0 0
0 0 
0 1 pf
0 1
0 2 pf
0 1 
0 1
0 1
0 1
*/ 

	WHEN("Tenho apenas um processo, dois frames e 5 páginas sendo acessadas com repetições");

	res = memvirt(1,2,"small3.txt",5);
	THEN("Working set é total de frames (pela restrição de contar apenas as páginas carregadas)");
	WS(res, 2, 1);
	THEN("Três acessos são pf");
	PFRATE(res, .3, 1);
	FREE(res);
}

void test_2_procs(){
	struct result * res;
/* 
0 0
0 1
1 0
1 1
0 0 
0 1
1 0
1 1
0 0
*/
	WHEN("Tenho dois processos, cada um com duas páginas, quatro frames e equilíbrio de acessos");

	res = memvirt(2,4,"twoprocs.txt",4);
	THEN("Working sets iguais");
	WS(res, 2, 2); 

	THEN("Espero ter só pf compulsórios");
	PFRATE(res, .4, 1);
	FREE(res);
}

void test_many_procs(){
	struct result * res;

	WHEN("Tenho 4 processos, nove frames e apenas um processo acessando");

	res = memvirt(4,9,"small.txt",5);
	THEN("Espero ter apenas 1 pf");
	PFRATE(res, .1, 1);
	FREE(res);
}

void test_many_procs2(){
	struct result * res;
/* 
0 0
0 1
0 2
0 3
0 4
0 5
0 6
0 7
0 8
0 9
*/

	WHEN("Tenho 2 processos, 20 frames e apenas um processo acessando 5 páginas");

	res = memvirt(2,20,"small2.txt",5);
	THEN("Espero ter WS maior");

	WS(res, 2, 1); 
	THEN("Espero só ter pf");
	PFRATE(res, 1, 1);
	FREE(res);
}

void test_clock(){
	struct result * res;
/* 
0 0 pf
0 1 pf 
0 2 pf
0 3 pf 
0 0 
0 0 
0 4 pf 
0 1 
5/8 = 
*/ 
	WHEN("Tenho um processo, 4 frames e 5 páginas");
	res = memvirt(1, 4, "clock.txt", 5); 
	THEN("Espero que a vitima seja 0 e não cause pf");
	PFRATE(res, 5.0/8.0, 3);
	FREE(res);

/* 
0 0 pf
0 0 
0 1 pf
0 2 pf (não pega livre)
0 0 pf 
1 0 pf
1 1 pf
1 0 
*/ 


	WHEN("Tenho dois processos e 5 frames");
	res = memvirt(2, 5, "clock2.txt", 10); 
	THEN("Espero que o primeiro processo fique com 2 frames (não pegue um livre, já está no limite)");
	PFRATE(res, .75, 3);
	FREE(res);

}

int main(){

	test_param();
	test_small();
	test_small2();
	test_small3();
	test_2_procs();
	test_many_procs();
	test_many_procs2();
	test_clock();

	printf("Grading \n");
	GRADEME();

	if (grade==maxgrade)
		return 0;
	else return grade;



}

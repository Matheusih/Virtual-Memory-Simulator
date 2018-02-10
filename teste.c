#include <stdlib.h>
#include <stdio.h>
#include "simplegrade.h"
#include <stdint.h>

struct result{
	uint32_t * refs; /* referências a páginas por processo (um vetor!) */
	uint32_t * pfs;  /* page faults por processo */
	float * pf_rate; /* taxa de page fault por processo em % */
	uint32_t avg_ws; /* working set médio, arredondado para baixo */
	float total_pf_rate; /* taxa de page fault para toda a simulação */
};

void test_simples();
struct result * memvirt(uint32_t num_procs, uint32_t num_frames, char * filename, uint32_t interval);

int main(){
	test_simples();
}


void test_simples(){
    struct result * res;
    printf("2 Processos, 3 Frames-> Espero NULL\n");
    res = memvirt(2,3,"test2proc.txt",0);
    isNull(res,0);
    
    printf("2 Processos, 10 Frames-> Espero 7 - 3 refs, 10 Pagefaults\n");
    res = memvirt(2,10,"test2proc.txt",10);
    isEqual(res->refs[0],5,0);
    isEqual(res->refs[1],4,0);
    isEqual(res->pf_rate[0],100,0);
    isEqual(res->pf_rate[1],100,0);
    isEqual(res->avg_ws,5,0);
    
    printf("3 Processos, 12 Frames, Intervalo 12 e 30 ops\n");
    res = memvirt(3,12,"test3proc.txt",12);
    for(int i = 0; i < 3; i++){
        isEqual(res->refs[i],8,0);
        isEqual(res->pfs[i],8,0);
    }
    
    free(res);
}
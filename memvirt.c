#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct result{
	uint32_t * refs; /* referências a páginas por processo (um vetor!) */
	uint32_t * pfs;  /* page faults por processo */
	float * pf_rate; /* taxa de page fault por processo em % */
	uint32_t avg_ws; /* working set médio, arredondado para baixo */
	float total_pf_rate; /* taxa de page fault para toda a simulação */
};

struct frames{
	int32_t PID;
	int32_t ref;
	struct frames * next;
	struct frames * head;
	struct frames * tail;
	int32_t dirt;
};

struct frames * setaZZ;

uint32_t ciclos = 0;

struct frames *  startFrames(struct frames * fr, uint32_t num_frames);
uint32_t searchFrames(struct frames * fr,uint32_t proc, uint32_t ref, uint32_t flag, uint32_t *cFperProc,uint32_t *mFperProc);
uint32_t searchFrames4Max(struct frames * fr,uint32_t proc, uint32_t ref, uint32_t flag, uint32_t *cFperProc,uint32_t *mFperProc);
uint32_t searchFreeFrames(struct frames * seta,uint32_t proc, uint32_t ref,struct frames ** nseta);
uint32_t searchInProcFrames(struct frames * seta,uint32_t proc, uint32_t ref, uint32_t targetProc,struct frames **nseta);
void startRes(struct result *res,uint32_t num_procs);
void destroyBuffer(struct frames * fr);

struct result * memvirt(uint32_t num_procs, uint32_t num_frames, char * filename, uint32_t interval);



/*
int main(){
    struct result * res = memvirt(3,12,"test3proc.txt",12);
    return 0;
}
*/


struct result * memvirt(uint32_t num_procs, uint32_t num_frames, char * filename, uint32_t interval){
    struct result * res = malloc(sizeof(struct result));
	
	FILE *fp = fopen(filename,"r");
	struct frames * fr = NULL;
	char line[100];
	uint32_t proc, ref, i,j, flag = 0, total = 0, total_ref = 0,total_pf = 0;
	uint32_t mFperProc[num_procs]; //max frames per process
    uint32_t cFperProc[num_procs]; //current frames per process
	uint32_t temp = floor(  num_frames / num_procs );
    char num1[10],num2[10], *c = malloc(sizeof(char*));
    ciclos = 0;
    if( temp < 2 ) return NULL;
    struct frames * setas[num_procs];
    struct frames * nseta = malloc(sizeof(struct frames *));
    
	for( i=0 ; i<num_procs ; i++ ){
        mFperProc[i] = temp;
        cFperProc[i] = 0;
    }
	
	
	// CADA PROCESSO RECEBE  floor(  num_frames / num_procs ) até ser atualizado pela formula  (uint32_t) ((float) ws/total) * (total de frames)
	
    startRes(res,num_procs);
    fr = startFrames(fr,num_frames);
    
	setaZZ = fr;
    
    for( i=0 ; i<num_procs ; i++ ){
        setas[i] = fr;
    }
    
    
    while( fgets(line, sizeof(line), fp) ){
        flag = 0;
        i = 0;
        j = 0;
        *c = line[j];
        while(*c != (char)'\n' && *c != '\0' ){
            if(*c == '\r')
                break;
            if(*c == ' ' && flag == 1)
                break;
            if(*c == ' '){
                flag = 1;
                num1[i] = '\n';
                i = 0;
            }
            else if(flag == 1){
                num2[i] = *c;
                i++;
            }
            else{
                num1[i] = *c;
                i++;
            }
            j++;
            *c = line[j];
        }
        num2[i] = '\n';
        proc = atoi(num1);
        ref = atoi(num2);
        //printf("proc:%d  ref:%d\n", proc, ref);
		//proc = line[0] - 49;
		//ref = line[2] - 48;
        if(interval != 0 && ciclos == interval){
            total = 0;
            for( i=0 ; i<num_procs ; i++ )
                total += cFperProc[i];
            for( i=0 ; i<num_procs ; i++ ){ //CALCULO DO WORKING SET
                float var = cFperProc[i];
                var /= total;
                var *= num_frames;
                /*if((int) var < 2){  //SE ALGUM PROCESSO RECEBER MENOS DE 2 FRAMES, RETURN NULL
                    printf("RETORNANDO NULL");
                    for(i=0;i<num_procs;i++){
                        res->avg_ws += mFperProc[i];
                        total_ref += res->refs[i];
                        total_pf += res->pfs[i];
                        res->pf_rate[i] = (float) res->refs[i] / res->pfs[i];
                        //res->pf_rate[i] *= 100;
                        printf("refs e pfs:\n");
                        printf("ref:%d pf:%d,   rate:%.1f\n", res->refs[i],res->pfs[i],res->pf_rate[i]);
                    }
                    res->total_pf_rate = total_pf / total_ref;
                    //res->total_pf_rate = total_ref / total_pf;
                    res->avg_ws /= num_procs;
                    res->avg_ws = floor((double)res->avg_ws);
                    return NULL;
                }*/
                mFperProc[i] = (uint32_t) var;
            }
            for(i=0;i<num_procs;i++){
                //printf("CURRENT FRAMES: %d,  MAX FRAMES:%d\n",cFperProc[i], mFperProc[i]);
            }
            ciclos = 0;
        }
       // 
        //PROCURA POR FRAMES LIVRES
        if(cFperProc[proc] >= mFperProc[proc]){
            //printf("CURRENT FRAMES: %d,  MAX FRAMES:%d\n",cFperProc[proc], mFperProc[proc]);
            //printf("Procurando dentro do proprio processo\n");
            if(searchInProcFrames(setas[proc],proc,ref,proc,&nseta)){ //0 - Hit, 1 - pagefault
                ++res->pfs[proc];
            }
            setas[0] = nseta;
        }
        else{
            int aux = searchFreeFrames(setas[proc],proc,ref,&nseta); // 2 - NoFreeFrames // 1 - PF //  0 - Hit
            
            if(  aux == 2 ){  //no free frames
                flag = 0;
                for(i=0;i<num_procs;i++){   
                    if(cFperProc[i] > mFperProc[i] ){    //PROCURA POR PROCESSO COM MAIS FRAMES QUE DEVERIA
                        if(proc != i){
                            //printf("CURRENT FRAMES: %d,  MAX FRAMES:%d\n",cFperProc[i], mFperProc[i] );
                           // printf("procurando em outro processo\n");
                            if(searchInProcFrames(setas[proc],proc,ref,i,&nseta)){ //0 - Hit, 1 - pagefault
                                ++cFperProc[proc];
                                --cFperProc[i];
                                flag = 1;
                                break;
                            }
                            setas[0] = nseta;
                        }
                        //break;
                    }
                }
                if(flag == 0){  //PROCURA PAGINA VITIMA DENTRO DO PROPRIO PROCESSO
                    if(searchInProcFrames(setas[proc],proc,ref,proc,&nseta)){ //0 - Hit, 1 - pagefault
                        ++res->pfs[proc];
                    }
                    setas[0] = nseta;
                }
            }
            else if( aux == 1 ){
                ++cFperProc[proc];
                ++res->pfs[proc];
                setas[0] = nseta;
            }
            else{ //aux = 0
                setas[0] = nseta;
                //++res->pfs[proc];
            }
        }
        ++res->refs[proc];
        ++ciclos;
	}
    for(i=0;i<num_procs;i++){
        res->avg_ws += cFperProc[i];
        total_ref += res->refs[i];
        total_pf += res->pfs[i];
        res->pf_rate[i] = (float) res->refs[i] / res->pfs[i];
        //res->pf_rate[i] *= 100;
        //printf("refs e pfs:\n");
        //printf("ref:%d pf:%d,   rate:%.1f\n", res->refs[i],res->pfs[i],res->pf_rate[i]);
    }
    float tpr = total_pf;
    tpr /= total_ref;
    res->total_pf_rate = tpr;
    res->avg_ws /=  num_procs;

    //res->avg_ws = num_frames;
    //res->avg_ws /= total_ref;
    //printf("\n\n");
    //printf("avg_ws:%d\n",res->avg_ws);
    //printf("%.1f\n",res->total_pf_rate);
    destroyBuffer(fr);
    return res;
}
uint32_t searchFreeFrames(struct frames * seta,uint32_t proc, uint32_t ref,struct frames ** nseta){ //proc = PID do processo, ref = pagina que está sendo referenciada 
	struct frames * p = seta;
	for(;;){
        do{ //procura na memoria se a pagina já foi referenciada
            if((uint32_t)p->ref == ref && (uint32_t) p->PID == proc){ // hit
                p->dirt = 1;
                //res->refs[proc]++;
                seta = p->next;
                *nseta = &(*p);
                //printf("Hit\n");
                return 0;
            }
            p = p->next;
        } while( p != seta );
        p = seta;
        do{
            if(p->dirt == -1){
                    p->dirt = 1;
                    p->ref = ref;
                    p->PID = proc;
                    seta = p->next;
                    *nseta = &(*p->next);
                    //printf("Pagefault\n");
                    return 1;  //pagefault
            }
            p = p->next;
        }while( p != seta );
        break;
    }
    return 2; //no free frames
}
uint32_t searchInProcFrames(struct frames * seta,uint32_t proc, uint32_t ref, uint32_t targetProc,struct frames **nseta){ //proc = PID do processo, ref = pagina que está sendo referenciada 
	struct frames * p = seta;
	for(;;){
        do{ //procura na memoria se a pagina já foi referenciada
            if((uint32_t)p->ref == ref && (uint32_t)p->PID == proc){ // hit
                p->dirt = 1;
                //res->refs[proc]++;
                seta = p->next;
                *nseta = &(*p);
                //printf("Hit\n");
                return 0;
            }
            p = p->next;
        }while( p != seta );
        p = seta;
        for(;;){
            if((uint32_t)seta->PID == targetProc){
                if(seta->dirt == 1){
                    seta->dirt = 0;
                    //seta = seta->next;
                }
                else if(seta->dirt == 0){
                    seta->dirt = 1;
                    seta->ref = ref;
                    //printf("\nPAGINA VITIMA: %d", seta->ref);
                    seta->PID = proc;
                    *nseta = &(*p->next);
                    //printf("Pagefault\n");
                    return 1; // Pagefault
                    
                }
            }
            seta = seta->next;
            *nseta = &(*p->next);
        }
        break;
    }
    return 2; //erro
}




void startRes(struct result *res,uint32_t num_procs){
	res->refs = calloc(num_procs,sizeof(uint32_t));
	res->pfs = calloc(num_procs,sizeof(uint32_t));
	res->pf_rate = calloc(num_procs,sizeof(float));
	res->avg_ws = 0;
	res->total_pf_rate = 0;
}
struct frames * startFrames(struct frames * fr, uint32_t num_frames){
	uint32_t i;
    struct frames * p;
	fr = malloc(sizeof(struct frames));
    p = fr;
    fr->PID = -1;
    fr->ref = -1;
    fr->dirt = -1;
	for(i=1;i<num_frames;i++){
		fr->next = malloc(sizeof(struct frames));
		fr->next->PID = -1;
		fr->next->ref = -1;
        fr->next->dirt = -1;
        fr = fr->next;
	}
    fr->next = p;
    return fr;
}



void destroyBuffer(struct frames * fr){
    struct frames * p = fr->next;
    struct frames * q;
    while(p != fr){
        q = p;
        p = p->next;
        free(q);
    }
    free(fr);
}































/*
uint32_t searchFrames(struct frames * fr,uint32_t proc, uint32_t ref, uint32_t flag, uint32_t *cFperProc,uint32_t *mFperProc){ //proc = PID do processo, ref = pagina que está sendo referenciada 
	struct frames * p = seta->next;
	for(;;){
        while( p != seta ){ //procura na memoria se a pagina já foi referenciada
            if(p->ref == ref){ // hit
                p->dirt = 1;
                //res->refs[proc]++;
                seta = seta->next;
                return 0;
            }
            p = p->next;
        }
        res->pfs[proc]++;
        if( seta->dirt != -1 && cFperProc[seta->PID+1] <= 2 ){ //pagina já possui numero minimo de frames, então nao sera vitima
            seta = seta->next;
        } 
        else{
            if(seta->dirt == 0){ //pagina sem chance, então será vitima
                seta->dirt = 1;
                seta->ref = ref;
                seta->PID = proc;
                seta = seta->next;
                break;
            }
            else if(seta->dirt == 1){ //pagina com chance, então tirar chance e continuar
                seta->dirt = 0;
                seta = seta->next;
            }
            else if(seta->dirt == -1){
                seta->dirt = 1;
                seta->ref = ref;
                seta->PID = proc;
                seta = seta->next;
                break;
            }
        }
        //seta = seta->next;
	}
    return 0;
}
uint32_t searchFrames4Max(struct frames * fr,uint32_t proc, uint32_t ref, uint32_t flag, uint32_t *cFperProc,uint32_t *mFperProc){
    struct frames * p = seta->next;
    for(;;){
        while( p != seta ){ //procura na memoria se a pagina já foi referenciada
            if(p->ref == ref){ // hit
                p->dirt = 1;
                res->refs[proc]++;
                seta = seta->next;
                return 0;
            }
            p = p->next;
        }
        res->pfs[proc]++;
        if( seta->PID == proc ){ //procurar vitima dentro do proprio processo
            if(seta->dirt == 0){ //vitima
                seta->dirt = 1;
                seta->ref = ref;
                seta->PID = proc;
                seta = seta->next;
                break;
            }
            else if(seta->dirt == 1){
                seta->dirt = 0;
                seta = seta->next;
            }
            else if(seta->dirt == -1){
                seta->dirt = 1;
                seta->ref = ref;
                seta->PID = proc;
                seta = seta->next;
                break;
            }
        }
        seta = seta->next;
	}
    return 0;
}*/


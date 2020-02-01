/*
    Sirus Shahini
    ~cyn
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int global_x                = 0;
unsigned long global_lock   = 0;

/*
    A simple lock implementation by cmpxchg instruction
    param uadr: the address of the memory area to use as a user space lock
*/
int ulock(void * uadr){
    unsigned long volatile r =0 ;
    asm volatile(
        "xor %%rax,%%rax\n"
        "mov $1,%%rbx\n"
        "lock cmpxchg %%ebx,(%1)\n"
        "sete (%0)\n"
        : : "r"(&r),"r" (uadr)
        : "%rax","%rbx"
    );
    return (r) ? 1 : 0;
}

struct args{ int dump; };

void *test_thread(void *p){
    int thread_id = *((int *)((struct args*)p));
    int temp;
    int i;
    for (i=0;i<5;i++){
        if (ulock((void *)&global_lock)){
            //Critical Area Start
            printf("Thread %d accuired lock\n",thread_id);fflush(stdout);
            printf("          reading global var:");
            temp = global_x;
            printf("%d\n",global_x);fflush(stdout);
            printf("          writing global var:");
            temp++;
            global_x = temp;
            printf("%d\n",global_x);fflush(stdout);
            printf("          releasing lock\n\n");fflush(stdout);
            //Critical Area End
            global_lock = 0;
            sleep(0.01);
        }else{
            sleep(0.01);i--;
        }
    }

}


int main(){
    pthread_t t1,t2;
    struct args *arg1,*arg2;
    arg1 = malloc(sizeof (struct args));
    arg2 = malloc(sizeof (struct args));
    arg1->dump = 1;
    arg2->dump = 2;
    pthread_create(&t1,NULL,test_thread,arg1);
    pthread_create(&t2,NULL,test_thread,arg2);
    pthread_join(t1,NULL);
    //sleep(0.1);
    pthread_join(t2,NULL);
    printf("Final value of the global var: %d\n",global_x);
    free(arg1);
    free(arg2);
    return 0;
}

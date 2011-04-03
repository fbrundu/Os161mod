#include <types.h>
#include <lib.h>
#include <wchan.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include "opt-synchprobs.h"

#define SIZE_S 5
#define SIZE_N 10

/* semaphore to synchronize threads with main */
static struct semaphore *gsem = NULL;
/* semaphore for the variable modified */
static struct semaphore *msem = NULL;
/* semaphore vector to handle concorrent access to vector */
static struct semaphore **sv = NULL;
static int num_elem = 0;
static int *vect = NULL;
/* flag that indicates that some elements have been swapped */
static int modified = 1;

static
void
init_sem_v(void)
{
  int i, n = num_elem-1;
  if(gsem == NULL){
    gsem = sem_create("gsem",0);
    if(gsem == NULL){
      panic("tt5.init_sem: gsem create failed\n");
    }
  }
  if(msem == NULL){
    msem = sem_create("msem",1);
    if(msem == NULL){
      panic("tt5.init_sem: msem create failed\n");
    }
  }
  if(sv == NULL){
    sv = kmalloc(n*sizeof(struct semaphore*));
    if(sv == NULL)
      panic("tt5.init_sem: sv allocation failed\n");
    else {
      for(i = 0; i < n; i++){
	sv[i] = sem_create("sem_v",1);
	if(sv[i] == NULL)
	  panic("tt5.init_sem: sem_create failed\n");
      }
    }
  }
}

static
void
print_vect(){
  int i;
  kprintf("Il vettore ordinato è il seguente:\n");
  for(i = 0; i < num_elem; i++){
    kprintf("[%d] -> [%d]\n", i+1, vect[i]);
  }
}

static
int
read_num(char str[]){
  unsigned int i;
  int n = 0;
  for(i = 0; i < strlen(str); i++){
    n *= 10;
    n += (str[i]-'0');
  }
  return n;
}

static
void
set_vect(){
  int i;
  char str[SIZE_S+1];
  for(i = 0; i < num_elem; i++){
    kprintf("Insert element %d: ", i+1);
    kgets(str,SIZE_S);
    vect[i] = read_num(str);
  }
}

static
void
swppr(void *junk, unsigned long ind){
  int tmp;

  (void)junk;

  P(msem);
  P(sv[ind]);
  P(sv[ind+1]);
  if(vect[ind] > vect[ind+1]){
    tmp = vect[ind];
    vect[ind] = vect[ind+1];
    vect[ind+1] = tmp;
    modified++;
  }
  V(sv[ind]);
  V(sv[ind+1]);
  V(msem);
  /* synchronize with the master */
  V(gsem);
}

static
void
run_vect()
{
  int i, result;
  char name[SIZE_N+1];
  P(msem);
  for(i = 0; i < num_elem-1; i++){
    snprintf(name, sizeof(name), "t %d %d", i, i+1);
    result = thread_fork(name, swppr, NULL, i, NULL);
    if (result) {
      panic("Thread fork failed: swapper -> %s\n", strerror(result));
    }
    kprintf("MAIN: %d threads created\n", num_elem-1);
  }
  while(modified > 0){
    kprintf("MAIN: Modified. Loop\n");
    modified = 0;
    V(msem);
 
    i = num_elem-1;
    while(i>0){
      kprintf("MAIN: Waiting %d threads\n", i);
      P(gsem);
      i--;
    }
    P(msem);
  }
}


int
threadtest5(int nargs, char **args)
{
  (void)nargs;
  (void)args;
  char str[SIZE_S+1];
  
  kprintf("Insert vector size: ");
  kgets(str,SIZE_S);
  num_elem = read_num(str);
  vect = kmalloc(num_elem*sizeof(int));
  set_vect();
  init_sem_v();
  kprintf("Starting thread test 5\n");
  run_vect();
  print_vect();
  kprintf("\nThread test done.\n");

  return 0;
}

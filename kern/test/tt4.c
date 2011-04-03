#include <types.h>
#include <lib.h>
#include <wchan.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include "opt-synchprobs.h"

#define SIZE 20

static struct semaphore *tsem = NULL;
static struct semaphore *ssem = NULL;

static char str[SIZE] = "\0";
static int num = 0;

static
void
init_sem(void)
{
	if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("tt4.tsem: sem_create failed\n");
		}
	}
	if (ssem==NULL) {
		ssem = sem_create("ssem", 0);
		if (ssem == NULL) {
			panic("tt4.ssem: sem_create failed\n");
		}
	}
}

static
void
sender_th()
{
  int i;
  while(1){
    kgets(str, SIZE-1);
    str[strlen(str)] = '\0';
    if(strcmp(str,"end") == 0)
      break;
    V(tsem);
    /* delay */
    for(i=0;i<100000;i++){
      ;
    }
    P(tsem);
    num++;
    kprintf("La stringa manipolata è %s.\n",str);
  }
  V(tsem);
  V(ssem);
}

static
void
receiver_th()
{
  unsigned int i;
  while(1){
    P(tsem);
    i=0;
    //kprintf("rec: Ho ricevuto la stringa %s.\n", str);
    if(strcmp(str,"end") == 0)
      break;
    while(i < strlen(str)){
      //kprintf("rec: Carattere corrente %c -> ", str[i]);
      if(str[i] >= 'a' && str[i] <= 'z')
	str[i] = str[i] - 'a' + 'A';
      //kprintf("%c.\n",str[i]);
      i++;
    }
    V(tsem);
    /* delay */
    for(i=0;i<100000;i++){
      ;
    }
  }
  V(ssem);
}

static
void
run_send_rec()
{
  int result;
  result = thread_fork("sender", sender_th, NULL, 0, NULL);
  if (result) {
    panic("Thread fork failed: sender -> %s\n", strerror(result));
  }
  result = thread_fork("receiver", receiver_th, NULL, 0, NULL);
  if (result) {
    panic("Thread fork failed: receiver -> %s\n", strerror(result));
  }
  P(ssem);
  P(ssem);
  kprintf("Il numero di stringhe processate è stato %d.\n", num);
}

int
threadtest4(int nargs, char **args)
{
  (void)nargs;
  (void)args;
  
  init_sem();
  kprintf("Starting thread test 4\n");
  run_send_rec();
  kprintf("\nThread test done.\n");

  return 0;
}

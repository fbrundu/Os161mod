#include <types.h>
#include <lib.h>
#include <wchan.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include "opt-synchprobs.h"

struct _sem_vect{
  struct semaphore* *v;
  int n;
} sem_vect;

static struct sem_vect sv = NULL;

static
void
init_sem(int n)
{
  if(sv == NULL){
    sv = kmalloc(sizeof(struct sem_vect));
    if(sv == NULL)
      panic("tt5.sv: sv allocation failed\n");
    else {
      if (tsem==NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("tt4.tsem: sem_create failed\n");
		}

    }
  }
}

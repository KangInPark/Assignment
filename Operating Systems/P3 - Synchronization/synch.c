#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "proc.h"
#include "synch.h"

int mutex_init(struct mutex_t *mutex)
{
	if(mutex->valid)return -2;
	initlock(&mutex->lock, "mutex");
	mutex->valid = 1;
	mutex->locked = 0;
	mutex->qsize = 0;
	return 0;
}

int mutex_lock(struct mutex_t *mutex)
{
	if(!mutex->valid)return -2;
	if(myproc()->has_mutex)return -3;
	acquire(&mutex->lock);
	if(mutex->locked){
		mutex->queue[mutex->qsize++]= myproc();
		sleep(mutex, &mutex->lock);
	}
	mutex->locked = 1;
	mutex->current = myproc();
	mutex->current->has_mutex = 1;
	release(&mutex->lock);
	return 0;
}

int mutex_unlock(struct mutex_t *mutex)
{
	if(!mutex->valid)return -2;
	if(mutex->current != myproc())return -3;
	acquire(&mutex->lock);
	mutex->locked = 0;
	mutex->current->has_mutex = 0;
	mutex->current = 0;
	if(mutex->qsize){
		struct proc *p = mutex->queue[0];
		for(int i = 0; i< mutex->qsize -1; i++){
			mutex->queue[i] = mutex->queue[i+1];
		}
		mutex->qsize--;
		p->state = RUNNABLE;
	}
	release(&mutex->lock);
	return 0;
}

int cond_init(struct cond_t *cond)
{
	if(cond->active)return -2;
	initlock(&cond->lock, "cond");
	cond->active = 1;
	cond->qsize = 0;
	return 0;
}

int cond_wait(struct cond_t *cond, struct mutex_t *mutex)
{
	if(!cond->active)return -2;
	if(mutex->current != myproc())return -3;
	acquire(&cond->lock);
	mutex_unlock(mutex);
	cond->queue[cond->qsize++] = myproc();
	sleep(cond, &cond->lock);
	mutex_lock(mutex);
	release(&cond->lock);
	return 0;
}

int cond_signal(struct cond_t *cond)
{
	if(!cond->active)return -2;
	acquire(&cond->lock);
	if(cond->qsize){
		struct proc *p = cond->queue[0];
		for(int i = 0 ; i < cond->qsize -1 ; i++){
			cond->queue[i] = cond->queue[i+1];
		}
		cond->qsize--;
		p->state = RUNNABLE;
	}
	release(&cond->lock);	
	return 0;
}

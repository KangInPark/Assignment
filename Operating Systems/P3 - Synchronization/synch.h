#define NTHREAD 8

struct mutex_t
{
	int valid;
	int locked;
	struct spinlock lock;
	struct proc *current;
	struct proc *queue[NTHREAD-1];
	int qsize;
};

struct cond_t
{
	int active;
	struct spinlock lock;
	struct proc *queue[NTHREAD-1];
	int qsize;
};

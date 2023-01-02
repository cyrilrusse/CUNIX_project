#ifndef __SEMA_H__
#define __SEMA_H__

int sem_wait(int idSem, bool nowait);
int sem_signal(int idSem);

#endif
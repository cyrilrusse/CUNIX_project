#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


int sem_wait(int idSem, bool nowait){
   struct sembuf op;
   op.sem_num = 0;
   op.sem_op = -1;
   op.sem_flg = SEM_UNDO;
   if(nowait)
      op.sem_flg |= IPC_NOWAIT;
   return semop(idSem, &op, 1);
}

int sem_signal(int idSem){
   struct sembuf op;
   op.sem_num = 0;
   op.sem_op = +1;
   op.sem_flg = SEM_UNDO;
   return semop(idSem, &op, 1);
}

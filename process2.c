#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 0x1234
struct SharedData{
    bool isChildDone;
    int multiple;
    int counter;
};

int main(void){
    int shmid = shmget(SHM_KEY, sizeof(struct SharedData),0666);
    if (shmid == -1){
        perror("process2: shmget failed");
        return 1;
    }

    struct SharedData *data = (struct SharedData*) shmat(shmid,NULL,0);
    if (data == (void*)-1){
        perror("process2: shmat failed");
        return 1;
    }

    while ((data->counter) <= 100){
        sleep(1);
    }

    int counter2 = 0;
    int cycle = 0;
    pid_t pid = getpid();

    while (counter2 > -500 && (data->counter) <= 500){
        int m = data->multiple;
        int isMult = ((m != 0) && (counter2 % m ==0)); //checking divisbility
        if (isMult){
            printf("Cycle number: %d - %d is a multiple of %d (child %d) [shared.counter=%d]\n",cycle,counter2,m,pid,data->counter);

        }else {
            printf("Cycle number: %d (child %d) [shared.counter=%d]\n",cycle,pid,data->counter);

        }

        counter2--;
        sleep(1);
        cycle++;
    }

    data -> isChildDone = true;

    if(shmdt(data) == -1){
        perror("process2: shmdt failed");
    }

    return 0;
}
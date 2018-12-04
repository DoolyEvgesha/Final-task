#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define MSGSZ 128


#define LOCK 2
#define CAB 4
#define WOMAN 5
#define MAN 6
#define HUMANZ 7

#define my_assert(ERR_MSG, CONDITION)                                        \
if(!(CONDITION))                                                             \
    {                                                                        \
        fprintf(stderr, ERR_MSG": %s\n", strerror(errno));                   \
        exit(errno);                                                         \
    }

int msgid;
int mslock;
long numSpace;
long W;
long M;

int womenInside = 0;
int menInside = 0;
int peopleInside = 0;

struct message{
    long mtype;           // Тип сообщения
    char mtext[MSGSZ];    // Само сообщение, длиной MSGSZ.
};

static void send_msg(int msgtype, int mode) {
    struct message msg;
    msg.mtype = msgtype;
    //int res = snprintf(msg.mtext, sizeof(msg.mtext), "%d", sex);
    //my_assert("SNPRINTF", res >= 0);
    int res = msgsnd(msgid, (void *) &msg, sizeof(msg.mtext), mode);
    my_assert("MSGSND", res >= 0);
}

int get_msg(int msgtype, int mode) {
    struct message msg;

    ssize_t res = msgrcv(msgid, (void *) &msg, sizeof(msg.mtext), msgtype, mode);
    //my_assert("MSGRCV", res >= 0);
    //printf("res = %d\n", res);

    return res;
}

/*void entrance(){
    printf("START\n");

    int count = 0, countWoman = 0, countMan = 0;
    if(W%numSpace == 0) countWoman = W/numSpace;
    else {
        while(countWoman < (W/numSpace) + 1){
            countWoman++;
        }
    }
    while(count < ((W+M)/numSpace) + 1) {
        count++;
    }
    if(M%numSpace == 0) countMan = M/numSpace;
    else {
        while(countMan < (M/numSpace) + 1){
            countMan++;
        }
    }

    printf("ENTER count = %d, W = %d, M = %d, ", count, countWoman, countMan);
    printf("w = %ld,m = %ld\n ", W, M);
//=================================================================
    for(int i = 0; i < countWoman; i++) {
        char sex = 0;
        if(i != countWoman - 1) {
            for (int j = 0; j < numSpace; j++) {
                sex = get_msg(READY_W);
                printf("ENTER: W %ld ready\n", j + i * numSpace);
                send_msg(IN_W, 0);
                printf("ENTER: W entered\n");
            }
            for(int j = 0; j < numSpace; j++) {
                get_msg(FINISH_W);
                printf("ENTER woman %d finished\n", j);
            }
        }else{
            for (int j = 0; j < (W - (countWoman-1)*numSpace); j++) {
                sex = get_msg(READY_W);
                printf("ENTER: W %ld ready\n", j + i * numSpace);
                send_msg(IN_W, 0);
                printf("ENTER: W entered\n");
            }
            for(int j = 0; j < (W - (countWoman-1)*numSpace); j++) {
                get_msg(FINISH_W);
                printf("ENTER woman %d finished\n", j);
            }
        }
        printf("pack of WOMAN finished\n");
    }

//=================================================================
    for(int i = 0; i < countMan; i++) {
        char sex = 0;
        if(i != countMan - 1) {
            for (int j = 0; j < numSpace; j++) {
                sex = get_msg(READY_M);
                printf("ENTER: M %ld ready\n", j + i * numSpace);
                send_msg(IN_M, 0);
                printf("ENTER: M entered\n");
            }
            for(int j = 0; j < numSpace; j++) {
                get_msg(FINISH_M);
                printf("ENTER man %d finished\n", j);
            }
        }else{
            for (int j = 0; j < (M - (countMan-1)*numSpace); j++) {
                sex = get_msg(READY_M);
                printf("ENTER: M %ld ready\n", j + i * numSpace);
                send_msg(IN_M, 0);
                printf("ENTER: M entered\n");
            }
            for(int j = 0; j < (M - (countMan-1)*numSpace); j++) {
                get_msg(FINISH_M);
                printf("ENTER man %d finished\n", j);
            }
        }
        printf("pack of WOMAN finished\n");
    }

//=================================================================
    printf("FINISH\n");
    exit(EXIT_SUCCESS);
}*/

void human(int index, int sex){

    if(sex == 1){ //WOMAN
        do {
            get_msg(LOCK, 0);
            int res = 0;
            res = get_msg(WOMAN, IPC_NOWAIT);//MSG_COPY
            if (res != 0) {
                send_msg(WOMAN, 0);
                get_msg(MAN, IPC_NOWAIT);
                send_msg(HUMANZ, 0);
                send_msg(LOCK, 0);
                break;
            }
            send_msg(LOCK, 0);
            sleep(1);
        } while(1);

        get_msg(CAB, 0);
        printf("W #%d washing\n", index);
        send_msg(CAB, 0);
        printf("W #%d finished washing\n", index);

        get_msg(LOCK, 0);
        get_msg(HUMANZ, 0);
        int last = get_msg(HUMANZ, IPC_NOWAIT);
        if(!last) {
            send_msg(MAN, 0);
        } else {
            send_msg(HUMANZ, 0);
        }
        send_msg(LOCK, 0);

        exit(EXIT_SUCCESS);
    }
    else if(sex == 2){ //MAN
        do {
            get_msg(LOCK, 0);
            int res = 0;
            res = get_msg(MAN, IPC_NOWAIT);//MSG_COPY
            if (res != 0) {
                send_msg(MAN, 0);
                get_msg(MAN, IPC_NOWAIT);
                send_msg(HUMANZ, 0);
                send_msg(LOCK, 0);
                break;
            }
            send_msg(LOCK, 0);
            sleep(1);
        } while(1);

        get_msg(CAB, 0);
        printf("M #%d washing\n", index);
        send_msg(CAB, 0);
        printf("M #%d finished washing\n", index);

        get_msg(LOCK, 0);
        get_msg(HUMANZ, 0);
        int last = get_msg(HUMANZ, IPC_NOWAIT);
        if(!last) {
            send_msg(WOMAN, 0);
        } else {
            send_msg(HUMANZ, 0);
        }
        send_msg(LOCK, 0);

        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 4)
        exit(EXIT_FAILURE);

    W = strtol(argv[1], NULL, 10);
    M = strtol(argv[2], NULL, 10);
    numSpace = strtol(argv[3], NULL, 10);
    if(numSpace == 0)
        exit(EXIT_FAILURE);

    msgid = msgget(IPC_PRIVATE, 0600);
    my_assert("msgget", msgid != -1);

    pid_t pid = 0;

    for(int i = 0; i < numSpace; i++)
        send_msg(CAB, 0);

    for (int i = 0; i < W; i++) {// forks WOMEN
        pid = fork();
        if (pid == 0) {
            human(i, 1);
        }
    }

    for (int i = W; i < W + M; i++) { //forks for MEN
        pid = fork();
        if (pid == 0) {
            human(i, 2);
        }
    }

    send_msg(LOCK,0);
    send_msg(WOMAN,0);
    send_msg(MAN,0);

    for (int i = 0; i < W + M; i++)
        wait(NULL);

    int res = msgctl(msgid, IPC_RMID, NULL);
    my_assert("msgctl", res >= 0); //IPC_RMID -- Операция удаления очереди сообщений с дескриптором msqid

    return 0;
}

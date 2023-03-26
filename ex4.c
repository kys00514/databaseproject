#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>

int num_send_sig = 1;
int num_recv_sig = 0;
int num_recv_ack = 0;
pid_t pid;

void recv_ack_handler(int sig) {
    if (sig == SIGUSR1) {
        num_recv_ack++;
        printf("receiver: received #%d signal and sending ack\n", num_recv_sig);
        kill(pid, SIGUSR1);
        num_recv_sig++;
        if (num_recv_ack == num_send_sig) {
            kill(pid, SIGINT);
            printf("receiver: total received signal(s): %d\n", num_recv_sig);
            exit(0);
        }
    }
}

void terminate_handler(int sig) {
    if (sig == SIGINT) {
        printf("sender: total remaining signal(s): %d\n", num_send_sig - num_recv_ack);
        exit(0);
    }
}

void sending_handler(int sig) {
    if (sig == SIGALRM) {
        if (num_recv_ack < num_send_sig) {
            printf("sender: sending signal #%d\n", num_recv_ack + 1);
            kill(pid, SIGUSR1);
            alarm(1);
        }
    }
}

void sending_ack(int sig) {
    if (sig == SIGUSR1) {
        kill(getppid(), SIGUSR1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <num_send_sig>\n", argv[0]);
        exit(1);
    }

    num_send_sig = atoi(argv[1]);
    printf("total number of signal(s): %d\n", num_send_sig);

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        signal(SIGUSR1, recv_ack_handler);
        signal(SIGINT, terminate_handler);
        while (1) {
            pause();
        }
    } else {
        signal(SIGUSR1, sending_ack);
        signal(SIGALRM, sending_handler);
        alarm(1);
        while (1) {
            pause();
        }
    }

    return 0;
}

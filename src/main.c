#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "gc/gc.h"
#include "ipc/ipc.h"
#include "ipc/resourceserver.h"
#include "main/ufo.h"

int ARGC = 0;
char** ARGV = NULL;

int PARENT_TO_CHILD[2];
int CHILD_TO_PARENT[2];

void any_sanityCheck(void);
void methodTable_setupMethods(void);

int main(int argc, char* argv[]) {
    ARGC = argc;
    ARGV = argv;
    if (pipe(PARENT_TO_CHILD) == -1 || pipe(CHILD_TO_PARENT) == -1) {
        fprintf(stderr, "Unable to open parent/child pipe\n");
        exit(1);
    }
    srand(time(0));
    //pid_t parentPid = getpid();
    pid_t pid = fork();
    if (pid == 0) {
        // child = UFO evaluator
        char* name = "ufo_evaluator";
        if (prctl(PR_SET_NAME, name, NULL, NULL, NULL) < 0) {
            perror("prctl()");
        }
        //printf("I am the child\n");
        //char string[32] = {0};
        //ipc_readString(PARENT_TO_CHILD[0], 5, string);
        //printf("child read string '%s'\n", string);
        //string[0] = 'h';
        //ipc_writeString(CHILD_TO_PARENT[1], 5, string);
        any_sanityCheck();
        gc_start();
        methodTable_setupMethods();
        ufo_start();
        ufo_stop();
        gc_stop();
        ipc_writeString(CHILD_TO_PARENT[1], 16, "DShutting down\n\0");
        ipc_writeString(CHILD_TO_PARENT[1], 2, "UQ");
    }
    else {
        // parent = resource server
        char* name = "ufo_server";
        if (prctl(PR_SET_NAME, name, NULL, NULL, NULL) < 0) {
            perror("prctl()");
        }
        //printf("I am the parent\n");
        //ipc_writeString(PARENT_TO_CHILD[1], 5, "Hello");
        //char string[32] = {0};
        //ipc_readString(CHILD_TO_PARENT[0], 5, string);
        //printf("parent got sting '%s'\n", string);
        resourceServer(pid, CHILD_TO_PARENT[0], PARENT_TO_CHILD[1]);
    }
}

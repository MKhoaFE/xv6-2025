#include "kernel/types.h"
#include "user/user.h"
int main() {
    int pipe_to_child[2], pipe_to_parent[2];
    char buffer[10];
    if (pipe(pipe_to_child) < 0 || pipe(pipe_to_parent) < 0) {
        fprintf(2, "Cant Create pipe!\n");
        exit(1);
    }

    int pid = fork(); 

    if (pid == 0) { 
        close(pipe_to_child[1]); 
        close(pipe_to_parent[0]);

     
        read(pipe_to_child[0], buffer, 1);
        if (buffer[0] == 'i') {
            printf("%d: Received ping\n", getpid());
        }


        write(pipe_to_parent[1], "o", 1);

        close(pipe_to_child[0]);
        close(pipe_to_parent[1]);
    } else { 
        close(pipe_to_child[0]);  
        close(pipe_to_parent[1]);


        write(pipe_to_child[1], "i", 1);


        read(pipe_to_parent[0], buffer, 1);
        if (buffer[0] == 'o') {
            printf("%d: Recieved pong\n", getpid());
        }

        close(pipe_to_child[1]);  
        close(pipe_to_parent[0]); 
    }

    exit(0);
}
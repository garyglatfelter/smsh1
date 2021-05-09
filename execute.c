/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
{
  int	pid ;
  int	child_info = -1;
  bool run_in_background = false;

  if ( argv[0] == NULL )		/* nothing succeeds	*/
    return 0;

  //get argv size
  int last_index = 0;
  while(argv[last_index] != NULL){
    last_index++;
  }
  last_index--; //back up one to get last index

  if(!strcmp(argv[last_index],"&") ){
    //make last index null; don't want to pass to execvp
    argv[last_index] = realloc(argv[last_index],0);
    run_in_background = true;
  }

  if ( (pid = fork())  == -1 )
    perror("fork");
  else if ( pid == 0 ){ //this is forked child process
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    execvp(argv[0], argv);
    perror("cannot execute command");
    exit(1);
  }
  else { //this is parent process

    if(run_in_background)
      printf("Started process %d in the background\n",pid);
    else{
      if ( waitpid(pid,&child_info,0) == -1 )
        perror("wait");
    }
  }
  return child_info;
}

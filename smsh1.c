/*
 * Project 5 - Simplified shell
 *   Dakota, Gary, Reece
 *
 *
 */
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "smsh.h"
#include <string.h>
#include <sys/wait.h>

int changeDir(char *nextDir);
char *updatePrompt();

int main()
{
  char  *cmdline, *prompt, **arglist;
  int   result;
  void  setup();
  char *cdString = "cd";
  char *exitString = "exit";
  int val;

  prompt = updatePrompt();
  setup();

  while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){

    //first check if any background processes completed.
    int status;
    pid_t cpid = waitpid(-1, &status, WNOHANG);
    if(cpid > 0){
       //a child process finished, print child pid and child return value
      if ( WIFEXITED(status) ){
        printf("Child process %d completed with status: %d\n",
          cpid, WEXITSTATUS(status) );
      }
    }

    //now build arg list from user entry at prompt
    if ( (arglist = splitline(cmdline)) != NULL  ){

      //check if user desires exit
      if (strcmp(arglist[0], exitString)== 0){
        if (arglist[1] != NULL) //user desires exit with return value
          val = atoi(arglist[1]);
        else  //default return value = 0
          val = 0;
        printf("Goodbye..\n");
        freelist(arglist);
        free(cmdline);
        exit(val);
      }

      //check if user desires to change directory
      if (strstr(cmdline, "cd")){
        changeDir(arglist[1]);
        prompt = updatePrompt();
      } else {
      result = execute(arglist);
      freelist(arglist);
      }
    }
    free(cmdline);
  }
  return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
  signal(SIGINT,  SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
  fprintf(stderr,"Error: %s,%s\n", s1, s2);
  exit(n);
}

int changeDir(char *nextDir){
  char cwd[256];
  char *pastDir = "..";
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    perror("getcwd() error");
  else if (strcmp(nextDir, pastDir) == 0){
    if (chdir("..") == -1)   //gary - what about relative like ./projects/prj5/ etc;
      perror("chdir() error");
  } else {
  strcat(cwd, "/");   //gary- what about entries like /usr/class/csci296/ etc.; one that is full path
  strcat(cwd, nextDir);
  if (chdir(cwd) == -1)
    perror("chdir() error");
  }
}

char *updatePrompt(){
  char cwd[256], *usr;
  getcwd(cwd, sizeof(cwd));
  usr =  getlogin();
  strcat(usr, "@myshell:");
  strcat(usr, "\033[0;32m~");
  strcat(usr, cwd);
  strcat(usr, " $ ");
  strcat(usr, "\033[0m");
  return usr;
}

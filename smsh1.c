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
#include <pwd.h>

int changeDir(char *nextDir);
char *updatePrompt();
void rick_grimes();

int main()
{
  char  *cmdline, *prompt, **arglist;
  int   result;
  void  setup();
  char *cdString = "cd";
  char *exitString = "exit";
  int val;
  char *user_home; //home directory for user

  prompt = updatePrompt();
  setup();

  //get home directory of user
  struct passwd *pw = getpwuid( getuid() );
  user_home = pw->pw_dir;

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
        rick_grimes();
        while( waitpid(-1,NULL, WNOHANG) >0 ) //loop until all children waited for
          ;

        exit(val);
      }

      //check if user desires to change directory
      if ( (strcmp(arglist[0], "cd")) == 0){
        if (arglist[1] == NULL) //if no args, return to default
          changeDir(user_home);
        else
          changeDir(arglist[1]);

        prompt = updatePrompt();
      } else {
      result = execute(arglist);
      freelist(arglist);
      }
    }
    free(cmdline);
  }

  rick_grimes();
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

/*
 * changeDir changes current directory
 *
 *  Parameters:
 *    char arry of absolute file path, or relative file path.
 *    note: after arguments parsed by split line, absolute path will lack
 *      leading backslash.
 *  Returns: 0 on succes, -1 on error.
*/
int changeDir(char *nextDir){
  int success = 0;
  char absolute_path[BUFSIZ] = "/";
  if(nextDir != NULL && strlen(nextDir)>0 && nextDir[0] != '.'){
    // if here, not a relative path. need to add backslash that
    //  gets removed from split line.c
    strcat(absolute_path,nextDir);
    printf("absDir: %s\n",absolute_path);
  }
  if(chdir(nextDir) ==-1){
    perror("chdir error");
    success = -1;
  }
  return success;
}

/*
 *  updatePrompt will update the shell prompt to reflect user and current
 *  working directory
 *
 *  Returns: character array of updated prompt.
*/
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


/*
 * Leave no zombies behind
*/
void rick_grimes(){
  kill(0,SIGINT); //kill them all

  while( waitpid(-1,NULL, WNOHANG) >0 ) //loop until all children waited for
    ;
}

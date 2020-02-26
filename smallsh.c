/*
David Passaro
Feb 26 2020
CS344 Winter
Program 3: smallsh.c

Readme also here
 --compile with--->  gcc -std=gnu99 -g -o smallsh smallsh.c  
   */

//includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


//fn declarations
int prompt();
int execute(char **cmds,int *forkNow,pid_t *pids);
void shift(pid_t *pids, int idx);
int checkPids(int *pids);
void checkStatus(int child);
void catchSIGTSTP(int signo);


//global bool for catchSTPSIG
int SIGTSTPBool = 0;//0 is false, greater than 0is true


int main(int argc, char *argv[]){
  struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};//CiTED Lecture
  SIGINT_action.sa_handler = SIG_IGN;//creating sig structs, set handler to ignore
  sigaction(SIGINT, &SIGINT_action,NULL);//set sigactions

  SIGTSTP_action.sa_handler = catchSIGTSTP;//handle function set
  sigfillset(&SIGTSTP_action.sa_mask);//whole set all sigs
  SIGTSTP_action.sa_flags = SA_RESTART;//restart for waits and getline interupts
  sigaction(SIGTSTP, &SIGTSTP_action,NULL);//set sigaction
  
  //prompt has most cmd line fns
  prompt();
  return 0;
}
/*
Purpose
Parameters
Returns
*/
/*
Purpose: catch SIGTSTP signal and change to forground only mode
   & symbol ignored
Parameters: signo number of signal to catch
Returns: void
*/
void catchSIGTSTP(int signo){
  if (SIGTSTPBool == 0){//starts as 0
    char *message = "Entering foreground-only mode\n:";
    write(STDOUT_FILENO, message, 31);//write with byte numbers
    SIGTSTPBool=1;//switch bool  
  }
  else{
      char *message = "Exiting foreground-only mode\n:";
      write(STDOUT_FILENO, message, 30);
      SIGTSTPBool=0;
  }
}

/*
Purpose: propmts user for input and handles parsing and bg pid upkeep 
Parameters: none
Returns: 0 for success or 1 for error
*/
int prompt(){
  char *getBuf;//user input
  char *target ="$$";//for pid expansion
  int forkCount=0;//reset fork controller
  size_t buflen = 2048;//for getline
  size_t chars;
  int numChrs;
  int count;//$$ nums
  int i,j,k;//counters
  int cmdsLen=0;//num words in input cmd
  char *cmds[513];//words input
  char cwd[100];//cur working dir
  int forkNow=0;//fork bool controller
  int childExitStatus=-5;//exit val child
  pid_t pids[30];//background pids
  for (i=0;i<30;i++){pids[i]=NULL;}//init null
  for (i=0;i<513;i++){cmds[i]=NULL;}
  getBuf = (char *)malloc(buflen * sizeof(char));

  while(1){
    forkNow=0;//reset fork flag
    forkCount = 0;
      i=0;
      while(cmds[i]!=NULL){//reset cmds arr
        free(cmds[i]);
        cmds[i]=NULL;i++;
        }
    cmdsLen=0;
    if( getBuf == NULL)
    {
        perror("Allocation of buffer failed");
        exit(1);
    }
  
    write(1,": ", 1);fflush(stdout);//prompt char
    numChrs = getline(&getBuf,&buflen,stdin);//get input here
    getBuf[strcspn(getBuf, "\n")] = '\0';//cited lecture ########  removes \n char
    char* position = strchr(getBuf, ' ');//check for spaces in string getBuf input
    char* tok;
    char c[2] = " ";
    if(position != NULL ){//separate words (found space in str)
      tok = strtok(getBuf,c);
      i=0;//reset i
      while(tok !=NULL){//put five strings in max
        cmds[i]= (char *)malloc((100) * sizeof(char));//alloc arr
        if(cmds[i] ==NULL){
          perror("Alloc of cmds array failed");
        }
        memset(cmds[i],'\0',(100));//init \0
        strcpy(cmds[i],tok);//copy token
        if(SIGTSTPBool==1 && strcmp(cmds[i],"&")==0){//if in foreground mode and have &
          free(cmds[i]);//free & and set null to ignore &
          cmds[i]=NULL;
        }
        tok= strtok(NULL, c);//inc vars
        cmdsLen++;
        i++;
      }
    }
    if(position == NULL){//put getBuf into cmd[0]
      cmds[0]= (char *)malloc( 100* sizeof(char));
      if(cmds[0] ==NULL){
        perror("Alloc of cmds array failed");
      }
        memset(cmds[0],'\0',sizeof(100));
        strcpy(cmds[0],getBuf);
        cmdsLen=1;
    }
    count=0;
    for (k=0;cmds[k]!=NULL;k++){
      if(strstr(cmds[k],target)){//if we found a $$ to expand inc count
        count++;
      }
    }
    if (count!=0){//contains $$
      i=0;
      while(cmds[i]!= NULL){///cite guidence from https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
        char *p;
        int w=0;
        count=0;
        int subLen = strlen(target);
        int oldLen = strlen(cmds[i]);
        char string [100];
        memset(string,'\0',100);
        int q = 0;
        strcpy(string, cmds[i]);//copy over original 
        memset(cmds[i],'\0',100);//clear for result building
        char  pidstr [100];
        snprintf(pidstr,100,"%d",getpid());//turn pid to string
        int pLen = strlen(pidstr); 
        int counterFirst$$ = 0;
        w=0;
        char *ptr=string;
        while(string[q]){//while not at end \0
          if(strstr(string,target)== ptr && counterFirst$$==0)//if beginning is substring 
          {
            strcpy(&cmds[i][w],pidstr);//put in result at front
            w += pLen;//inc char*s
            q += 2;
            counterFirst$$++;
          }
          
          else{
            p=(strstr(&string[q],target));//check for more $$ and set p to addr 
            if(p && (*(p+1)=='$') && (p == (&string[q]))){//if pointer p found $ in string and next char is $ (two in row as in $$) 
             // and p points to where we are in string
              strcat(cmds[i],pidstr);//concat onto cmds i
              q += 2;//inc char*
              w += pLen;
            }
            else{
            cmds[i][w++] = string[q++];//else copy the char
            }
          }
        }
        i++;
      }
    }
    i=0;
    if (getBuf[0]=='#'){//if was comment then check bg list for completions and restart loop (cont)
      checkPids(pids);
      continue;
    }

    else if(( strcmp(getBuf,"exit")==0) ||(strcmp (cmds[0],"exit")==0 && strcmp(cmds[1],"&")==0)){//check for exit (end child processes later)
      free(getBuf);  
       i=0;
      while(cmds[i]!=NULL){
        free(cmds[i]);
        cmds[i]=NULL;i++;
      }//free mem and kill bg pids
      i=0;
      while(pids[i]!=NULL){
        if(pids[i] > 0){
          kill(pids[i], SIGKILL);
        }
        pids[i]=NULL;
        i++; 
      }
      return(0);//return success
    }
    else if((strcmp(cmds[0],"cd")==0 && position == NULL)|| (strcmp (cmds[0],"cd") ==0 && strcmp(cmds[1],"&")==0)){//check for cd or cd &
      if((strcmp(getBuf,"cd")==0)){// if just cd
      }
      chdir(getenv("HOME"));//go home dir
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      checkPids(pids);//check bgs
      continue;
      }
    }
    else if ((strcmp(getBuf,"status")==0 ||(strcmp (cmds[0],"status")==0 && strcmp(cmds[1],"&")==0))){
      checkStatus(childExitStatus);//report last status and check bgs
      checkPids(pids);
      continue;

    }
    if( position!=NULL && strcmp(cmds[0] ,"cd")==0 && strcmp (cmds[1],"&")!=0){//more commands after cd (separate if than from above)
      if(chdir(cmds[1])!=0){perror("chdir failed");}//go to new dir specified     
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      }
      checkPids(pids);
      continue;
    }
    else{//start exec
      forkCount++;
      if(forkCount==50){abort();}//avoid forkbomb
      forkNow=1;
      childExitStatus=execute(cmds,&forkNow,pids);//update childexitstatus
      if (WIFSIGNALED(childExitStatus)){  // cited lecture - check if exit status or if sig and report sig
        int termSignal = WTERMSIG(childExitStatus);
        printf("terminated by signal %d.\n",termSignal);fflush(stdout);
      }
      checkPids(pids);
      continue;
    }
    printf("continue?");fflush(stdout);
  }//while 1 outer      
  free(getBuf);
  return 0;
}//main



/*
Purpose report exit status or sig killed 
Parameters child exit status val
Returns nothing, prints only
*/
void checkStatus(int childExitStatus){//prints out status from last finished cmd
  if(childExitStatus ==-5){printf("exit status 0.\n");fflush(stdout);}//not changed 
  else if (WIFEXITED(childExitStatus)){//if exit status
    int exitStatus = WEXITSTATUS(childExitStatus);
    printf("exit value %d.\n",exitStatus);fflush(stdout);//print it
  }
  else if (WIFSIGNALED(childExitStatus)){//signal
    int termSignal = WTERMSIG(childExitStatus);
    printf("terminated by signal %d.\n",termSignal);fflush(stdout);
  }//prints sig
}



/*
Purpose looks at child pids for completions
Parameters pids array
Returns int
*/
int checkPids(int *pids){
  int i=0;
  int flag = 0;
 // int reports[30];
  int stat = -5;
  int childExitStatus=-5;
  pid_t childPID_actual;//code cited lecture
  i=0;
  while(pids[i] != NULL){
    childPID_actual = waitpid(pids[i],&childExitStatus,WNOHANG);//wait without pausing ret 0 if nothing finished
    if (childPID_actual != 0){
      printf("Background pid %d is done: ",childPID_actual);fflush(stdout);//print before exit status
      checkStatus(childExitStatus);//report status
      shift(pids, i);//send index to shift to move down arr
      flag = 1;
    }
    i++;
  }
  return flag;
}


/*
Purpose shifts array down to cover hole of completed child 
Parameters arr of pids
Returns void
*/
void shift(int *pids, int idx){
  if (idx == 0 && pids[idx+1] == NULL ){pids[0] = NULL;}//only one
  else {
    while(pids[idx] != NULL){//while next is not null (not last) shift list down 
      pids[idx] = pids[idx+1];
      idx++;
    }
  }
}


/*
Purpose exec commands 
Parameters  cmds arr, fork bool, pids arr
Returns int exit val
*/
int execute(char **cmds,int *forkNow, pid_t * pids){
  //cited lecture code
  int j=0;
  int i=0;
  int result=-5;
  int l=0;
  pid_t actualPid;
  int bg = 0;
  pid_t spawnPid=-5;
  int childExitStatus=-5;
  int inpIdx=100;
  int outIdx = 100;
  //for(i=0;i<30;i++){}
  //printf("   about to enter redirect\n");fflush(stdout);
  //while(cmds[i]!=NULL){printf("%s    %d\n", cmds[i],i);fflush(stdout);i++;}
 
  spawnPid= fork();// fork here
  l=0;
  while(cmds[l] != NULL){ // check for bg process before deleting commands
    if (strcmp(cmds[l],"&" )==0 && cmds[l+1]==NULL){//if bg & and it's last cmd 
      bg = 1;
      free(cmds[l]);
      cmds[l] = NULL; //set flag and null
    }
    l++;
  }
  switch(spawnPid){//switch on pids after fork
    case -1:  {perror("Hull breach!\n");exit(1);break;}
    case 0: {//child
      struct sigaction SIGTSTP_action = {0};//CiTED Lecture
      SIGTSTP_action.sa_handler = SIG_IGN;//ign in child
      sigaction(SIGTSTP, &SIGTSTP_action,NULL);
      i=0;
      while(cmds[i] != NULL){//will be null at end of values
        if(bg==1){//background
          struct sigaction SIGINT_action = {0};//CiTED Lecture     if bg than ign
          SIGINT_action.sa_handler = SIG_IGN;
          sigaction(SIGINT, &SIGINT_action,NULL);
          int targetFD = open("/dev/null", O_WRONLY );//Write only open
          result = dup2(targetFD,1);//reroute stdout to null
          if(result == -1){
            perror("ERROR dup2( , 1) failed\n");//fflush(stdout);
          } 
          int targetFD2 = open("/dev/null", O_RDONLY );//reroute sdtin to null
          result = dup2(targetFD2,0);
          if(result == -1){
            perror("ERROR dup2( , 2) failed\n");//fflush(stdout);
          }
        }
        else{//foreground
          struct sigaction SIGINT_action = {0};//CiTED Lecture
          SIGINT_action.sa_handler = SIG_DFL;//default
          sigaction(SIGINT, &SIGINT_action,NULL);
        }
        if(strcmp(cmds[i], ">")==0){//look for output redirect
          outIdx = i;
          int targetFD = open(cmds[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0640);//Write only
          if(targetFD== -1){
            printf("ERROR file not opened\n");fflush(stdout);
            childExitStatus=1;//set status
            return 1;
          }else{
            result = dup2(targetFD,1);
            if(result == -1){//set stdout
              perror("ERROR dup2() failed\n");//fflush(stdout);
              //exit(2);
            }
          }
        }
        else if(strcmp(cmds[i], "<")==0){//look forinput redirect
          inpIdx = i;
          int targetFD = open(cmds[i+1], O_RDONLY );//read only
          if(targetFD== -1){
            printf("ERROR file not opened\n");fflush(stdout);
            exit(3);
          }
          else{
            result= dup2(targetFD,0);//stdin
            if(result == -1){
              printf("ERROR dup2() failed\n");fflush(stdout);
            }
          }
        }
        i++;
      }//while cmds !=null
      l=0;
      if(inpIdx < outIdx ){//figure which index to start at getting rid of < or > 
        j=inpIdx;
        while (cmds[j] != NULL){
          free(cmds[j]);
          cmds[j]=NULL;
          j++;
        }
      }
      else if (inpIdx > outIdx ){
        j=outIdx;
        while (cmds[j] != NULL){
          free(cmds[j]);
          cmds[j]=NULL;
          j++;
        }
      }
      if(*forkNow==1 ){
        i=0;
        if (execvp(*cmds, cmds)<0){//exec here
          perror("Exec failure");
          exit(2);break;
        }
      }
      *forkNow=0;//flag safe forking
    }//case 0
    default:{//parent

      if(bg==0){//foreground
        //block sigtstp before wait
        sigset_t x;
        sigemptyset (&x);
        sigaddset(&x, SIGTSTP);
        sigprocmask(SIG_BLOCK, &x, NULL);//block SIGTSTP for wait
        actualPid=waitpid(spawnPid, &childExitStatus,0);
        sigprocmask(SIG_UNBLOCK, &x, NULL);//unblock
      }
      else if(bg == 1){//bg set 
        if(pids[0] == NULL){//first process start arr
          pids[0]=spawnPid;      //set in list
          printf("Background pid is (%d)\n",spawnPid);fflush(stdout);
        }
        else{//not first process
          i=0;
          while(pids[i] != NULL){
            if(pids[i+1] == NULL){//if next one is null put in last spot
              pids[i+1] = spawnPid;
              printf("Background pid is (%d) i= %d\n",spawnPid,i);fflush(stdout);
              break;
            }
            i++;
          }
        }
      }
      
    }
  }
  if(bg == 0){
  return childExitStatus;//return if foreground
  }
}
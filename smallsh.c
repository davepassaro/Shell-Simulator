/*to do 
1 kk redir bg
2 kk $$
3 signals 
4 kill at exit  
5 kk getline (and wait ) interruption restart
6 open permissions
 RESTART and the global 
And to set and an ignore/default in the child */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>



int prompt();
int execute(char **cmds,int *forkNow,pid_t *pids);
void shift(pid_t *pids, int idx);
int checkPids(int *pids);
void checkStatus(int child);
void catchSIGTSTP(int signo);


//global for catchSTPSIG
int SIGTSTPBool = 0;//0 is false, greater than 0is true

int main(int argc, char *argv[]){
  struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};//CiTED Lecture
  SIGINT_action.sa_handler = SIG_IGN;
  sigaction(SIGINT, &SIGINT_action,NULL);

  SIGTSTP_action.sa_handler = catchSIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = SA_RESTART;
  sigaction(SIGTSTP, &SIGTSTP_action,NULL);

  prompt();
  return 0;
}

void catchSIGTSTP(int signo){
  if (SIGTSTPBool == 0){
    char *message = "Entering foreground-only mode\n:";
    write(STDOUT_FILENO, message, 31);
    SIGTSTPBool=1;
  }
  else{
      char *message = "Exiting foreground-only mode\n:";
      write(STDOUT_FILENO, message, 30);
      SIGTSTPBool=0;
  }
}
int prompt(){
  char *getBuf;
  char *target ="$$";

  int forkCount=0;
  size_t buflen = 2048;//for getline
  size_t chars;
  int numChrs;
  int count;//$$ nums
  //int multiWords=0; 
  int i,j,k;
  int cmdsLen=0;//num words in input cmd
  char *cmds[513];
  char cwd[100];
  int forkNow=0;
  int childExitStatus=-5;
  pid_t pids[30];
  for (i=0;i<30;i++){pids[i]=NULL;}
  for (i=0;i<513;i++){cmds[i]=NULL;}
  getBuf = (char *)malloc(buflen * sizeof(char));

  while(1){
    forkNow=0;//reset fork flag
    forkCount = 0;
   /* for (i=0;i<cmdsLen;i++){//reset array and cmdLen
      if(cmds[i] != NULL){
        free(cmds[i]);
        cmds[i] =NULL;
      }
    }*/
       i=0;
      while(cmds[i]!=NULL){
        free(cmds[i]);
        cmds[i]=NULL;i++;
        }
     // p= (strstr(cmds[i],target))
    cmdsLen=0;
    if( getBuf == NULL)
    {
        perror("Allocation of buffer failed");
        exit(1);
    }
  
    write(1,": ", 1);fflush(stdout);
    numChrs = getline(&getBuf,&buflen,stdin);
    getBuf[strcspn(getBuf, "\n")] = '\0';//cited lecture ########
    //strcpy(cmds[0],getBuf);
    char* position = strchr(getBuf, ' ');//check for spaces in string getBuf input
    //printf("pos = %s",position);fflush(stdout);
    char* tok;
    char c[2] = " ";
    if(position != NULL ){//separate words (found space in str)
      tok = strtok(getBuf,c);
      //printf("   strtoking   ");fflush(stdout);

      i=0;//reset i
      //put words in array cmds separated by ' ' spaces
      while(tok !=NULL){//put five strings in max
       // printf(" %s toc\n",tok);fflush(stdout);
        cmds[i]= (char *)malloc((100) * sizeof(char));
        if(cmds[i] ==NULL){
          perror("Alloc of cmds array failed");
        }
        memset(cmds[i],'\0',(100));
        strcpy(cmds[i],tok);
        if(SIGTSTPBool==1 && strcmp(cmds[i],"&")==0){
          free(cmds[i]);
          cmds[i]=NULL;
        }
        //printf(" %s \n",cmds[i]);fflush(stdout);
        tok= strtok(NULL, c);
        cmdsLen++;
        i++;
      }
    }
    if(position == NULL){//put getBuf into cmd[0]
      //printf("one word \n");fflush(stdout);
      cmds[0]= (char *)malloc( 100* sizeof(char));
      if(cmds[0] ==NULL){
        perror("Alloc of cmds array failed");
      }
       // printf("here1");fflush(stdout);
        memset(cmds[0],'\0',sizeof(100));
        strcpy(cmds[0],getBuf);
        cmdsLen=1;
    }
    count=0;
    for (k=0;cmds[k]!=NULL;k++){
      if(strstr(cmds[k],target)){
        count++;
      }
    }
    if (count!=0){
      i=0;
      while(cmds[i]!= NULL){///cite guidence from https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
        char *p;
        int w=0;
        count=0;
        int subLen = strlen(target);
        int oldLen = strlen(cmds[i]);
       // for (w=0;cmds[w]!=0;w++){
         // if(strstr(&cmds[i][w],target)){
        //    count++;
       //   }
      //  }
      // p= (strstr(cmds[i],target))
        char string [100];
        memset(string,'\0',100);
        int q = 0;
    // printf( "   %s 2   ",cmds[0]);fflush(stdout);

        strcpy(string, cmds[i]);//copy over original 
       // printf( "   %s string\n",string);fflush(stdout);
        memset(cmds[i],'\0',100);//clear for result building
        //printf("1st  %s cmds  ",cmds[i]);fflush(stdout);

        char  pidstr [100];
        snprintf(pidstr,100,"%d",getpid());
        //printf("   %s   pstr   ",pidstr);
        int pLen = strlen(pidstr); 
        int counterFirst$$ = 0;
        w=0;
        char *ptr=string;
        while(string[q]){
          //printf(" start q=%c ",string[q]);fflush(stdout);
          //printf(" 0(w=%d   q =%d, cmds=%s)   ",w,q,cmds[i]);fflush(stdout);

          if(strstr(string,target)== ptr && counterFirst$$==0)//beginning is substring
          {
            //printf(" 1 i=%d w=%d cmdsiw %s ",i,w,cmds[i]);fflush(stdout);
        //  printf(" 1  ");fflush(stdout);

            strcpy(&cmds[i][w],pidstr);//put in result at front
            w += pLen;
            q += 2;
            //count--;
          // printf(" 1( cmds=%s)   ",w,q,cmds[i]);fflush(stdout);
            //break;
            counterFirst$$++;
          }
          
          else{
          // printf(" 2  ");fflush(stdout);

            p=(strstr(&string[q],target));
          // printf(" p= %c  ",*p);fflush(stdout);

            if(p && (*(p+1)=='$') && (p == (&string[q]))){//if pointer p found $ in string and next char is $ (two in row) 
          //  printf(" 2  ");fflush(stdout);    // and p points to where we are in string
              strcat(cmds[i],pidstr);
              q += 2;
              w += pLen;
            // printf(" 3 cmds= %s  ",cmds[0]);fflush(stdout);

            }
            else{
          //  printf("   3  ");fflush(stdout);

            cmds[i][w++] = string[q++];//else copy the char
            }
          }
          //printf("    %c = c     ",cmds[i][w]);fflush(stdout);
        }
      // cmds[i][w]='\0';
      // printf("  %s cmds  ",cmds[i]);fflush(stdout);
      // if(p){

        //}
        i++;
      }
    }
    i=0;
  //  while (cmds[i]!=NULL){printf("cmds= %s\n",cmds[i]);fflush(stdout);i++;}
    if (getBuf[0]=='#'){
      //printf("skip\n");fflush(stdout);
      checkPids(pids);
      continue;
    }

    else if(( strcmp(getBuf,"exit")==0) ||(strcmp (cmds[0],"exit")==0 && strcmp(cmds[1],"&")==0)){//check for exit (end child processes later)
     // printf("exiting");fflush(stdout);
      free(getBuf);  
       i=0;
      while(cmds[i]!=NULL){
        free(cmds[i]);
        cmds[i]=NULL;i++;
        
      }
      return(0);//add in exit closing bg processes
    }
    else if((strcmp(cmds[0],"cd")==0 && position == NULL)|| (strcmp (cmds[0],"cd") ==0 && strcmp(cmds[1],"&")==0)){//check for cd
      if((strcmp(getBuf,"cd")==0)){
      //printf("cding");fflush(stdout);
      }
      chdir(getenv("HOME"));//go home
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      //printf("cwd= %s\n",cwd);fflush(stdout);
      checkPids(pids);
      continue;
      }
    }
    else if ((strcmp(getBuf,"status")==0 ||(strcmp (cmds[0],"status")==0 && strcmp(cmds[1],"&")==0))){
      //printf("statusing");fflush(stdout);
      checkStatus(childExitStatus);//didn't find new finished status so
      checkPids(pids);
      continue;

    }

      //printf("cmds =  %d\n",cmdsLen);fflush(stdout);
      //for (i=0;i<cmdsLen;i++){printf("%s    %d\n", cmds[i],i);fflush(stdout);}
    


    if( position!=NULL && strcmp(cmds[0] ,"cd")==0 && strcmp (cmds[1],"&")!=0){//more commands after cd (separate if than from above)
     // printf("dif dir  %s",cmds[1]);fflush(stdout);
      if(chdir(cmds[1])!=0){perror("chdir failed");}//go to new dir     
      if(getcwd(cwd,sizeof(cwd)) != NULL){
        //printf("cwd= %s\n",cwd);fflush(stdout);
      }
      checkPids(pids);

      continue;
    }
    else{//start exec
      //printf("   execing  \n ");fflush(stdout);
      
      forkCount++;
      if(forkCount==50){abort();}//avoid forkbomb
      forkNow=1;
      childExitStatus=execute(cmds,&forkNow,pids);//update childexitstatus
      //printf("here2");fflush(stdout);
      /*if(childExitStatus ==-5){printf("exit status 0.\n");fflush(stdout);}
      else if (WIFEXITED(childExitStatus)){
    int exitStatus = WEXITSTATUS(childExitStatus);
    printf("exit value %d.\n",exitStatus);fflush(stdout);
  }
  else*/
      if (WIFSIGNALED(childExitStatus)){
        int termSignal = WTERMSIG(childExitStatus);
        printf("terminated by signal %d.\n",termSignal);fflush(stdout);
      }

      checkPids(pids);

      continue;
    }
    printf("continue?");fflush(stdout);

         //actualPid=waitpid(-1, &childExitStatus,WNOHANG);
  }//while 1 outer      
  free(getBuf);
  return 0;
}//main

void checkStatus(int childExitStatus){//prints out status from last finished cmd
  if(childExitStatus ==-5){printf("exit status 0.\n");fflush(stdout);}
  else if (WIFEXITED(childExitStatus)){
    int exitStatus = WEXITSTATUS(childExitStatus);
    printf("exit value %d.\n",exitStatus);fflush(stdout);
  }
  else if (WIFSIGNALED(childExitStatus)){
    int termSignal = WTERMSIG(childExitStatus);
    printf("terminated by signal %d.\n",termSignal);fflush(stdout);
  }
}
int checkPids(int *pids){
  int i=0;
  int flag = 0;
 // int reports[30];
  int stat = -5;
  int childExitStatus=-5;
  pid_t childPID_actual;//code cited lecture
  //for (i=0;i<30;i++){reports[i]= NULL;})
  //printf("ckpid?");fflush(stdout);
  
  i=0;
  while(pids[i] != NULL){
    childPID_actual = waitpid(pids[i],&childExitStatus,WNOHANG);
    if (childPID_actual != 0){
      printf("Background pid %d is done: ",childPID_actual);fflush(stdout);//print before exit status
      checkStatus(childExitStatus);
      shift(pids, i);//send index to shift to move down arr
      flag = 1;
    }
    //printf("Child i = %d is pid = %d",i, pids[i]);fflush(stdout);

    i++;
  }
  return flag;
}
void shift(int *pids, int idx){
  if (idx == 0 && pids[idx+1] == NULL ){pids[0] = NULL;}//only one
  else {
    while(pids[idx] != NULL){//while next is not null (not last)
      pids[idx] = pids[idx+1];
      idx++;
    
    }
  }
}






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
 
  spawnPid= fork();
  l=0;
  while(cmds[l] != NULL){ // check for bg process before deleting commands
    if (strcmp(cmds[l],"&" )==0 && cmds[l+1]==NULL){
      bg = 1;
      //printf(" first bg ");fflush(stdout);
      free(cmds[l]);
      cmds[l] = NULL; 

    }
    l++;
  }
  switch(spawnPid){
    case -1:  {perror("Hull breach!\n");exit(1);break;}
    case 0: {//child
     // i=0;while(cmds[i]!=NULL){printf("%s    %d\n", cmds[i],i);fflush(stdout);i++;}
      //set default sig action
      struct sigaction SIGINT_action = {0};//CiTED Lecture
      SIGINT_action.sa_handler = SIG_DFL;
      sigaction(SIGINT, &SIGINT_action,NULL);

      struct sigaction SIGTSTP_action = {0};//CiTED Lecture
      SIGTSTP_action.sa_handler = SIG_IGN;
      sigaction(SIGTSTP, &SIGTSTP_action,NULL);
      i=0;
      while(cmds[i] != NULL){//will be null at end of values
       // printf("%s found at i=%d",cmds[i],i);fflush(stdout);
        if(bg==1){
          int targetFD = open("/dev/null", O_WRONLY );//Write only
          result = dup2(targetFD,1);
          if(result == -1){
            perror("ERROR dup2( , 1) failed\n");//fflush(stdout);
            //exit(2);
          } 
          int targetFD2 = open("/dev/null", O_RDONLY );
          result = dup2(targetFD2,2);
          if(result == -1){
            perror("ERROR dup2( , 2) failed\n");//fflush(stdout);
            //exit(2);
          }
        }
        if(strcmp(cmds[i], ">")==0){//look for output redirect
          //printf("\n%s file to open\n",cmds[i+1]);fflush(stdout);
          outIdx = i;
          int targetFD = open(cmds[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);//Write only
          if(targetFD== -1){
            printf("ERROR file not opened\n");fflush(stdout);
            childExitStatus=1;
            return 1;
          }else{
           // printf(" file open suc    ");fflush(stdout);
          
            result = dup2(targetFD,1);
            if(result == -1){
              perror("ERROR dup2() failed\n");//fflush(stdout);
              //exit(2);
            }
          }
        }
        else if(strcmp(cmds[i], "<")==0){//look forinput redirect
          //printf("%s file to input",cmds[i+1]);fflush(stdout);
          inpIdx = i;
          int targetFD = open(cmds[i+1], O_RDONLY , 0644);//read only !!!!!!!!!!!!!!!!!!!!!!!change back!!!!!!!!!!!!!!!!!!!!!!!!!
          if(targetFD== -1){
            printf("ERROR file not opened\n");fflush(stdout);
            exit(3);
          }
          else{
            //printf(" file open suc");fflush(stdout);
          
            result= dup2(targetFD,0);
            if(result == -1){
              printf("ERROR dup2() failed\n");fflush(stdout);
              //return 1;
            }
          }
        }
       // printf("\ninpIdx = %d     outIdx = %d   i=%d\n",inpIdx,outIdx,i);fflush(stdout);
        i++;
      }//while cmds !=null
      l=0;

      if(inpIdx < outIdx ){
        //printf("inpIdx=%d > outIdx= %d",inpIdx, outIdx);fflush(stdout);

        j=inpIdx;
        while (cmds[j] != NULL){
          free(cmds[j]);
          cmds[j]=NULL;
          j++;
        }
      }
      else if (inpIdx > outIdx ){
        //printf("inpIdx=%d < outIdx=%d",inpIdx,outIdx);fflush(stdout);
       
        j=outIdx;
        while (cmds[j] != NULL){
          free(cmds[j]);
          cmds[j]=NULL;
          j++;
        }
      }
      if(*forkNow==1 ){
       // printf("    actual exec  ->new cmds ");fflush(stdout);
        i=0;
        //while(cmds[i]!=NULL){printf("\n2---%s    %d\n", cmds[i],i);fflush(stdout);i++;}
        if (execvp(*cmds, cmds)<0){
          perror("Exec failure");
          exit(2);break;
        }
      }
      *forkNow=0;//flag safe forking
      //exit(0);
    }//case 0
    default:{
      if(bg==0){
         actualPid=waitpid(spawnPid, &childExitStatus,0);
        //printf("Parent: %d: Child(%d) terminated\n",getpid(),actualPid);fflush(stdout);
      }
      else if(bg == 1){
        //actualPid=waitpid(-1, &childExitStatus,WNOHANG);
        //printf(" bg ");fflush(stdout);
        if(pids[0] == NULL){//first process start arr
          pids[0]=spawnPid;      
          printf("Background pid is (%d)\n",spawnPid);fflush(stdout);
        }
        else{//not first process
          i=0;
          while(pids[i] != NULL){
            if(pids[i+1] == NULL){//if next one is null p
              pids[i+1] = spawnPid;
              printf("Background pid is (%d) i= %d\n",spawnPid,i);fflush(stdout);
              break;
            }
            i++;
          }
        }
      }
      i=0;
      while(pids[i] != NULL){
       // printf("Child i= %d is %d\n",i, pids[i]);fflush(stdout);
        i++;
      }
    }
  }
  if(bg == 0){
  return childExitStatus;}
  //printf("here1");fflush(stdout);
}
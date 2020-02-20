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




int main(int argc, char *argv[]){
  prompt();
  return 0;
}

int prompt(){
  char *getBuf;
  int forkCount=0;
  size_t buflen = 2048;//for getline
  size_t chars;
  int numChrs;
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
    for (i=0;i<cmdsLen;i++){//reset array and cmdLen
      if(cmds[i] != NULL){
        free(cmds[i]);
        cmds[i] =NULL;
      }
    }
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
        cmds[i]= (char *)malloc(sizeof(tok) * sizeof(char));
        if(cmds[i] ==NULL){
          perror("Alloc of cmds array failed");
        }
        memset(cmds[i],'\0',sizeof(tok));
        strcpy(cmds[i],tok);
        //printf(" %s \n",cmds[i]);fflush(stdout);
        tok= strtok(NULL, c);
        cmdsLen++;
        i++;
      }
    }
    else if(position == NULL){//put getBuf into cmd[0]
     // printf("one word \n");fflush(stdout);
      cmds[0]= (char *)malloc(sizeof(getBuf) * sizeof(char));
      if(cmds[0] ==NULL){
        perror("Alloc of cmds array failed");
      }
        memset(cmds[0],'\0',sizeof(tok));
        strcpy(cmds[0],getBuf);
    }

    if (getBuf[0]=='#'){
      //printf("skip\n");fflush(stdout);
      checkPids(pids);
      continue;
    }

    else if(( strcmp(getBuf,"exit")==0) ||(strcmp (cmds[0],"exit")==0 && strcmp(cmds[1],"&")==0)){//check for exit (end child processes later)
     // printf("exiting");fflush(stdout);
      free(getBuf);  
      for (i=0;i<cmdsLen;i++){//reset array and cmdLen
      if(cmds[i] != NULL){
        free(cmds[i]);
        cmds[i] =NULL;
        }
      }
      return(0);//add in exit closing bg processes
    }
    else if((strcmp(cmds[0],"cd")==0 && position == NULL)|| (strcmp (cmds[0],"cd") ==0 && strcmp(cmds[1],"&")==0)){//check for cd
      if((strcmp(getBuf,"cd")==0)){
      //printf("cding");fflush(stdout);
      }
      chdir(getenv("HOME"));//go home
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      printf("cwd= %s\n",cwd);fflush(stdout);
      checkPids(pids);
      continue;
      }
    }
    else if ((strcmp(getBuf,"status")==0 ||(strcmp (cmds[0],"exit")==0 && strcmp(cmds[1],"&")==0))){
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
        printf("cwd= %s\n",cwd);fflush(stdout);
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
  while(cmds[l] != NULL){ // check for bg process before deleting commands
    if (strcmp(cmds[l],"&") == 0){
      bg = 1;
      //printf(" first bg ");fflush(stdout);
      free(cmds[l]);
      cmds[l] = NULL; 
      /*int result = dup2(targetFD,1);
      if(result == -1){
        perror("ERROR dup2() failed\n");//fflush(stdout);
        //exit(2);
      }int result = dup2(targetFD,1);
      if(result == -1){
        perror("ERROR dup2() failed\n");//fflush(stdout);
        //exit(2);
      }*/
    }
    l++;
  }
  spawnPid= fork();

  switch(spawnPid){
    case -1:  {perror("Hull breach!\n");exit(1);break;}
    case 0: {//child
     // i=0;while(cmds[i]!=NULL){printf("%s    %d\n", cmds[i],i);fflush(stdout);i++;}
      i=0;
      while(cmds[i] != NULL){//will be null at end of values
       // printf("%s found at i=%d",cmds[i],i);fflush(stdout);

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
          
            int result = dup2(targetFD,1);
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
          
            int result= dup2(targetFD,0);
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

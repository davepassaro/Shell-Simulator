/* cat userinput_adv.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
  
int prompt();
int execute(char **cmds,int *forkNow);
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
  for (i=0;i<513;i++){cmds[i]=NULL;}
  getBuf = (char *)malloc(buflen * sizeof(char));

  while(1){
    forkNow=0;//reset fork flag
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
    char* tok;
    char c[2] = " ";

    if (getBuf[0]=='#'){
      printf("skipped\n");fflush(stdout);
      continue;
    }

    else if(( strcmp(getBuf,"exit")==0)){//check for exit (end child processes later)
      printf("exiting");fflush(stdout);
      free(getBuf);  

      return(0);//add in exit closing bg processes
    }
    else if( strcmp(getBuf,"cd")==0){//check for cd
      printf("cding");fflush(stdout);
      chdir(getenv("HOME"));//go home
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      printf("cwd= %s\n",cwd);fflush(stdout);
      continue;
      }
    }
    else if (strcmp(getBuf,"status")==0){

    }
    else if(position != NULL ){//separate words (found space in str)
      tok = strtok(getBuf,c);
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
      printf("cmds =  %d\n",cmdsLen);fflush(stdout);
      for (i=0;i<cmdsLen;i++){printf("%s    %d\n", cmds[i],i);fflush(stdout);}
    }
    else if(position == NULL){//put getBuf into cmd[0]
      cmds[i]= (char *)malloc(sizeof(getBuf) * sizeof(char));
      if(cmds[i] ==NULL){
        perror("Alloc of cmds array failed");
      }
        memset(cmds[i],'\0',sizeof(tok));
        strcpy(cmds[i],getBuf);
    }

    if( position!=NULL && strcmp(cmds[0] ,"cd")==0){//more commands after cd (separate if than from above)
     // printf("abtchdir  %s",cmds[1]);fflush(stdout);
      if(chdir(cmds[1])!=0){perror("chdir failed");}//go to new dir     
      if(getcwd(cwd,sizeof(cwd)) != NULL){
        printf("cwd= %s\n",cwd);fflush(stdout);
      }
      continue;
    }
    else{//start exec
      forkCount++;
      if(forkCount==50){abort();}//avoid forkbomb
      forkNow=1;
      int childExitStatus=execute(cmds,&forkNow);
      continue;
    }
    printf("continue?");fflush(stdout);

  }//while 1 outer
  free(getBuf);
  return 0;
}//main

int execute(char **cmds,int *forkNow){
  //cited lecture code
  
  pid_t spawnPid=-5;
  int childExitStatus=-5;

  spawnPid= fork();
  switch(spawnPid){
    case -1:  {perror("Hull breach!\n");exit(1);break;}
    case 0: {
      if(*forkNow==1){
        if (execvp(*cmds, cmds)<0){
          perror("Exec failure");
          exit(2);break;
        }
      }
      *forkNow=0;//flag safe forking
    }
    default:{
      pid_t actualPid=waitpid(spawnPid, &childExitStatus,0);
      printf("Parent: %d: Child(%d) terminated\n",getpid(),actualPid);fflush(stdout);
    }
  }
  return childExitStatus;
}

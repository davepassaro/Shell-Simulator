/* cat userinput_adv.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
  
int prompt();

int main(int argc, char *argv[]){
  prompt();
  return 0;
}

int prompt(){
  char *getBuf;
  size_t buflen = 2048;//for getline
  size_t chars;
  int numChrs;
  //int multiWords=0; 
  int i,j,k;
  int cmdsLen=0;//num words in input cmd
  char *cmds[513];
  char cwd[100];
  for (i=0;i<513;i++){cmds[i]=NULL;}
  getBuf = (char *)malloc(buflen * sizeof(char));

  /*for( i=0;i<512;i++){
    cmds[i]= (char *)malloc(30 * sizeof(char));
    memset(cmds[i],'\0',30);
    if(cmds[i] ==NULL){
      perror("Alloc of cmds array failed");
    }
  }*/
  while(1){
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
    //printf("%s getline\n",getBuf);fflush(stdout);
    /*if (getBuf[0]=='e' && getBuf[1]=='c' && getBuf[2]=='h' && getBuf[3]=='o'){
      printf("echo!\n");fflush(stdout);
      }
    //check for echo cmd and comments first
    else */if (getBuf[0]=='#'){
      printf("skipped\n");fflush(stdout);
      continue;
    }

    else if(( strcmp(getBuf,"exit")==0)){//check for exit (end child processes later)
      printf("exiting");fflush(stdout);
      free(getBuf);  
      /*for (i=0;i<512;i++){
        free(cmds[i]);
        cmds[i] =NULL;
      }*/
  
      return(0);//add in exit closing bg processes
    }
    else if( strcmp(getBuf,"cd")==0){//check for cd
      printf("cding");fflush(stdout);
      chdir(getenv("HOME"));//go home
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      printf("cwd= %s\n",cwd);fflush(stdout);
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
    if( cmds[0] !=NULL && strcmp(cmds[0] ,"cd")==0){//more commands after cd (separate if than from above)
     // printf("abtchdir  %s",cmds[1]);fflush(stdout);
      if(chdir(cmds[1])!=0){perror("chdir failed");}//go to new dir     
      if(getcwd(cwd,sizeof(cwd)) != NULL){
        printf("cwd= %s\n",cwd);fflush(stdout);
      }
    }

  }//while 1 outer
  free(getBuf);
  return 0;
}//main



  //write(1,numChrs, 3);fflush(stdout);
    //write(1," characters were read.\n",24);fflush(stdout);
    //write(1,"You typed: ",11);fflush(stdout);
    //printf("%s\n",getBuf);fflush(stdout);//REMOVE PRINTF
    //printf("chr= %c\n",getBuf[0]);fflush(stdout);//REMOVE PRINTF
    //write(1,&getBuf,numChrs);fflush(stdout);
    //write(1,"\n",1);
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
  size_t buflen = 60;
  size_t chars;
  int numChrs;
  int multiWords=0; 
  int i,j,k;
  int cmdsLen=1;
  char *cmds[512];
  char cwd[100];

  getBuf = (char *)malloc(buflen * sizeof(char));

  for( i=0;i<512;i++){
    cmds[i]= (char *)malloc(30 * sizeof(char));
    memset(cmds[i],'\0',30);
    if(cmds[i] ==NULL){
      perror("Alloc of cmds array failed");
    }
  }
  while(1){
    if( getBuf == NULL)
    {
        perror("Allocation of buffer failed");
        exit(1);
    }
  
    write(1,": ", 1);fflush(stdout);
    numChrs = getline(&getBuf,&buflen,stdin);
    getBuf[strcspn(getBuf, "\n")] = '\0';
    strcpy(cmds[0],getBuf);
    char* position = strchr(getBuf, ' ');
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

    else if(( strcmp(getBuf,"exit")==0)){//check for exit
      //printf("exiting");fflush(stdout);
      free(getBuf);  
      for (i=0;i<512;i++){
        free(cmds[i]);
        cmds[i] =NULL;
      }
  
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
      //*position = '\0';//change to end first word
      //multiWords=1;
      //printf("abt to strtoc");fflush(stdout);
      tok = strtok(getBuf,c);
      i=0;
      while(tok !=NULL){//put five strings in max
       // printf(" %s toc\n",tok);fflush(stdout);
        strcpy(cmds[i],tok);
        //printf(" %s \n",cmds[i]);fflush(stdout);
        tok= strtok(NULL, c);
        cmdsLen++;
        i++;
      }
    }
    if( strcmp(cmds[0] ,"cd")==0){//more commands after
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
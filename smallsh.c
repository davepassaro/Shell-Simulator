/* cat userinput_adv.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
  
int cmds();

int main(int argc, char *argv[]){
  cmds();
}

int cmds(){
  char *getBuf;
  size_t buflen = 60;
  size_t chars;
  int numChrs;
  int multwords=0; 
  int i,j,k;
  char cmd1[200];
  char cmd2[200];
  char cmd3[200];
  char cmd4[200];
  char cmd5[200];
  char cmd6[200];

  while(1){
    multword=0;
    getBuf = (char *)malloc(buflen * sizeof(char));
    if( getBuf == NULL)
    {
        perror("Allocation of buffer failed");
        exit(1);
    }
    //write(1,numChrs, 3);fflush(stdout);
    //write(1," characters were read.\n",24);fflush(stdout);
    //write(1,"You typed: ",11);fflush(stdout);
    //printf("%s\n",getBuf);fflush(stdout);//REMOVE PRINTF
    //printf("chr= %c\n",getBuf[0]);fflush(stdout);//REMOVE PRINTF
    //write(1,&getBuf,numChrs);fflush(stdout);
    //write(1,"\n",1);
    write(1,": ", 1);fflush(stdout);
    numChrs = getline(&getBuf,&buflen,stdin);
    getBuf[strcspn(getBuf, "\n")] = '\0';
    strcpy(cmd1,getBuf);
    char* position = strchr(getBuf, ' ');
    
    if (getBuf[0]=='e' && getBuf[0]=='c' && getBuf[0]=='h' && getBuf[0]=='o'){printf("echo";fflush(stdout);}
    //check for echo cmd and comments first
    else if (getBuf[0]=='#'){
     // printf("skipped\n");fflush(stdout);
      continue;
    }
    else if(position != NULL && getBuf){//separate words (found space in str)
      *position = '\0';//change to end first word
      multword=1;
      strcpy(cmd1,getBuf);//put first word here
      *position = ' ';//change back to space
      for (i=0;i<strlen(getBuf);i++){//at location after space put that word in
        if (getBuf[i]== ' '){//first word passed
          k=0
          for ( j=(1+i);j<strlen(getBuf);j++){
            cmd2[k]=getBuf[j];
            k++;
          }//more than one word in input

    
    else if(strcmp(getBuf,"exit")==0){//check for exit
      //printf("exiting");fflush(stdout);
      free(getBuf);  
      return(0);//add in exit closing bg processes
    }
    else if(multword==0 && strcmp(getBuf,"cd")==0){//check for cd
     // printf("cding");fflush(stdout);
      chdir(getenv("HOME"));//go home
      char cwd[100];
      if(getcwd(cwd,sizeof(cwd)) != NULL){
      //printf("cwd= %s\n",cwd);fflush(stdout);
      }
    else if(multiword==1 && strcmp(getBuf,"cd")==0){
      chdir(getenv());//go to new dir
    }
     

  }//while 1 outer
  free(getBuf);
  return 0;
}//main
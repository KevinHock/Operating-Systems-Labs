#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum;

void *runner(void *param);//void *param);

int main(int argc, char *argv[]){
  pthread_t tid;
  //pthread_attr_t attr;
  
  if(argc != 2){
    fprintf(stderr, "usage a.out<integer value>\n");
    return -1;
  }
  if(atoi(argv[1])<0){
    fprintf(stderr,"Argument %d must be non-negative\n",atoi(argv[1]));
    return -1;
  }

//Get default attributes  
//  pthread_attr_in(&attr);

//create thread
  pthread_create(&tid,NULL,runner,argv[1]);

//now wait for thread to exit
  pthread_join(tid,NULL);
  printf("sum = %d\n",sum);
}

void *runner(void *param){
  int i,upper=atoi(param);

  sum=0;
  if(upper>0){
    for(i = 1; i <= upper; i++){
      sum+=i;
    }
  }
  
  pthread_exit(0);
}

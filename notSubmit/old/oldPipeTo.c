/*********************************************************************
 * WORKING HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *********************************************************************************************************/
int pipeTo(process_list *pl){
  // Fork twice until next == NULL then wrap and pass to parse command
  
  pid_t pid1, pid2;
  int i;
  
  // For all processes:
  for(i = 0; i < pl->num_processes; i++){

    // Create aset of file descriptors:
    int fd[2];
    
    // Create a pipe;
    pipe(fd);
    
    // Create a fork for the first process:
    pid1 = fork();
    
    // FIRST CHILD HERE
    if(pid1 == 0){
      // Child process closes input side of pipe
	    close(fd[0]);
      
      //Make all output go to fd[1]
      dup2(fd[1],1);
      
      //Black box
      ls2file();
      
      exit(0);
    }
        
    // PARENT HERE:
    else{
		  // Parent process closes up output side of pipe 
      close(fd[1]);
      
      //Make all fd[0] go to STDIN
      dup2(fd[0],0);
		  
      pid_t pid = fork();
      
      //The child's code
      if(pid == 0){
        // Black box next arg
        next 
            
      }else{
        wait(NULL);
        
        //Close file
        close(fd[0]);
      }
	  }
  }
  return 0;
}

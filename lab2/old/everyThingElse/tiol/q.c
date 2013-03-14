me=malloc(sizeof(char)*(firstSpace-lastSlash+1));
 67         if(exename==NULL){
 68         //      fprintf(2,"Nothing like a bad malloc for 
 70                 
 71         strncpy(exename,cmd[lastSlash],firstSpace-lastSlash);

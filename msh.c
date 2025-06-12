//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h> /* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8

int totalsum=0;
// files in case of redirection
char filev[3][64];
//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
printf("****  Exiting MSH **** \n");
//signal(SIGINT, siginthandler);
exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
//reset first
for(int j = 0; j < 8; j++)
argv_execvp[j] = NULL;

int i = 0;
for ( i = 0; argvv[num_command][i] != NULL; i++)
argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
/**** Do not delete this code.****/
int end = 0;
int executed_cmd_lines = -1;
char *cmd_line = NULL;
char *cmd_lines[10];

if (!isatty(STDIN_FILENO)) {
cmd_line = (char*)malloc(100);
while (scanf(" %[^\n]", cmd_line) != EOF){
if(strlen(cmd_line) <= 0) return 0;
cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
strcpy(cmd_lines[end], cmd_line);
end++;
fflush (stdin);
fflush(stdout);
}
}

/*********************************/

char ***argvv = NULL;
int num_commands;


while (1)
{
int status = 0;
int command_counter = 0;
int in_background = 0;
signal(SIGINT, siginthandler);
// Prompt
write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));
// Get command
//********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
executed_cmd_lines++;
if( end != 0 && executed_cmd_lines < end) {
command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
}
else if( end != 0 && executed_cmd_lines == end) {
return 0;
}
else {
command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
}
//************************************************************************************************

/************************ STUDENTS CODE ********************************/
 
     
 
  if (command_counter > 0){//if more than 1 command is introduced
		if (command_counter > MAX_COMMANDS){//if the number of commands is greater than 8, we display an error
			printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
		}else{
			// Print command
			//print_command(argvv, filev, in_background);
		}
 		int input,output,error,std_in,std_out,std_err;
		std_in=dup(0);//we save the standard input so that we can come back to it later
		std_out=dup(1);//we save the standard output so that we can come back to it later
		std_err=dup(2);//we save the standard error so that we can come back to it later
 
           
                 
		getCompleteCommand(argvv, 0);//get the first command
		if ((strcmp(argv_execvp[0],"mycalc")==0)&&(command_counter==1)){//check if we need to perform the internal command mycalc
         if (strcmp(argv_execvp[2],"add")==0){//case we need to perform addition operation
           int operator1= atoi(argv_execvp[1]);//get the operators and transform them into int type variable
           int operator2 = atoi(argv_execvp[3]);
           int sum= operator1+operator2;//perform addition
           totalsum=totalsum + sum;//"Acc" addition with previous results
           char env_var[20];
           sprintf(env_var, "%d",totalsum);//covert the data totalsum into a string and stores it in the buffer env_varr
           const char *x=env_var;
           char *result;
           setenv("Acc",x,1);//update enviroment variable
           result=getenv("Acc");//store Acc in a variable to print it
           char message[100];
           snprintf(message, 100, "[OK] %d + %d = %d; Acc %s\n", operator1, operator2, sum, result);
           write(STDERR_FILENO,message,strlen(message));
           
          }else if (strcmp(argv_execvp[2],"mod")==0){
           //case mod operation
           int operator1= atoi(argv_execvp[1]);//get the operators and transform them into int type variable
           int operator2 = atoi(argv_execvp[3]);
           int module = operator1 % operator2;// perform the modulus
           int quotient = operator1 / operator2;//perform the division to find the cuotient
          char message[100]; 
          snprintf(message, 100,"[OK] %d %% %d = %d; Quotient %d \n", operator1, operator2, module, quotient);
          write(STDERR_FILENO,message,strlen(message));
         }else {//not matching syntax structure introduced
            char message[100]; 
            snprintf(message, 100,"[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n ");
           write(STDERR_FILENO,message,strlen(message));
         }



 		}else if ((strcmp(argv_execvp[0],"mycp")==0)&&(command_counter == 1)){
				int filetoread,filetowrite, nread; //declare variables
				int BUFFERSIZE = 1024; //establish the dimension of the message
				char buff [BUFFERSIZE]; // declare the vector os characters with length BUFFERSIZE
				if((argv_execvp[1]==NULL)||(argv_execvp[2]==NULL)){ // if the user doesn't introduce bothe files, then print ERROR
						char message[100];
						snprintf(message, 100,"[ERROR] The structure of the command is mycp <original_file><copied_file>\n");
					        write(STDOUT_FILENO,message,strlen(message));
					        return -1;
						
				}
				if((filetoread =open(argv_execvp[1],O_RDONLY,0666))<0){ // print an error if there is any opening the file to read
						char message[100];
						snprintf(message, 100,"[ERROR] Error opening original file\n");
					        write(STDOUT_FILENO,message,strlen(message));
						return -1;
				}
				if((filetowrite=open(argv_execvp[2],O_WRONLY| O_CREAT, 0666))<0){ // open, create or truncate the file used for writing, if any error was found print a message
						write(STDOUT_FILENO, "[ERROR]", strlen("[ERROR]")); //standard error output
						perror("\nError opening the file\n");
						return -1; // exit
				}
				while((nread=read(filetoread,buff,BUFFERSIZE))>0){ // while loop to read
						if((write(filetowrite,buff,nread))<0){ // if any problems while writing, print an error
							perror("\nError writing in the file");
							return -1; // exit
						}
				}
				if(nread<0){ //print a message if the is any error while reading the file
						perror("\nError reading the file\n");
						return -1; // exit and return to while loop
				}
				int close1 = close(filetoread); // close file to read
				int close2 = close(filetowrite); // close file to write
				if ((close1<0)||(close2<0)){ // if any of the close gives as an error, print a message
						perror("Error when closing the file\n ");
						return -1; // exit and return to while loop
				}
				 //otherwise, the program has succeeded therefore print a message
				char message[100];
				snprintf(message, 100,"[OK] Copy has been successful between %s and %s\n", argv_execvp[1], argv_execvp[2]);
				write(STDERR_FILENO,message,strlen(message));


   //execution of external commands
  }else if (command_counter==1){//just one command is introduced
			if (strcmp(filev[0],"0")!=0){//input redirection
					input=open(filev[0],O_RDONLY,0666);//obtain the input fd
					close(0);// close the standard input
					dup2(input,0);//set the new input
			}
			if (strcmp(filev[1], "0")!=0){
					output=open(filev[1],O_WRONLY|O_CREAT,0666);//obtain the output fd                    
					close(1);//close the standard output
					dup2(output,1);//set the new output
			}
			if (strcmp(filev[2],"0")!=0){
					close(2);//close the standard error
					error==open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666);
			}

			int pid,status;
			pid=fork();
			switch(pid){
					case -1:
							perror("fork");
							return -1;

					case 0://child
							getCompleteCommand(argvv, 0);//get the command to be executed
							execvp(argv_execvp[0], argv_execvp);//execute
							//we do not reach this line if the execution is successfull
							return -1;

					default:
							if(in_background==0){//if we do not execute the command in background we should wait                  
									if ((wait(&status))==-1){//the parent waits for the child to finish
											perror("error while waiting");
											return -1;
									}
  						}
					}// END PARENT
					dup2(std_in,0);
					dup2(std_out,1);  
					dup2(std_err,2);

  	}else{ //MULTIPLE COMMANDS CASE 
				if (strcmp(filev[0],"0")!=0){//input redirection
						input=open(filev[0],O_RDONLY,0666);//obtain the input fd
						close(0);// close the standard input
						dup2(input,0);//set the new input
				}
				if (strcmp(filev[1], "0")!=0){
						output=open(filev[1],O_RDWR|O_CREAT|O_TRUNC,0666);//obtain the output fd                    
						close(1);//close the standard output
						dup2(output,1);//set the new output
				}
				if (strcmp(filev[2],"0")!=0){
						close(2);//close the standard error
						error==open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666);
				}
			 int auxiliar_counter=0;//number of times a command has been performed    
       int status;

       while (auxiliar_counter!=command_counter){//still need to execute a command
					 int Pipe[2];//create a pipe to communicate between child and father
					 pid_t child;//create a child per commmand
       
					 if (pipe(Pipe) < 0) {//check no error in the cration of the pipe
						  perror("\nerror pipe\n");
						  exit(-1);
           }
                       
		       child = fork();//duplicate parent process

		       if(auxiliar_counter==command_counter-1){
							switch(child){
									case -1:
											perror("\nfork\n");//error while duplicating the process
											return -1;

									case 0://child
              				getCompleteCommand(argvv, auxiliar_counter);// get  the command to be executed
                   		execvp(argv_execvp[0], argv_execvp);//execute
		                  perror("\nerror while executing last\n");//if we reach these line the execution fails
		                  return -1;  
                   
								 default:
											if ((wait(&status))==-1){//the parent wait for the child
													perror("\nerror default\n");//error while waiting
													return -1;
											}
            		 } //END PARENT
                 auxiliar_counter=auxiliar_counter+1;//last command has been executed
           }else{// case is not last command we executed and communicate the result of the execution with the parent
             	switch(child){
                	case -1:
                     perror("\nfork\n");//error while duplicating the process
                     return -1;
                     
                  case 0://child
             				 getCompleteCommand(argvv, auxiliar_counter);//get the command to be excuted
			               if ((close(STDOUT_FILENO))<0){//close standard input
	               				 perror("\nerror while closing standar out put\n");
                     }
                     if ((dup(Pipe[1]))<0){//duplicate the output of the pipe
                     		 perror("\nerror while duplicating\n");
                     }
                     if((close(Pipe[1]))<0){//close the output of the pipe
                     		 perror("\nerror while closing\n");
                     }
                     if ((close(Pipe[0]))<0){//close the iput of the pipe
                     		 perror("\nerror while closing\n");
                     }             
                  	 execvp(argv_execvp[0], argv_execvp);//execute the command
                     perror("\nerror while executing\n");//if it reachs this lines an error while executing
                     return -1;  
                             
								 default:
										if(in_background==0){//if it is not executed in background we should wait                    
												if ((wait(&status))==-1){//the parent waits for the child
														perror("\nerror while waiting\n");
														return -1;
											 }
        						}
              	 }//END PARENT 
                 if((close(STDIN_FILENO))<0){//close the standard input
                		perror("\nerror while closing\n");
                 }
                 if ((dup(Pipe[0]))<0){//duplicate the input of the pipe
               		 perror("\nerror while duplicating\n");
                 }
                 if ((close(Pipe[0]))<0){//close the input of the pipe
                		perror("\nerror while closing\n");
                 }
                 if ((close(Pipe[1]))<0){//close the output of the pipe
                  	perror("\nerror while closing\n");
                 }
                 auxiliar_counter=auxiliar_counter+1;//one command has been executed
                 }
               }   
               dup2(std_out,1); //reset standard input, output and error
             	 dup2(std_in,0);
               dup2(std_err,2);
                   
        			}//close else (case n commands)
					}//close if (more than one command)
				}//Close main while loop
				return 0;
		}//Close the maindup2(std_out,1);  
                



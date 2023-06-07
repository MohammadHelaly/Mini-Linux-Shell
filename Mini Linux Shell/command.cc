
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "command.h"
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
int flag=0;
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
}

void
Command::print()
{
	
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

/*
void log(int sig){
time_t x;
time(&x);

FILE *file=fopen("log","a");
fprintf(file,"%s",ctime(&x) );
fclose(file);
return;
}
*/

/*FILE* log;
void log(int sig) {
    int status, childPID;
    childPID = wait(&status);
    time_t currentTime = time(0);

    if (childPID >= 0) {
		FILE *file=fopen("log","a");
        fprintf(file, "child process ID: %d    termination status: %d    termination time: %s\n", childPID, status, ctime(&currentTime));
		fclose(file);
    }
}
*/

void
Command::execute()
{	
	
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	
	if(strcmp(_simpleCommands[0]->_arguments[0] ,"exit" ) ==0){
	printf("Goodbye.\n");
	exit(0);
	flag=1;
	}
	
	//signal(SIGCHLD,log);	
	
	if(strcmp(_simpleCommands[0]->_arguments[0] ,"cd" ) ==0){
		if(_simpleCommands[0]->_arguments[1]){
			chdir(_simpleCommands[0]->_arguments[1]);
			printf("Current directory: %s.\n",_simpleCommands[0]->_arguments[1]);
		}
		else{
			chdir(getenv("HOME"));
			printf("Current directory: HOME.");
		}
	}

	if(!flag){

	print();

	pid_t pid;
	int status,defaultin=0,defaultout=0,defaulterr=0,out=0,in=0,err=0;
	int fdpipe [_numberOfSimpleCommands-1][2];	

	defaultin = dup(0);
	defaultout = dup(1);
	defaulterr = dup(2);

	if(_errFile){
		if(!write)
		err = creat(_errFile,0777);
		else
		err = open(_errFile,O_WRONLY|O_APPEND|O_CREAT,0777);
		dup2(err,2);
		dup2(err,1);
	}
	if(_outFile){
		if(!write)
		out = creat(_outFile,0777);
		else{
		out = open(_outFile,O_WRONLY|O_APPEND|O_CREAT,0777);
		}
		dup2(out,1);
	}
	if(_inputFile){
		in = open(_inputFile,O_RDONLY);
		dup2(in,0);
	}
	for(int i=0 ; i<_numberOfSimpleCommands;i++){

		if(_numberOfSimpleCommands>1){
			pipe(fdpipe[i]);
			if(i==0){
				dup2(fdpipe[0][1],1);
				}
			if(i>=1 && i < _numberOfSimpleCommands-1){
				dup2(fdpipe[i-1][0],0);
				dup2(fdpipe[i][1],1);
			}
			if(i==_numberOfSimpleCommands-1){
				dup2(_outFile?out:defaultout,1);
				dup2(fdpipe[i-1][0],0);
			}
		}
	char *com=_simpleCommands[i]->_arguments[0];
	char *arg[_simpleCommands[i]->_numberOfArguments + 1];
	int j = 0;
	for(j=0;j<_simpleCommands[i]->_numberOfArguments;j++){
	arg[j] = _simpleCommands[i]->_arguments[j] ;
	}
	arg[j] = NULL;

	//signal(SIGCHLD,log);
	pid=fork();

	if(pid==0){

	if(_numberOfSimpleCommands>1){
	
	if(i<_numberOfSimpleCommands-1){
		close(fdpipe[i][1]);
	}
	
	if(i>0){ 
		close(fdpipe[i-1][0]);
	}
	}	

	execvp(com,arg);

	}	
	if(pid!=0){
		if(_numberOfSimpleCommands>1){
			if(i<_numberOfSimpleCommands-1){
				close(fdpipe[i][1]);
				}
			if(i>0){ 
					close(fdpipe[i-1][0]);
				}
			}
		if(!_background){
			/*waitpid(0,NULL,0);*/
			int status,cid;
			time_t currentTime = time(0);
			cid=waitpid(pid,&status,0);
	FILE *file=fopen("log","a");
        fprintf(file, "    - child process ID: %d    termination status: %d    termination time: %s\n", cid, status, ctime(&currentTime));
		fclose(file);
			//signal(SIGCHLD,log);
		}
	}
	}

	dup2(defaultin,0);
	dup2(defaultout,1);
	dup2(defaulterr,2);

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
    }
    /*else
    exit(0);*/
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{
	time_t currentTime = time(0);
	FILE *file=fopen("log","a");
        fprintf(file, "\n- Session: %s\n\n",ctime(&currentTime));
	fclose(file);
	signal(SIGINT,SIG_IGN);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}


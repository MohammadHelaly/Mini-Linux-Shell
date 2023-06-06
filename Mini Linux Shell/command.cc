
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
int cd=0;
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
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void log(int sig){
time_t x;
time(&x);

FILE *file=fopen("log","a");
fprintf(file,"%s",ctime(&x) );
fclose(file);
return;
}
void
Command::execute()
{	
	
	
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	
	
	if(strcmp(_simpleCommands[0]->_arguments[0] ,"exit" ) ==0){
	printf("GoodBye\n");
	exit(0);
	cd=1;
	}
	
	signal(SIGCHLD,log);	
	
	if(strcmp(_simpleCommands[0]->_arguments[0] ,"cd" ) ==0){

	if(_simpleCommands[0]->_arguments[1]){
	chdir(_simpleCommands[0]->_arguments[1]);
	}
	else{
		chdir("/home/usefwalid");
	}
	
	}

	
	if(!cd){
	
	int i=0;
	char *com=_simpleCommands[0]->_arguments[0];
	char *arg[_simpleCommands[0]->_numberOfArguments + 1];

		for(i=0;i<_simpleCommands[0]->_numberOfArguments;i++){
	arg[i] = _simpleCommands[0]->_arguments[i] ;

	}
	arg[i] = NULL;
	int pid=fork();
	if(pid==0){
	
	
	if(_outFile!=NULL){
	int file=open(_outFile, O_RDWR | O_CREAT ,0666);
	dup2(file,1);
	
	}	
	if(_inputFile!=NULL){
	int file=open(_inputFile, O_RDWR ,0666);
	dup2(file,0);	
	}	
	execvp(com,arg);
	

	}	
	if(pid!=0){

	waitpid(0,NULL,0);

	}
	

	
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
    }
    else
    exit(0);

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
	signal(SIGINT,SIG_IGN);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}


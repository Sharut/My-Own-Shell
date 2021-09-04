#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include<readline/readline.h> 
#import<readline/history.h> 
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

const int max_history_size = 5;
char *history[max_history_size];
int history_count=0;
int total_history_count=0;


// ## Initialise the terminal by clearing screen and a brain map
void design(){ 
	system("clear");

    printf("               __,--\"\"\"\"\"\"\"\"\"--,.\n");
    printf("         _ -\'\"                  _\\ ^-,_\n");
    printf("      ,-\"                     _/        \\_\n");
    printf("   ,'                    /_    |           \\\n");
    printf("   ,\'                    /_    |           \\\n");
    printf("  /           _____,--\"\"\"     /         )   \\\n");
    printf(" /           /               /         (     |\n");
    printf("|          /                /      )         |\n");
    printf("|         /                |                  \\\n");
    printf("(     (_/\\      )                 /            \\\n");
    printf(" \\        \\_          ____,====\"\"\"    /        |\n");
    printf("  \\                /\"                /\"\"       |\n");
    printf("   \\_          _,-\" |___,-\'--------\'\"          |\n");
    printf("     \"`------\"\"   --\"                 ,-\'      /\n");
    printf("            /                     ---\"        /\n");
    printf("            \\___/          __,-----,___       )\n");
    printf("                \\     ,--\'\"============\"\"\"\"-\'\"\\\n");
    printf("                 \"-\'\" |  |=================/\n");
    printf("                      /___\\===============/\n");
    printf("                      /  |=============/\"\n");
    printf("                       \\   \\_________,-\"\n");
    printf("                       |   |\n");
    printf("                      |   | \n");
    printf("\n\n");
    printf("      *************** ATTENTION ****************\n");
    printf("      ******************************************\n");
    printf("      *******This is Sharut's Bash Shell *******\n");
    printf("      ******************************************\n");
    printf("      ******************************************\n");
    printf("\n\n");
}   

// ## Function to replace a word (keyword) in the given phrase 
char* replaceWord(char* phrase, char* key, char* replace) {
	char* result;
    result = (char*)malloc(100); 
    int counter = 0; 
    while (*phrase) { 
        if (strstr(phrase, key) == phrase) { 
            strcpy(&result[counter], replace); 
            phrase += strlen(key);
            counter += strlen(replace);            
        } 
        else
            result[counter++] = *phrase++; 
    } 
    result[counter] = '\0'; 
    return result; 
}


// ## Function to add a command to history
void add_to_history(char* command){

	//# Add to the array if current history length < max_history_size
	if(history_count < max_history_size){
		history[history_count] = command;
		history_count++;
		total_history_count = history_count;
	}

	//# Shift all elements of the array to left by 1 step and append the new command
	else{
		for(int i=1;i<max_history_size;i++){
			history[i-1] = history[i];
		}
		history[history_count-1]=command;
		total_history_count++;
	}

}

//## Function to print history
void print_history(){
	for(int i=0;i<history_count;i++){
		if(total_history_count>5) printf("%d %s\n", total_history_count - max_history_size + 1 + i, history[i]);
		else printf("%d %s\n", i+1, history[i]);
	}
}

//## Function to implement change director command
void change_directory(char* initial_dir, char** path, char** parsed){
	
	//# Check if user didn't enter any directory 
	if(parsed[1]==NULL){
		printf("cd: Please enter name of a directory !\n");
		return;
	}
    char pwd[2000];

    //# Get path of the current working directory
    getcwd(pwd, sizeof(pwd)); 
    char* path_with_space;

    //# Check if the directory entered by user doesn't exist
    DIR *dr = opendir(replaceWord(parsed[1], "~", pwd)); 
    if (dr) closedir(dr);
    else if (dr == NULL){ 
        printf("cd: No such file or directory: %s\n", parsed[1]); 
        return; 
    } 

    //# Change directory
    chdir(replaceWord(parsed[1], "~", initial_dir));
    getcwd(pwd, sizeof(pwd));
    *path = replaceWord(pwd, initial_dir, "MTL458:~");
    return;

}

// ## Function to print path of the current working directory
void present_directory(char* initial_dir){
	char pwd[2000]; 
	char* path;
    getcwd(pwd, sizeof(pwd));
    path = replaceWord(pwd , initial_dir, "~");
    printf("%s\n", path); 

}

// ## In-build command execution function
void basic_commands(char** parsed){
	int rc = fork(); 
	if (rc < 0) { 
        printf("Fork Failed \n"); 
        return; 
    } 
    else if (rc == 0){
    	if (execvp(parsed[0], parsed) < 0) { 
            printf("Couldn't execute command\n"); 
            exit(1); // If execvp fails, the child process will continue onward. Hence the child will process the next command, rather than the parent.
        }
        return;
    }  
    else
        wait(NULL);
        return; 
}


char** read_input(char* initial_dir, char** path, char *str, char** parsed){
    int i, index, flag=0, quote_count = 0, curr_ind=0;
    char* spaced_path;  
    for (i = 0; i < 1000; i++) { 
    	spaced_path = malloc(100*sizeof(char));
    	flag=0; quote_count=0;
        if(str == NULL || strlen(str) == 0){
        	parsed[i]=NULL;
        	break;}

        //# parse the string and maintain a count of " so far. If count = 0, break on space
        //# Whenever count is even, break on space else do not break        
        if (str[0] != '\0') {
            for(index=0; index < strlen(str); index++){
            	if(str[index]=='\"'){ flag=1; quote_count = 1 - quote_count;}
            	if(flag){ if(isspace(str[index]) && !quote_count){ break;}}
            	else{ if(isspace(str[index])) {break;}}
            }

            curr_ind = 0;
            for(int j=0;j<index; j++){
            	if(str[j]=='\"')  curr_ind--;
            	else spaced_path[curr_ind] = str[j];
            	curr_ind++;
            }

            spaced_path[curr_ind] = '\0';
            parsed[i] = spaced_path;
            str = str + index+1;
        }
 
        if (parsed[i] == NULL) break; 
    }

    // # Implement CD Command
    if(strcmp(parsed[0],"cd")==0){
        change_directory(initial_dir, path, parsed);
    }

    // # Implement PWD Command
    else if(strcmp(parsed[0],"pwd")==0) present_directory(initial_dir);
    
    // # Implement EXIT Command	
    else if(strcmp(parsed[0],"exit")==0){
    	printf("See you later...\n");
    	exit(0);
    }

    // # Implement HISTORY Command
    else if(strcmp(parsed[0],"history")==0){
    	print_history();
    	add_to_history("history");
    }
    
    // # Implement CLEAR Command
    else if(strcmp(parsed[0],"clear")==0) system("clear");
    else basic_commands(parsed);
    return parsed;
}


int main(void)
{
    char initial_dir[2000];
    getcwd(initial_dir, sizeof(initial_dir)); 
    char* input, *parsedArgs[1000];
    char** separated_args; 
    char *path = "MTL458:~";
    design();
    while(1){   
        printf("%s$ ", path); 	
    	input = readline(""); 
        if(strlen(input)==0) continue;

        //## Accept input command of length less than equal to 128. Maximumlimit of readline() is 256 characters. 
        else if(strlen(input)>128){
        	printf("Error: Exceeding input buffer length (128 characters) !\n");
        	continue;
        }

        //## Add the command "history" to history, after displaying history
        if(strcmp(input, "history")) add_to_history(strdup(input));
        read_input(initial_dir, &path, input, parsedArgs);
    }

    return 0;
}
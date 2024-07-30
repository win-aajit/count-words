#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char* argv[]){

	if(argc !=(1||2)){
		printf("Error: Expected one argument, found %d", argc-1);
		return -1;
	}
	char c;
	int count = 0;
	int numLines = 0;
	int numChars = 0;
	int numWords = 0;
	bool prevWhite = true;
	int fd = open(argv[1],O_RDONLY);
	if(fd==-1){
		printf("Error: the given filename \"%s\" does not exist", argv[1]);
		return -1;
	}

	while( read(fd, &c, 1) > 0){
		count++;
		if(c == '\n') {
			numLines++;
			numChars++;
			prevWhite = true;
		}
		else if(isspace(c)){
			numChars++;
			prevWhite = true;
		}
		else if(isprint(c)){
			if(prevWhite){
				numWords++;
			}
			numChars++;
			prevWhite = false;
		}
	}
	if(count == 0){
		printf("0 0 0 %s\n", argv[1]);
		return 0;
		}
	if(c != '\n') numLines++;
	close(fd);
	printf("%d %d %d %s\n",numLines, numWords, numChars, argv[1]);
	return 0;
}

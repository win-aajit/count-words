#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>

#define CHUNKSIZE 10240

typedef struct{
	char* word;
	int count;
}WordCount;

int processFile(const char* filename, WordCount* wordCounts, int* totalCount){
	int fd = open(filename, O_RDONLY);

	if(fd == -1){
		printf("Error");
		return;
	}

	char buf [1];
	char * wordBuffer = malloc(CHUNKSIZE * sizeof(char));
	int bufferSize = 10240;
	int wordIndex = 0;

	while(read(fd, buf, 1) > 0) {
		if(isalpha(buf[0]) || buf[0] == '\''){
			wordBuffer[wordIndex++] = buf[0];
		}
		else if(buf[0] == '-' && wordIndex > 0 && isalpha(wordBuffer[wordIndex-1])){
			wordBuffer[wordIndex++] = buf[0];
		}
		else if(wordIndex > 0){ //end of word
			if(wordBuffer[wordIndex - 1] == '-'){
				wordBuffer[wordIndex--] = '\0';
			}
			else{
				wordBuffer[wordIndex] = '\0';
			}

			int i;
			for(i = 0; i < *totalCount; i++){
				if(strcmp(wordCounts[i].word, wordBuffer) == 0){
					wordCounts[i].count++;
					break;
				}
			}

			if(i == *totalCount){
				wordCounts[i].word = strdup(wordBuffer);
				wordCounts[i].count = 1;
				(*totalCount)++;
			}

			wordIndex = 0;
		}
	}
	close(fd);
}

void processDirectory(const char* dirname, WordCount* wordCounts, int* totalCount){
	DIR* dir = opendir(dirname);

	if(dir == NULL){
		perror("Error opening dir");
		return;
	}

	struct dirent *entry;

	while ((entry = readdir(dir))!= NULL) {
		if(entry->d_type == DT_REG && strcmp(entry->d_name + strlen(entry->d_name)-4, ".txt") == 0){
			char path[1024];
			int x = snprintf(path, sizeof(path), "%s/%s\n", dirname, entry->d_name);
			write(STDOUT_FILENO, path, x);
			processFile(path, wordCounts, totalCount);
		}
		else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..")!= 0){
			char subdir[1024];
			int x = snprintf(subdir, sizeof(subdir), "%s/%s\n", dirname, entry->d_name);
			printf("1");
			write(STDOUT_FILENO, subdir, x);
			processDirectory(subdir, wordCounts, totalCount);

		}
	}
	closedir(dir);
}


int cmpWords(void* x, void* y){
	WordCount* wordX = (WordCount*)x;
	WordCount *wordY = (WordCount *)y;
    if (wordX->count == wordY->count) {
        return strcmp(wordX->word, wordY->word);
    }
    return wordY->count - wordX->count;
}

void writeWords(WordCount* wordCounts, int totalCount){
	qsort(wordCounts,totalCount, sizeof(WordCount), cmpWords);
	printf("4");
	for(int i = 0; i < totalCount; i++) {
		char buf[1024];
		int z = snprintf(buf, sizeof(buf), "%s: %d\n", wordCounts[i].word, wordCounts[i].count);
		write(STDOUT_FILENO, buf, z);
	}
}



int main(int argc, char* argv[]){
	if(argc < 2){
		perror("Not enough arguments\n");
		//return -1;
	}

	WordCount *wordCounts = malloc(CHUNKSIZE * sizeof(WordCount));
	int totalCount = 0;
	for(int i = 1; i < argc; i++){
		struct stat fStat;
		if(stat(argv[i], &fStat) == 0){
			if(S_ISDIR(fStat.st_mode)){
				processDirectory(argv[i], wordCounts, &totalCount);
			}
			else{
				processFile(argv[i], wordCounts, &totalCount);
			}
		}
		else{
			perror("Error fetching");
		}
	}
	writeWords(wordCounts, totalCount);
	return 0;
}

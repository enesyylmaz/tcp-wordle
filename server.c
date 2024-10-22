#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 7
#define WORDS_ARRAY_SIZE 50

char wordsArray[WORDS_ARRAY_SIZE][WORD_LENGTH + 1];

void initializeWordsArray() {
	const char* wordsList[] = {
	    "apple", "baker", "camel", "dance", "eager",
	    "frost", "grain", "happy", "jewel", "lemon",
	    "music", "novel", "ocean", "piano", "quiet",
	    "rider", "silly", "table", "vivid", "wrist",
	    "yacht", "zebra", "above", "brave", "claim",
	    "drown", "early", "flame", "grace", "haste",
	    "inbox", "jolly", "karma", "leash", "mirth",
	    "noble", "overt", "plumb", "quiet", "rugby",
	    "shred", "table", "unzip", "vouch", "whale",
	    "xerox", "youth", "acorn", "blitz", "crane"
	};


	for (int i = 0; i < WORDS_ARRAY_SIZE; i++) {
	    	strncpy(wordsArray[i], wordsList[i % (sizeof(wordsList) / sizeof(wordsList[0]))], WORD_LENGTH);
	    	wordsArray[i][WORD_LENGTH] = '\0';
	}
}


void* thread_proc(void *arg);


int main(int argc, char *argv[])
{
	struct sockaddr_in sAddr;
	int listensock;
	int newsock;
	int result;
	pthread_t thread_id;
	int val;
	
	initializeWordsArray();
    
	listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	val = 1;
	result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	if (result < 0) {
	    	perror("server4");
	    	return 0;
	}

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(6458);
	sAddr.sin_addr.s_addr = INADDR_ANY;

	result = bind(listensock, (struct sockaddr *) &sAddr, sizeof(sAddr));
	if (result < 0) {
	    	perror("exserver4");
	    	return 0;
	}

	result = listen(listensock, 5);
	if (result < 0) {
	    	perror("exserver4");
	    	return 0;
	}

	while (1) {
	    	newsock = accept(listensock, NULL ,NULL);
	    	result = pthread_create(&thread_id, NULL, thread_proc, (void *) (intptr_t) newsock);
	    	if (result != 0) {
			printf("Could not create thread.\n");
			return 0;
	    	}
	    	pthread_detach(thread_id);
	    	sched_yield();
	}
}

void sendFeedback(int sock, const char* feedback) {
	send(sock, feedback, WORD_LENGTH, 0);
}

void* thread_proc(void *arg)
{
	int sock;
	char targetWord[WORD_LENGTH + 1];
	char guess[WORD_LENGTH + 1];
	char feedback[WORD_LENGTH * 10 + 1];
	char message[200];
	int attempts = 0;

	printf("child thread %lu created.\n", pthread_self());
	sock = (int) (intptr_t) arg;

	srand(time(NULL));
	
	int wordIndex = rand() % WORDS_ARRAY_SIZE;
	strcpy(targetWord, wordsArray[wordIndex]);

	printf("child thread %lu target word: %s\n", pthread_self(), targetWord);

	while (attempts < MAX_ATTEMPTS) {

	    	ssize_t recv_result = recv(sock, guess, WORD_LENGTH, 0);
	    	
	    	if (recv_result == 0) {
	    		printf("child thread %lu disconnected!\n", pthread_self());
	    		break;
	    	}
	    	
	    	if (recv_result > 0 && guess[recv_result - 1] == '\n') {
	    		guess[recv_result - 1] = '\0';
		} else {
	    		guess[recv_result] = '\0';
		}

	    	
	    	guess[WORD_LENGTH] = '\0';
	    	
	    	for (int i = 0; i < WORD_LENGTH; i++) {
			if (guess[i] == targetWord[i]) {
		    		feedback[i] = targetWord[i];
			} else {
		    		feedback[i] = '_';
			}
	    	}
	    	feedback[WORD_LENGTH] = '\0';

	    	if (strcmp(guess, targetWord) == 0) {
			printf("child thread %lu guessed the word correctly!\n", pthread_self());
			sendFeedback(sock, "Game");
			break;
	    	} else {
			printf("child thread %lu guess: %s\n", pthread_self(), guess);
			sendFeedback(sock, feedback);
	    	}

	    	attempts++;
	}
	
	sendFeedback(sock, targetWord);
	
	close(sock);
	printf("child thread %lu finished.\n", pthread_self());
}





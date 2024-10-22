#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 7

void playWordle(int sock, int childnum);

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in sAddr;
	char buffer[25];

	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = INADDR_ANY;
	sAddr.sin_port = 0;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(sock, (const struct sockaddr *) &sAddr, sizeof(sAddr));

	sAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sAddr.sin_port = htons(6458);

	if (connect(sock, (const struct sockaddr *) &sAddr, sizeof(sAddr)) != 0) {
    		perror("client");
    		return 0;
	}
	
	printf("Welcome to the WORDLE game!\n");
	printf("You have 7 attempts to guess the correct 5-letter word. Good luck!\n");
	printf("Correct letter placements will be shown in the feedback.\n\n");
	
	playWordle(sock, 1);

	close(sock);

	return 0;
}

void playWordle(int sock, int childnum)
{
	char guess[WORD_LENGTH * 2];
	char feedback[WORD_LENGTH + 1];
	char message[200];
	int attempts = 0;
	int succeed;

	while (attempts < MAX_ATTEMPTS) {
	
	    	
	    	while (1) {
			printf("Guess %i: ", attempts + 1);
			fgets(guess, sizeof(guess), stdin);

			size_t len = strlen(guess);
			if (len > 0 && guess[len - 1] == '\n') {
			    	guess[len - 1] = '\0';
			    	len--;
			} else {
			    	int c;
			    	while ((c = getchar()) != '\n' && c != EOF);
			}
			
			if (len == WORD_LENGTH) {
		    		break;
			} else {
		    		printf("Invalid input. Please enter a %d-letter word.\n\n", WORD_LENGTH);
			}
		}

	    	send(sock, guess, WORD_LENGTH, 0);

	    	recv(sock, feedback, WORD_LENGTH, 0);

	    	if (strcmp(feedback, "Game") == 0) {
			printf("Game!\n");
			int succeed = 1;
			break;
	    	} else {
			printf("Feedback: %s\n\n", feedback);
	    	}

    		attempts++;
    		
    		
	}
	
	if (strcmp(feedback, "Game") != 0){
	
		recv(sock, feedback, WORD_LENGTH, 0);
		
		printf("The word was: %s\n", feedback);
	}
	

}





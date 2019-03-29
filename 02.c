#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct {
	int cap;
	int len;
	int error;
} Buffer_s, *Buffer;

int main(int argc, char *argv[]) {
	// Buffer inititalization

	int fd = open("shared_mem", O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		fprintf(stderr, "error opening file for shared memory\n");
		exit(1);
	}
	ftruncate(fd, sizeof(Buffer_s));

	Buffer buffer = (Buffer)mmap(
			NULL,
			sizeof(Buffer_s),
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd, 
			0);

	if(buffer == MAP_FAILED) {
		close(fd);
		fprintf(stderr, "error creating shared memory\n");
		exit(1);
	}

	// No longer need file handle
	close(fd);

	printf("Creating a buffer size 10\n\n");
	buffer->cap = 10;
	buffer->len = 0;
	buffer->error = 0;


	// Set sleeping times for producer and consumer

	int sleep_producer;
	int sleep_consumer;

	srand(time(NULL));
	if (random() % 2) {
		sleep_producer = 1;
		sleep_consumer = 2;
	}
	else {
		sleep_producer = 2;
		sleep_consumer = 1;
	}

	pid_t producer;
	pid_t consumer = getpid();

	int producer_len;
	int consumer_len;

	producer = fork();
	if(producer < 0) {
		munmap(buffer, sizeof(Buffer_s));
		fprintf(stderr, "error while forking\n");
		exit(1);
	}

	if(producer > 0) {
		printf("<- : Producer PID: %d\n", producer);

		while(!buffer->error) {
			if(buffer->cap == buffer->len) {
				printf("<- : Producer attempted to overflow the buffer!\n");
				buffer->error = 1;
				break;
			}

			producer_len = ++buffer->len;
			printf("<- len(%d): Producer puts something in buffer\n", producer_len);
			sleep(sleep_producer);
		}

		exit(0);
	}
	else {
		printf("-> : Consumer PID: %d\n", consumer);
		if(buffer->len == 0) {
			sleep(3);
		}

		while(!buffer->error) {
			if(buffer->len == 0) {
				printf("-> : Consumer tried to underflow the buffer!\n");
				buffer->error = 1;
				break;
			}

			consumer_len = --buffer->len;
			printf("-> len(%d): Consumer takes something from the buffer\n", consumer_len);
			sleep(sleep_consumer);
		}

	}


	munmap(buffer, sizeof(Buffer_s));
	return 0;
}

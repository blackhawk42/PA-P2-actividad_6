#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	FILE *log_file;

	pid_t process_id;
	pid_t sid;

	time_t timestamp;

	process_id = fork();
	if(process_id < 0) {
		fprintf(stderr, "error: fork failed\n");
		exit(1);
	}

	// Killing father process
	if(process_id > 0) {
		printf("PID of child: %d\n", process_id);
		printf("parent shutting down\n");
		exit(0);
	}

	// Daemonizing
	sid = setsid();
	if(sid < 0) {
		exit(1);
	}

	// Closing the standards
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	log_file = fopen("log_file.txt", "w");

	while(1) {
		timestamp = time(NULL);
		fprintf(log_file, "%llu\n", (long long unsigned)timestamp);
		fflush(log_file);

		// Every minute
		sleep(5);
	}

	return 0;
}

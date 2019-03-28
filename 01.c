#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
	FILE *log_file;

	pid_t process_id;
	pid_t sid;

	time_t unix_timestamp;
	struct tm *tm_info;
	char timestamp[32];


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
		time(&unix_timestamp);
		tm_info = localtime(&unix_timestamp);
		strftime(timestamp, 32, "%Y-%m-%d %H:%M:%S", tm_info);

		fprintf(log_file, "%s\n", timestamp);
		fflush(log_file);

		// Every minute
		sleep(60);
	}

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <fcntl.h>	  // for open
#include <unistd.h>	  // for close
#include <sys/wait.h> // for waiting child process
#include <poll.h>	  // for polling
#include <stdbool.h>  // for boolean
#include <pthread.h>  // for creating thread

struct ThreadArgs
{
	int thread_id;
	int pipe_fd[2];
};

void *thread_function(void *arg)
{
	struct ThreadArgs *thread_args = (struct ThreadArgs *)arg;
	int threadid = thread_args->thread_id;

	int data[4] = {};
	for (int i = 0; i < 1; i++)
	{
		data[0] = i;
		data[1] = i + 1;
		data[2] = i + 2;
		data[3] = i + 3;
		ssize_t writed = write(thread_args->pipe_fd[1], data, sizeof(data));
		if (writed != -1)
		{
			printf("[Thread%d]Writed amount of data:%ld \n", threadid, writed);
		}
		else
		{
			printf("[1]Write to pipe error\n");
		}
		int new_pipe_fdWriteEnd = dup(thread_args->pipe_fd[1]);
		// close(thread_args->pipe_fd[1]); // let the os handle closing pipe write end.
		data[0] = i;
		data[1] = i + 222;
		data[2] = i + 223;
		data[3] = i + 224;
		ssize_t writed2 = write(new_pipe_fdWriteEnd, data, sizeof(data));
		if (writed2 != -1)
		{

			printf("[Thread%d]Writed amount of data:%ld \n", threadid, writed2);
		}
		else
		{

			printf("[2]Write to pipe error\n");
		}
	}
}

int main()
{
	int pipe_fd[2];
	if (-1 == pipe(pipe_fd))
	{
		perror("creating a pipe error\n");
		return -1;
	}

	int newchild = fork();
	if (newchild == 0)
	{
		printf("[Child] PID: %d\n", getpid());
		int num_threads = 1;
		pthread_t threads[num_threads];
		struct ThreadArgs thread_args[num_threads];

		for (int i = 0; i < num_threads; i++)
		{
			thread_args[i].thread_id = i + 1;
			thread_args[i].pipe_fd[0] = pipe_fd[0];
			thread_args[i].pipe_fd[1] = pipe_fd[1];

			if (pthread_create(&threads[i], NULL, thread_function, &thread_args[i]) != 0)
			{
				printf("Creating thread error \n");
				return -1;
			}
		}
		// Wait for child threads to finish writing
		for (int i = 0; i < num_threads; i++)
		{
			pthread_join(threads[i], NULL);
		}
	}
	else
	{
		int readbuff[100];
		while (1)
		{
			ssize_t bytes_read = read(pipe_fd[0], readbuff, sizeof(readbuff));
			if (bytes_read > 0)
			{
				fflush(NULL);
				printf("Read %zd bytes from the pipe:\n", bytes_read);
				for (ssize_t i = 0; i < bytes_read / sizeof(int); i++)
				{
					printf("buff[%zd] = %d\n", i, readbuff[i]);
				}
			}
			else if (bytes_read == 0)
			{
				// End of file (pipe closed)
				printf("pipe closed");
				break;
			}
			else
			{
				printf("Read from pipe error");
				break;
			}
		}
	}
	return 0;
}
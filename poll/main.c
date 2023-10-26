#include <stdio.h>
#include <string.h>
#include <fcntl.h>	  // for open
#include <unistd.h>	  // for close
#include <sys/wait.h> // for waiting child process
#include <poll.h>	  // for polling
#include <stdbool.h>  // for boolean
#include <pthread.h>  // for creating thread

int main()
{

	const char *filenames[3] = {"example1.txt", "example2.txt", "example3.txt"};
	int filedescriptors[3];
	for (int i = 0; i < 3; i++)
	{
		filedescriptors[i] = open(filenames[i], O_CREAT | O_RDWR, 0666);
	}
	printf("File descriptors:\nfd1%d\nfd2:%d\nfd3:%d\n", filedescriptors[0], filedescriptors[1], filedescriptors[2]);

	// Fd storage.
	struct pollfd fds[3];
	int numfds = sizeof(fds) / sizeof(fds[0]);
	for (int i = 0; i < numfds; i++)
	{
		fds[i].fd = filedescriptors[i];
		fds[i].events = POLLIN | POLLOUT;
	}

	for (int k = 0; k < 6; k++)
	{
		// poll it.
		int ready = poll(fds, numfds, 0);
		if (ready > 0)
		{
			for (int i = 0; i < numfds; i++)
			{
				if (fds[i].revents & POLLIN)
				{
					printf("fd%d has data to read.\n", i + 1);

					char buffer[8000];
					ssize_t bytes_read = read(filedescriptors[i], buffer, sizeof(buffer));

					if (bytes_read > 0)
					{
						printf("Read %zd bytes: [%s]\n", bytes_read, buffer);
					}
					else if (bytes_read == 0)
					{
						printf("End of file for fd%d.\n", i + 1);
					}
					else
					{
						// Handle the read error.
						perror("read error");
					}
				}
				if (fds[i].revents & POLLIN)
				{
					// lseek(filedescriptors[i], 0, SEEK_SET); // Move to file indicator 0.row
					ssize_t bytes_written = write(filedescriptors[i], "123", 3);
					if (bytes_written > 0)
					{
						printf("Wrote %zd bytes.\n", bytes_written);
					}
					else if (bytes_written == 0)
					{
						printf("Write operation was interrupted.\n");
					}
					else
					{
						perror("write error");
					}
				}
			}
		}
		else if (read == 0)
		{
			printf("There is no event.");
		}
		else
		{
			perror("poll error");
		}
	}
	for (int i = 0; i < 3; i++)
	{
		close(filedescriptors[i]);
	}

	// TODO: implement poll and epoll.
	return 0;
}
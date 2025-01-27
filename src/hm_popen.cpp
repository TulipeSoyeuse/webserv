#include "hm_popen.hpp"

hm_popen::hm_popen(std::string &f, CGI cgi) : all_read(false)
{
	int stdout_pipe[2];
	int stderr_pipe[2];
	if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1)
	{
		perror("pipe");
		return;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return;
	}
	if (pid == 0)
	{
		// Child process
		close(stdout_pipe[0]); // Close reading end of stdout pipe in child
		close(stderr_pipe[0]); // Close reading end of stderr pipe in child

		// Redirect stdout & stderr to stdout_pipe & stderr_pipe
		dup2(stdout_pipe[1], STDOUT_FILENO);
		dup2(stderr_pipe[1], STDERR_FILENO);

		// Close write ends after redirection
		close(stdout_pipe[1]);
		close(stderr_pipe[1]);

		char *const argv[3] = {(char *const)"-f", (char *const)f.c_str(), (char *)0};
		std::cout << "this is path :" << argv[0] << std::cout;
		if (execve(get_CGI_exec(cgi), argv, NULL) == -1)
		{
			perror("execve");
		}
	}
	else
	{
		close(stdout_pipe[1]); // Close writing end of stdout pipe in parent
		close(stderr_pipe[1]); // Close writing end of stderr pipe in parent
		waitpid(-1, NULL, 0);
		subprocess_stdout_fd = stdout_pipe[0];
		subprocess_stderr_fd = stderr_pipe[0];
	}

}



const char *hm_popen::get_CGI_exec(const CGI &cgi) const
{
	if (cgi == PHP)
		return "/usr/bin/php";
	if (cgi == BASH)
		return "/bin/bash";
	else
		return "";
}

size_t hm_popen::read_out(char *buffer, const size_t buffer_size)
{
	size_t nbytes = read(subprocess_stdout_fd, buffer, buffer_size - 1);
	buffer[nbytes] = 0;
	if (nbytes < buffer_size - 1)
		all_read = true;
	//std::cout << buffer;
	return (nbytes);
}

size_t hm_popen::read_err(char *buffer, const size_t buffer_size)
{
	size_t nbytes = read(subprocess_stderr_fd, buffer, buffer_size - 1);
	buffer[nbytes] = 0;
	if (nbytes < buffer_size - 1)
		all_read = true;
	return (nbytes);
}

const bool &hm_popen::is_all_read() const
{
	return (all_read);
}

hm_popen::~hm_popen()
{
	close(subprocess_stderr_fd);
	close(subprocess_stdout_fd);
}

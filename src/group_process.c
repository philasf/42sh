/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   group_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <asarandi@student.42.us.org>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/15 15:11:18 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/17 22:04:31 by asarandi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft.h"

void	group_process_destroy(t_process **group)
{
	int	i;

	i = 0;
	while (group[i] != NULL)
	{
		(void)process_destroy(group[i]);
		i++;
	}
	free(group);
	return ;
}

int		group_process_wait(t_process **group)
{
	int i;
	int pid;
	int	status;

	pid = 0;
	status = 0;
	while ((pid = wait(&status)) != -1)
	{
		i = 0;
		while (group[i])
		{
			if (group[i]->pid == pid)
			{
				group[i]->exit_code = WEXITSTATUS(status);
				break ;
			}
			i++;
		}
	}
	return (status);
}

void	group_fix_redirect(t_process **group, int i, int count, int *pipes)
{
	int		fd;

	count += 0;
	if (group[i + 1] != NULL)
	{
		if (group[i + 1]->ast->parent->type == RED_NEXT)
		{
			fd = open(group[i + 1]->ast->name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				perror(SHELL_NAME);
			pipes[(i * 2) + 1] = fd;
		}
		if (group[i + 1]->ast->parent->type == RED_NNEXT)
		{
			fd = open(group[i + 1]->ast->name, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				perror(SHELL_NAME);
			pipes[(i * 2) + 1] = fd;
		}
	}
}

void	group_fork_exec(t_process **group, int i, int count, int *pipes)
{
	t_process	*p;

	(void)group_fix_redirect(group, i, count, pipes);
	p = group[i];
	if ((p->pid = fork()) == 0)
	{
		if (i < count - 1)
			dup2(pipes[(i * 2) + 1], 1);
		if ((i > 0) && (i < count))
			dup2(pipes[(i * 2) - 2], 0);
		i = 0;
		while (i < count - 1)
		{
			close(pipes[i * 2]);
			close(pipes[(i * 2) + 1]);
			i++;
		}
		execve(p->fullpath, p->argv, p->envp);
	}
	else if (p->pid == -1)
		(void)fatal_error_message(g_sh, E_FORK);
	return ;
}

int		group_process_execute(t_shell *sh, t_process **group, int i, int count)
{
	char	*path;
	char	*fullpath;
	int		*pipes;

	pipes = group_process_make_pipes(group, &i, &count);
	while (group[i])
	{
		if (group[i]->ast->type == CMD)
		{
			if (is_valid_executable_file(group[i]->argv[0]) == 1)
				fullpath = ft_strdup(group[i]->argv[0]);
			else
			{
				if ((path = find_command_path(sh, group[i]->argv[0])) == NULL)
				{
					free(path);
					(void)ft_printf(STDERR_FILENO, "%s: %s: command not found\n",
							SHELL_NAME, group[i]->argv[0]);
					return (group_process_close_pipes(pipes, count) + 1);
				}
				group[i]->fullpath = dir_slash_exec(path, group[i]->argv[0]);
				free(path);
			}
			(void)group_fork_exec(group, i, count, pipes);
		}
		i++;
	}
	return (group_process_close_pipes(pipes, count));
}

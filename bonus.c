/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 14:49:05 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 17:47:04 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

int	error_exit(char *msg);

static char	*read_line_stdin(void)
{
	char	buf[MAX_DIGITS + 2];
	int		i;
	int		n;
	char	c;

	i = 0;
	n = read(0, &c, 1);
	while (n > 0 && c != '\n')
	{
		if (i < MAX_DIGITS)
			buf[i++] = c;
		n = read(0, &c, 1);
	}
	if (n <= 0 && i == 0)
		return (NULL);
	buf[i] = '\0';
	return (ft_strdup(buf));
}

static void	process_line(char *line, t_dict *d)
{
	char	*result;

	if (!ft_str_isnum(line))
		error_exit("Error");
	else
	{
		result = convert(line, d);
		if (!result)
			error_exit("Dict Error");
		else
		{
			ft_putstr(result);
			write(1, "\n", 1);
			free(result);
		}
	}
}

int	run_stdin(char *dict_file)
{
	t_dict	d;
	char	*line;

	dict_init(&d);
	if (dict_load(dict_file, &d) < 0)
	{
		dict_free(&d);
		return (error_exit("Dict Error"));
	}
	line = read_line_stdin();
	while (line)
	{
		process_line(line, &d);
		free(line);
		line = read_line_stdin();
	}
	dict_free(&d);
	return (0);
}

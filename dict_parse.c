/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dict_parse.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 13:03:53 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:07:34 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

static char	*open_and_alloc(char *filename, int *fd)
{
	char	*buf;

	*fd = open(filename, O_RDONLY);
	if (*fd < 0)
		return (NULL);
	buf = malloc(BUF_SIZE + 1);
	if (!buf)
		return (close(*fd), NULL);
	return (buf);
}

char	*read_file_buf(char *filename)
{
	int		fd;
	char	*buf;
	int		n;
	int		sum;

	buf = open_and_alloc(filename, &fd);
	if (!buf)
		return (NULL);
	sum = 0;
	n = read(fd, buf, BUF_SIZE);
	while (n > 0)
	{
		sum += n;
		if (sum >= BUF_SIZE)
			break ;
		n = read(fd, buf + sum, BUF_SIZE - sum);
	}
	close(fd);
	if (n < 0)
		return (free(buf), NULL);
	buf[sum] = '\0';
	return (buf);
}

static int	store_entry(char *key_str, char *val_str, t_dict *d)
{
	char	*trimmed_key;
	char	*trimmed_val;

	trimmed_key = ft_trim(key_str);
	if (!trimmed_key || !ft_str_isnum(trimmed_key))
		return (free(trimmed_key), -1);
	trimmed_val = ft_trim(val_str);
	if (!trimmed_val)
		return (free(trimmed_key), -1);
	if (dict_add(d, trimmed_key, trimmed_val) < 0)
	{
		free(trimmed_key);
		free(trimmed_val);
	}
	return (0);
}

static int	parse_line(char *line, t_dict *d)
{
	char	*colon;

	colon = line;
	while (*colon && *colon != ':')
		colon++;
	if (!*colon)
		return (-1);
	*colon = '\0';
	return (store_entry(line, colon + 1, d));
}

int	process_lines(char *buf, t_dict *d)
{
	char	*line;
	char	*end;

	line = buf;
	while (*line)
	{
		end = line;
		while (*end && *end != '\n')
			end++;
		if (*end == '\n')
		{
			*end = '\0';
			if (*line && parse_line(line, d) < 0)
				return (-1);
			line = end + 1;
		}
		else
		{
			if (*line)
				parse_line(line, d);
			break ;
		}
	}
	return (0);
}

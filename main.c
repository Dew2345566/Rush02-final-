/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 13:21:17 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 17:47:48 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

int	error_exit(char *msg)
{
	write(1, msg, ft_strlen(msg));
	write(1, "\n", 1);
	return (1);
}

static int	setup_args(int argc, char **argv, char **dict_file, char **num_str)
{
	if (argc == 2)
	{
		*dict_file = DICT_FILE;
		*num_str = argv[1];
	}
	else if (argc == 3)
	{
		*dict_file = argv[1];
		*num_str = argv[2];
	}
	else
		return (-1);
	return (0);
}

static int	print_result(char *result, t_dict *d)
{
	if (!result)
	{
		dict_free(d);
		return (error_exit("Dict Error"));
	}
	ft_putstr(result);
	write(1, "\n", 1);
	free(result);
	dict_free(d);
	return (0);
}

static int	run(char *dict_file, char *num_str)
{
	t_dict	d;

	dict_init(&d);
	if (dict_load(dict_file, &d) < 0)
	{
		dict_free(&d);
		return (error_exit("Dict Error"));
	}
	return (print_result(convert(num_str, &d), &d));
}

int	main(int argc, char **argv)
{
	char	*dict_file;
	char	*num_str;

	if (setup_args(argc, argv, &dict_file, &num_str) < 0)
		return (error_exit("Error"));
	if (ft_strcmp(num_str, "-") == 0)
		return (run_stdin(dict_file));
	if (!ft_str_isnum(num_str))
		return (error_exit("Error"));
	return (run(dict_file, num_str));
}

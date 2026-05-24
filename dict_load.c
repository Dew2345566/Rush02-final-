/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dict_load.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 13:20:22 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:08:17 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

int	dict_load(char *filename, t_dict *d)
{
	char	*buf;
	int		ret;

	buf = read_file_buf(filename);
	if (!buf)
		return (-1);
	ret = process_lines(buf, d);
	free(buf);
	return (ret);
}

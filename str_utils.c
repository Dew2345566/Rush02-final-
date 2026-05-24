/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 14:11:00 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:10:11 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

char	*ft_trim(char *str)
{
	int		start;
	int		end;
	int		i;
	char	*r;

	if (!str)
		return (NULL);
	start = 0;
	while (str[start] == ' ' || str[start] == '\t')
		start++;
	end = ft_strlen(str) - 1;
	while (end >= start && (str[end] == ' ' || str[end] == '\t'))
		end--;
	if (end < start)
		return (ft_strdup(""));
	r = malloc(end - start + 2);
	if (!r)
		return (NULL);
	i = 0;
	while (start <= end)
		r[i++] = str[start++];
	r[i] = '\0';
	return (r);
}

int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

int	ft_str_isnum(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	ft_strcmp(char *a, char *b)
{
	int	i;

	i = 0;
	while (a[i] && b[i] && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

char	*ft_strdup_n(char *s, int n)
{
	char	*res;
	int		i;

	if (!s)
		return (NULL);
	res = malloc(n + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		res[i] = s[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

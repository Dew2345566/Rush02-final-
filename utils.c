/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 14:11:00 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:10:27 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

int	ft_strlen(char *str)
{
	int	len;

	len = 0;
	while (str && str[len])
		len++;
	return (len);
}

char	*ft_strdup(char *s)
{
	char	*res;
	int		i;

	if (!s)
		return (NULL);
	res = malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!res)
		return (NULL);
	i = 0;
	while (s[i])
	{
		res[i] = s[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

char	*ft_strjoin(char *a, char *b)
{
	char	*res;
	int		i;
	int		j;

	res = malloc(ft_strlen(a) + ft_strlen(b) + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (a && a[i])
	{
		res[i] = a[i];
		i++;
	}
	j = 0;
	while (b && b[j])
		res[i++] = b[j++];
	res[i] = '\0';
	return (res);
}

char	*ft_strjoin_sp(char *a, char *b)
{
	char	*temp;
	char	*res;

	if (!a)
		return (ft_strdup(b));
	if (!b || !b[0])
		return (ft_strdup(a));
	temp = ft_strjoin(a, " ");
	if (!temp)
		return (NULL);
	res = ft_strjoin(temp, b);
	free(temp);
	return (res);
}

void	ft_putstr(char *s)
{
	if (s)
		write(1, s, ft_strlen(s));
}

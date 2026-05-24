/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 15:29:13 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:09:05 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

char	*get_sep(char *prev, char *curr);

static int	count_spaces(char *s)
{
	int	n;

	n = 1;
	while (*s)
	{
		if (*s == ' ')
			n++;
		s++;
	}
	return (n);
}

static char	**split_words(char *s, int *count)
{
	char	**words;
	char	*p;
	int		i;

	*count = count_spaces(s);
	words = malloc(sizeof(char *) * (*count + 1));
	if (!words)
		return (NULL);
	i = 0;
	p = s;
	words[i++] = p;
	while (*p)
	{
		if (*p == ' ')
		{
			*p = '\0';
			words[i++] = p + 1;
		}
		p++;
	}
	words[i] = NULL;
	return (words);
}

static char	*append_word(char *res, char *sep, char *word)
{
	char	*tmp;

	tmp = ft_strjoin(res, sep);
	free(res);
	if (!tmp)
		return (NULL);
	res = ft_strjoin(tmp, word);
	free(tmp);
	return (res);
}

char	*proper_syntax(char *raw)
{
	char	**words;
	char	*copy;
	char	*res;
	int		count;
	int		i;

	copy = ft_strdup(raw);
	if (!copy)
		return (NULL);
	words = split_words(copy, &count);
	if (!words)
		return (free(copy), NULL);
	res = ft_strdup(words[0]);
	i = 1;
	while (i < count && res)
	{
		res = append_word(res, get_sep(words[i - 1], words[i]), words[i]);
		i++;
	}
	free(words);
	free(copy);
	return (res);
}

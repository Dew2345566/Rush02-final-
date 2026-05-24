/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dict.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 12:36:33 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:07:05 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

void	dict_init(t_dict *d)
{
	d->size = 0;
}

int	dict_add(t_dict *d, char *key, char *val)
{
	int	i;

	i = 0;
	while (i < d->size)
	{
		if (!ft_strcmp(d->entries[i].key, key))
		{
			free(d->entries[i].value);
			d->entries[i].value = val;
			return (0);
		}
		i++;
	}
	if (d->size >= MAX_ENTRIES)
		return (-1);
	d->entries[d->size].key = key;
	d->entries[d->size].value = val;
	d->size++;
	return (0);
}

char	*dict_value_get(t_dict *d, char *key)
{
	int	i;

	i = 0;
	while (i < d->size)
	{
		if (!ft_strcmp(d->entries[i].key, key))
			return (d->entries[i].value);
		i++;
	}
	return (NULL);
}

char	*dict_floor(t_dict *d, char *n)
{
	char	*best;
	int		i;

	best = NULL;
	i = 0;
	while (i < d->size)
	{
		if (bn_cmp(d->entries[i].key, n) <= 0)
		{
			if (!best || bn_cmp(d->entries[i].key, best) > 0)
				best = d->entries[i].key;
		}
		i++;
	}
	return (best);
}

void	dict_free(t_dict *d)
{
	int	i;

	i = 0;
	while (i < d->size)
	{
		free(d->entries[i].key);
		free(d->entries[i].value);
		i++;
	}
	d->size = 0;
}

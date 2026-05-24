/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 13:35:58 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:05:33 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

static char	*bn_sub_loop(char *buf, char *n, char *key)
{
	int	ln;
	int	lk;
	int	borrow;
	int	diff;

	ln = ft_strlen(n) - 1;
	lk = ft_strlen(key) - 1;
	borrow = 0;
	while (ln >= 0)
	{
		diff = (n[ln] - '0') - borrow;
		if (lk >= 0)
			diff -= (key[lk--] - '0');
		borrow = diff < 0;
		buf[ln--] = '0' + (diff + 10 * borrow);
	}
	return (buf);
}

static char	*bn_sub_str(char *n, char *key)
{
	char	buf[MAX_DIGITS + 1];

	buf[ft_strlen(n)] = '\0';
	return (strip_zeros(ft_strdup(bn_sub_loop(buf, n, key))));
}

static char	*convert_large(char *n, char *key, char *keyword, t_dict *d)
{
	char	*left;
	char	*right;
	char	*result;
	char	*rem;

	left = convert(strip_zeros(bn_div(n, key, &rem)), d);
	rem = strip_zeros(rem);
	if (!left)
		return (free(rem), NULL);
	result = ft_strjoin_sp(left, keyword);
	free(left);
	if (!result)
		return (free(rem), NULL);
	if (bn_is_zero(rem))
		return (free(rem), result);
	right = convert(rem, d);
	free(rem);
	if (!right)
		return (free(result), NULL);
	left = ft_strjoin_sp(result, right);
	free(result);
	free(right);
	return (left);
}

static char	*convert_small(char *n, char *key, char *keyword, t_dict *d)
{
	char	*result;
	char	*right;
	char	*left;
	char	*rem;

	result = ft_strdup(keyword);
	if (!result)
		return (NULL);
	rem = bn_sub_str(n, key);
	if (!rem || bn_is_zero(rem))
		return (free(rem), result);
	right = convert(rem, d);
	free(rem);
	if (!right)
		return (free(result), NULL);
	left = ft_strjoin_sp(result, right);
	free(result);
	free(right);
	return (left);
}

char	*convert(char *n, t_dict *d)
{
	char	*key;
	char	*keyword;

	if (!n)
		return (NULL);
	keyword = dict_value_get(d, n);
	if (keyword && bn_cmp(n, "100") < 0)
		return (ft_strdup(keyword));
	key = dict_floor(d, n);
	if (!key)
		return (NULL);
	keyword = dict_value_get(d, key);
	if (!keyword)
		return (NULL);
	if (bn_cmp(key, "100") >= 0)
		return (convert_large(n, key, keyword, d));
	return (convert_small(n, key, keyword, d));
}

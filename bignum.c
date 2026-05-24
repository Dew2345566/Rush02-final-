/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bignum.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 14:46:05 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 16:09:22 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

char	*strip_zeros(char *s)
{
	int		start;
	int		len;
	char	*res;
	int		i;

	len = ft_strlen(s);
	start = 0;
	while (start < len - 1 && s[start] == '0')
		start++;
	res = malloc(len - start + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (start < len)
		res[i++] = s[start++];
	res[i] = '\0';
	free(s);
	return (res);
}

int	bn_is_zero(char *n)
{
	int	i;

	i = 0;
	while (n[i] == '0')
		i++;
	return (n[i] == '\0');
}

int	bn_cmp(char *a, char *b)
{
	int	la;
	int	lb;
	int	i;

	la = ft_strlen(a);
	lb = ft_strlen(b);
	if (la != lb)
		return (la - lb);
	i = 0;
	while (a[i] && a[i] == b[i])
		i++;
	if (!a[i])
		return (0);
	return ((int)(a[i] - b[i]));
}

char	*bn_div(char *n, char *divisor, char **remainder)
{
	int		ln;
	int		ld;
	int		split;
	char	*quot;
	char	*rem;

	ln = ft_strlen(n);
	ld = ft_strlen(divisor);
	split = ln - (ld - 1);
	if (split <= 0)
	{
		*remainder = ft_strdup(n);
		return (ft_strdup("0"));
	}
	quot = strip_zeros(ft_strdup_n(n, split));
	if (split < ln)
		rem = strip_zeros(ft_strdup_n(n + split, ln - split + 1));
	else
		rem = ft_strdup("0");
	*remainder = rem;
	return (quot);
}

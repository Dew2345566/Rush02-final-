/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert_help.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cthongsa <cthongsa@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 17:37:46 by cthongsa          #+#    #+#             */
/*   Updated: 2026/05/24 17:38:12 by cthongsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rush02.h"

char	*large_remainder(char *result, char *rem, t_dict *d)
{
	char	*right;
	char	*left;

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

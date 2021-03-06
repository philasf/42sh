/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_validate.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brabo-hi <brabo-hi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/15 14:08:01 by asarandi          #+#    #+#             */
/*   Updated: 2018/04/22 02:36:14 by brabo-hi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/parse.h"

t_ast	*validate_lexer(t_ast *lex)
{
	t_ast	*cpy;

	cpy = lex;
	while (lex)
	{
		if (!ft_strlen(lex->name) || lex->type != CMD)
			return (NULL);
		if ((lex = lex->next) && lex->type == CMD)
			return (NULL);
		lex = lex ? lex->next : lex;
	}
	return (cpy);
}

t_ast	*validate_ast(t_ast *ast)
{
	return (ast);
}

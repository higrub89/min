#ifndef LEXER_H
# define LEXER_H

# include "minishell.h"

// Forward declarations: Le decimos al compilador que estos tipos existen,
// sin necesidad de incluir todo el archivo .h donde se definen.
// Esto rompe la dependencia circular.
struct s_token;
struct s_struct;

t_token *lexer(const char *input_line, t_struct *mini);
void free_tokens(t_token *head);

#endif
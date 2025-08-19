/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:39:22 by rhiguita          #+#    #+#             */
/*   Updated: 2025/06/20 14:58:16 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/inc/libft.h"
# include <stdbool.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

//Tipos de tokens.
typedef enum e_token_type
{
  WORD,       //Una palabra normal.
  PIPE,       //operador |
  IN,   //operador <
  OUT,  //operador >
  APPE_OUT, //operador >>
  HEREDOC,    //operador <<

} t_token_type;

//Estructura para un token individual.
typedef struct s_token
{
  char  *value;
  t_token_type type;
  struct s_token *next;
  struct s_token *prev;
} t_token;

//Tipos de redirección
typedef enum e_redir_type
{
  REDIR_IN,     // <
  REDIR_OUT,    // >
  REDIR_APPEND, // >>
  REDIR_HEREDOC, // <<
} t_redir_type;

// Estructura para las redirecciones.
typedef struct s_redirection
{
  t_redir_type type;
  char    *file;
  bool  expand_heredoc_content;
  struct s_redirection *next;
} t_redirection;

// Estructura para un comando.
typedef struct s_command
{
  char    **args;
  int num_args;
  t_redirection *redirections;
  int num_redirections;
  char *full_path;
  int heredoc_fd;
  struct s_command *next;
  struct s_command *prev;
} t_command;

typedef struct s_minishell
{
  char **envp;
  int last_exit_status;
  bool should_exit;
} t_struct;

# include "lexer.h"
# include "parser.h"
# include "redirection.h"
# include "expander.h"
# include "env_utils.h"
# include "main_utils.h"

#endif

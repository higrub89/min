#include "../inc/minishell.h"
#include "../inc/env_utils.h"

// Función de ayuda para imprimir la lista de comandos y sus detalles.
static void print_commands(t_command *cmd_list)
{
    int i;
    int cmd_num = 1;
    t_redirection *redir;

    if (!cmd_list)
    {
        printf("-> Parser returned NULL\n");
        return;
    }

    printf("\n--- ✅ Final Command Structure ---\n");
    while (cmd_list)
    {
        printf("  [Command #%d]\n", cmd_num++);
        
        // Imprimir argumentos
        printf("    Args: ");
        i = 0;
        if (cmd_list->args)
        {
            while (cmd_list->args[i])
            {
                printf("[\"%s\"] ", cmd_list->args[i]);
                i++;
            }
        }
        printf("\n");

        // Imprimir redirecciones
        printf("    Redirections: ");
        redir = cmd_list->redirections;
        if (!redir)
            printf("None");
        else
        {
            while (redir)
            {
                printf("(Type: %d, File: \"%s\") ", redir->type, redir->file);
                redir = redir->next;
            }
        }
        printf("\n");

        cmd_list = cmd_list->next;
    }
}

// Función principal para probar el flujo completo hasta el expander.
// Necesita 'argc' y 'argv' para recibir el entorno 'envp'.
int main(int argc, char **argv, char **envp)
{
    char        *input_line;
    t_token     *token_list;
    t_command   *command_list;
    t_struct    mini;

    // Evitar warnings de parámetros no usados
    (void)argc;
    (void)argv;

    // Inicializar la estructura mini con el entorno real
    if (init_minishell(&mini, envp) != 0)
        return (1);

    while (1)
    {
        input_line = readline("m_test> ");
        if (!input_line)
        {
            printf("exit\n");
            break;
        }
        if (input_line[0])
            add_history(input_line);

        // --- 1. Lexer ---
        token_list = lexer(input_line, &mini);
        if (!token_list)
        {
            free(input_line);
            continue;
        }

        // --- 2. Parser ---
        command_list = parse_input(token_list, &mini);
        free_tokens(token_list); // Los tokens ya no son necesarios
        if (!command_list)
        {
            free(input_line);
            continue;
        }

        // --- 3. Expander ---
        expand_variables(command_list, &mini);

        // --- 4. Imprimir resultado final ---
        print_commands(command_list);

        // --- 5. Liberar memoria ---
        free_commands(command_list);
        free(input_line);
        printf("-----------------------------------\n\n");
    }

    // Limpieza final
    cleanup_minishell(&mini);
    return (0);
}
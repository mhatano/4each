#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <stdarg.h>

typedef struct filename_chain_tag
{
    char* filename;
    struct filename_chain_tag* next;
} filename_chain;

typedef struct param_check_return_tag
{
    filename_chain * chain;
    char * command;
} param_check_return;

static int verbose = 0;
static int force_newline = 0;
static int show_filename = 0;

void error_msg(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void free_up(filename_chain* chain)
{
    while (chain != NULL) {
        filename_chain* next = chain->next;
        free(chain->filename);
        free(chain);
        chain = next;
    }
}

param_check_return param_check(int argc, char** argv)
{
    param_check_return return_value;
    char* command;
    int i;
    filename_chain* head;
    filename_chain* tail;

    return_value.chain = NULL;
    return_value.command = NULL;

    command = NULL;
    head = NULL;
    tail = NULL;

    if (argc == 1)
    {
        error_msg("%s : [Error] Parameters are missing. Please run with '%s -h' for help.\n", basename(argv[0]), basename(argv[0]));
        exit(EXIT_FAILURE);
    }

    for ( i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-e") == 0)
        {
            if (++i >= argc)
            {
                error_msg("%s : [Error] Illegal argument, '-e' should not be the last argument.\n", basename(argv[0]));
                exit(EXIT_FAILURE);
            }
            command = strdup(argv[i]);
            if (!command)
            {
                error_msg("%s : [Error] Memory could not be allocated.\n", basename(argv[0]));
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            force_newline = 1;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            show_filename = 1;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            error_msg("%s : Run quick command for each argument.\n", basename(argv[0]));
            error_msg("Usage: %s [-h] [-s] [-f] [-v] -e 'command' filenames...\n", basename(argv[0]));
            error_msg("  -h : Show help message.\n");
            error_msg("  -s : Show filename before processing.\n");
            error_msg("  -f : Force newline after command execution.\n");
            error_msg("  -v : Verbose output.\n");
            error_msg("  -e <command> : Specify command to run.\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            filename_chain* node = calloc(1, sizeof(filename_chain));
            if (!node)
            {
                error_msg("%s : [Error] Memory could not be allocated.\n", basename(argv[0]));
                free_up(head);
                free(command);
                exit(EXIT_FAILURE);
            }
            node->filename = strdup(argv[i]);
            if (!node->filename)
            {
                error_msg("%s : [Error] Memory could not be allocated.\n", basename(argv[0]));
                free(node);
                free_up(head);
                free(command);
                exit(EXIT_FAILURE);
            }
            node->next = NULL;

            if (!head)
            {
                head = tail = node;
            }
            else
            {
                tail->next = node;
                tail = node;
            }
        }
    }

    return_value.chain = head;
    return_value.command = command;
    return return_value;
}

int main(int argc, char** argv)
{
    filename_chain* chain;
    char* command;
    param_check_return param_check_result;
    int result ;

    param_check_result = param_check(argc, argv);
    chain = param_check_result.chain;
    command = param_check_result.command;

    if (!chain || !command)
    {
        error_msg("%s : [Error] No files or command specified.\n",basename(argv[0]));
        free_up(chain);
        free(command);
        return EXIT_FAILURE;
    }

    if (verbose)
    {
        printf("command : %s\n", command);
    }

    int index = 0;
    filename_chain* current;
    for (current = chain; current != NULL; current = current->next, index++)
    {
        size_t cmd_len = strlen(command) + 1 + strlen(current->filename) + 1;
        char* current_command = malloc(cmd_len);
        if (!current_command)
        {
            error_msg("%s : [Error] Memory allocation error during command construction.\n",basename(argv[0]));
            free_up(chain);
            free(command);
            return EXIT_FAILURE;
        }
        snprintf(current_command, cmd_len, "%s %s", command, current->filename);

        if (verbose)
        {
            printf("filename[%d] = '%s'\n", index, current->filename);
            printf("start command : %s\n", current_command);
            fflush(stdout);
        }
        else if (show_filename)
        {
            printf("file: %s\n", current->filename);
            fflush(stdout);
        }


        result = system(current_command);
        if ( result == -1 )
        {
            error_msg("%s : [Error] Command execution failed for '%s'.\n", basename(argv[0]), current->filename);
        }

        if (force_newline)
        {
            putchar('\n');
        }
        fflush(stdout);
        free(current_command);
    }

    free_up(chain);
    free(command);

    return EXIT_SUCCESS;
}

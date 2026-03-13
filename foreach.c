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

static int verbose = 0;
static int force_newline = 0;
static int show_filename = 0;
static char* command = NULL;

static char* cmdname = NULL;

#define err_msg(fmt,...) error_msg(__func__,fmt,##__VA_ARGS__)

void error_msg(const char*,const char*,...) __attribute__((format(printf, 2, 3)));

void error_msg(const char* func, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char *fnc_fmt = malloc(strlen(cmdname) + strlen(func) + strlen(format) + 1);
    if (!fnc_fmt)
    {
        fprintf(stderr, "%s : [Error] Memory could not be allocated.\n", cmdname);
        exit(EXIT_FAILURE);
    }
    snprintf(fnc_fmt, strlen(cmdname) + strlen(func) + strlen(format) + 1, "%s : %s : %s", cmdname, func, format);
    vfprintf(stderr, fnc_fmt, args);
    fflush(stderr);
    free(fnc_fmt);
    va_end(args);
}
void error_msg(const char* func, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char *fnc_fmt = malloc(strlen(cmdname) + strlen(func) + strlen(format) + 1);
    if (!fnc_fmt)
    {
        fprintf(stderr,"%s : [Error] Memory could not be allocated.\n", cmdname);
        exit(EXIT_FAILURE);
    }
    snprintf(fnc_fmt, strlen(cmdname) + strlen(func) + strlen(format) + 1, "%s : %s : %s", cmdname, func, format);
    vfprintf(stderr, fnc_fmt, args);
    fflush(stderr);
    free(fnc_fmt);
    va_end(args);
}

#define vrbs_msg(fmt,...) verbose_msg(__func__, fmt, ##__VA_ARGS__)

void verbose_msg(const char* func, const char* format, ...) __attribute__((format(printf, 2, 3)));
void verbose_msg(const char* func, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char *fnc_fmt = malloc(strlen(cmdname) + strlen(func) + strlen(format) + 1);
    if (!fnc_fmt)
    {
        printf("%s : [Error] Memory could not be allocated.\n", cmdname);
        exit(EXIT_FAILURE);
    }
    snprintf(fnc_fmt, strlen(cmdname) + strlen(func) + strlen(format) + 1, "%s : %s : %s", cmdname, func, format);
    vprintf(fnc_fmt, args);
    fflush(stdout);
    free(fnc_fmt);
    va_end(args);
}

void showfname_msg(const char*,...) __attribute__((format(printf, 1, 2)));

void showfname_msg(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char *fnc_fmt = malloc(strlen(cmdname) + strlen(func) + strlen(format) + 1);
    if (!fnc_fmt)
    {
        printf("%s : [Error] Memory could not be allocated.\n", cmdname);
        exit(EXIT_FAILURE);
    }
    snprintf(fnc_fmt, strlen(cmdname) + strlen(func) + strlen(format) + 1, "%s : %s : %s", cmdname, func, format);
    vprintf(fnc_fmt, args);
    fflush(stdout);
    free(fnc_fmt);
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

filename_chain* param_check(int argc, char** argv)
{
    int i;

    if (argc == 1)
    {
        error_msg("%s : [Error] Parameters are missing. Please run with '%s -h' for help.\n", cmdname, cmdname);
        exit(EXIT_FAILURE);
    }

    filename_chain *head = NULL, *tail = NULL;

    for ( i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-e") == 0)
        {
            if (++i >= argc)
            {
                error_msg("%s : [Error] Illegal argument, '-e' should not be the last argument.\n", cmdname);
                exit(EXIT_FAILURE);
            }
            free(command);
            command = strdup(argv[i]);
            if (!command)
            {
                error_msg("%s : [Error] Memory could not be allocated.\n", cmdname);
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
            error_msg("%s : Run quick command for each argument.\n", cmdname);
            error_msg("Usage: %s [-h] [-s] [-f] [-v] -e 'command' filenames...\n", cmdname);
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
                error_msg("%s : [Error] Memory could not be allocated.\n", cmdname);
                free_up(head);
                free(command);
                exit(EXIT_FAILURE);
            }
            node->filename = strdup(argv[i]);
            if (!node->filename)
            {
                error_msg("%s : [Error] Memory could not be allocated.\n", cmdname);
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

    return head;
}

int main(int argc, char** argv)
{
    filename_chain* chain;
    int result ;

    if ( argv[0] == NULL )
    {
        fprintf(stderr,"%s : [Error] No command name available.\n",__func__);
        return EXIT_FAILURE;
    }
    
    char * argv0 = strdup(argv[0]);
    cmdname = strdup(basename(argv0));
    free(argv0);
    
    chain = param_check(argc, argv);

    if (!chain || !command)
    {
        error_msg("%s : [Error] No files or command specified.\n",cmdname);
        free_up(chain);
        free(command);
        free(cmdname);
        return EXIT_FAILURE;
    }

    if (verbose)
    {
        vrbs_msg("command : %s\n", command);
    }

    int index = 0;
    filename_chain* current;
    for (current = chain; current != NULL; current = current->next, index++)
    {
        size_t cmd_len = strlen(command) + strlen(current->filename) + 3;
        char* current_command = malloc(cmd_len);
        if (!current_command)
        {
            error_msg("%s : [Error] Memory allocation error during command construction.\n",cmdname);
            free_up(chain);
            free(command);
            free(cmdname);
            return EXIT_FAILURE;
        }
        snprintf(current_command, cmd_len, "%s %s", command, current->filename);

        if (verbose)
        {
            vrbs_msg("filename[%d] = '%s'\n", index, current->filename);
            vrbs_msg("start command : %s\n", current_command);
        }
        else if (show_filename)
        {
            showfname_msg("file: %s\n", current->filename);
            showfname_flush();
        }


        result = system(current_command);
        if ( result == -1 )
        {
            error_msg("%s : [Error] Command execution failed for '%s'.\n", cmdname, current->filename);
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
    free(cmdname);

    return EXIT_SUCCESS;
}

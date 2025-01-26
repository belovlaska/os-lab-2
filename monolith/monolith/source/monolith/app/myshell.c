
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

void print_help() {
    printf("Available commands:\n");
    printf("  ema-search-int <iterations>  - Run the EMA search benchmark.\n");
    printf("  factorize <iterations>       - Run the factorize benchmark.\n");
    printf("  both <iterations>            - Run both benchmarks in parallel.\n");
    printf("  help                         - Show this help message.\n");
    printf("  exit                         - Exit the shell.\n");
}

void run_task_with_exec(const char* task_name, int iterations) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        char iterations_str[16];
        snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);

        if (strcmp(task_name, "factorize") == 0) {
            execl("./factorize", "factorize", iterations_str, NULL);
        } else if (strcmp(task_name, "ema-search-int") == 0) {
            execl("./ema-search-int", "ema-search-int", iterations_str, NULL);
        } else {
            fprintf(stderr, "Unknown task: %s\n", task_name);
            exit(EXIT_FAILURE);
        }
        // If exec fails
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    // Parent process continues
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "ema-search-int") == 0) {
            if (argc != 3) {
                printf("Usage: ema-search-int <iterations>\n");
                return 1;
            }
            int iterations = atoi(argv[2]);
            if (iterations <= 0) {
                printf("Iterations must be positive.\n");
                return 1;
            }
            run_task_with_exec("ema-search-int", iterations);
            wait(NULL); // Wait for child to finish
        } else if (strcmp(argv[1], "factorize") == 0) {
            if (argc != 3) {
                printf("Usage: factorize <iterations>\n");
                return 1;
            }
            int iterations = atoi(argv[2]);
            if (iterations <= 0) {
                printf("Iterations must be positive.\n");
                return 1;
            }
            run_task_with_exec("factorize", iterations);
            wait(NULL); // Wait for child to finish
        } else if (strcmp(argv[1], "both") == 0) {
            if (argc != 3) {
                printf("Usage: both <iterations>\n");
                return 1;
            }
            int iterations = atoi(argv[2]);
            if (iterations <= 0) {
                printf("Iterations must be positive.\n");
                return 1;
            }

            pid_t pid1 = fork();
            if (pid1 < 0) {
                perror("Failed to fork process for factorize");
                return 1;
            }
            if (pid1 == 0) {
                char iterations_str[16];
                snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);
                execl("./factorize", "factorize", iterations_str, NULL);
                perror("execl failed for factorize");
                exit(EXIT_FAILURE);
            }

            pid_t pid2 = fork();
            if (pid2 < 0) {
                perror("Failed to fork process for ema-search-int");
                return 1;
            }
            if (pid2 == 0) {
                char iterations_str[16];
                snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);
                execl("./ema-search-int", "ema-search-int", iterations_str, NULL);
                perror("execl failed for ema-search-int");
                exit(EXIT_FAILURE);
            }

            // Parent process waits for both children
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        } else if (strcmp(argv[1], "help") == 0) {
            print_help();
        } else {
            printf("Unknown command: %s\n", argv[1]);
            print_help();
        }
    } else {
        char command[256];
        char* args[10];
        int running = 1;
        printf("Welcome to the shell! Type 'help' for available commands.\n");
        while (running) {
            struct timespec shell_start, shell_end;
            clock_gettime(CLOCK_REALTIME, &shell_start);

            printf("~ ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = 0;

            if (strlen(command) == 0) {
                clock_gettime(CLOCK_REALTIME, &shell_end);
                double time_spent = (shell_end.tv_sec - shell_start.tv_sec) +
                                    (shell_end.tv_nsec - shell_start.tv_nsec) / 1e9;
                printf("Shell execution time: %.8f seconds\n", time_spent);
                continue;
            }

            char* token = strtok(command, " ");
            int arg_count = 0;
            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            if (arg_count > 0) {
                if (strcmp(args[0], "ema-search-int") == 0) {
                    if (arg_count != 2) {
                        printf("Usage: ema-search-int <iterations>\n");
                    } else {
                        int iterations = atoi(args[1]);
                        if (iterations <= 0) {
                            printf("Iterations must be positive.\n");
                        } else {
                            run_task_with_exec("ema-search-int", iterations);
                            wait(NULL);
                        }
                    }
                } else if (strcmp(args[0], "factorize") == 0) {
                    if (arg_count != 2) {
                        printf("Usage: factorize <iterations>\n");
                    } else {
                        int iterations = atoi(args[1]);
                        if (iterations <= 0) {
                            printf("Iterations must be positive.\n");
                        } else {
                            run_task_with_exec("factorize", iterations);
                            wait(NULL);
                        }
                    }
                } else if (strcmp(args[0], "both") == 0) {
                    if (arg_count != 2) {
                        printf("Usage: both <iterations>\n");
                    } else {
                        int iterations = atoi(args[1]);
                        if (iterations <= 0) {
                            printf("Iterations must be positive.\n");
                        } else {
                            pid_t pid1 = fork();
                            if (pid1 < 0) {
                                perror("Failed to fork process for factorize");
                                return 1;
                            }
                            if (pid1 == 0) {
                                char iterations_str[16];
                                snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);
                                execl("./factorize", "factorize", iterations_str, NULL);
                                perror("execl failed for factorize");
                                exit(EXIT_FAILURE);
                            }

                            pid_t pid2 = fork();
                            if (pid2 < 0) {
                                perror("Failed to fork process for ema-search-int");
                                return 1;
                            }
                            if (pid2 == 0) {
                                char iterations_str[16];
                                snprintf(iterations_str, sizeof(iterations_str), "%d", iterations);
                                execl("./ema-search-int", "ema-search-int", iterations_str, NULL);
                                perror("execl failed for ema-search-int");
                                exit(EXIT_FAILURE);
                            }

                            waitpid(pid1, NULL, 0);
                            waitpid(pid2, NULL, 0);
                        }
                    }
                } else if (strcmp(args[0], "help") == 0) {
                    print_help();
                } else if (strcmp(args[0], "exit") == 0) {
                    running = 0;
                } else {
                    printf("Unknown command: %s\n", args[0]);
                    print_help();
                }
            }

            clock_gettime(CLOCK_REALTIME, &shell_end);
            double time_spent = (shell_end.tv_sec - shell_start.tv_sec) +
                                (shell_end.tv_nsec - shell_start.tv_nsec) / 1e9;
            printf("Shell execution time: %.3f seconds\n", time_spent);
        }
    }

    return 0;
}

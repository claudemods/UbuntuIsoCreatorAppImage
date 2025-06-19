#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RED "\033[31m"
#define DARK_RED "\033[91m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

// Function prototypes
void run_sudo_command(const char *command, const char *password);
void create_iso();
void run_iso_in_qemu();
void main_menu();
char* prompt(const char *prompt_text);
void error_box(const char *title, const char *message);
void message_box(const char *title, const char *message);
void progress_dialog(const char *message);
void run_command(const char *command);

int main() {
    main_menu();
    return 0;
}

void run_sudo_command(const char *command, const char *password) {
    printf("Running command: %s\n", command);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        char full_command[1024];
        snprintf(full_command, sizeof(full_command), "sudo -S %s", command);

        execl("/bin/sh", "sh", "-c", full_command, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[0]);
        write(pipefd[1], password, strlen(password));
        write(pipefd[1], "\n", 1);
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                printf("Command failed with exit code %d.\n", exit_code);
            }
        }
    }
}

void create_iso() {
    char *iso_name = prompt("What do you want to name your .iso? ");
    if (!iso_name || strlen(iso_name) == 0) {
        error_box("Input Error", "ISO name cannot be empty.");
        free(iso_name);
        return;
    }

    char application_dir_path[1024];
    if (getcwd(application_dir_path, sizeof(application_dir_path)) == NULL) {
        perror("getcwd");
        free(iso_name);
        return;
    }

    char build_image_dir[2048];
    snprintf(build_image_dir, sizeof(build_image_dir), "%s/build-image-noble", application_dir_path);

    progress_dialog("Creating ISO...");

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H%M", t);

    char iso_file_name[256];
    snprintf(iso_file_name, sizeof(iso_file_name), "%s_amd64_%s.iso", iso_name, timestamp);

    char xorriso_command[4096];
    snprintf(xorriso_command, sizeof(xorriso_command),
             "xorriso -as mkisofs -o %s -V 2025 -iso-level 3 "
             "-isohybrid-mbr /usr/lib/ISOLINUX/isohdpfx.bin "
             "-c isolinux/boot.cat -b isolinux/isolinux.bin "
             "-no-emul-boot -boot-load-size 4 -boot-info-table "
             "-eltorito-alt-boot -e boot/grub/efiboot.img "
             "-no-emul-boot -isohybrid-gpt-basdat %s",
             iso_file_name, build_image_dir);

    char *sudo_password = prompt("Enter your sudo password: ");
    if (!sudo_password || strlen(sudo_password) == 0) {
        error_box("Input Error", "Sudo password cannot be empty.");
        free(iso_name);
        free(sudo_password);
        return;
    }

    run_sudo_command(xorriso_command, sudo_password);
    message_box("Success", "ISO creation completed.");

    char *choice = prompt("Press 'm' to go back to main menu or Enter to exit: ");
    if (choice && choice[0] == 'm') {
        run_command("ruby /opt/claudemods-iso-konsole-script/demo.rb");
    }

    free(iso_name);
    free(sudo_password);
    free(choice);
}

void run_iso_in_qemu() {
    const char *qemu_script = "/opt/claudemods-iso-konsole-script/Supported-Distros/qemu.rb";
    char command[1024];
    snprintf(command, sizeof(command), "ruby %s", qemu_script);
    run_command(command);
}

void main_menu() {
    const char *ascii_art = RED
    "░█████╗░██╗░░░░░░█████╗░██╗░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗\n"
    "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝\n"
    "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░\n"
    "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗\n"
    "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝\n"
    "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░\n"
    "\n"
    "ClaudeMods ISO Creator Script v1.0 Written in C!" RESET;

    while (1) {
        printf("\033[H\033[J");
        printf("%s\n", ascii_art);
        printf(DARK_RED "Main Menu:\n" RESET);
        printf(DARK_RED "1. Create ISO\n" RESET);
        printf(DARK_RED "2. Run ISO in QEMU\n" RESET);
        printf(DARK_RED "3. Exit\n" RESET);
        char *choice = prompt("Choose an option: ");

        if (strcmp(choice, "1") == 0) {
            create_iso();
        } else if (strcmp(choice, "2") == 0) {
            run_iso_in_qemu();
        } else if (strcmp(choice, "3") == 0) {
            printf("Exiting...\n");
            free(choice);
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
        free(choice);
    }
}

#define _GNU_SOURCE
#include <stdio.h>

char* prompt(const char *prompt_text) {
    printf("%s", prompt_text);
    char *input = NULL;
    size_t len = 0;
    ssize_t read = getline(&input, &len, stdin);
    if (read == -1) {
        perror("getline");
        return NULL;
    }
    if (read > 0 && input[read-1] == '\n') {
        input[read-1] = '\0';
    }
    return input;
}

void error_box(const char *title, const char *message) {
    printf("%s: %s\n", title, message);
}

void message_box(const char *title, const char *message) {
    printf("%s: %s\n", title, message);
}

void progress_dialog(const char *message) {
    printf("%s\n", message);
}

void run_command(const char *command) {
    printf("Running command: %s\n", command);
    int exit_code = system(command);
    if (exit_code != 0) {
        printf("Command failed with exit code %d.\n", exit_code);
    }
}

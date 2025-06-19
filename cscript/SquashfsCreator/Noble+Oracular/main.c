#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

#define MAX_PATH PATH_MAX
#define MAX_CMD 16384

void print_banner() {
    printf("\033[31m");
    printf(
        "░█████╗░██╗░░░░░░█████╗░██╗░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗\n"
        "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝\n"
        "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░\n"
        "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗\n"
        "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝\n"
        "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░\n"
    );
    printf("\033[0m");
    printf("\033[31mClaudemods SquashFS Creator v1.02+ Special This Could Take a While!\033[0m\n\n");
}

bool dir_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void run_command(const char* command) {
    printf("Executing: %s\n", command);
    int status = system(command);
    if (status != 0) {
        fprintf(stderr, "Command failed with status %d: %s\n", status, command);
        exit(EXIT_FAILURE);
    }
}

char* get_kernel_version() {
    static char version[256] = "unknown";
    FILE* fp = popen("uname -r", "r");
    if (!fp) {
        perror("Failed to get kernel version");
        return version;
    }

    if (fgets(version, sizeof(version), fp)) {
        version[strcspn(version, "\n")] = 0;
    }
    
    pclose(fp);
    return version;
}

void clone_system(const char* clone_dir) {
    if (dir_exists(clone_dir)) {
        printf("Skipping removal of existing clone directory: %s\n", clone_dir);
    } else {
        mkdir(clone_dir, 0755);
    }

    const char* command = "sudo rsync -aHAxSr --numeric-ids --info=progress2 "
                         "--include=dev --include=usr --include=proc --include=tmp --include=sys "
                         "--include=run --include=media "
                         "--exclude=dev/* --exclude=proc/* --exclude=tmp/* --exclude=sys/* "
                         "--exclude=run/* --exclude=media/* --exclude=clone_system_temp "
                         "/ clone_system_temp";

    printf("Cloning system directory to: %s\n", clone_dir);
    run_command(command);
}

void create_squashfs_image(void) {
    const char* command = "sudo mksquashfs clone_system_temp /home/$USER/.config/cui/build-image-noble/live/filesystem.squashfs "
                         "-comp xz -Xbcj x86 -b 1M -no-duplicates -no-recovery "
                         "-always-use-fragments -wildcards -xattrs";

    printf("Creating SquashFS image: filesystem.squashfs\n");
    run_command(command);
}

void delete_clone_system_temp(void) {
    char command[MAX_CMD];
    strcpy(command, "sudo rm -rf clone_system_temp");
    printf("Deleting temporary clone directory: clone_system_temp\n");
    run_command(command);

    struct stat st;
    if (stat("filesystem.squashfs", &st) == 0) {
        strcpy(command, "sudo rm -f filesystem.squashfs");
        printf("Deleting SquashFS image: filesystem.squashfs\n");
        run_command(command);
    } else {
        printf("SquashFS image does not exist: filesystem.squashfs\n");
    }
}

int main() {
    print_banner();

    char cwd[MAX_PATH];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd failed");
        return EXIT_FAILURE;
    }

    printf("Kernel version: %s\n", get_kernel_version());
    printf("Current directory: %s\n", cwd);

    const char* clone_dir = "clone_system_temp";

    printf("Temporary clone directory: %s\n", clone_dir);

    printf("Select an option:\n");
    printf("3. Max compression (xz)\n");
    printf("4. Create SquashFS from clone_system_temp\n");
    printf("5. Delete clone_system_temp and SquashFS image\n");

    int choice;
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }

    switch (choice) {
        case 3:
            if (!dir_exists(clone_dir)) {
                clone_system(clone_dir);
            }
            create_squashfs_image();
            break;
        case 4:
            if (!dir_exists(clone_dir)) {
                fprintf(stderr, "Error: clone_system_temp doesn't exist\n");
                return EXIT_FAILURE;
            }
            create_squashfs_image();
            break;
        case 5:
            delete_clone_system_temp();
            break;
        default:
            fprintf(stderr, "Invalid choice. Exiting.\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

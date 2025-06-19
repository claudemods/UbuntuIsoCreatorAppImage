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

// Use PATH_MAX from limits.h for maximum path safety
#define MAX_PATH PATH_MAX
#define MAX_CMD 16384
#define MAX_EXCLUDES 50

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

void clone_system(const char* clone_dir, const char** exclude_dirs, int exclude_count) {
    if (dir_exists(clone_dir)) {
        printf("Skipping removal of existing clone directory: %s\n", clone_dir);
    } else {
        mkdir(clone_dir, 0755);
    }

    char command[MAX_CMD] = "sudo rsync -aHAxSr --numeric-ids --info=progress2 "
    "--include=dev --include=proc --include=tmp --include=sys "
    "--include=run --include=media "
    "--exclude=dev/* --exclude=proc/* --exclude=tmp/* --exclude=sys/* "
    "--exclude=run/* --exclude=media/* --exclude=clone_system_temp ";

    for (int i = 0; i < exclude_count; i++) {
        char relative_dir[MAX_PATH];
        strncpy(relative_dir, exclude_dirs[i], sizeof(relative_dir) - 1);
        relative_dir[sizeof(relative_dir) - 1] = '\0';

        while (relative_dir[0] == '/') {
            memmove(relative_dir, relative_dir + 1, strlen(relative_dir));
        }

        if (strlen(command) + strlen("--exclude=") + strlen(relative_dir) + 2 > sizeof(command)) {
            fprintf(stderr, "Command buffer overflow\n");
            exit(EXIT_FAILURE);
        }
        strcat(command, "--exclude=");
        strcat(command, relative_dir);
        strcat(command, " ");
    }

    if (strlen(command) + strlen("/ ") + strlen(clone_dir) + 1 > sizeof(command)) {
        fprintf(stderr, "Command buffer overflow\n");
        exit(EXIT_FAILURE);
    }
    strcat(command, "/ ");
    strcat(command, clone_dir);

    printf("Cloning system directory to: %s\n", clone_dir);
    run_command(command);
}

void create_squashfs_image(const char* output_image, const char* clone_dir,
                           const char** exclude_dirs, int exclude_count) {
    char command[MAX_CMD];
    size_t written = snprintf(command, sizeof(command),
                              "sudo mksquashfs %s %s -comp xz -Xbcj x86 -b 1M -no-duplicates -no-recovery -always-use-fragments -wildcards -xattrs",
                              clone_dir, output_image);

    if (written >= sizeof(command)) {
        fprintf(stderr, "Failed to build mksquashfs command\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < exclude_count; i++) {
        char relative_dir[MAX_PATH];
        strncpy(relative_dir, exclude_dirs[i], sizeof(relative_dir) - 1);
        relative_dir[sizeof(relative_dir) - 1] = '\0';

        while (relative_dir[0] == '/') {
            memmove(relative_dir, relative_dir + 1, strlen(relative_dir));
        }

        if (strlen(command) + strlen(" -e ") + strlen(relative_dir) + 1 > sizeof(command)) {
            fprintf(stderr, "Command buffer overflow\n");
            exit(EXIT_FAILURE);
        }
        strcat(command, " -e ");
        strcat(command, relative_dir);
    }

    printf("Creating SquashFS image: %s\n", output_image);
    run_command(command);
                           }

                           void delete_clone_system_temp(const char* clone_dir, const char* squashfs_image) {
                               char command[MAX_CMD];
                               if (snprintf(command, sizeof(command), "sudo rm -rf %s", clone_dir) >= (int)sizeof(command)) {
                                   fprintf(stderr, "Path too long for clone_dir\n");
                                   exit(EXIT_FAILURE);
                               }
                               printf("Deleting temporary clone directory: %s\n", clone_dir);
                               run_command(command);

                               struct stat st;
                               if (stat(squashfs_image, &st) == 0) {
                                   if (snprintf(command, sizeof(command), "sudo rm -f %s", squashfs_image) >= (int)sizeof(command)) {
                                       fprintf(stderr, "Path too long for squashfs_image\n");
                                       exit(EXIT_FAILURE);
                                   }
                                   printf("Deleting SquashFS image: %s\n", squashfs_image);
                                   run_command(command);
                               } else {
                                   printf("SquashFS image does not exist: %s\n", squashfs_image);
                               }
                           }

                           int main() {
                               print_banner();

                               char cwd[MAX_PATH];
                               if (!getcwd(cwd, sizeof(cwd))) {
                                   perror("getcwd failed");
                                   return EXIT_FAILURE;
                               }

                               char build_dir[MAX_PATH];
                               if (snprintf(build_dir, sizeof(build_dir), "/opt/claudemods-iso-konsole-script/Supported-Distros/Ubuntu/build-image-noble/live") >= (int)sizeof(build_dir)) {
                                   fprintf(stderr, "Path too long for build_dir\n");
                                   return EXIT_FAILURE;
                               }
                               mkdir(build_dir, 0755);

                               char squashfs_image[MAX_PATH];
                               if (snprintf(squashfs_image, sizeof(squashfs_image), "%s/filesystem.squashfs", build_dir) >= (int)sizeof(squashfs_image)) {
                                   fprintf(stderr, "Path too long for squashfs_image\n");
                                   return EXIT_FAILURE;
                               }

                               const char* exclude_dirs[] = {
                                   "var/lib/docker", "clone_system_temp", "etc/fstab", "etc/mtab",
                                   "etc/udev/rules.d/70-persistent-cd.rules", "etc/udev/rules.d/70-persistent-net.rules"
                               };
                               int exclude_count = sizeof(exclude_dirs)/sizeof(exclude_dirs[0]);

                               char clone_dir[MAX_PATH];
                               if (snprintf(clone_dir, sizeof(clone_dir), "%s/clone_system_temp", cwd) >= (int)sizeof(clone_dir)) {
                                   fprintf(stderr, "Path too long for clone_dir\n");
                                   return EXIT_FAILURE;
                               }
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
                                           clone_system(clone_dir, exclude_dirs, exclude_count);
                                       }
                                       create_squashfs_image(squashfs_image, clone_dir, exclude_dirs, exclude_count);
                                       break;
                                   case 4:
                                       if (!dir_exists(clone_dir)) {
                                           fprintf(stderr, "Error: clone_system_temp doesn't exist\n");
                                           return EXIT_FAILURE;
                                       }
                                       create_squashfs_image(squashfs_image, clone_dir, exclude_dirs, exclude_count);
                                       break;
                                   case 5:
                                       delete_clone_system_temp(clone_dir, squashfs_image);
                                       break;
                                   default:
                                       fprintf(stderr, "Invalid choice. Exiting.\n");
                                       return EXIT_FAILURE;
                               }

                               return EXIT_SUCCESS;
                           }

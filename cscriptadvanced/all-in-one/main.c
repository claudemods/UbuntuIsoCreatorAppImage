#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <libgen.h>
#include <dirent.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#define MAX_PATH PATH_MAX
#define MAX_CMD 16384
#define RED "\033[31m"
#define DARK_RED "\033[91m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

// ==================== UTILITY FUNCTIONS ====================

void print_blue(const char *text) {
    printf("\033[34m%s\033[0m\n", text);
}

void message_box(const char *title, const char *message) {
    printf("\033[32m%s\033[0m\n", title);
    printf("\033[32m%s\033[0m\n", message);
}

void error_box(const char *title, const char *message) {
    printf("\033[31m%s\033[0m\n", title);
    printf("\033[31m%s\033[0m\n", message);
}

void progress_dialog(const char *message) {
    printf("\033[33m%s\033[0m\n", message);
}

void run_command(const char *command) {
    printf("\033[34mRunning command: %s\033[0m\n", command);
    int status = system(command);
    if (status != 0) {
        printf("\033[31mCommand failed with exit code: %d\033[0m\n", WEXITSTATUS(status));
    }
}

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

bool dir_exists(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
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
    printf("\033[31mClaudemods ISO Tools v1.0\033[0m\n\n");
}

// ==================== ISO CONFIGURATOR FUNCTIONS ====================

void install_dependencies_ubuntu() {
    progress_dialog("Installing dependencies...");
    const char *packages =
    "cryptsetup dmeventd isolinux libaio-dev libcares2 "
    "libdevmapper-event1.02.1 liblvm2cmd2.03 live-boot "
    "live-boot-doc live-boot-initramfs-tools live-config-systemd "
    "live-tools lvm2 pxelinux syslinux syslinux-common "
    "thin-provisioning-tools squashfs-tools xorriso";

    char command[512];
    sprintf(command, "sudo apt-get install -y %s", packages);
    run_command(command);
    message_box("Success", "Dependencies installed successfully.");
}

void copy_vmlinuz_noble() {
    progress_dialog("Copying vmlinuz (Noble)...");
    run_command("sudo cp /boot/vmlinuz-6.11.0-26-generic /home/$USER/.config/cui/build-image-noble/live/");
    message_box("Success", "Vmlinuz copied successfully.");
}

void copy_vmlinuz_oracular() {
    progress_dialog("Copying vmlinuz (Oracular)...");
    run_command("sudo cp /boot/vmlinuz-6.11.0-26-generic /home/$USER/.config/cui/build-image-oracular/live/");
    message_box("Success", "Vmlinuz copied successfully.");
}

void generate_initrd_noble() {
    progress_dialog("Generating Initramfs (Noble)...");
    run_command("sudo mkinitramfs -o \"/home/$USER/.config/cui/build-image-noble/live/initrd.img-$(uname -r)\" \"$(uname -r)\"");
    message_box("Success", "Initramfs generated successfully.");
}

void generate_initrd_oracular() {
    progress_dialog("Generating Initramfs (Oracular)...");
    run_command("sudo mkinitramfs -o \"/home/$USER/.config/cui/build-image-oracular/live/initrd.img-$(uname -r)\" \"$(uname -r)\"");
    message_box("Success", "Initramfs generated successfully.");
}

void edit_grub_cfg_noble() {
    progress_dialog("Opening grub.cfg (Noble)...");
    run_command("nano /home/$USER/.config/cui/build-image-noble/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

void edit_grub_cfg_oracular() {
    progress_dialog("Opening grub.cfg (Oracular)...");
    run_command("nano /home/$USER/.config/cui/build-image-oracular/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

void edit_isolinux_cfg_noble() {
    progress_dialog("Opening isolinux.cfg (Noble)...");
    run_command("nano /home/$USER/.config/cui/build-image-noble/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

void edit_isolinux_cfg_oracular() {
    progress_dialog("Opening isolinux.cfg (Oracular)...");
    run_command("nano /home/$USER/.config/cui/build-image-oracular/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

// ==================== ISO CREATOR FUNCTIONS ====================

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
             "-eltorito-alt-boot -e boot/grub/efi.img "
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

// ==================== SQUASHFS CREATOR FUNCTIONS ====================

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

// ==================== MENU FUNCTIONS ====================

void noble_menu() {
    int choice;
    do {
        print_blue("\nChoose your Noble 24.04 action:");
        print_blue("1. Install Dependencies");
        print_blue("2. Copy vmlinuz");
        print_blue("3. Generate Initrd");
        print_blue("4. Edit grub.cfg");
        print_blue("5. Edit isolinux.cfg");
        print_blue("6. Back to Main Menu");
        printf("> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: install_dependencies_ubuntu(); break;
            case 2: copy_vmlinuz_noble(); break;
            case 3: generate_initrd_noble(); break;
            case 4: edit_grub_cfg_noble(); break;
            case 5: edit_isolinux_cfg_noble(); break;
            case 6: return;
            default: print_blue("Invalid choice."); break;
        }
    } while (choice != 6);
}

void oracular_menu() {
    int choice;
    do {
        print_blue("\nChoose your Oracular 24.10 action:");
        print_blue("1. Install Dependencies");
        print_blue("2. Copy vmlinuz");
        print_blue("3. Generate Initrd");
        print_blue("4. Edit grub.cfg");
        print_blue("5. Edit isolinux.cfg");
        print_blue("6. Back to Main Menu");
        printf("> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: install_dependencies_ubuntu(); break;
            case 2: copy_vmlinuz_oracular(); break;
            case 3: generate_initrd_oracular(); break;
            case 4: edit_grub_cfg_oracular(); break;
            case 5: edit_isolinux_cfg_oracular(); break;
            case 6: return;
            default: print_blue("Invalid choice."); break;
        }
    } while (choice != 6);
}

void squashfs_menu() {
    char cwd[MAX_PATH];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd failed");
        return;
    }
    
    printf("Kernel version: %s\n", get_kernel_version());
    printf("Current directory: %s\n", cwd);
    
    const char* clone_dir = "clone_system_temp";
    
    printf("Temporary clone directory: %s\n", clone_dir);
    
    printf("Select an option:\n");
    printf("1. Max compression (xz)\n");
    printf("2. Create SquashFS from clone_system_temp\n");
    printf("3. Delete clone_system_temp and SquashFS image\n");
    printf("4. Back to Main Menu\n");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input\n");
        return;
    }
    
    switch (choice) {
        case 1:
            if (!dir_exists(clone_dir)) {
                clone_system(clone_dir);
            }
            create_squashfs_image();
            break;
        case 2:
            if (!dir_exists(clone_dir)) {
                fprintf(stderr, "Error: clone_system_temp doesn't exist\n");
                return;
            }
            create_squashfs_image();
            break;
        case 3:
            delete_clone_system_temp();
            break;
        case 4:
            return;
        default:
            fprintf(stderr, "Invalid choice.\n");
            break;
    }
}

void iso_creator_menu() {
    while (1) {
        printf("\033[H\033[J");
        print_banner();
        printf(DARK_RED "ISO Creator Menu:\n" RESET);
        printf(DARK_RED "1. Create ISO\n" RESET);
        printf(DARK_RED "2. Run ISO in QEMU\n" RESET);
        printf(DARK_RED "3. Back to Main Menu\n" RESET);
        char *choice = prompt("Choose an option: ");
        
        if (strcmp(choice, "1") == 0) {
            create_iso();
        } else if (strcmp(choice, "2") == 0) {
            run_iso_in_qemu();
        } else if (strcmp(choice, "3") == 0) {
            printf("Returning to main menu...\n");
            free(choice);
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
        free(choice);
    }
}

// ==================== MAIN MENU ====================

int main() {
    int choice;
    do {
        printf("\033[H\033[J");  // Clear screen
        print_banner();
        print_blue("\n=== Main Menu ===");
        print_blue("1. Noble 24.04 Configuration");
        print_blue("2. Oracular 24.10 Configuration");
        print_blue("3. ISO Creator");
        print_blue("4. SquashFS Creator");
        print_blue("5. Exit");
        printf("> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: noble_menu(); break;
            case 2: oracular_menu(); break;
            case 3: iso_creator_menu(); break;
            case 4: squashfs_menu(); break;
            case 5: break;
            default: print_blue("Invalid choice."); break;
        }
    } while (choice != 5);

    return 0;
}

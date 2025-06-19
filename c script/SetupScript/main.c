#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define PATH_MAX 4096
#define COMMAND_MAX 8192

// ANSI Color Codes
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// Function prototypes
void main_menu();
void print_dark_red(const char *text);
void print_version_menu(const char *version);
void print_action_menu(const char *version);
int run_command(const char *command);
void copy_vmlinuz(const char *version);
void generate_initrd(const char *version);
void edit_grub_cfg(const char *version);
void edit_isolinux_cfg(const char *version);
void install_dependencies();
void clear_screen();

int main() {
    int choice;
    char input[10];
    
    while (1) {
        clear_screen();
        main_menu();
        print_dark_red("Choose your Ubuntu version:");
        print_dark_red("1. Noble 24.04");
        print_dark_red("2. Oracular 24.10");
        print_dark_red("3. Exit");
        printf("Select: ");
        
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);
        
        if (choice == 3) break;
        
        const char *version = (choice == 1) ? "noble" : "oracular";
        
        while (1) {
            clear_screen();
            main_menu();
            print_version_menu(version);
            print_action_menu(version);
            printf("Select action: ");
            
            fgets(input, sizeof(input), stdin);
            int action = atoi(input);
            
            if (action == 6) break;
            
            switch (action) {
                case 1: install_dependencies(); break;
                case 2: copy_vmlinuz(version); break;
                case 3: generate_initrd(version); break;
                case 4: edit_grub_cfg(version); break;
                case 5: edit_isolinux_cfg(version); break;
                default: print_dark_red("Invalid choice");
            }
            printf(CYAN "\nPress Enter to continue..." RESET);
            getchar();
        }
    }
    return 0;
}

void main_menu() {
    printf(RED
    "░█████╗░██╗░░░░░░█████╗░██╗░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗\n"
    "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝\n"
    "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░\n"
    "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗\n"
    "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝\n"
    "░╚════╝░╚══════╝╚═╝░░╚═╝░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░\n"
    RESET
    "claudemods iso configurator v1.01\n");
}

void print_dark_red(const char *text) {
    printf(RED "%s" RESET "\n", text);
}

void print_version_menu(const char *version) {
    printf(MAGENTA "\n╔══════════════════════════════════════════╗\n" RESET);
    printf(MAGENTA "║" RESET "  Ubuntu %s ISO Configuration  " MAGENTA "║\n" RESET,
           strcmp(version, "noble") == 0 ? "Noble 24.04" : "Oracular 24.10");
    printf(MAGENTA "╚══════════════════════════════════════════╝\n\n" RESET);
}

void print_action_menu(const char *version) {
    printf(YELLOW "1. Install Dependencies\n" RESET);
    printf(YELLOW "2. Copy vmlinuz to build-image-%s/live\n" RESET, version);
    printf(YELLOW "3. Generate initrd\n" RESET);
    printf(YELLOW "4. Edit grub.cfg\n" RESET);
    printf(YELLOW "5. Edit isolinux.cfg\n" RESET);
    printf(RED "6. Back to main menu\n" RESET);
}

int run_command(const char *command) {
    printf(BLUE "\nExecuting: %s\n" RESET, command);
    int result = system(command);
    if (result != 0) {
        printf(RED "Command failed with error %d\n" RESET, result);
        return -1;
    }
    return 0;
}

void copy_vmlinuz(const char *version) {
    char mkdir_cmd[COMMAND_MAX];
    char cp_cmd[COMMAND_MAX];
    
    // Create directory structure
    strcpy(mkdir_cmd, "mkdir -p build-image-");
    strcat(mkdir_cmd, version);
    strcat(mkdir_cmd, "/live");
    run_command(mkdir_cmd);
    
    // Copy vmlinuz to live directory
    strcpy(cp_cmd, "sudo cp /boot/vmlinuz-$(uname -r) build-image-");
    strcat(cp_cmd, version);
    strcat(cp_cmd, "/live/vmlinuz");
    run_command(cp_cmd);
}

void generate_initrd(const char *version) {
    char command[COMMAND_MAX];
    strcpy(command, "sudo mkinitramfs -o build-image-");
    strcat(command, version);
    strcat(command, "/live/initrd.img-$(uname -r) $(uname -r)");
    run_command(command);
}

void edit_grub_cfg(const char *version) {
    char command[COMMAND_MAX];
    strcpy(command, "nano build-image-");
    strcat(command, version);
    strcat(command, "/boot/grub/grub.cfg");
    run_command(command);
}

void edit_isolinux_cfg(const char *version) {
    char command[COMMAND_MAX];
    strcpy(command, "nano build-image-");
    strcat(command, version);
    strcat(command, "/isolinux/isolinux.cfg");
    run_command(command);
}

void install_dependencies() {
    const char *packages = "cryptsetup dmeventd isolinux libaio-dev libcares2 "
                          "libdevmapper-event1.02.1 liblvm2cmd2.03 live-boot "
                          "live-boot-doc live-boot-initramfs-tools live-config-systemd "
                          "live-tools lvm2 pxelinux syslinux syslinux-common "
                          "thin-provisioning-tools squashfs-tools xorriso";
    
    char command[COMMAND_MAX];
    strcpy(command, "sudo apt-get install -y ");
    strcat(command, packages);
    run_command(command);
}

void clear_screen() {
    printf("\033[H\033[J");
}

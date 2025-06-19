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
#define RESET   "\x1b[0m"

// Function prototypes
void main_menu();
void print_dark_red(const char *text);
int run_command(const char *command);
void copy_vmlinuz(const char *version);
void generate_initrd(const char *version);
void edit_config(const char *version, const char *file);
void install_dependencies();
void clear_screen();
char* get_kernel_version();

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
            print_dark_red(choice == 1 ? "Noble 24.04 Actions:" : "Oracular 24.10 Actions:");
            print_dark_red("1. Install Dependencies");
            print_dark_red("2. Copy vmlinuz");
            print_dark_red("3. Generate initrd");
            print_dark_red("4. Edit boot configs");
            print_dark_red("5. Back to main menu");
            printf("Select action: ");
            
            fgets(input, sizeof(input), stdin);
            int action = atoi(input);
            
            if (action == 5) break;
            
            switch (action) {
                case 1: install_dependencies(); break;
                case 2: copy_vmlinuz(version); break;
                case 3: generate_initrd(version); break;
                case 4: {
                    printf("Edit:\n1. grub.cfg\n2. isolinux.cfg\nSelect: ");
                    fgets(input, sizeof(input), stdin);
                    int cfg_choice = atoi(input);
                    if (cfg_choice == 1) edit_config(version, "grub.cfg");
                    else if (cfg_choice == 2) edit_config(version, "isolinux.cfg");
                    break;
                }
                default: print_dark_red("Invalid choice");
            }
            printf("\nPress Enter to continue...");
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

int run_command(const char *command) {
    printf(BLUE "Running: %s\n" RESET, command);
    return system(command);
}

char* get_kernel_version() {
    static char version[KERNEL_VERSION_MAX];
    FILE *fp = popen("uname -r", "r");
    if (!fp) {
        print_dark_red("Failed to get kernel version");
        return NULL;
    }
    if (!fgets(version, sizeof(version), fp)) {
        pclose(fp);
        print_dark_red("Failed to read kernel version");
        return NULL;
    }
    pclose(fp);
    version[strcspn(version, "\n")] = 0; // Remove newline
    return version;
}

void copy_vmlinuz(const char *version) {
    char *kernel_version = get_kernel_version();
    if (!kernel_version) return;

    char cmd[COMMAND_MAX];
    
    // Create directory structure
    sprintf(cmd, "mkdir -p build-image-%s/live", version);
    run_command(cmd);
    
    // Copy vmlinuz (from versioned source to unversioned destination)
    sprintf(cmd, "sudo cp /boot/vmlinuz-%s build-image-%s/live/vmlinuz", 
            kernel_version, version);
    
    if (run_command(cmd) == 0) {
        printf(GREEN "Successfully copied vmlinuz-%s to build-image-%s/live/vmlinuz\n" RESET, 
               kernel_version, version);
    }
}

void generate_initrd(const char *version) {
    char *kernel_version = get_kernel_version();
    if (!kernel_version) return;

    char cmd[COMMAND_MAX];
    sprintf(cmd, "sudo mkinitramfs -o build-image-%s/live/initrd.img-%s %s", 
            version, kernel_version, kernel_version);
    
    if (run_command(cmd) == 0) {
        printf(GREEN "initrd.img-%s generated in build-image-%s/live/\n" RESET, 
               kernel_version, version);
    }
}

void edit_config(const char *version, const char *file) {
    char cmd[COMMAND_MAX];
    sprintf(cmd, "nano build-image-%s/%s/%s", 
            version,
            strcmp(file, "grub.cfg") == 0 ? "boot/grub" : "isolinux",
            file);
    run_command(cmd);
}

void install_dependencies() {
    const char *cmd = "sudo apt-get install -y "
        "cryptsetup dmeventd isolinux libaio-dev libcares2 "
        "libdevmapper-event1.02.1 liblvm2cmd2.03 live-boot "
        "live-boot-doc live-boot-initramfs-tools live-config-systemd "
        "live-tools lvm2 pxelinux syslinux syslinux-common "
        "thin-provisioning-tools squashfs-tools xorriso";
    
    if (run_command(cmd) == 0) {
        printf(GREEN "Dependencies installed successfully\n" RESET);
    }
}

void clear_screen() {
    printf("\033[H\033[J");
}

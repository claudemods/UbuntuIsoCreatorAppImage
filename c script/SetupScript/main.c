#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>

// Constants for buffer sizes
#define PATH_MAX 4096
#define COMMAND_MAX 8192
#define KERNEL_VERSION_MAX 256
#define PACKAGE_LIST_MAX 2048

// ANSI color codes
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"

// Function prototypes
void main_menu();
void print_dark_red(const char *text);
int run_command(const char *command);
char *prompt(const char *prompt_text);
void progress_dialog(const char *message);
void message_box(const char *title, const char *message);
void error_box(const char *title, const char *message);
char *find_vmlinuz();
void copy_vmlinuz();
void generate_initramfs();
void edit_grub_cfg();
void edit_syslinux_cfg();
void install_dependencies();
void copy_vmlinuz_debian();
void generate_initrd();
void edit_grub_cfg_debian();
void edit_isolinux_cfg_debian();
void noble_24_04();
void oracular_24_10();
void install_dependencies_oracular();
void copy_vmlinuz_oracular();
void generate_initrd_oracular();
void edit_grub_cfg_oracular();
void edit_isolinux_cfg_oracular();
void install_dependencies_noble();
void copy_vmlinuz_noble();
void generate_initrd_noble();
void edit_grub_cfg_noble();
void edit_isolinux_cfg_noble();
void install_dependencies_arch();
void optional_dependencies_menu();
void install_virt_manager();
void install_gnome_boxes();
int detect_distribution();
void clear_screen();

int main() {
    while (1) {
        clear_screen();
        main_menu();
        print_dark_red("Choose your distribution:");
        print_dark_red("1. Arch");
        print_dark_red("2. Ubuntu");
        print_dark_red("3. Debian");
        char choice_str[10];
        fgets(choice_str, sizeof(choice_str), stdin);
        int choice = atoi(choice_str);
        if (choice == 1) {
            print_dark_red("Choose your Arch Linux action:");
            print_dark_red("1. Install Dependencies");
            print_dark_red("2. Copy Vmlinuz");
            print_dark_red("3. Generate Initramfs");
            print_dark_red("4. Edit grub.cfg");
            print_dark_red("5. Edit syslinux.cfg");
            char arch_choice_str[10];
            fgets(arch_choice_str, sizeof(arch_choice_str), stdin);
            int arch_choice = atoi(arch_choice_str);
            switch (arch_choice) {
                case 1: install_dependencies_arch(); break;
                case 2: copy_vmlinuz(); break;
                case 3: generate_initramfs(); break;
                case 4: edit_grub_cfg(); break;
                case 5: edit_syslinux_cfg(); break;
                default: print_dark_red("Invalid choice for Arch Linux action.");
            }
        } else if (choice == 2) {
            print_dark_red("Choose your Ubuntu version:");
            print_dark_red("1. Noble 24.04");
            print_dark_red("2. Oracular 24.10");
            char ubuntu_choice_str[10];
            fgets(ubuntu_choice_str, sizeof(ubuntu_choice_str), stdin);
            int ubuntu_choice = atoi(ubuntu_choice_str);
            if (ubuntu_choice == 1) {
                noble_24_04();
                print_dark_red("Choose your Noble 24.04 action:");
                print_dark_red("1. Install Dependencies");
                print_dark_red("2. Copy vmlinuz");
                print_dark_red("3. Generate Initrd");
                print_dark_red("4. Edit grub.cfg");
                print_dark_red("5. Edit isolinux.cfg");
                char noble_choice_str[10];
                fgets(noble_choice_str, sizeof(noble_choice_str), stdin);
                int noble_choice = atoi(noble_choice_str);
                switch (noble_choice) {
                    case 1: install_dependencies_noble(); break;
                    case 2: copy_vmlinuz_noble(); break;
                    case 3: generate_initrd_noble(); break;
                    case 4: edit_grub_cfg_noble(); break;
                    case 5: edit_isolinux_cfg_noble(); break;
                    default: print_dark_red("Invalid choice for Noble 24.04 action.");
                }
            } else if (ubuntu_choice == 2) {
                oracular_24_10();
                print_dark_red("Choose your Oracular 24.10 action:");
                print_dark_red("1. Install Dependencies");
                print_dark_red("2. Copy vmlinuz");
                print_dark_red("3. Generate Initrd");
                print_dark_red("4. Edit grub.cfg");
                print_dark_red("5. Edit isolinux.cfg");
                char oracular_choice_str[10];
                fgets(oracular_choice_str, sizeof(oracular_choice_str), stdin);
                int oracular_choice = atoi(oracular_choice_str);
                switch (oracular_choice) {
                    case 1: install_dependencies_oracular(); break;
                    case 2: copy_vmlinuz_oracular(); break;
                    case 3: generate_initrd_oracular(); break;
                    case 4: edit_grub_cfg_oracular(); break;
                    case 5: edit_isolinux_cfg_oracular(); break;
                    default: print_dark_red("Invalid choice for Oracular 24.10 action.");
                }
            } else {
                print_dark_red("Invalid choice for Ubuntu version.");
            }
        } else if (choice == 3) {
            print_dark_red("Choose your Debian action:");
            print_dark_red("1. Install Dependencies");
            print_dark_red("2. Copy vmlinuz");
            print_dark_red("3. Generate Initrd");
            print_dark_red("4. Edit grub.cfg");
            print_dark_red("5. Edit isolinux.cfg");
            char debian_choice_str[10];
            fgets(debian_choice_str, sizeof(debian_choice_str), stdin);
            int debian_choice = atoi(debian_choice_str);
            switch (debian_choice) {
                case 1: install_dependencies(); break;
                case 2: copy_vmlinuz_debian(); break;
                case 3: generate_initrd(); break;
                case 4: edit_grub_cfg_debian(); break;
                case 5: edit_isolinux_cfg_debian(); break;
                default: print_dark_red("Invalid choice for Debian action.");
            }
        } else {
            print_dark_red("Invalid choice for distribution.");
        }
        print_dark_red("Press Enter to return to the main menu or type 'exit' to quit.");
        char response[100];
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = 0;
        if (strcmp(response, "exit") == 0) {
            break;
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
    printf(BLUE "Running command: %s" RESET "\n", command);
    int status = system(command);
    if (status != 0) {
        printf(RED "Command Execution Error: Command '%s' failed with exit code %d." RESET "\n",
               command, WEXITSTATUS(status));
        return -1;
    }
    return 0;
}

char *prompt(const char *prompt_text) {
    printf("%s", prompt_text);
    static char input[256];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    return input;
}

void progress_dialog(const char *message) {
    printf(YELLOW "%s" RESET "\n", message);
}

void message_box(const char *title, const char *message) {
    printf(GREEN "%s" RESET "\n", title);
    printf(GREEN "%s" RESET "\n", message);
}

void error_box(const char *title, const char *message) {
    printf(RED "%s" RESET "\n", title);
    printf(RED "%s" RESET "\n", message);
}

char *find_vmlinuz() {
    static const char *search_paths[] = {
        "/boot/vmlinuz-linux",
        "/boot/vmlinuz-linux-zen",
        "/boot/vmlinuz-linux-lts",
        NULL
    };
    for (int i = 0; search_paths[i] != NULL; i++) {
        if (access(search_paths[i], F_OK) == 0) {
            return (char *)search_paths[i];
        }
    }
    return "";
}

void copy_vmlinuz() {
    progress_dialog("Copying Vmlinuz...");
    char *vmlinuz_path = find_vmlinuz();
    if (strlen(vmlinuz_path) == 0) {
        error_box("Error", "Failed to find vmlinuz.");
        return;
    }

    const char *destination_dir = "/Arch/build-image-arch/live";
    char destination_path[PATH_MAX];
    strncpy(destination_path, destination_dir, PATH_MAX - 1);
    strncat(destination_path, "/vmlinuz", PATH_MAX - strlen(destination_path) - 1);

    // Create directory if it doesn't exist
    struct stat st = {0};
    if (stat(destination_dir, &st) == -1) {
        mkdir(destination_dir, 0755);
    }

    char command[COMMAND_MAX];
    strncpy(command, "cp ", COMMAND_MAX - 1);
    strncat(command, vmlinuz_path, COMMAND_MAX - strlen(command) - 1);
    strncat(command, " ", COMMAND_MAX - strlen(command) - 1);
    strncat(command, destination_path, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) != 0) {
        error_box("Error", "Failed to copy vmlinuz.");
        return;
    }

    char new_destination_path[PATH_MAX];
    strncpy(new_destination_path, destination_dir, PATH_MAX - 1);
    strncat(new_destination_path, "/vmlinuz-linux-zen", PATH_MAX - strlen(new_destination_path) - 1);

    if (rename(destination_path, new_destination_path) != 0) {
        error_box("Error", "Failed to rename vmlinuz.");
        return;
    }
    message_box("Success", "Vmlinuz copied and renamed successfully.");
}

void generate_initramfs() {
    progress_dialog("Generating Initramfs...");
    const char *mkinitcpio_command = "sudo mkinitcpio -c live.conf -g Arch/build-image-arch/live/initramfs-linux.img";
    if (run_command(mkinitcpio_command) == 0) {
        message_box("Success", "Initramfs generated successfully.");
    }
}

void edit_grub_cfg() {
    const char *grub_cfg_path = "/opt/claudemods-iso-konsole-script/Supported-Distros/Arch/build-image-arch/boot/grub/grub.cfg";
    if (access(grub_cfg_path, F_OK) == 0) {
        char command[PATH_MAX + 10];
        strncpy(command, "kate ", PATH_MAX + 10 - 1);
        strncat(command, grub_cfg_path, PATH_MAX + 10 - strlen(command) - 1);
        run_command(command);
    } else {
        error_box("Error", "grub.cfg not found at specified path.");
    }
}

void edit_syslinux_cfg() {
    const char *syslinux_cfg_path = "/opt/claudemods-iso-konsole-script/Supported-Distros/Arch/build-image-arch/isolinux/isolinux.cfg";
    if (access(syslinux_cfg_path, F_OK) == 0) {
        char command[PATH_MAX + 10];
        strncpy(command, "kate ", PATH_MAX + 10 - 1);
        strncat(command, syslinux_cfg_path, PATH_MAX + 10 - strlen(command) - 1);
        run_command(command);
    } else {
        error_box("Error", "syslinux.cfg not found at specified path.");
    }
}

void install_dependencies() {
    progress_dialog("Installing dependencies...");
    const char *packages[] = {
        "cryptsetup", "dmeventd", "isolinux", "libaio1", "libc-ares2",
        "libdevmapper-event1.02.1", "liblvm2cmd2.03", "live-boot",
        "live-boot-doc", "live-boot-initramfs-tools", "live-config-systemd",
        "live-tools", "lvm2", "pxelinux", "syslinux", "syslinux-common",
        "thin-provisioning-tools", "squashfs-tools", "xorriso", NULL
    };

    char package_list[PACKAGE_LIST_MAX] = "";
    for (int i = 0; packages[i] != NULL; i++) {
        if (strlen(package_list) + strlen(packages[i]) + 1 >= PACKAGE_LIST_MAX) {
            error_box("Error", "Package list too long.");
            return;
        }
        strncat(package_list, packages[i], PACKAGE_LIST_MAX - strlen(package_list) - 1);
        strncat(package_list, " ", PACKAGE_LIST_MAX - strlen(package_list) - 1);
    }

    char command[COMMAND_MAX];
    strncpy(command, "sudo apt-get install -y ", COMMAND_MAX - 1);
    strncat(command, package_list, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Dependencies installed successfully.");
        optional_dependencies_menu();
    }
}

void copy_vmlinuz_debian() {
    progress_dialog("Copying vmlinuz...");
    FILE *fp = popen("uname -r", "r");
    if (!fp) {
        error_box("Error", "Failed to get kernel version.");
        return;
    }
    char kernel_version[KERNEL_VERSION_MAX];
    if (!fgets(kernel_version, sizeof(kernel_version), fp)) {
        pclose(fp);
        error_box("Error", "Failed to read kernel version.");
        return;
    }
    pclose(fp);
    kernel_version[strcspn(kernel_version, "\n")] = 0;

    char vmlinuz_path[PATH_MAX];
    strncpy(vmlinuz_path, "/boot/vmlinuz-", PATH_MAX - 1);
    strncat(vmlinuz_path, kernel_version, PATH_MAX - strlen(vmlinuz_path) - 1);

    if (access(vmlinuz_path, F_OK) != 0) {
        error_box("Error", "Failed to find vmlinuz.");
        return;
    }

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        error_box("Error", "Failed to get current directory.");
        return;
    }

    char destination_dir[PATH_MAX];
    strncpy(destination_dir, cwd, PATH_MAX - 1);
    strncat(destination_dir, "/Debian/build-image-debian/live/", PATH_MAX - strlen(destination_dir) - 1);

    struct stat st = {0};
    if (stat(destination_dir, &st) == -1) {
        if (mkdir(destination_dir, 0755) != 0) {
            error_box("Error", "Failed to create destination directory.");
            return;
        }
    }

    char destination_path[PATH_MAX];
    strncpy(destination_path, destination_dir, PATH_MAX - 1);
    strncat(destination_path, "vmlinuz-", PATH_MAX - strlen(destination_path) - 1);
    strncat(destination_path, kernel_version, PATH_MAX - strlen(destination_path) - 1);

    char command[COMMAND_MAX];
    strncpy(command, "cp ", COMMAND_MAX - 1);
    strncat(command, vmlinuz_path, COMMAND_MAX - strlen(command) - 1);
    strncat(command, " ", COMMAND_MAX - strlen(command) - 1);
    strncat(command, destination_path, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Vmlinuz copied successfully.");
    }
}

void generate_initrd() {
    progress_dialog("Generating Initramfs...");
    char command[COMMAND_MAX];
    strncpy(command, "sudo mkinitramfs -o \"$(pwd)/Debian/build-image-debian/live/initrd.img-$(uname -r)\" \"$(uname -r)\"", COMMAND_MAX - 1);
    if (run_command(command) == 0) {
        message_box("Success", "Initramfs generated successfully.");
    }
}

void edit_grub_cfg_debian() {
    const char *grub_cfg_path = "/opt/claudemods-iso-konsole-script/Supported-Distros/Debian/build-image-debian/boot/grub/grub.cfg";
    if (access(grub_cfg_path, F_OK) == 0) {
        char command[PATH_MAX + 10];
        strncpy(command, "kate ", PATH_MAX + 10 - 1);
        strncat(command, grub_cfg_path, PATH_MAX + 10 - strlen(command) - 1);
        run_command(command);
    } else {
        error_box("Error", "grub.cfg not found at specified path.");
    }
}

void edit_isolinux_cfg_debian() {
    const char *isolinux_cfg_path = "/opt/claudemods-iso-konsole-script/Supported-Distros/Debian/build-image-debian/isolinux/isolinux.cfg";
    if (access(isolinux_cfg_path, F_OK) == 0) {
        char command[PATH_MAX + 10];
        strncpy(command, "kate ", PATH_MAX + 10 - 1);
        strncat(command, isolinux_cfg_path, PATH_MAX + 10 - strlen(command) - 1);
        run_command(command);
    } else {
        error_box("Error", "isolinux.cfg not found at specified path.");
    }
}

void noble_24_04() {
    print_dark_red("You chose Noble 24.04.");
    print_dark_red("This feature is being implemented soon.");
}

void oracular_24_10() {
    print_dark_red("You chose Oracular 24.10.");
}

void install_dependencies_oracular() {
    progress_dialog("Installing dependencies...");
    const char *packages[] = {
        "cryptsetup", "dmeventd", "isolinux", "libaio-dev", "libcares2",
        "libdevmapper-event1.02.1", "liblvm2cmd2.03", "live-boot",
        "live-boot-doc", "live-boot-initramfs-tools", "live-config-systemd",
        "live-tools", "lvm2", "pxelinux", "syslinux", "syslinux-common",
        "thin-provisioning-tools", "squashfs-tools", "xorriso", NULL
    };

    char package_list[PACKAGE_LIST_MAX] = "";
    for (int i = 0; packages[i] != NULL; i++) {
        if (strlen(package_list) + strlen(packages[i]) + 1 >= PACKAGE_LIST_MAX) {
            error_box("Error", "Package list too long.");
            return;
        }
        strncat(package_list, packages[i], PACKAGE_LIST_MAX - strlen(package_list) - 1);
        strncat(package_list, " ", PACKAGE_LIST_MAX - strlen(package_list) - 1);
    }

    char command[COMMAND_MAX];
    strncpy(command, "sudo apt-get install -y ", COMMAND_MAX - 1);
    strncat(command, package_list, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Dependencies installed successfully.");
        optional_dependencies_menu();
    }
}

void copy_vmlinuz_oracular() {
    progress_dialog("Copying vmlinuz...");
    FILE *fp = popen("uname -r", "r");
    if (!fp) {
        error_box("Error", "Failed to get kernel version.");
        return;
    }
    char kernel_version[KERNEL_VERSION_MAX];
    if (!fgets(kernel_version, sizeof(kernel_version), fp)) {
        pclose(fp);
        error_box("Error", "Failed to read kernel version.");
        return;
    }
    pclose(fp);
    kernel_version[strcspn(kernel_version, "\n")] = 0;

    const char *vmlinuz_path = "/boot/vmlinuz";
    if (access(vmlinuz_path, F_OK) != 0) {
        error_box("Error", "Failed to find vmlinuz.");
        return;
    }

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        error_box("Error", "Failed to get current directory.");
        return;
    }

    char destination_dir[PATH_MAX];
    strncpy(destination_dir, cwd, PATH_MAX - 1);
    strncat(destination_dir, "/Supported-Distros/Ubuntu/build-image-oracular/live/", PATH_MAX - strlen(destination_dir) - 1);

    struct stat st = {0};
    if (stat(destination_dir, &st) == -1) {
        mkdir(destination_dir, 0755);
    }

    char destination_path[PATH_MAX];
    strncpy(destination_path, destination_dir, PATH_MAX - 1);
    strncat(destination_path, "vmlinuz-", PATH_MAX - strlen(destination_path) - 1);
    strncat(destination_path, kernel_version, PATH_MAX - strlen(destination_path) - 1);

    char command[COMMAND_MAX];
    strncpy(command, "cp ", COMMAND_MAX - 1);
    strncat(command, vmlinuz_path, COMMAND_MAX - strlen(command) - 1);
    strncat(command, " ", COMMAND_MAX - strlen(command) - 1);
    strncat(command, destination_path, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Vmlinuz copied successfully.");
    }
}

void generate_initrd_oracular() {
    progress_dialog("Generating Initramfs...");
    char command[COMMAND_MAX];
    strncpy(command, "sudo mkinitramfs -o \"$(pwd)/Ubuntu/build-image-oracular/live/initrd.img-$(uname -r)\" \"$(uname -r)\"", COMMAND_MAX - 1);
    if (run_command(command) == 0) {
        message_box("Success", "Initramfs generated successfully.");
    }
}

void edit_grub_cfg_oracular() {
    progress_dialog("Opening grub.cfg for editing...");
    run_command("kate /opt/claudemods-iso-konsole-script/Supported-Distros/Ubuntu/build-image-oracular/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

void edit_isolinux_cfg_oracular() {
    progress_dialog("Opening isolinux.cfg for editing...");
    run_command("kate /opt/claudemods-iso-konsole-script/Supported-Distros/Ubuntu/build-image-oracular/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

void install_dependencies_noble() {
    progress_dialog("Installing dependencies...");
    const char *packages[] = {
        "cryptsetup", "dmeventd", "isolinux", "libaio-dev", "libcares2",
        "libdevmapper-event1.02.1", "liblvm2cmd2.03", "live-boot",
        "live-boot-doc", "live-boot-initramfs-tools", "live-config-systemd",
        "live-tools", "lvm2", "pxelinux", "syslinux", "syslinux-common",
        "thin-provisioning-tools", "squashfs-tools", "xorriso", NULL
    };

    char package_list[PACKAGE_LIST_MAX] = "";
    for (int i = 0; packages[i] != NULL; i++) {
        if (strlen(package_list) + strlen(packages[i]) + 1 >= PACKAGE_LIST_MAX) {
            error_box("Error", "Package list too long.");
            return;
        }
        strncat(package_list, packages[i], PACKAGE_LIST_MAX - strlen(package_list) - 1);
        strncat(package_list, " ", PACKAGE_LIST_MAX - strlen(package_list) - 1);
    }

    char command[COMMAND_MAX];
    strncpy(command, "sudo apt-get install -y ", COMMAND_MAX - 1);
    strncat(command, package_list, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Dependencies installed successfully.");
        optional_dependencies_menu();
    }
}

void copy_vmlinuz_noble() {
    progress_dialog("Copying vmlinuz...");
    FILE *fp = popen("uname -r", "r");
    if (!fp) {
        error_box("Error", "Failed to get kernel version.");
        return;
    }
    char kernel_version[KERNEL_VERSION_MAX];
    if (!fgets(kernel_version, sizeof(kernel_version), fp)) {
        pclose(fp);
        error_box("Error", "Failed to read kernel version.");
        return;
    }
    pclose(fp);
    kernel_version[strcspn(kernel_version, "\n")] = 0;

    const char *vmlinuz_path = "/boot/vmlinuz";
    if (access(vmlinuz_path, F_OK) != 0) {
        error_box("Error", "Failed to find vmlinuz.");
        return;
    }

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        error_box("Error", "Failed to get current directory.");
        return;
    }

    char destination_dir[PATH_MAX];
    strncpy(destination_dir, cwd, PATH_MAX - 1);
    strncat(destination_dir, "/Supported-Distros/Ubuntu/build-image-noble/live/", PATH_MAX - strlen(destination_dir) - 1);

    struct stat st = {0};
    if (stat(destination_dir, &st) == -1) {
        mkdir(destination_dir, 0755);
    }

    char destination_path[PATH_MAX];
    strncpy(destination_path, destination_dir, PATH_MAX - 1);
    strncat(destination_path, "vmlinuz-", PATH_MAX - strlen(destination_path) - 1);
    strncat(destination_path, kernel_version, PATH_MAX - strlen(destination_path) - 1);

    char command[COMMAND_MAX];
    strncpy(command, "cp ", COMMAND_MAX - 1);
    strncat(command, vmlinuz_path, COMMAND_MAX - strlen(command) - 1);
    strncat(command, " ", COMMAND_MAX - strlen(command) - 1);
    strncat(command, destination_path, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        message_box("Success", "Vmlinuz copied successfully.");
    }
}

void generate_initrd_noble() {
    progress_dialog("Generating Initramfs...");
    char command[COMMAND_MAX];
    strncpy(command, "sudo mkinitramfs -o \"$(pwd)/Ubuntu/build-image-noble/live/initrd.img-$(uname -r)\" \"$(uname -r)\"", COMMAND_MAX - 1);
    if (run_command(command) == 0) {
        message_box("Success", "Initramfs generated successfully.");
    }
}

void edit_grub_cfg_noble() {
    progress_dialog("Opening grub.cfg for editing...");
    run_command("kate /opt/claudemods-iso-konsole-script/Supported-Distros/Ubuntu/build-image-noble/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

void edit_isolinux_cfg_noble() {
    progress_dialog("Opening isolinux.cfg for editing...");
    run_command("kate /opt/claudemods-iso-konsole-script/Supported-Distros/Ubuntu/build-image-noble/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

void install_dependencies_arch() {
    progress_dialog("Installing dependencies...");
    run_command("kate /opt/claudemods-iso-konsole-script/Supported-Distros/information/arch-dependencies.txt");
    run_command("sudo pacman -U /opt/claudemods-iso-konsole-script/Supported-Distros/Arch/calamares-files/calamares-eggs-3.3.10-1-x86_64.pkg.tar.zst");
    const char *packages[] = {
        "arch-install-scripts", "bash-completion", "dosfstools", "erofs-utils",
        "findutils", "git", "grub", "jq", "libarchive", "libisoburn",
        "lsb-release", "lvm2", "mkinitcpio-archiso", "mkinitcpio-nfs-utils",
        "mtools", "nbd", "pacman-contrib", "parted", "procps-ng", "pv",
        "python", "rsync", "squashfs-tools", "sshfs", "syslinux", "xdg-utils",
        "bash-completion", "zsh-completions", "kernel-modules-hook",
        "virt-manager", NULL
    };

    char package_list[PACKAGE_LIST_MAX] = "";
    for (int i = 0; packages[i] != NULL; i++) {
        if (strlen(package_list) + strlen(packages[i]) + 1 >= PACKAGE_LIST_MAX) {
            error_box("Error", "Package list too long.");
            return;
        }
        strncat(package_list, packages[i], PACKAGE_LIST_MAX - strlen(package_list) - 1);
        strncat(package_list, " ", PACKAGE_LIST_MAX - strlen(package_list) - 1);
    }

    char command[COMMAND_MAX];
    strncpy(command, "sudo pacman -S --needed ", COMMAND_MAX - 1);
    strncat(command, package_list, COMMAND_MAX - strlen(command) - 1);

    if (run_command(command) == 0) {
        printf("All packages installed successfully!\n");
        optional_dependencies_menu();
    }
}

void optional_dependencies_menu() {
    while (1) {
        print_dark_red("Choose an optional dependency to install:");
        print_dark_red("1. virt-manager");
        print_dark_red("2. gnome-boxes");
        print_dark_red("3. Return to main menu");
        char choice_str[10];
        fgets(choice_str, sizeof(choice_str), stdin);
        choice_str[strcspn(choice_str, "\n")] = 0;
        int choice = atoi(choice_str);
        switch (choice) {
            case 1: install_virt_manager(); break;
            case 2: install_gnome_boxes(); break;
            case 3: return;
            default: print_dark_red("Invalid choice for optional dependency.");
        }
    }
}

void install_virt_manager() {
    progress_dialog("Installing virt-manager...");
    int distro = detect_distribution();
    if (distro == 1) { // Arch
        run_command("sudo pacman -S virt-manager");
    } else if (distro == 2 || distro == 3) { // Debian/Ubuntu
        run_command("sudo apt-get install -y virt-manager");
    } else {
        error_box("Error", "Unsupported distribution for virt-manager installation.");
        return;
    }
    message_box("Success", "virt-manager installed successfully.");
}

void install_gnome_boxes() {
    progress_dialog("Installing gnome-boxes...");
    int distro = detect_distribution();
    if (distro == 1) { // Arch
        run_command("sudo pacman -S gnome-boxes");
    } else if (distro == 2 || distro == 3) { // Debian/Ubuntu
        run_command("sudo apt-get install -y gnome-boxes");
    } else {
        error_box("Error", "Unsupported distribution for gnome-boxes installation.");
        return;
    }
    message_box("Success", "gnome-boxes installed successfully.");
}

int detect_distribution() {
    if (access("/etc/arch-release", F_OK) == 0) {
        return 1; // Arch
    } else if (access("/etc/debian_version", F_OK) == 0) {
        return 2; // Debian
    } else if (access("/etc/lsb-release", F_OK) == 0) {
        return 3; // Ubuntu
    }
    return 0; // Unknown
}

void clear_screen() {
    printf("\033[H\033[J");
}

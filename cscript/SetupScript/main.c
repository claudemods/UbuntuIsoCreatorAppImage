#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// Function to run a command and check exit status
void run_command(const char *command) {
    printf("\033[34mRunning command: %s\033[0m\n", command);
    int status = system(command);
    if (status != 0) {
        printf("\033[31mCommand failed with exit code: %d\033[0m\n", WEXITSTATUS(status));
    }
}

// Function to display a colored message
void print_blue(const char *text) {
    printf("\033[34m%s\033[0m\n", text);
}

// Function to display a success message
void message_box(const char *title, const char *message) {
    printf("\033[32m%s\033[0m\n", title);
    printf("\033[32m%s\033[0m\n", message);
}

// Function to display an error message
void error_box(const char *title, const char *message) {
    printf("\033[31m%s\033[0m\n", title);
    printf("\033[31m%s\033[0m\n", message);
}

// Function to simulate a progress dialog
void progress_dialog(const char *message) {
    printf("\033[33m%s\033[0m\n", message);
}

// Install dependencies for Noble/Oracular
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

// Copy vmlinuz (Noble)
void copy_vmlinuz_noble() {
    progress_dialog("Copying vmlinuz (Noble)...");
    run_command("sudo cp /boot/vmlinuz-6.11.0-26-generic /home/$USER/.config/cui/build-image-noble/live/");
    message_box("Success", "Vmlinuz copied successfully.");
}

// Copy vmlinuz (Oracular)
void copy_vmlinuz_oracular() {
    progress_dialog("Copying vmlinuz (Oracular)...");
    run_command("sudo cp /boot/vmlinuz-6.11.0-26-generic /home/$USER/.config/cui/build-image-oracular/live/");
    message_box("Success", "Vmlinuz copied successfully.");
}

// Generate initrd (Noble)
void generate_initrd_noble() {
    progress_dialog("Generating Initramfs (Noble)...");
    run_command("sudo mkinitramfs -o \"/home/$USER/.config/cui/build-image-noble/live/initrd.img-$(uname -r)\" \"$(uname -r)\"");
    message_box("Success", "Initramfs generated successfully.");
}

// Generate initrd (Oracular)
void generate_initrd_oracular() {
    progress_dialog("Generating Initramfs (Oracular)...");
    run_command("sudo mkinitramfs -o \"/home/$USER/.config/cui/build-image-oracular/live/initrd.img-$(uname -r)\" \"$(uname -r)\"");
    message_box("Success", "Initramfs generated successfully.");
}

// Edit grub.cfg (Noble)
void edit_grub_cfg_noble() {
    progress_dialog("Opening grub.cfg (Noble)...");
    run_command("nano /home/$USER/.config/cui/build-image-noble/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

// Edit grub.cfg (Oracular)
void edit_grub_cfg_oracular() {
    progress_dialog("Opening grub.cfg (Oracular)...");
    run_command("nano /home/$USER/.config/cui/build-image-oracular/boot/grub/grub.cfg");
    message_box("Success", "grub.cfg opened for editing.");
}

// Edit isolinux.cfg (Noble)
void edit_isolinux_cfg_noble() {
    progress_dialog("Opening isolinux.cfg (Noble)...");
    run_command("nano /home/$USER/.config/cui/build-image-noble/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

// Edit isolinux.cfg (Oracular)
void edit_isolinux_cfg_oracular() {
    progress_dialog("Opening isolinux.cfg (Oracular)...");
    run_command("nano /home/$USER/.config/cui/build-image-oracular/isolinux/isolinux.cfg");
    message_box("Success", "isolinux.cfg opened for editing.");
}

// Main menu for Noble
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

// Main menu for Oracular
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

// Main function
int main() {
    int choice;
    do {
        print_blue("\n=== claudemods ISO Configurator ===");
        print_blue("1. Noble 24.04");
        print_blue("2. Oracular 24.10");
        print_blue("3. Exit");
        printf("> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: noble_menu(); break;
            case 2: oracular_menu(); break;
            case 3: break;
            default: print_blue("Invalid choice."); break;
        }
    } while (choice != 3);

    return 0;
}

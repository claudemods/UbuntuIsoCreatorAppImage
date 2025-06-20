 #!/bin/bash

# Fancy Loading Bar Function (Cyan #24ffff)
function loading_bar() {
    local duration=${1:-3.7}  # Default: 3.7 seconds
    local width=50
    local progress=0
    local increment=$((100 * 10 / (duration * 10) / width))

    echo -ne "\e[38;2;36;255;255m"  # Set color to #24ffff
    while [ $progress -le 100 ]; do
        local filled=$((progress * width / 100))
        local empty=$((width - filled))
        printf "\r["
        printf "%${filled}s" | tr ' ' '█'
        printf "%${empty}s" | tr ' ' ' '
        printf "] %3d%%" $progress
        sleep 0.1
        ((progress += increment))
    done
    echo -e "\e[0m"  # Reset color
}

# Show loading bar at start
echo -e "\e[38;2;36;255;255mInitializing setup...\e[0m"
loading_bar 3.7

# Clone Repo
git clone https://github.com/claudemods/UbuntuIsoCreatorAppImage.git || {
    echo -e "\e[31mFailed to clone repo!\e[0m"
    exit 1
}

# Compile Components (YOUR EXACT COMMANDS)
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/SetupScript && make && cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/isocreator/noble/ && make && cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/SquashfsCreator/Noble+Oracular && make




# User input for build type
while true; do
    read -p "Choose build type (noble/oracular): " build_type
    case "$build_type" in
        noble|oracular)
            echo -e "\e[32mSelected: $build_type\e[0m"
            break
            ;;
        *)
            echo -e "\e[31mInvalid choice! Please enter 'noble' or 'oracular'.\e[0m"
            ;;
    esac
done

# Handle build image
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/Build-Images
if [ -f "build-image-$build_type.zip" ]; then
    echo -e "\e[38;2;36;255;255mExtracting build-image-$build_type.zip...\e[0m"
    mkdir -p /home/$USER/.config/cui
    unzip -q "build-image-$build_type.zip" -d /home/$USER/.config/cui/ || exit 1
else
    echo -e "\e[31mError: build-image-$build_type.zip not found!\e[0m"
    exit 1
fi

# Copy compiled files
# Copy each compiled executable individually
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/MainApp/
cp UbuntuIsoCreatorAppImage/cscript/MainApp/mainapp.bin /home/$USER/.config/cui/
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/SetupScript/
cp UbuntuIsoCreatorAppImage/cscript/SetupScript/ubuntusetupscript.bin /home/$USER/.config/cui/
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/isocreator/noble/
cp UbuntuIsoCreatorAppImage/cscript/isocreator/noble/ubuntuisocreator.bin /home/$USER/.config/cui/
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript/SquashfsCreator/Noble+Oracular
cp UbuntuIsoCreatorAppImage/cscript/SquashfsCreator/Noble+Oracular/ubuntusquashfscreator.bin /home/$USER/.config/cui/ || cp_failed=1

if [ "$cp_failed" = 1 ]; then
    echo -e "\e[31mFailed to copy one or more compiled files!\e[0m"
    exit 1
fi

echo -e "\e[1;32mInstallation completed successfully!\e[0m"

echo -e "\e[1;32mInstallation completed successfully!\e[0m"
cd /home/$USER/UbuntuIsoCreatorAppImage/cscript
sudo cp UbuntuIsoCreatorAppImage/cscript/cui.desktop /usr/share/applications
echo you can launch cui.desktop from applications menu

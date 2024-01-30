#!/bin/bash

# Get the absolute path to the current directory
# It is the directory from which you run this script
current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Path to the NetworkManager Dispatcher script
dispatcher_script_path="/etc/NetworkManager/dispatcher.d/faffetch_tracking_network_script.sh"

# Check if the script is running with sudo
if [[ -n "$SUDO_USER" ]]; then
    echo "Running as sudo by user: $SUDO_USER"
else
    echo "Not running as sudo"
    exit 1
fi

# Function to install the script
install_script() {
    # Compile the C++ code using make
    make clean &&
    make
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed. Please check the dependencies then try again."
        exit 1
    fi
    # Content of the NetworkManager Dispatcher script
    dispatcher_script_content="#!/bin/bash\n\n"
    dispatcher_script_content+="if [[ \$2 == \"connectivity-change\" ]];then\n"
    dispatcher_script_content+="    $current_dir/bin/get_net_info &> $current_dir/cache.csv \n"
    dispatcher_script_content+="fi"

    # Create the NetworkManager Dispatcher script
    echo -e "${dispatcher_script_content}" >"${dispatcher_script_path}"
    echo "NetworkManager Dispatcher script created at: ${dispatcher_script_path}"

    # Make the script executable
    chmod +x "${dispatcher_script_path}"

    echo "Testing dispatcher script"

    # Execute the dispatcher script with test arguments
    $dispatcher_script_path "x" "connectivity-change"

    echo "Modifying /home/$SUDO_USER/.bashrc"
    # Remove existing alias and lines containing 'faffetch' in .bashrc
    sed -i '/alias faffetch=.*/d' "/home/$SUDO_USER/.bashrc"
    sed -i '/faffetch/d' "/home/$SUDO_USER/.bashrc"

    # Add new alias for faffetch with cache directory parameter
    echo "alias faffetch=\"$current_dir/bin/faffetch $current_dir\"" >>"/home/$SUDO_USER/.bashrc"
    echo "faffetch" >>"/home/$SUDO_USER/.bashrc"

    # Source the updated .bashrc using sudo
    echo "Source /home/$SUDO_USER/.bashrc as user $SUDO_USER"
    sudo su $SUDO_USER
    source ".bashrc"

}

# Function to uninstall the script
uninstall_script() {
    # Remove the NetworkManager Dispatcher script
    rm -f "${dispatcher_script_path}"

    # Remove lines from .bashrc containing the faffetch alias
    sed -i '/alias faffetch=.*/d' "/home/$SUDO_USER/.bashrc"
    sed -i '/faffetch/d' "/home/$SUDO_USER/.bashrc"

    echo "Removed hook in /home/$SUDO_USER/.bashrc"
    echo "NetworkManager Dispatcher script removed from: ${dispatcher_script_path}"
    echo "Uninstaller script executed successfully!"
}

# Check if any arguments are provided, if not, default to install
if [[ $# -eq 0 ]]; then
    install_script
else
    # Check for install or uninstall option
    case "$1" in
    install)
        install_script
        ;;
    uninstall)
        uninstall_script
        ;;
    *)
        echo "Usage: $0 {install|uninstall}"
        exit 1
        ;;
    esac
fi

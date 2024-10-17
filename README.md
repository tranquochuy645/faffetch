# FAFFETCH
It's like neofetch, more useless, but fast

![Real usage](./imgs/result.png?raw=true "Real usage")


- Information is picked up directly from proc files and ipinfo.io API requests.

- The result of ipinfo.io API requests is wisely cached, so there is NO ANNOYING NETWORK DELAY every time you open up a new shell.

 Execution time:
 ![Performance](./imgs/time.png?raw=true "Performance")

## Prerequisites

- Linux OS only!! 
- Tested on Debian 12 (It's likely to work on every Linux distro that uses systemd)
- lcurl and a C++ compiler
  ```
   sudo apt install libcurl4-openssl-dev 
  ```
- NetworkManager service  (Run "nmcli --version" to check)
- sudo access on your machine
## Installation

The installer script requires sudo privileges to write a new file to your "/etc/NetworkManager/dispatcher.d" to set up the dispatcher that triggers refreshing your network information.

Basically, it runs the "get_net_info.cpp" main function every time there is a connectivity-change event.

For your convenience:

```
git clone https://github.com/tranquochuy645/faffetch
cd faffetch
sudo bash install.sh
```

It will now be available as a shell alias in your user's environment using:

```
faffetch
```

## Uninstall

```
sudo bash install.sh uninstall
```

## Configuration
Nope, you have to manually edit the source code.

It might be a pain to change anything except the ASCII logo since I didn't intend to write an easy-to-modify script in the first place. It was for my personal use and made to run as fast as possible.

Hint: You can begin by modifying the hard-coded logo in "src/logo.hpp"; then take a look at "src/main.cpp" to go further.

### Feel free to copy, edit, or use it as you like!

### Any contributions are welcomed!

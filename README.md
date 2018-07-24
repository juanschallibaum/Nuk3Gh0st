<img src="https://github.com/JuanSchallibaum/Nuk3Gh0st/blob/master/logo.jpg" width="300" height="300" />

# Nuk3 Gh0st

Universal LKM Linux kernel rootkit, designed to work in any kernel version and both architectures (x86 and x64).

## Functionality

The rootkit can do the following:

- Grant root privileges to a userland process
- Hide process by PID
- Unhide a previously hidden process by PID
- Hide files or directories by their name
- Unhide previously hidden files or directories
- Hide TCP ports (for IPv4 and IPv6)
- Unhide previously hidden TCP ports
- Hide UDP ports (for IPv4 and IPv6)
- Unhide previously hidden UDP ports
- Hide itself
- Unhide itself
- Protect against being unloaded by the user
- Disable the unload protection

## Tested On

|           Distro            |          Kernel         |    Arch    |
|-----------------------------|-------------------------|------------|
| Kali Linux (Rolling)        |  4.17.9                 |   x86_64   |
| Kali Linux (Rolling)        |  4.16.18                |   x86_64   |
| Kali Linux (Rolling)        |  4.14.0-kali3-amd64     |   x86_64   |
| Ubuntu 16.04.4 LTS          |  4.13.0-36-generic      |   x86_64   |
| Fedora 26                   |  4.11.8-300.fc26        |   x86_64   |
| Debian 9                    |  4.9.0-7-686            |   i686     |
| Kali Linux (Rolling)        |  4.4.142                |   x86_64   |
| Kali Linux (Rolling)        |  3.16.57                |   x86_64   |
| Wifislax                    |  3.12.36                |   i686     |
| Lihuen 5.10 (Debian based)  |  3.2.0-4-amd64          |   x86_64   |
| Ubuntu 10.04.4 LTS          |  2.6.32-38-generic-pae  |   i686     |

In Wifislax x86 with 3.12.36, doesn't work file hidding functionality, but pid hidding works well.

## Build

Install a compiler, Linux headers and all other things required for us to build the rootkit:

```sh
apt-get update
apt-get install build-essential
```

### Actual Building

```sh
make
```

## Use

Load rootkit:

```sh
insmod rootkit.ko
```

Use rootkit:

```sh
   $ ./client --help

   $$    $$            $$         $$$$$$        
   $$$   $$            $$     $$ $$    $$       
   $$$$  $$  $$    $$  $$    $$        $$       
   $$ $$ $$  $$    $$  $$   $$     $$$$$        
   $$  $$$$  $$    $$  $$$$$$         $$        
   $$   $$$  $$    $$  $$   $$   $$    $$       
   $$    $$   $$$$$$   $$    $$   $$$$$$        

    $$$$$$  $$        $$$$$$             $$     
   $$    $$ $$       $$$   $$            $$     
   $$       $$$$$$$  $$$$  $$  $$$$$$$ $$$$$$   
   $$  $$$$ $$    $$ $$ $$ $$ $$         $$     
   $$    $$ $$    $$ $$  $$$$  $$$$$$    $$     
   $$    $$ $$    $$ $$   $$$       $$   $$ $$  
    $$$$$$  $$    $$  $$$$$$  $$$$$$$    $$$$   

                   By Juan Schällibaum          

   Usage: ./client [OPTION]...

   Options:
     --root-shell                 Grants you root shell access.
     --hide-pid=PID               Hides the specified PID.
     --unhide-pid=PID             Unhides the specified PID.
     --hide-file=FILENAME         Hides the specified FILENAME globally.
                                  Must be a filename without any path.
     --unhide-file=FILENAME       Unhides the specified FILENAME.
     --hide-tcp-port=PORT         Hides the specified tcp PORT.
     --unhide-tcp-port=PORT       Unhides the specified tcp PORT.
     --hide-tcp6-port=PORT        Hides the specified tcp6 PORT.
     --unhide-tcp6-port=PORT      Unhides the specified tcp6 PORT.
     --hide-udp-port=PORT         Hides the specified udp PORT.
     --unhide-udp-port=PORT       Unhides the specified udp PORT.
     --hide-udp6-port=PORT        Hides the specified udp6 PORT.
     --unhide-udp6-port=PORT      Unhides the specified udp6 PORT.
     --hide                       Hides the rootkit LKM.
     --unhide                     Unhides the rootkit LKM.
     --help                       Print this help message.
     --protect                    Protects the rootkit from rmmod.
     --unprotect                  Disables the rmmod protection.
```

Unload rootkit:

```sh
./client --unhide
./client --unprotect
rmmod rootkit.ko
```

## License
This project is licensed under [GPLv2](LICENSE).

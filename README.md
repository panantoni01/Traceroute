# Traceroute
Simple traceroute program written in C, using IPv4 protocol and raw sockets. In this version ICMP $echo request$ packets with increasing ttl value are sent, similarly to Linux ```traceroute -I``` command.

## Usage
Using raw sockets requires *root* privilages, so in order to work this program must be run with appropiate capabilities:
```
> make
> sudo setcap cap_net_raw=eip traceroute
> ./traceroute
```
You can also run this the executable as *root* using ```sudo``` command however, this is less recommended.
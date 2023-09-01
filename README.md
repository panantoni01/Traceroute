# Traceroute
Simple traceroute program which displays possible route to a specified host, written in C. For now, two methods of tracing are implemented:
* `./traceroute -I` - send ICMP **echo request** packets with increasing TTL value.
* `./traceroute -U` - send UDP datagrams with increasing TTL value to destinations ports starting from 33434.

## Usage
First, build the executable:
```
make
```
The `traceroute` executable will be built in the root directory of the project.

### ICMP tracing
This tracing method uses raw sockets for sending ICMP **echo request** packets. Therefore, appropriate capabilities are needed:
```
sudo setcap cap_net_raw=eip traceroute
```
Once the capabilities are set, you can run ICMP traceroute to a specific host:
```
./traceroute -I example.org
```
Running `traceroute` with `sudo` instead of setting the capabilities is also possible.

This is the default tracing method which will be used if no explicit command line option is specified.

### UDP tracing
Sending UDP datagrams does not require any additional privileges from the user. In order to run UDP version of the program, simply run:
```
./traceroute -U example.org
```

### Installation
The executable can be installed to `/usr/local/bin` directory by executing:
```
make install
```

### Command line options

* `-I` or `-U` - choose ICMP or UDP tracing method (default is ICMP).
* `-n` - don't resolve IP addresses to hostnames.
* `-f first_ttl` - TTL value to start from (default is 1)
* `-m max_ttl` - the maximum number of hops (default is 30).
* `-q num_send` - specifies how many packets will be sent for each hop (default is 3).
* `-s dest_port` - (UDP only) port number to start sending packets to (default is 33434). This number will be incremented after each hop. 
* `-w wait_time` - number of seconds to wait for packets to arrive for each hop (default is 1).

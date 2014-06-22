program to test the servocontroller application running on the Raspberry Pi
===========================================================================

Each client needs its IP address. (The ServoController identifies the client by the IP Addres not the username)

On a Linux host, mutliple IP addresses can be assigned to one interface.

eg. If the host is connected to the RocketColibri Wifi of the ServoController the primare IP address is assigned over DHCP.

Check the IP with the command

# sudo ip addr show
2: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP qlen 1000
    link/ether c8:f7:33:da:68:2b brd ff:ff:ff:ff:ff:ff
    inet 192.168.200.109/24 brd 192.168.200.255 scope global wlan0

you can add more IP interfaces to wlan0 with the command:
#sudo ip addr add 192.168.200.240/24

# sudo ip addr show
2: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP qlen 1000
    link/ether c8:f7:33:da:68:2b brd ff:ff:ff:ff:ff:ff
    inet 192.168.200.109/24 brd 192.168.200.255 scope global wlan0
    inet 192.168.200.240/24 scope global secondary wlan0


Interactive Mode
---------------------------------------------------------------------------
A command promt is displayed to the user. New clients can be added and
removed with interactive commands from the command line interface:

> set-server-ip 192.168.200.1

> connect clientA 192.168.200.109

> disconnect clientA

> send-hello clientA

> send-cdc clientA


Changed active and passiv clients received by the telemetry command are displayed 
on the command line interface:

> clienA connected passiv

> clientA active

> clientA disconnected



Batch Mode
---------------------------------------------------------------------------
The application is started from the command line with the test-script as the command line argument

{
	"servocontroller" : "192.168.200.1"
	“rocketcolibri“ : [{ “name” : “clientA”, “ip”:”192.168.200.109”}, 
        		   { “name” : “clientB”, “ip”:”192.168.200.240”}]
	“testproc” : { {
	{“send-hello” : “clientA”},
	{“wait-passive” : [“clientA”] }
	{“sleep”: 5000 },
	{“send-hello” : “clientB”},
	{“wait-passive” : [“clientA”, “clientB”] }
	{“send-cdc”: “clientA” }
	{“wait-active” : “clientA” }
	{“wait-passive” : [“clientB”] }
	{“sleep”: 5000 },
	{“disconnect” : “clientA”}
}


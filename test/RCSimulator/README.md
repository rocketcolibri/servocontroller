program to test the servocontroller application running on the Raspberry Pi
===========================================================================

Interactive Mode
---------------------------------------------------------------------------
A command promt is displayed to the user. New clients can be added and
removed with interactive commands from the command line interface:


# connect clientA 192.168.1.1

# disconnect clientA

# send-hello clientA

# send-cdc clientA


Changed active and passiv clients received by the telemetry command are displayed 
on the command line interface:

# clienA connected passiv

# clientA active

# clientA disconnected



Batch Mode
---------------------------------------------------------------------------
The application is started from the command line with the test-script as the command line argument

{
	"servocontroller" : "192.168.1.200"
	“rocketcolibri“ : [{ “name” : “clientA”, “ip”:”192.168.1.1”}, 
        		   { “name” : “clientB”, “ip”:”192.168.1.2”}]
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


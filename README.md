# Zero-Configuration-Service

If you wish to use the same docker testing environment, run the RunDocker.bat in the terminal with docker desktop open (and properly set up). 
It will create the container, mount the directory, and install/update necessary packages in the container. It requires passing a string name for the new container as the sole argument.

 Note: A service is considered down if a heartbeat isn't detected after 5 seconds.

Active relay experiment:

LAN A bedroom
service: speaker-X
service2: zcsLight
app: listens to service4 ad
app2: listens to service2 ad

LAN B kitchen
service3: zcsDoorlock
service4: zcsToaster
app3: listens to service1 ad
app4: listens to service2 ad
_______________________
Open 9 docker terminals (ideally all from different containers).

In each of them, run one of each of the services/apps (and a relay) with the commands outlined below.

The apps listen to specific services, and will print logs when they get posts about the desired ads, which will demonstrate how
the relay allows communication between LANs.

RELAY 
./relay

LAN A services
./service 224.1.1.1
./service2 224.1.1.1

LAN B
./service3 224.1.2.1
./service4 224.1.2.1

LAN A apps
./app 224.1.1.1
./app2 224.1.1.1

LAN B apps
./app3 224.1.2.1
./app4 224.1.2.1

_______
Note: simpler experiment with 3 terminals
./relay
./app 224.1.1.1 
./service4 224.1.2.1
Expected behavior: app (LAN A) will log service4 (LAN B). Note that the log only gets printed when the relay is active.
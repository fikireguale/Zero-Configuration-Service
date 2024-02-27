# Zero-Configuration-Service

If you wish to use the same docker testing environment, run the RunDocker.bat in the terminal with docker desktop open (and properly set up). It will create the container, mount the directory, and install/update necessary packages in the container.

In assignment1 folder:
 1) make clean
 2) make
 3) Open 4 terminals and run one in each: ./service ./service2 ./app ./app2
 4) After 'Test C', log for app2 looks like this:
 Note: A service is considered down if a heartbeat isn't detected after 5 seconds.


-------- LOG START --------
Node name: zcsLight
Satus: UP
Last Heartbeat: 0.000000 seconds ago
Attributes:
        type: light
        location: bedroom
        make: ikea
Event Log:
Total events: 3
        Status: UP; at time: Sun Feb 11 22:28:20 2024

        Status: DOWN; at time: Sun Feb 11 22:28:52 2024

        Status: UP; at time: Sun Feb 11 22:28:56 2024

Node name: speaker-X
Satus: UP
Last Heartbeat: 1.000000 seconds ago
Attributes:
        type: speaker
        location: kitchen
        make: yamaha
Event Log:
Total events: 3
        Status: UP; at time: Sun Feb 11 22:28:20 2024

        Status: DOWN; at time: Sun Feb 11 22:28:39 2024

        Status: UP; at time: Sun Feb 11 22:28:41 2024


--------- LOG END ---------

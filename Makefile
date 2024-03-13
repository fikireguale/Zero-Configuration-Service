ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: service.c zcs.c app.c registry.c multicast.c relay.c
	gcc service.c zcs.c registry.c multicast.c -o service
	gcc service2.c zcs.c registry.c multicast.c -o service2
	gcc app.c zcs.c registry.c multicast.c -o app
	gcc app2.c zcs.c registry.c multicast.c -o app2
	gcc relay.c multicast.c -o relay

clean:
	$(RM) service service2 app app2 service.exe service2.exe app.exe app2.exe relay relay.exe
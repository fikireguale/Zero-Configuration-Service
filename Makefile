ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: zcs.c registry.c multicast.c relay.c
	gcc service.c zcs.c registry.c multicast.c -o service
	gcc service2.c zcs.c registry.c multicast.c -o service2
	gcc service3.c zcs.c registry.c multicast.c -o service3
	gcc service4.c zcs.c registry.c multicast.c -o service4
	gcc app.c zcs.c registry.c multicast.c -o app
	gcc app2.c zcs.c registry.c multicast.c -o app2
	gcc app3.c zcs.c registry.c multicast.c -o app3
	gcc app4.c zcs.c registry.c multicast.c -o app4
	gcc relay.c multicast.c -o relay

clean:
	$(RM) service service2 service3 service4 app app2 app3 app4 relay service.exe service2.exe service3.exe service4.exe app.exe app2.exe app3.exe app4.exe relay.exe
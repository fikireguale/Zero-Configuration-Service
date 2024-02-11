ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: service.c zcs.c app.c registry.c multicast.c
	gcc service.c zcs.c registry.c multicast.c -o service
	gcc service2.c zcs.c registry.c multicast.c -o service2
	gcc app.c zcs.c registry.c multicast.c -o app

clean:
	$(RM) service service2 app service.exe service2.exe app.exe
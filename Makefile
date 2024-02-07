ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: service.c zcs.c app.c registry.c multicast.c
	gcc service.c zcs.c registry.c multicast.c -o service
	gcc app.c zcs.c registry.c multicast.c -o app

clean:
	$(RM) service app service.exe app.exe
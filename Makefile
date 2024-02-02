ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: service.c zcs.c app.c registry.c
	gcc service.c zcs.c registry.c -o service
	gcc app.c zcs.c registry.c -o app

clean:
	$(RM) service app service.exe app.exe
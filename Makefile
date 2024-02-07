ifdef OS
	RM = del /Q
else
	RM = rm -rf
endif

make: service.c zcs.c app.c registry.c multicast.c receiver.c send_test.c
	gcc service.c zcs.c registry.c multicast.c -o service
	gcc app.c zcs.c registry.c multicast.c -o app
	gcc multicast.c receiver.c -o receiver_test
	gcc multicast.c send_test.c -o sender_test

clean:
	$(RM) service app service.exe app.exe sender_test receiver_test sender_test.exe receiver_test.exe
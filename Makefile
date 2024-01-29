make: service.c zcs.c app.c
	gcc service.c zcs.c -o service
	gcc app.c zcs.c -o app

clean:
	rm service; rm app;
int federer (int argc, char * argv []) {
    (void) argc;
    (void) argv; /* silence the warnings */

    logi ("Hi, I'm Roger.");
    socket_init ();
    SOCKET real = hot_server_socket ();

    for (;;) hot_serve (real);

    closesocket (real);
    logi ("Roger out.");
    return 0;
}


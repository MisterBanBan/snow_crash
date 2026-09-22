We can see that there is a blacksun.c and a blacksun executable in the /opt folder for this level.

We cannot execute it because we lack some permission. But by reading the source code we can determine that this is the code for some sort of server through a unix socket. A few commands are available including the command ADMIN that reads the flag and prints it.

By looking further we can see that a service names blacksun.service is launching the blacksun executable with the flag08 user.

We need to make a programm in order to communicate with the server and find a way to get the flag.

But the problem is that to get the flag we should need to be in ADMIN state using the AUTH command that's waiting for a authentification key hidden to us.

But there is a mistake in the server.

When we launch the admin command it checks our current status using the `CHECK_STATE` macro.
This macro is defined like this : 

```
#define CHECK_STATE(ctx, required)                                      \
    ( ((required) == STATE_ADMIN)                                       \
        ? ((ctx)->state >= STATE_AUTH)                                  \
        : ((ctx)->state == (required)) )
```

And there we can see the mistake. Usually it checks if the state is the one thats required. For exemple when trying to use the AUTH command it first checks that we are in the AUTH status aquired after sending the hello command with the right magic payload. But for som reason when the required state is ADMIN it checks if the current state is >= to the `STATE_AUTH` not strictly > or >= to `STATE_ADMIN`. Which means we can execute the ADMIN command in AUTH state that is simply aquired after the HELLO command before any auth.

So to aquire the flag we can simple make a program that :
- Connect to the right socket launched by the service with blacksun.
- properly builds the header to send the hello command with the right magic payload and checksum found in blacksun.c.
- Then send the admin command and retrieve the flag using the `CHECK_STATE` error.

The `blacksun_client.c` in ressources/ do exactly that. Simply copy it in /tmp, compile it and executes the output executable to get the flag.

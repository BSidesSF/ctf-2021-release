# Challenge

This is a client/server app that somewhat simulates the Starcraft login.
Basically, when you connect, the server sends a challenge which is a small amount
of machine code that will do a checksum on memory and send back the result.

If the user has changed memory, it'll fail! That includes software breakpoints!

There are several solutions:

* Dump memory and calculate the checksum on the dump (that's what my solution does)
* Build a proxy and "steal" the connection from the legit binary
* Add breakpoints at the send/recv calls and inject data after everything is done

There are lots of ways to solve it! Hopefully none are surprisingly easy. :)

# Build / deploy

To build the binaries, run ./build-binaries.sh.

Very importantly, the binary on the server and the binary the user has MUST be
100% identical. Any small change and it'll fail for sure.

The Dockerfile included should work great.

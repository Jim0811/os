savedcmd_/home/a1115528/os/simple.mod := printf '%s\n'   simple.o | awk '!x[$$0]++ { print("/home/a1115528/os/"$$0) }' > /home/a1115528/os/simple.mod

savedcmd_nic.mod := printf '%s\n'   nic.o | awk '!x[$$0]++ { print("./"$$0) }' > nic.mod

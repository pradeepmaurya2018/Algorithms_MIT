savedcmd_procviz.mod := printf '%s\n'   procviz.o | awk '!x[$$0]++ { print("./"$$0) }' > procviz.mod

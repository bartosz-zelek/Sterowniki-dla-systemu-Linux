savedcmd_/home/bartek/labdrivers/lab1/dev_module.mod := printf '%s\n'   dev_module.o | awk '!x[$$0]++ { print("/home/bartek/labdrivers/lab1/"$$0) }' > /home/bartek/labdrivers/lab1/dev_module.mod

import configparser

config = configparser.ConfigParser()
config.read("opal.ini")
print((config['OPAL']['OpalPassword']))
print((config['USER']['UserPassword']))

config['OPAL']['OpalPassword'] = "CONFIGURD"
config['Password']['UserPassword'] = "CONGIGURED"

with open('opal.ini', 'w') as conf:
    config.write(conf)
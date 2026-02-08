from xmlrpc.server import SimpleXMLRPCServer
def add(x, y):
    return x + y
server = SimpleXMLRPCServer(("localhost", 8004))
server.register_function(add, "add")
print("Server is running...")
server.serve_forever()



import xmlrpc.client
proxy = xmlrpc.client.ServerProxy("http://localhost:8004/")
print("3 + 5 =", proxy.add(3, 5))
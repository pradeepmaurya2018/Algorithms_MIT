class HttpRequest():
    def __init__(self, url, method, headers, param=None, body=None, timeout=None):
        self.url=url
        self.method=method
        self.header=headers
        # self.param=param
        # self.body=body
        # self.timeout=timeout
        print(f"{self.url=} {self.method=} {self.header=}")


httpRequest=HttpRequest("https://api.example.com/data", method="POST", headers="header")


class HttpRequestBuilder():

    class Builder():
        def __init__(self):
            self.url = None
            self.method = None
            self.header = None
        def addUrl(self, url):
            self.url=url
            return self
        def addMethod(self, method):
            self.method=method
            return self
        def addHeader(self, header):
            self.header=header
            return self
        def build(self):
            return HttpRequestBuilder(self)

    def __init__(self, builder:Builder):
        self.url=builder.url
        self.method=builder.method
        self.header=builder.header
        print(f"{self.url=} {self.method=} {self.header=}")

httpRequestBuilder=HttpRequestBuilder.Builder()\
                       .addUrl("https://api.example.com/data")\
                        .addMethod("POST")\
                        .addHeader("header").build()

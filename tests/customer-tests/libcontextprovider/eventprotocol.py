from twisted.internet.protocol import Protocol, Factory, ClientFactory

class EventProtocol(Protocol):
    def __init__(self, queue=None):
        self.queue = queue

    def dataReceived(self, data):
        if self.queue is not None:
            self.queue.handle_event(Event('socket-data', protocol=self,
                data=data))

    def sendData(self, data):
        self.transport.write(data)

class EventProtocolFactory(Factory):
    def __init__(self, queue):
        self.queue = queue

    def buildProtocol(self, addr):
        proto =  EventProtocol(self.queue)
        self.queue.handle_event(Event('socket-connected', protocol=proto))
        return proto

class EventProtocolClientFactory(EventProtocolFactory, ClientFactory):
    pass

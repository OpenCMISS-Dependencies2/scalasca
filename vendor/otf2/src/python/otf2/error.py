class Error(Exception):
    """ Base class for OTF2 high level exceptions. """
    def __str__(self):
        return self.msg


class TraceReaderError(Error):
    def __init__(self, msg):
        self.msg = "Semantic error in the input trace: " + msg


class TraceWriterError(Error):
    def __init__(self, msg):
        self.msg = "Semantic error trying to write the trace: " + msg

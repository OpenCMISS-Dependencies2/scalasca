from contextlib import closing

import _otf2

from .event_reader import BufferedEventReader
from .error import TraceReaderError
from .registry import DefinitionRegistry


# The contextlib.closing documentation actually suggests to name this thing open
# so we do this despite the shadowing
def open(*args, **kwargs):
    """
        Returns a :py:class:`otf2.reader.Reader` object usable within a :py:obj:`with` statement.

        :param str anchor_file_path: the path to the anchor file (mostly 'some/path/trace.otf2')
        :param int batch_events: the number of events, which are batched while reading the events
                from the trace (default: 100)
    """
    return closing(Reader(*args, **kwargs))


class Reader(object):
    """
        This class is used to read existing traces

        :param str anchor_file_path: the path to the anchor file (mostly 'some/path/trace.otf2')
        :param int batch_events: the number of events, which are batched while reading the events
                from the trace (default: 100)
    """
    def __init__(self, anchor_file_path, batch_events=100, collective_callbacks=None,
                 collective_data=None, global_comm_context=None, local_comm_context=None):
        self._handle = _otf2.Reader_Open(anchor_file_path)

        if collective_callbacks is None:
            _otf2.Reader_SetSerialCollectiveCallbacks(self._handle)
        else:
            _otf2.Reader_SetCollectiveCallbacks(self._handle, collective_callbacks,
                                                collective_data, global_comm_context,
                                                local_comm_context)

        self._definitions = DefinitionRegistry()
        self.batch_events = batch_events

        global_def_reader = _otf2.Reader_GetGlobalDefReader(self._handle)

        def_reader_callbacks = _otf2.GlobalDefReaderCallbacks_New()
        self.definitions._set_global_def_reader_callbacks(def_reader_callbacks)
        _otf2.Reader_RegisterGlobalDefCallbacks(self._handle, global_def_reader,
                                                def_reader_callbacks, self.definitions)
        _otf2.GlobalDefReaderCallbacks_Delete(def_reader_callbacks)
        _otf2.Reader_ReadAllGlobalDefinitions(self._handle, global_def_reader)
        _otf2.Reader_CloseGlobalDefReader(self._handle, global_def_reader)
        # <---  self.definitions contains all definitions from trace  --->

        # Copy locations in case someone modifies the definitions before we need them
        # In this variable we keep all locations that were part of the initially read definitions
        self._locations = [location for location in self.definitions.locations]

        self._event_reader = BufferedEventReader(self, self.batch_events)
        self._global_evt_reader_handle = None

        if self.definitions.clock_properties is None:
            raise TraceReaderError("Trace has no ClockProperties definition.")

    def _get_global_evt_reader_handle(self, locations):
        if locations is None:
            locations = self._locations
        for location in locations:
            if location not in self._locations:
                raise ValueError("Trying to select invalid location while reading trace.")
            _otf2.Reader_SelectLocation(self._handle, location._ref)

        # The error handling follows the official example of the otf2_mpi_reader_example
        def_files_open = False
        try:
            _otf2.Reader_OpenDefFiles(self._handle)
            def_files_open = True
        except _otf2.Error:
            pass

        try:
            _otf2.Reader_OpenEvtFiles(self._handle)
        except _otf2.Error:
            pass

        for location in locations:
            if def_files_open:
                def_reader = _otf2.Reader_GetDefReader(self._handle, location._ref)
                if def_reader:
                    _otf2.Reader_ReadAllLocalDefinitions(self._handle, def_reader)
                    _otf2.Reader_CloseDefReader(self._handle, def_reader)

            _otf2.Reader_GetEvtReader(self._handle, location._ref)

        if def_files_open:
            _otf2.Reader_CloseDefFiles(self._handle)

        assert self._global_evt_reader_handle is None
        self._global_evt_reader_handle = _otf2.Reader_GetGlobalEvtReader(self._handle)
        return self._global_evt_reader_handle

    def close(self):
        if self._handle is None:
            return

        if self._global_evt_reader_handle is not None:
            _otf2.Reader_CloseGlobalEvtReader(self._handle, self._global_evt_reader_handle)
            _otf2.Reader_CloseEvtFiles(self._handle)
            self._global_evt_reader_handle = None

        _otf2.Reader_Close(self._handle)
        self._handle = None

    @property
    def timer_resolution(self):
        """
            Returns the resolution of the timer of the read trace
        """
        return self.definitions.clock_properties.timer_resolution

    @property
    def events(self):
        """
            Returns an iterable object representing all events in the opened trace.

            Example:

            .. code-block:: python

                for location, event in trace.events(location_foo):
                    print("Encountered event at {}".format(event.time))

            :param Iterable locations: all locations, for which events will be read, (None means for every location)

        """
        return self._event_reader

    @property
    def definitions(self):
        """
            Returns a :py:class:`otf2.registry.DefinitionRegistry` object representing all
            definitions in the trace.
        """
        return self._definitions

    @property
    def handle(self):
        """
            Returns the handle to the underlaying OTF2 Reader.
        """
        if self._handle is None:
            raise TraceReaderError("Trying to access handle, but reader is already closed.")
        return self._handle

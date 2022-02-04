from contextlib import closing

import _otf2

from .definitions import Location, ClockProperties
from .definition_writer import DefinitionWriter
from .enums import *
from .error import TraceWriterError
from .event_writer import EventWriter
from .registry import DefinitionRegistry


# The contextlib.closing documentation actually suggests to name this thing open
# so we do this despite the shadowing
def open(*args, **kwargs):
    """
        Returns a :py:class:`otf2.reader.Reader` object usable within a :py:obj:`with`
        statement.

        :param str anchor_path: the path to the trace root directory
        :param str archive_name: the name of the trace archive (default: "traces")
    """

    return closing(Writer(*args, **kwargs))


def _default_pre_flush(user_data, file_type, flush_location, caller_data, final):
    return FlushType.FLUSH


def _use_param_doc_from(doc_func):
    def wrapper(func):
        doc = func.__doc__.format(params=doc_func.__doc__)
        func.__doc__ = doc
        return func
    return wrapper


class Writer(object):
    """
        This class is used to write a new trace file.

        .. Warning:: Parallel trace writing using collective callbacks is currently not supported.

        :param str anchor_path: the path to the trace root directory
        :param str archive_name: the name of the trace archive (default: 'traces')
        :param int timer_resolution: the resolution of the used timer (default: 1)
        :param otf2.registry.DefinitionRegistry definitions: used to share definitions with
            other :py:class:`otf2.writer.Writer` and :py:class:`otf2.reader.Reader` (default: None)
    """
    def __init__(self, archive_path,
                 archive_name="traces",
                 file_mode=FileMode.WRITE,
                 chunk_size_events=1024 * 1024, chunk_size_definitions=4 * 1024 * 1024,
                 file_substrate=FileSubstrate.POSIX, compression=Compression.NONE,
                 pre_flush=_default_pre_flush, post_flush=None, flush_data=None,
                 collective_callbacks=None, collective_data=None, global_comm_context=None,
                 local_comm_context=None, memory_callbacks=None, memory_data=None,
                 timer_resolution=None, definitions=None):

        self._file_mode = file_mode

        self._first_timestamp = None
        self._last_timestamp = None

        self._event_writers = dict()

        self._handle = _otf2.Archive_Open(archive_path, archive_name, file_mode,
                                          chunk_size_events, chunk_size_definitions,
                                          file_substrate, compression)

        # We MUST keep those flush callbacks, they cannot be GC'ed...
        self._flush_callbacks = _otf2.FlushCallbacks(pre_flush=pre_flush, post_flush=post_flush)
        _otf2.Archive_SetFlushCallbacks(self._handle, self._flush_callbacks, flush_data)

        if collective_callbacks is None:
            _otf2.Archive_SetSerialCollectiveCallbacks(self._handle)
        else:
            _otf2.Archive_SetCollectiveCallbacks(self._handle, collective_callbacks,
                                                 collective_data, global_comm_context,
                                                 local_comm_context)

        if memory_callbacks is not None:
            _otf2.Archive_SetMemoryCallbacks(self._handle, memory_callbacks, memory_data)

        _otf2.Archive_OpenDefFiles(self._handle)
        _otf2.Archive_OpenEvtFiles(self._handle)

        if definitions is None:
            definitions = DefinitionRegistry()

            # Reserve id 0 for empty string
            definitions.strings.create("")

        if timer_resolution is None:
            if definitions.clock_properties is None:
                definitions.clock_properties = ClockProperties(None, timer_resolution=1)
        else:
            if definitions.clock_properties is None:
                definitions.clock_properties = ClockProperties(None,
                                                               timer_resolution=timer_resolution)
            else:
                definitions.clock_properties.timer_resolution = timer_resolution

        self._global_definition_registry = definitions

    def _write_global_definitions(self):
        if not self.is_master:
            return
        with DefinitionWriter(self) as global_def_writer:
            if self._first_timestamp is not None:
                duration = self._last_timestamp - self._first_timestamp
                global_offset = self._first_timestamp
            else:
                # No timestamps
                duration = 0
                global_offset = 0
            global_def_writer.write_clock_properties(self.timer_resolution,
                                                     global_offset, duration)
            self.definitions.write(global_def_writer)

    def close(self):
        if self._handle is None:
            return
        self._close_event_writers()
        _otf2.Archive_CloseEvtFiles(self._handle)
        _otf2.Archive_CloseDefFiles(self._handle)

        self._write_global_definitions()

        _otf2.Archive_Close(self._handle)
        self._handle = None

    # This is rather dangerous... Not good to have exceptions here
    def __del__(self):
        self.close()

    def _update_timestamps(self, timestamp):
        if self._first_timestamp is None:
            self._first_timestamp = timestamp
            self._last_timestamp = timestamp
        else:
            self._first_timestamp = min(self._first_timestamp, timestamp)
            self._last_timestamp = max(self._last_timestamp, timestamp)

    def event_writer_from_location(self, location):
        """
            Returns an :py:class:`otf2.event_writer.EventWriter` for the given
            :py:class:`otf2.definitions.Location`.

            :param otf2.definitions.Location location: the location definition
            :return: :py:class:`otf2.event_writer.EventWriter`
        """
        if not isinstance(location, Location):
            raise TypeError("Wrong type for first argument. Expected a Location. Got {}."
                            .format(type(location)))
        if location._ref not in self._event_writers:
            self._event_writers[location._ref] = EventWriter(self, location)
        return self._event_writers[location._ref]

    @_use_param_doc_from(Location)
    def event_writer(self, *args, **kwargs):
        """
            Returns an :py:class:`otf2.event_writer.EventWriter` for the
            :py:class:`otf2.definitions.Location` equivalent to the given arguments.

{params}
:return: :py:class:`otf2.event_writer.EventWriter`
        """
        # I know, the last bit in the doc comment is pretty ugly, but it is necessary to remove the
        # indentation, otherwise Sphinx freaks out :(
        if len(args) > 0 and isinstance(args[0], Location):
            raise TypeError("Wrong type for first argument.")
        location = self.definitions.location(*args, **kwargs)
        return self.event_writer_from_location(location)

    @property
    def definitions(self):
        """
            Provides access to the global trace definitions.
            In the parallel case, all calls to the definitions must be collective.

            :return: :py:class:`otf2.registry.DefinitionRegistry`
        """
        return self._global_definition_registry

    @property
    def timer_resolution(self):
        """
            Returns the resolution of the used timer
        """
        return self.definitions.clock_properties.timer_resolution

    @property
    def handle(self):
        """
            Returns the handle to the underlaying OTF2 Archive.
        """
        if self._handle is None:
            raise TraceWriterError("Trying to access handle, but writer is already closed.")
        return self._handle

    def _close_event_writers(self):
        for writer in self._event_writers.values():
            writer.close()

    # setter/getter
    @property
    def file_mode(self):
        return self._file_mode

    @file_mode.setter
    def file_mode(self, new_file_mode):
        _otf2.Archive_SwitchFileMode(self._handle, new_file_mode)
        self._file_mode = new_file_mode

    @property
    def machine_name(self):
        return _otf2.Archive_GetMachineName(self._handle)

    @machine_name.setter
    def machine_name(self, machine_name):
        _otf2.Archive_SetMachineName(self._handle, machine_name)

    @property
    def description(self):
        return _otf2.Archive_GetDescription(self._handle)

    @description.setter
    def description(self, description):
        _otf2.Archive_SetDescription(self._handle, description)

    @property
    def creator(self):
        return _otf2.Archive_GetCreator(self._handle)

    @creator.setter
    def creator(self, creator):
        _otf2.Archive_SetCreator(self._handle, creator)

    # Read only
    @property
    def is_master(self):
        return _otf2.Archive_IsMaster(self._handle)

    @property
    def version(self):
        return _otf2.Archive_GetVersion(self._handle)

    @property
    def chunk_size(self):
        return _otf2.Archive_GetChunkSize(self._handle)

    @property
    def file_substrate(self):
        return _otf2.Archive_GetFileSubstrate(self._handle)

    @property
    def compression(self):
        return _otf2.Archive_GetCompression(self._handle)

    @property
    def trace_id(self):
        return _otf2.Archive_GetTraceId(self._handle)

# Generated by OTF2 Template Engine

import _otf2.GlobalDefWriter
from .definitions import *


class DefinitionWriter(object):
    """
        This class wraps the OTF2_GlobalDefWriter.

        Used internally only.
    """
    def __init__(self, archive):
        assert archive
        self._handle = _otf2.Archive_GetGlobalDefWriter(archive._handle)
        self._archive = archive

    def __enter__(self):
        assert self._handle
        return self

    def __exit__(self, *args):
        self.close()

    def __del__(self):
        """Destructor.  Calls close()."""
        # The try/except block is in case this is called at program
        # exit time, when it's possible that globals have already been
        # deleted, and then the close() call might fail.  Since
        # there's nothing we can do about such failures and they annoy
        # the end users, we suppress the traceback.
        try:
            self.close()
        except:
            pass

    def close(self):
        if self._handle:
            try:
                _otf2.Archive_CloseGlobalDefWriter(self._archive._handle, self._handle)
            finally:
                self._archive = None
                self._handle = None

    @property
    def handle(self):
        """
            Gives access to the underlaying OTF2 global definition writer
        """
        if self._handle is None:
            raise TraceWriterError(
                "Trying to access handle, but GlobalDefWriter is already closed.")
        return self._handle

    @property
    def number_of_definitions(self):
        return _otf2.GlobalDefWriter.GetNumberOfDefinitions(self._handle)

    @property
    def number_of_locations(self):
        return _otf2.GlobalDefWriter.GetNumberOfLocations(self._handle)

    def write(self, definition, *args):
        {
            Paradigm: self.write_paradigm,
            ParadigmProperty: self.write_paradigm_property,
            IoParadigm: self.write_io_paradigm,
            String: self.write_string,
            Attribute: self.write_attribute,
            SystemTreeNode: self.write_system_tree_node,
            LocationGroup: self.write_location_group,
            Location: self.write_location,
            Region: self.write_region,
            Callsite: self.write_callsite,
            Callpath: self.write_callpath,
            Group: self.write_group,
            MetricMember: self.write_metric_member,
            MetricClass: self.write_metric_class,
            MetricInstance: self.write_metric_instance,
            Comm: self.write_comm,
            Parameter: self.write_parameter,
            RmaWin: self.write_rma_win,
            MetricClassRecorder: self.write_metric_class_recorder,
            SystemTreeNodeProperty: self.write_system_tree_node_property,
            SystemTreeNodeDomain: self.write_system_tree_node_domain,
            LocationGroupProperty: self.write_location_group_property,
            LocationProperty: self.write_location_property,
            CartDimension: self.write_cart_dimension,
            CartTopology: self.write_cart_topology,
            CartCoordinate: self.write_cart_coordinate,
            SourceCodeLocation: self.write_source_code_location,
            CallingContext: self.write_calling_context,
            CallingContextProperty: self.write_calling_context_property,
            InterruptGenerator: self.write_interrupt_generator,
            IoFileProperty: self.write_io_file_property,
            IoRegularFile: self.write_io_regular_file,
            IoDirectory: self.write_io_directory,
            IoHandle: self.write_io_handle,
            IoPreCreatedHandleState: self.write_io_pre_created_handle_state,
            CallpathParameter: self.write_callpath_parameter,
        }[type(definition)](*args)

    def write_clock_properties(this, timerResolution, globalOffset, traceLength):
        _otf2.GlobalDefWriter_WriteClockProperties(this._handle, timerResolution, globalOffset, traceLength)

    def write_paradigm(this, paradigm, name, paradigmClass):
        _otf2.GlobalDefWriter_WriteParadigm(this._handle, paradigm, name, paradigmClass)

    def write_paradigm_property(this, paradigm, property, type, value):
        _otf2.GlobalDefWriter_WriteParadigmProperty(this._handle, paradigm, property, type, value)

    def write_io_paradigm(this, self, identification, name, ioParadigmClass, ioParadigmFlags, properties, types, values):
        _otf2.GlobalDefWriter_WriteIoParadigm(this._handle, self, identification, name,
                                              ioParadigmClass, ioParadigmFlags, properties, types, values)

    def write_string(this, self, string):
        _otf2.GlobalDefWriter_WriteString(this._handle, self, string)

    def write_attribute(this, self, name, description, type):
        _otf2.GlobalDefWriter_WriteAttribute(this._handle, self, name, description, type)

    def write_system_tree_node(this, self, name, className, parent):
        _otf2.GlobalDefWriter_WriteSystemTreeNode(this._handle, self, name, className, parent)

    def write_location_group(this, self, name, locationGroupType, systemTreeParent):
        _otf2.GlobalDefWriter_WriteLocationGroup(this._handle, self, name, locationGroupType, systemTreeParent)

    def write_location(this, self, name, locationType, numberOfEvents, locationGroup):
        _otf2.GlobalDefWriter_WriteLocation(this._handle, self, name, locationType, numberOfEvents, locationGroup)

    def write_region(this, self, name, canonicalName, description, regionRole, paradigm, regionFlags, sourceFile, beginLineNumber, endLineNumber):
        _otf2.GlobalDefWriter_WriteRegion(this._handle, self, name, canonicalName, description, regionRole, paradigm, regionFlags, sourceFile, beginLineNumber, endLineNumber)

    def write_callsite(this, self, sourceFile, lineNumber, enteredRegion, leftRegion):
        _otf2.GlobalDefWriter_WriteCallsite(this._handle, self, sourceFile, lineNumber, enteredRegion, leftRegion)

    def write_callpath(this, self, parent, region):
        _otf2.GlobalDefWriter_WriteCallpath(this._handle, self, parent, region)

    def write_group(this, self, name, groupType, paradigm, groupFlags, members):
        _otf2.GlobalDefWriter_WriteGroup(this._handle, self, name, groupType, paradigm, groupFlags, members)

    def write_metric_member(this, self, name, description, metricType, metricMode, valueType, base, exponent, unit):
        _otf2.GlobalDefWriter_WriteMetricMember(this._handle, self, name, description, metricType, metricMode, valueType, base, exponent, unit)

    def write_metric_class(this, self, metricMembers, metricOccurrence, recorderKind):
        _otf2.GlobalDefWriter_WriteMetricClass(this._handle, self, metricMembers, metricOccurrence, recorderKind)

    def write_metric_instance(this, self, metricClass, recorder, metricScope, scope):
        _otf2.GlobalDefWriter_WriteMetricInstance(this._handle, self, metricClass, recorder, metricScope, scope)

    def write_comm(this, self, name, group, parent):
        _otf2.GlobalDefWriter_WriteComm(this._handle, self, name, group, parent)

    def write_parameter(this, self, name, parameterType):
        _otf2.GlobalDefWriter_WriteParameter(this._handle, self, name, parameterType)

    def write_rma_win(this, self, name, comm):
        _otf2.GlobalDefWriter_WriteRmaWin(this._handle, self, name, comm)

    def write_metric_class_recorder(this, metric, recorder):
        _otf2.GlobalDefWriter_WriteMetricClassRecorder(this._handle, metric, recorder)

    def write_system_tree_node_property(this, systemTreeNode, name, type, value):
        _otf2.GlobalDefWriter_WriteSystemTreeNodeProperty(this._handle, systemTreeNode, name, type, value)

    def write_system_tree_node_domain(this, systemTreeNode, systemTreeDomain):
        _otf2.GlobalDefWriter_WriteSystemTreeNodeDomain(this._handle, systemTreeNode, systemTreeDomain)

    def write_location_group_property(this, locationGroup, name, type, value):
        _otf2.GlobalDefWriter_WriteLocationGroupProperty(this._handle, locationGroup, name, type, value)

    def write_location_property(this, location, name, type, value):
        _otf2.GlobalDefWriter_WriteLocationProperty(this._handle, location, name, type, value)

    def write_cart_dimension(this, self, name, size, cartPeriodicity):
        _otf2.GlobalDefWriter_WriteCartDimension(this._handle, self, name, size, cartPeriodicity)

    def write_cart_topology(this, self, name, communicator, cartDimensions):
        _otf2.GlobalDefWriter_WriteCartTopology(this._handle, self, name, communicator, cartDimensions)

    def write_cart_coordinate(this, cartTopology, rank, coordinates):
        _otf2.GlobalDefWriter_WriteCartCoordinate(this._handle, cartTopology, rank, coordinates)

    def write_source_code_location(this, self, file, lineNumber):
        _otf2.GlobalDefWriter_WriteSourceCodeLocation(this._handle, self, file, lineNumber)

    def write_calling_context(this, self, region, sourceCodeLocation, parent):
        _otf2.GlobalDefWriter_WriteCallingContext(this._handle, self, region, sourceCodeLocation, parent)

    def write_calling_context_property(this, callingContext, name, type, value):
        _otf2.GlobalDefWriter_WriteCallingContextProperty(this._handle, callingContext, name, type, value)

    def write_interrupt_generator(this, self, name, interruptGeneratorMode, base, exponent, period):
        _otf2.GlobalDefWriter_WriteInterruptGenerator(this._handle, self, name, interruptGeneratorMode, base, exponent, period)

    def write_io_file_property(this, ioFile, name, type, value):
        _otf2.GlobalDefWriter_WriteIoFileProperty(this._handle, ioFile, name, type, value)

    def write_io_regular_file(this, self, name, scope):
        _otf2.GlobalDefWriter_WriteIoRegularFile(this._handle, self, name, scope)

    def write_io_directory(this, self, name, scope):
        _otf2.GlobalDefWriter_WriteIoDirectory(this._handle, self, name, scope)

    def write_io_handle(this, self, name, file, ioParadigm, ioHandleFlags, comm, parent):
        _otf2.GlobalDefWriter_WriteIoHandle(this._handle, self, name, file, ioParadigm, ioHandleFlags, comm, parent)

    def write_io_pre_created_handle_state(this, ioHandle, mode, statusFlags):
        _otf2.GlobalDefWriter_WriteIoPreCreatedHandleState(this._handle, ioHandle, mode, statusFlags)

    def write_callpath_parameter(this, callpath, parameter, type, value):
        _otf2.GlobalDefWriter_WriteCallpathParameter(this._handle, callpath, parameter, type, value)
import re
from itertools import chain
from collections import namedtuple
from six import with_metaclass, integer_types, string_types
from numbers import Number
from builtins import int

import _otf2

from .attribute_value import AttributeValue
from .enums import RegionRole, Paradigm, RegionFlag, LocationType, LocationGroupType, MetricType, \
    MetricMode, MetricScope, MetricOccurrence, RecorderKind, GroupType, GroupFlag, Type, Base, \
    Undefined, ParameterType, SystemTreeDomain, CartPeriodicity, InterruptGeneratorMode, \
    ParadigmClass, IoParadigmClass, IoParadigmFlag, IoParadigmProperty, \
    IoHandleFlag, IoAccessMode, IoStatusFlag
from . import enums  # Needed for namespace issue with Paradigm and ParadigmProperty
from .error import TraceReaderError, TraceWriterError


class DefinitionField(namedtuple("DefinitionField", ['name', 'type', 'private_name', 'default'])):
    def check_value(self, value):
        if value is None:
            return None

        expected_type = self.type

        # Special case: Strings are supposed to be created as native strings
        # instead of String definitions
        if expected_type is String:
            expected_type = string_types

        if isinstance(value, expected_type):
            return value

        if expected_type is tuple:
            return tuple(value)

        raise TypeError("Wrong type for {}, expected {}, actual {}"
                        .format(self.name, expected_type, type(value)))


def _generate_key(value):
    if isinstance(value, tuple):
        return tuple(_generate_key(e) for e in value)
    if isinstance(value, _RefDefinition):
        return type(value), value._ref
    return value


no_default = object()
self_type = object()


def _to_snake(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def _pluralize(name):
    if name[-1] == 's' and not name[-2] == 's':
        return name
    if name[-2:] == 'ss':
        return name + 'es'
    if name[-1] == 'y':
        return name[:-1] + 'ies'
    else:
        return name + 's'

def _make_property(field):
    """
    Create properties to access the private name
    """
    def getter(self):
        return getattr(self, field.private_name)

    def setter(self, value):
        self._set_field(field, value)

    return property(getter, setter)


definitions = []


def _generate_doc(name, field_descriptions):
    doc = ""
    for field in field_descriptions:
        if isinstance(field.type, tuple):
            field_type = field.type[0]
        else:
            field_type = field.type

        type_name = field_type.__name__
        if type_name == "String":
            module = ""
            type_name = "str"
        elif field_type.__module__ == "__builtin__":
            module = ""
        elif field_type.__module__ == "_otf2.GeneralDefinitions":
            module = "otf2."
        elif field_type.__module__ == "_otf2.Definitions":
            module = "otf2."
        elif type_name == "AttributeValue":
            module = ""
            type_name = "value_type"
        else:
            module = "{}.".format(field_type.__module__)

        doc += ":param {module}{type_name} {field.name}: ".format(field=field, module=module,
                                                                  type_name=type_name)
        if field_type == Type and field.default is None:
            doc += "The type is inferred from the current value and should " \
                   "not be provided by the user."
        elif field.default is no_default:
            doc += "required argument"
        else:
            doc += "default value {!r}".format(field.default)
        doc += "\n"

    return doc


class _DefinitionMeta(type):
    def __new__(mcls, name, bases, namespace):
        fields = namespace.get('_fields', ())
        namespace['__slots__'] = \
            tuple("_" + field[0] for field in fields) \
            + namespace.get('__slots__', ())

        namespace.setdefault('_registry_name', '_' + _pluralize(_to_snake(name)))

        clazz = type.__new__(mcls, name, bases, namespace)

        field_descriptions = []
        field_names = []
        for field in fields:
            try:
                field_name, default, field_type = field
            except ValueError:
                field_name, default = field
                # Heuristic to guess a good type.
                field_type = type(default)
                # String is special
                if field_type is str:
                    field_type = String
                elif isinstance(default, int):
                    field_type = int

            if field_type is self_type:
                field_type = clazz
            # Avoid stupid bugs
            assert isinstance(field_type, type) or all(isinstance(t, type) for t in field_type)
            assert not isinstance(default, type)

            private_name = "_" + field_name
            description = DefinitionField(field_name, field_type, private_name, default)
            field_descriptions.append(description)
            field_names.append(field_name)
            if not hasattr(clazz, field_name):
                setattr(clazz, field_name, _make_property(description))

        for base in bases:
            field_descriptions = list(getattr(base, '_fields', ())) + field_descriptions

        clazz._fields = tuple(field_descriptions)
        doc = _generate_doc(name, field_descriptions)
        if clazz.__doc__:
            doc = clazz.__doc__ + "\n" + doc
        clazz.__doc__ = doc
        if '_abstract' not in namespace and not name == "NewBase":
            definitions.append(clazz)
        return clazz


class _RefDefinitionMeta(_DefinitionMeta):
    def __new__(mcls, name, bases, namespace):
        if '_abstract' not in namespace and not name == "NewBase":
            namespace.setdefault('_ref_name', _to_snake(name))
            namespace.setdefault('_registry_name', '_' + _pluralize(namespace['_ref_name']))
            namespace.setdefault('undefined_ref', getattr(Undefined, namespace['_ref_name'].upper()))
            try:
                namespace.setdefault('type_id', getattr(Type, namespace['_ref_name'].upper()))
            except:
                pass

        return _DefinitionMeta.__new__(mcls, name, bases, namespace)


class _Definition(with_metaclass(_DefinitionMeta, object)):
    """
    Any definition. It is attached to a registry.
    """
    __slots__ = '_registry',
    _abstract = True

    def __init__(self, registry, *args, **kwargs):
        self._registry = registry

        if len(self._fields) < len(args) + len(kwargs):
            raise TypeError("Too many arguments.")

        for idx, field in enumerate(self._fields):
            try:
                value = args[idx]
            except IndexError:
                if field.name in kwargs:
                    value = kwargs.pop(field.name)
                elif field.default is not no_default:
                    value = field.default
                    # Avoid mutating the default itself...
                    if type(value) is tuple:
                        value = value.copy()
                else:
                    raise TypeError("Required parameter '{}' missing.".format(field.name))

            value = field.check_value(value)
            setattr(self, field.private_name, value)
        if kwargs:
            raise TypeError("Unused argument.")

    @staticmethod
    def _construct_arg(registry, def_type, val):
        if isinstance(def_type, type) and issubclass(def_type, _RefDefinition):
            return registry._get(def_type, val)
        return val

    @classmethod
    def _construct(cls, registry, *args):
        """ Construct a definition based on the parameters of a callback """
        conv_args = [cls._construct_arg(registry, field.type, arg)
                     for arg, field in zip(args, cls._fields)]
        return cls(registry._registry_for_type(cls), *conv_args)

    def _set_field(self, field, value):
        if self._registry is None:
            raise AttributeError("This object is immutable")
        old_key = self._key
        value = field.check_value(value)
        setattr(self, field.private_name, value)
        self._registry._update(old_key, self)

    @classmethod
    def _new_key(cls, registry, *args, **kwargs):
        # Create fake object without registry and ref just to get a key
        return cls(registry, None, *args, **kwargs)._key

    @property
    def _key(self):
        """
        A key identifies the actual values of a definition.
        It shall be valid for hashing and comparison.
        """
        # Possible optimization: Cache key!
        field_keys = [_generate_key(getattr(self, field.private_name)) for field in self._fields]
        # Put the class first when mixing different definition classes
        return tuple(chain((type(self),), field_keys))

    @staticmethod
    def _write_arg(writer, definition_registry, obj_type, obj):
        if obj is None:
            return obj_type.undefined_ref
        if obj_type is String:
            return definition_registry.strings.get_ref(obj, writer)
        if isinstance(obj_type, type) and issubclass(obj_type, _RefDefinition):
            return obj._ref
        if obj_type is AttributeValue:
            return obj.as_otf2(definition_registry, writer)
        return obj

    def write(self, writer, def_registry):
        if def_registry._registry_for_type(type(self)) is not self._registry:
            raise ValueError("Trying to write a definition with the wrong writer.")
        args = [self._write_arg(writer, def_registry, field.type,
                                getattr(self, field.private_name))
                for field in self._fields]
        writer.write(self, *args)

    def _str_fields(self):
        return ", ".join("'{}': {!r}".format(field.name, getattr(self, field.name))
                         for field in self._fields)

    def __str__(self):
        return "{}: {}".format(type(self).__name__, self._str_fields())


class _RefDefinition(with_metaclass(_RefDefinitionMeta, _Definition)):
    """
    A Definition with a ref (id) and a corresponding IdRegistry that manages this definition.
    """
    __slots__ = '_ref', '_properties'
    _abstract = True

    def __init__(self, registry, ref, *args, **kwargs):
        """ The constructor of a Definition is called by a registry"""
        self._ref = ref
        self._properties = {}
        super(_RefDefinition, self).__init__(registry, *args, **kwargs)

    @classmethod
    def _construct(cls, registry, ref, *args):
        """ Construct a definition based on the parameters of a callback """
        conv_args = [cls._construct_arg(registry, field.type, arg)
                     for arg, field in zip(args, cls._fields)]
        return cls(registry._registry_for_type(cls), ref, *conv_args)

    def write(self, writer, def_registry):
        if def_registry._registry_for_type(type(self)) is not self._registry:
            raise ValueError("Trying to write a definition with the wrong writer.")
        args = [self._write_arg(writer, def_registry, field.type,
                                getattr(self, field.private_name))
                for field in self._fields]
        writer.write(self, self._ref, *args)

    def __getitem__(self, key):
        return self._properties[key].value

    def __setitem__(self, key, value):
        self._properties[key].value = value

    def _add_property(self, key, property):
        assert(key not in self._properties)
        self._properties[key] = property

    def __str__(self):
        return "{} [{}]: {}".format(type(self).__name__, self._ref, self._str_fields())

    def __repr__(self):
        return "{} [{}]".format(type(self).__name__, self._ref)


class String(_RefDefinition):
    _fields = ('name', no_default, string_types),


class _NamedRefDefinition(_RefDefinition):
    """
    A definition with a name field as the first argument which is enforced to be of type str.
    """
    _fields = ('name', no_default, String),
    _abstract = True

    def __init__(self, registry, ref, name=None, *args, **kwargs):
        if name is None:
            name = "{} #{}".format(self.__class__.__name__, ref)
        super(_NamedRefDefinition, self).__init__(registry, ref, name, *args, **kwargs)

    def __repr__(self):
        return "{} [{}] '{}'".format(type(self).__name__, self._ref, self.name)


class _PropertyDefinition(_Definition):
    """
    Common base class for property definitions.
    All derived classes must have _fields[2] = 'type', _fields[3] = 'value'
    """
    _abstract = True

    def __init__(self, registry, arg0, arg1, type_id=None, value=None):
        if value is None:
            raise TypeError("Missing argument 'value' for PropertyDefinition.")
        if type_id is None:
            type_id = get_type_id(value)
        if not isinstance(value, AttributeValue):
            value = AttributeValue(value, type_id)
        if type_id != value.type_id:
            raise TypeError("Conflicting AttributeValue type ids: {} != {}".format(type_id,
                                                                                   value.type_id))
        super(_PropertyDefinition, self).__init__(registry, arg0, arg1, type_id, value)

        if isinstance(arg0, _RefDefinition):
            if isinstance(arg1, Parameter):
                # Basically any _NamedRefDefinition would be ok, but there's no use case currently
                key = arg1.name
            elif isinstance(arg1, string_types):
                key = arg1
            else:
                raise TypeError("Invalid type for name (can be string or Parameter)")

            # Adds property to referenced definitons 'arg0' under name 'arg1'
            arg0._add_property(key, self)

    @classmethod
    def _construct(cls, registry, arg0, arg1, type_id, value):
        value = AttributeValue._construct(registry, value, type_id)
        return super(_PropertyDefinition, cls)._construct(registry, arg0, arg1, type_id, value)

    @property
    def type(self):
        return self._value.type_id

    @property
    def value(self):
        return self._value.value

    @value.setter
    def value(self, new_value):
        self._value.set(new_value)

    def write(self, writer, def_registry):
        self._type = self._value.type_id
        super(_PropertyDefinition, self).write(writer, def_registry)


class ClockProperties(_Definition):
    # We don't want numbers.Number here.
    _fields = (('timer_resolution', no_default, integer_types),
               ('global_offset', 0, integer_types),
               ('trace_length', 0, integer_types))


class Attribute(_NamedRefDefinition):
    type_id = Type.ATTRIBUTE
    _fields = (('description', ''),
               ('type', Type.STRING))


class SystemTreeNode(_NamedRefDefinition):
    """
    .. Note:: The root of the tree should have the parent `None`
    """
    _fields = (('class_name', ''),
               ('parent', None, self_type))


class LocationGroup(_NamedRefDefinition):
    _fields = (('location_group_type', LocationGroupType.PROCESS),
               ('system_tree_parent', no_default, SystemTreeNode))


class Location(_NamedRefDefinition):
    """
    .. Note::

        The number of events is managed automatically when writing traces.
        Do not use the attribute when writing.

    """
    _fields = (('type', LocationType.CPU_THREAD),
               ('number_of_events', 0),
               ('group', no_default, LocationGroup))
    __slots__ = '_number_of_events_written',

    def write(self, writer, def_registry):
        try:
            self._number_of_events = self._number_of_events_written
        except AttributeError:
            # If no writer was opened for this definition, the attribute won't be assigned.
            pass
        super(Location, self).write(writer, def_registry)


class Region(_NamedRefDefinition):
    _fields = (('canonical_name', None, String),
               ('description', ''),
               ('region_role', RegionRole.FUNCTION),
               ('paradigm', enums.Paradigm.NONE),
               ('region_flags', RegionFlag.NONE),
               ('source_file', None, String),
               ('begin_line_number', 0),
               ('end_line_number', 0))

    def __init__(self, registry, ref, name=None, canonical_name=None, *args, **kwargs):
        if name is None:
            name = "{} #{}".format(self.__class__.__name__, ref)
        if canonical_name is None:
            canonical_name = name
        super(Region, self).__init__(registry, ref, name, canonical_name, *args, **kwargs)


class Callsite(_RefDefinition):
    _fields = (('source_file', None, String),
               ('line_number', 0),
               ('entered_region', no_default, Region),
               ('left_region', no_default, Region))


class Callpath(_RefDefinition):
    """
    .. Note:: Use `None` as parent to indicate that this is the topmost region in the Callpath.
    """
    _fields = (('parent', no_default, self_type),
               ('region', no_default, Region))


class Group(_NamedRefDefinition):
    """
    Groups of type COMM_GROUP may be constructed from a sequence of either ranks or location
    objects as members argument.
    All other group types must be constructed using a sequence of their respective Definition
    objects.
    The members property is always a sequence of Definition objects, even when the Group is
    constructed using a sequence of ranks.
    """
    _fields = (('group_type', no_default, GroupType),
               ('paradigm', enums.Paradigm.NONE),
               ('group_flags', GroupFlag.NONE),
               ('members', no_default, tuple))

    def __init__(self, registry, ref, name=None, group_type=no_default,
                 paradigm=enums.Paradigm.NONE, group_flags=GroupFlag.NONE, members=no_default):
        if group_type is no_default:
            raise TypeError("Required parameter 'group_type' missing.")
        if members is no_default:
            raise TypeError("Required parameter 'members' missing.")
        if group_type == GroupType.COMM_GROUP and len(members) > 0:
            if isinstance(members[0], Location):
                if not all(isinstance(l, Location) for l in members):
                    raise TypeError("members for a COMM_GROUP must be either all Locations or "
                                    "all ranks")
            else:
                parent_group = None

                for group in registry:
                    if group.paradigm == paradigm and group.group_type == GroupType.COMM_LOCATIONS:
                        parent_group = group

                if parent_group is None:
                    raise TraceReaderError(
                        "Missing referenced group for sup-group: '{}' #{}".format(name, ref))

                members = tuple(parent_group.location(mref) for mref in members)

        super(Group, self).__init__(registry, ref, name, group_type, paradigm, group_flags,
                                    members)


    # Group is special:
    # It converts a a _field between list of Definitions and list of references
    @classmethod
    def _construct(cls, registry, ref, name, group_type, paradigm, group_flags, member_refs):
        if group_type == GroupType.LOCATIONS:
            member_reg = registry.locations
        elif group_type == GroupType.REGIONS:
            member_reg = registry.regions
        elif group_type == GroupType.METRIC:
            member_reg = registry.metrics
        elif group_type == GroupType.COMM_LOCATIONS:
            member_reg = registry.locations
        elif group_type == GroupType.COMM_GROUP:
            # Defer rank->location conversion to normal constructor
            return cls(registry._registry_for_type(cls), ref, registry.strings[name], group_type,
                       paradigm, group_flags, member_refs)
        elif group_type == GroupType.COMM_SELF:
            member_reg = None
        else:
            raise TypeError("Unknown group_type: {}".format(group_type))

        members = [member_reg[mref] for mref in member_refs]

        return cls(registry._registry_for_type(cls), ref, registry.strings[name], group_type,
                   paradigm, group_flags, members)

    def write(self, writer, def_registry):
        if def_registry._registry_for_type(type(self)) is not self._registry:
            raise ValueError("Trying to write a definition with the wrong writer.")

        args = [self._write_arg(writer, def_registry, field.type,
                                getattr(self, field.private_name))
                for field in self._fields]

        if self._group_type == GroupType.COMM_GROUP:
            super_group_members = None

            for group in def_registry.groups:
                if group.paradigm == self._paradigm and group.group_type == GroupType.COMM_LOCATIONS:
                    super_group_members = group.members

            if super_group_members is None:
                raise TraceWriterError(
                    "Missing referenced group for sup-group: '{}' #{}".format(self._name, self._ref))

            args[-1] = [super_group_members.index(member) for member in self._members]

        writer.write(self, self._ref, *args)

    @staticmethod
    def _write_arg(writer, definition_registry, obj_type, obj):
        if obj_type is tuple:
            return tuple(m._ref for m in obj)
        return _NamedRefDefinition._write_arg(writer, definition_registry, obj_type, obj)

    def rank(self, location):
        """
        Get the rank / index of a given :py:class:`Location` or any other contained Definition with
        respect to this group.
        """
        # TODO optimize with bidirectional map
        return self.members.index(location)

    def location(self, rank):
        """
        Get the :py:class:`Location` or other Definition object for a given rank within this group
        """
        return self.members[rank]


class MetricMember(_NamedRefDefinition):
    _fields = (('description', ''),
               ('metric_type', MetricType.OTHER),
               ('metric_mode', MetricMode.ABSOLUTE_POINT),
               ('value_type', Type.DOUBLE),
               ('base', Base.DECIMAL),
               ('exponent', 0),
               ('unit', ''))

    def _convert_value(self, value):
        union_value = _otf2.MetricValue()
        if self.value_type == Type.UINT64:
            union_value.unsigned_int = int(value)
        elif self.value_type == Type.INT64:
            union_value.signed_int = int(value)
        elif self.value_type == Type.DOUBLE:
            union_value.floating_point = float(value)
        else:
            raise TypeError("Cannot convert to metric value type.")
        return union_value

    def _convert_union(self, union_value):
        """
        :param union_value: from low level, is actually a ctypes.Union
        :return: nice native python value of type corresponding to our value_type
        """
        if self.value_type == Type.UINT64:
            return union_value.unsigned_int
        elif self.value_type == Type.INT64:
            return union_value.signed_int
        elif self.value_type == Type.DOUBLE:
            return union_value.floating_point
        else:
            raise TypeError("Cannot convert to metric value type.")


class Metric(_RefDefinition):
    """
    Abstract base class for :py:class:`MetricClass` and :py:class:`MetricInstance`
    definitions.
    """
    _ref_name = 'metric'


class MetricClass(Metric):
    _ref_name = 'metric'
    _fields = (('members', no_default, tuple),
               ('occurrence', MetricOccurrence.ASYNCHRONOUS),
               ('recorder_kind', RecorderKind.UNKNOWN))

    # MetricClass is special:
    # It converts a a _field between tuple of Definitions and list of references
    @classmethod
    def _construct(cls, registry, ref, member_refs, occurence, recorder_kind):
        members = tuple(registry.metric_members[member_ref] for member_ref in member_refs)
        return super(MetricClass, cls)._construct(registry, ref, members, occurence, recorder_kind)

    @staticmethod
    def _write_arg(writer, definition_registry, obj_type, obj):
        if obj_type is tuple:
            return [m._ref for m in obj]
        return _RefDefinition._write_arg(writer, definition_registry, obj_type, obj)

    @property
    def _type_ids(self):
        return [member.value_type for member in self.members]

    def _convert_values(self, values):
        return [member._convert_value(value)
                for value, member in zip(values, self.members)]

    def _convert_unions(self, unions):
        return [member._convert_union(union)
                for union, member in zip(unions, self.members)]

    @property
    def member(self):
        if len(self.members) != 1:
            raise AttributeError("Trying to read single member from MetricClass with {} "
                                 "members".format(len(self.members)))
        return self.members[0]


class MetricInstance(Metric):
    """
    .. Note::

        The metric_scope is always computed based on the type of the current scope.
        Do not explicitly provide that argument.

    """
    _ref_name = 'metric'
    _fields = (('metric_class', no_default, MetricClass),
               ('recorder', no_default, Location),
               ('metric_scope', None, MetricScope),
               ('scope', no_default, _RefDefinition))

    def __init__(self, registry, ref, metric_class, recorder, metric_scope=None, scope=None):
        if scope is None:
            raise TypeError("Missing argument 'scope' for MetricInstance.")

        if metric_scope is None:
            if isinstance(scope, Location):
                metric_scope = MetricScope.LOCATION
            elif isinstance(scope, LocationGroup):
                metric_scope = MetricScope.LOCATION_GROUP
            elif isinstance(scope, SystemTreeNode):
                metric_scope = MetricScope.SYSTEM_TREE_NODE
            elif isinstance(scope, Group):
                metric_scope = MetricScope.GROUP
            else:
                raise TypeError("Invalid scope type: {}.".format(type(scope)))

        super(MetricInstance, self).__init__(registry, ref, metric_class, recorder, metric_scope, scope)

    @property
    def metric_scope(self):
        try:
            return {
                Location: MetricScope.LOCATION,
                LocationGroup: MetricScope.LOCATION_GROUP,
                SystemTreeNode: MetricScope.SYSTEM_TREE_NODE,
                Group: MetricScope.GROUP,
            }[type(self.scope)]
        except KeyError:
            raise TypeError("Invalid scope type.")

    @classmethod
    def _construct(cls, registry, ref, metric_class, recorder, metric_scope, scope):
        if metric_scope == MetricScope.LOCATION:
            scope_reg = registry.locations
        elif metric_scope == MetricScope.LOCATION_GROUP:
            scope_reg = registry.location_groups
        elif metric_scope == MetricScope.SYSTEM_TREE_NODE:
            scope_reg = registry.system_tree_nodes
        elif metric_scope == MetricScope.GROUP:
            if registry.groups[scope].group_type == GroupType.METRIC:
                raise TypeError("You cannot refer to a group of type metric from a metric. "
                                "Really, think about it, it doesn't make any sense.")
            scope_reg = registry.groups
        else:
            raise TypeError("Unexpected MetricScope")
        return cls(registry._registry_for_type(cls), ref, registry.metrics[metric_class],
                   registry.locations[recorder], metric_scope, scope_reg[scope])

    @property
    def _type_ids(self):
        return self.metric_class._type_ids

    def _convert_values(self, values):
        return self.metric_class._convert_values(values)

    def _convert_unions(self, unions):
        return self.metric_class._convert_unions(unions)

    @property
    def members(self):
        return self.metric_class.members

    @property
    def member(self):
        if len(self.members) != 1:
            raise AttributeError("Trying to read single member from MetricClass with {} "
                                 "members".format(len(self.members)))
        return self.members[0]

    def write(self, writer, def_registry):
        self._metric_scope = self.metric_scope
        super(MetricInstance, self).write(writer, def_registry)


class Comm(_NamedRefDefinition):
    _fields = (('group', no_default, Group),
               ('parent', None, self_type))

    def rank(self, location):
        """
        Get the rank / index of a given :py:class:`Location` with respect to this communicator.
        """
        return self.group.rank(location)

    def location(self, rank):
        """
        Get the :py:class:`Location` for a given rank within this communicator
        """
        return self.group.location(rank)


class Parameter(_NamedRefDefinition):
    _fields = (('parameter_type', ParameterType.STRING),)


class RmaWin(_NamedRefDefinition):
    _fields = (('comm', no_default, Comm),)

    def rank(self, location):
        """
        Get the rank / index of a given :py:class:`Location` with respect to the communicator
        of this RMA window.
        """
        return self.comm.rank(location)

    def location(self, rank):
        """
        Get the :py:class:`Location` for a given rank within the communicator of this RMA window.
        """
        return self.comm.location(rank)


class MetricClassRecorder(_Definition):
    _fields = (('metric_class', no_default, MetricClass),
               ('recorder', no_default, Location))


class SystemTreeNodeProperty(_PropertyDefinition):
    _fields = (('system_tree_node', no_default, SystemTreeNode),
               ('name', no_default, String),
               ('type', None, Type),
               ('value', no_default, AttributeValue))


class SystemTreeNodeDomain(_Definition):
    _fields = (('system_tree_node', no_default, SystemTreeNode),
               ('system_tree_node_domain', no_default, SystemTreeDomain))


class LocationGroupProperty(_PropertyDefinition):
    _fields = (('location_group', no_default, LocationGroup),
               ('name', no_default, String),
               ('type', None, Type),
               ('value', no_default, AttributeValue))


class LocationProperty(_PropertyDefinition):
    _fields = (('location', no_default, Location),
               ('name', no_default, String),
               ('type', None, Type),
               ('value', no_default, AttributeValue))


class CartDimension(_NamedRefDefinition):
    _fields = (("size", no_default, int),
               ("cart_periodicity", CartPeriodicity.FALSE))


class CartTopology(_NamedRefDefinition):
    """
    .. Note:: Dimensions is a tuple of :class:`otf2.definitions.CartDimension`.
    """
    _fields = (("communicator", no_default, Comm),
               ("dimensions", no_default, tuple))

    @classmethod
    def _construct(cls, registry, ref, name, communicator, dimension_refs):
        dimensions = tuple(registry.cart_dimensions[dimension_ref]
                           for dimension_ref in dimension_refs)
        return cls(registry._registry_for_type(cls), ref, registry.strings[name],
                   registry.comms[communicator], dimensions)

    @staticmethod
    def _write_arg(writer, definition_registry, obj_type, obj):
        if obj_type is tuple:
            return [m._ref for m in obj]
        return _NamedRefDefinition._write_arg(writer, definition_registry, obj_type, obj)


class CartCoordinate(_Definition):
    """
    .. Note:: Coordinates is a tuple of integers.
    """
    _fields = (("topology", no_default, CartTopology),
               ("rank", no_default, int),
               ("coordinates", no_default, tuple))


class SourceCodeLocation(_RefDefinition):
    _fields = (('file', no_default, String),
               ('line_number', 0))


class CallingContext(_RefDefinition):
    """
    .. Note:: Use `None` as parent to indicate that this is the topmost CallingContext
    """
    _fields = (("region", no_default, Region),
               ("source_code_location", None, SourceCodeLocation),
               ("parent", no_default, self_type))


class CallingContextProperty(_PropertyDefinition):
    _fields = (('calling_context', no_default, CallingContext),
               ('name', no_default, String),
               ('type', None, Type),
               ('value', no_default, AttributeValue))


class InterruptGenerator(_NamedRefDefinition):
    _fields = (("interrupt_generator_mode", InterruptGeneratorMode.TIME),
               ("base", Base.DECIMAL),
               ("exponent", 0),
               ("period", no_default, int))


class Paradigm(_Definition):
    """
    .. Note:: This definition has no reference. It describes an enum.
    """
    _fields = (("paradigm", no_default, enums.Paradigm),
               ("name", no_default, String),
               ("paradigm_class", no_default, ParadigmClass))


class ParadigmProperty(_PropertyDefinition):
    """
    .. Warning::

        Does **not** refer to a :class:`otf2.definitions.Paradigm`, but
        :class:`otf2.Paradigm`!

    """
    _fields = (("paradigm", no_default, enums.Paradigm),
               ("property", no_default, enums.ParadigmProperty),
               ("type", None, Type),
               ("value", no_default, AttributeValue))


class IoFile(_NamedRefDefinition):
    """
    Abstract base class for :py:class:`IoRegularFile` and :py:class:`IoDirectory`
    definitions.
    """
    _ref_name = 'io_file'


class IoRegularFile(IoFile):
    _ref_name = 'io_file'
    _fields = (("scope", no_default, SystemTreeNode),)


class IoDirectory(IoFile):
    _ref_name = 'io_file'
    _fields = (("scope", no_default, SystemTreeNode),)


class IoFileProperty(_PropertyDefinition):
    _fields = (("io_file", no_default, IoFile),
               ('name', no_default, String),
               ('type', None, Type),
               ('value', no_default, AttributeValue))


class IoParadigm(_RefDefinition):
    _fields = (("identification", no_default, String),
               ("name", no_default, String),
               ("io_paradigm_class", IoParadigmClass.SERIAL),
               ("io_paradigm_flags", IoParadigmFlag.NONE))
    # Reusing _properties from _RefDefinitions
    # They don't factor into the key

    def __init__(self, registry, ref, identification, name, io_paradigm_class, io_paradigm_flags,
                 properties=None):
        if properties is None:
            properties = {}
        for property, value in properties.items():
            if not isinstance(property, IoParadigmProperty):
                raise TypeError("property_values for IoParadigm must be a dict with keys of type "
                                "IoParadigmProperty. Violating key: {}".format(property))
            if not isinstance(value, AttributeValue):
                properties[property] = AttributeValue(value, get_type_id(value))


        super(IoParadigm, self).__init__(registry, ref, identification, name, io_paradigm_class,
                                         io_paradigm_flags)
        self._properties = properties

    def __setitem__(self, property, value):
        # We can set the properties directly here, even if no AttributeValue is set yet
        if isinstance(value, AttributeValue):
            self._properties[property] = value
        else:
            assert property in self._properties
            self._properties[property].value = value

    @classmethod
    def _construct(cls, registry, ref, identification, name, io_paradigm_class, io_paradigm_flags,
                   properties, types, values):
        property_attribute_values = {property: AttributeValue._construct(registry, value, type_id)
                                     for property, type_id, value
                                     in zip(properties, types, values)}
        return super(IoParadigm, cls)._construct(registry, ref, identification, name,
                                                 io_paradigm_class, io_paradigm_flags,
                                                 property_attribute_values)

    def write(self, writer, def_registry):
        if def_registry._registry_for_type(type(self)) is not self._registry:
            raise ValueError("Trying to write a definition with the wrong writer.")
        args = [self._write_arg(writer, def_registry, field.type,
                                getattr(self, field.private_name))
                for field in self._fields]
        properties_list = list(self._properties.keys())
        types_list = [av.type_id for av in self._properties.values()]
        values_list = [av.as_otf2(def_registry, writer) for av in self._properties.values()]
        writer.write(self, self._ref, *(args + [properties_list, types_list, values_list]))



class IoHandle(_NamedRefDefinition):
    _fields = (("file", no_default, IoFile),
               ("io_paradigm", no_default, IoParadigm),
               ("io_handle_flags", IoHandleFlag.NONE),
               ("comm", None, Comm),
               ("parent", None, self_type))

class IoPreCreatedHandleState(_Definition):
    _fields = (('io_handle', no_default, IoHandle),
               ('io_access_mode', no_default, IoAccessMode),
               ('io_status_flags', IoStatusFlag.NONE))

class CallpathParameter(_PropertyDefinition):
    _fields = (('callpath', no_default, Callpath),
               ('parameter', no_default, Parameter),
               ('type', None, Type),
               ('value', no_default, AttributeValue))

def get_type_id(value):
    if isinstance(value, AttributeValue):
        return value.type_id
    elif isinstance(value, int):
        return Type.INT64
    elif isinstance(value, Number):
        return Type.DOUBLE
    elif isinstance(value, str):
        return Type.STRING
    elif isinstance(value, _RefDefinition) and hasattr(value, 'type_id'):
        return value.type_id
    else:
        raise TypeError("Encountered value with unexpected type for an attribute value: "
                        "'{} ({})'".format(value, type(value)))

__all__ = [definition.__name__ for definition in definitions]
__all__ += ['get_type_id']

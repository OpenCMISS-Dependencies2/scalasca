import _otf2
from .enums import Type
from . import attribute_value


class AttributeList(object):
    def __init__(self, registry, attributes=None):
        if not attributes:
            self._handle = None
            return

        self._handle = _otf2.AttributeList_New()
        for attribute, value in attributes.items():
            self._set_value_by_type(registry, attribute, value)

    def close(self):
        if self._handle is None:
            return
        _otf2.AttributeList_Delete(self._handle)
        self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def _set_value_by_type(self, registry, attribute, value):
        _otf2.AttributeList_AddAttribute(self._handle, attribute._ref, attribute.type,
                                         attribute_value.make(registry, value, attribute.type))

    @classmethod
    def _construct(cls, registry, attribute_list):
        if attribute_list is None:
            return None
        count = _otf2.AttributeList_GetNumberOfElements(attribute_list)
        if count == 0:
            return None
        attributes = dict()
        for i in range(count):
            attr_ref, attr_type, attr_value = _otf2.AttributeList_GetAttributeByIndex(
                attribute_list, i)
            attr = registry.attributes[attr_ref]
            assert attr.type == attr_type
            attributes[attr] = attribute_value.parse(registry, attr_value, attr_type)
        return attributes

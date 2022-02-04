#!/usr/bin/env python

import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

import _otf2


def pre_flush(user_data, file_type, location, caller_data, final):
    del user_data, file_type, location, caller_data, final
    return _otf2.FLUSH


def post_flush(user_data, file_type, location):
    del user_data, file_type, location
    return get_time()


def get_time():
    get_time.counter += 1
    return get_time.counter


get_time.counter = -1


class TestOTF2AnchorFile(unittest.TestCase):
    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=self.old_cwd)
        os.chdir(self.tmp_dirname)

    def tearDown(self):
        os.chdir(self.old_cwd)
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    @staticmethod
    def archive_set_numbers(archive, location_count, global_defs_count):
        assert (location_count * 2 <= global_defs_count)
        glob_def_writer = _otf2.Archive_GetGlobalDefWriter(archive)
        for loc in range(location_count):
            name = "Location #{}".format(loc)
            _otf2.GlobalDefWriter_WriteString(glob_def_writer, loc, name)
            _otf2.GlobalDefWriter_WriteLocation(glob_def_writer, loc, loc,
                                                _otf2.LOCATION_TYPE_CPU_THREAD,
                                                0, loc)
        # now we have writen location_count locations and
        # 2 * location_count definitions
        for fake_string in range(location_count, global_defs_count - location_count):
            _otf2.GlobalDefWriter_WriteString(glob_def_writer, fake_string,
                                              "fake{}".format(fake_string))

        _otf2.Archive_CloseGlobalDefWriter(archive, glob_def_writer)

    def test(self):
        archive_path = "otf2_AnchorFile_trace/trace"
        archive_prefix = "archive"
        archive_name = archive_prefix + ".otf2"

        # Information stored in the trace. */
        chunk_size_events = 1024 * 1024
        chunk_size_defs = 4 * 1024 * 1024
        substrate = _otf2.SUBSTRATE_POSIX
        compression = _otf2.COMPRESSION_NONE
        num_locations = 123
        num_definitions = 456
        machine_name = "TestMachine"
        creator = "TestCreator"
        description = "TestDescription"
        # We will write only two properties. The third one will be deleted,
        # that means it won't be written to anchor file. */
        invalid_property_names = ["",  # empty
                                  "foo",  # no "::" at all
                                  "::foo",  # empty first component 1
                                  "::foo::bar",  # empty first component 2
                                  "foo::::",  # empty middle component 1
                                  "::::foo",  # empty middle component 2
                                  "foo::",  # empty last component 1
                                  "foo::bar::",  # empty last component 2
                                  "foo::ba ::baz",  # invalid character 1
                                  "foo::bar::ba ",  # invalid character 2
                                  # NOTE: None is not an invalid property name but an invalid argument
                                 ]
        string_property_names = ["NamespaceA::Property1", "NamespaceA::Property2", "NamespaceA::Property3"]
        string_property_values = ["Valid", "OK", "true"]
        bool_property_names = ["NamespaceB::BoolProperty1", "NamespaceB::BoolProperty2"]
        bool_property_values = [True, False]
        property_name_to_delete = "Whatever::Property"
        arbitrary_property_value = "Whatever::Value"
        value_to_delete_property = ""

        archive = _otf2.Archive_Open(archive_path,
                                     archive_prefix,
                                     _otf2.FILEMODE_WRITE,
                                     chunk_size_events,
                                     chunk_size_defs,
                                     substrate,
                                     compression)

        flush_callbacks = _otf2.FlushCallbacks(pre_flush=pre_flush, post_flush=post_flush)
        _otf2.Archive_SetFlushCallbacks(archive, flush_callbacks, None)

        # Set machine name, creator and description (optional)
        _otf2.Archive_SetMachineName(archive, machine_name)
        _otf2.Archive_SetCreator(archive, creator)
        _otf2.Archive_SetDescription(archive, description)

        _otf2.Archive_SetSerialCollectiveCallbacks(archive)

        self.archive_set_numbers(archive, num_locations, num_definitions)

        for invalid_property_name in invalid_property_names:
            with self.assertRaises(_otf2.Error) as cm:
                _otf2.Archive_SetProperty(archive, invalid_property_name, "value", True)
            self.assertEqual(cm.exception.code, _otf2.ERROR_PROPERTY_NAME_INVALID,
                             'Incorrect return type for invalid property name: "{}"'.format(invalid_property_name))

        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  value_to_delete_property,
                                  False)

        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  arbitrary_property_value,
                                  False)
        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  value_to_delete_property,
                                  True)

        _otf2.Archive_SetProperty(archive,
                                  string_property_names[0],
                                  arbitrary_property_value,
                                  False)

        _otf2.Archive_SetProperty(archive,
                                  string_property_names[1],
                                  string_property_values[1],
                                  False)

        _otf2.Archive_SetProperty(archive,
                                  string_property_names[0],
                                  string_property_values[0],
                                  True)

        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  arbitrary_property_value,
                                  False)

        _otf2.Archive_SetProperty(archive,
                                  string_property_names[2],
                                  string_property_values[2],
                                  False)

        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  value_to_delete_property,
                                  True)

        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  arbitrary_property_value,
                                  False)
        _otf2.Archive_SetProperty(archive,
                                  property_name_to_delete,
                                  value_to_delete_property,
                                  True)

        _otf2.Archive_SetBoolProperty(archive,
                                      bool_property_names[0],
                                      bool_property_values[0],
                                      False)

        _otf2.Archive_SetBoolProperty(archive,
                                      bool_property_names[1],
                                      bool_property_values[1],
                                      False)

        # Close archive. */
        _otf2.Archive_Close(archive)

        # Open new archive. */
        full_path = os.path.join(archive_path, archive_name)

        reader = _otf2.Reader_Open(full_path)
        _otf2.Reader_SetSerialCollectiveCallbacks(reader)

        version_read = _otf2.Reader_GetVersion(reader)
        version_expected = (_otf2.VERSION_MAJOR, _otf2.VERSION_MINOR, _otf2.VERSION_BUGFIX)
        self.assertEqual(version_read, version_expected, "Version mismatch")

        num_locations_read = _otf2.Reader_GetNumberOfLocations(reader)
        self.assertEqual(num_locations, num_locations_read, "Number of locations does not match")

        num_definitions_read = _otf2.Reader_GetNumberOfGlobalDefinitions(reader, )
        self.assertEqual(num_definitions, num_definitions_read, "Number of definitions does not match")

        (chunk_size_events_read, chunk_size_defs_read) = _otf2.Reader_GetChunkSize(reader)
        self.assertEqual(chunk_size_events, chunk_size_events_read, "Event chunk size does not match")
        self.assertEqual(chunk_size_defs, chunk_size_defs_read, "Definition chunk size does not match")

        substrate_read = _otf2.Reader_GetFileSubstrate(reader)
        self.assertEqual(substrate, substrate_read, "File substrate does not match")

        compression_read = _otf2.Reader_GetCompression(reader)
        self.assertEqual(compression, compression_read, "Compression does not match")

        machine_name_read = _otf2.Reader_GetMachineName(reader)
        self.assertEqual(machine_name, machine_name_read, "Machine name does not match")

        creator_read = _otf2.Reader_GetCreator(reader)
        self.assertEqual(creator, creator_read, "Creator does not match")

        description_read = _otf2.Reader_GetDescription(reader)
        self.assertEqual(description, description_read, "Description does not match")

        # Get additional meta-information (a.k.a. properties) */
        property_names_read = _otf2.Reader_GetPropertyNames(reader)

        self.assertEqual(len(property_names_read), len(string_property_names) + len(bool_property_names),
                         "Number of properties does not match")

        for (property_name_read, property_name_written, property_value_written) \
                in zip(property_names_read, string_property_names + bool_property_names,
                       string_property_values + bool_property_values):
            # First, this test writes #number_of_string_properties properties of type
            # string to the anchor file. Afterwards, #number_of_bool_properties are
            # written to the anchor file.

            # Property names are store in upper case. If everything works correctly,
            #  _otf2.Reader_GetProperty will return our property names converted from
            # mixed case to upper case. Therefore, we also have to convert property
            # names to upper case.
            converted_name = property_name_written.upper()

            self.assertEqual(property_name_read, converted_name, "Property name does not match")

            if isinstance(property_value_written, str):
                property_value_read = _otf2.Reader_GetProperty(reader, property_name_read)
            elif isinstance(property_value_written, bool):
                property_value_read = _otf2.Reader_GetBoolProperty(reader, property_name_read)
            else:
                self.fail("Invalid written property type")
            self.assertEqual(property_value_written, property_value_read, "Property value does not match")

        _otf2.Reader_Close(reader)


if __name__ == '__main__':
    unittest.main()

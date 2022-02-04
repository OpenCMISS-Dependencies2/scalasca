# Setup

Provide a Python 2.7+ with the jinja2 package, best is to use the `otf2` branch
via

    $ pip install -U git+ssh://git@gitlab.version.fz-juelich.de:10022/perftools/utils/jinja.git@otf2

# How to add new definitions, events, and enum entries to OTF2

1. `./configure --verbose` a build directory.

   The summary should include something like:

   ```
       Python for generator:       yes, using /usr/bin/python3 with 'jinja2' module in version 2.11.2-otf2
   ```

2. (A) Edit the data input files `share/otf2/otf2.{types,enums,defs,events}`
   to add new stuff to OTF2.

   - Ensure correct version annotations (i.e., `since=Version(X,Y)`). Use the
     next unreleased version and keep this up-to-date in case this changes.

   - Assign new entities identifiers starting at 254 and counting towards 0.
     This ensures that your new entities wont change, if new entities will be
     re-integrated into master before your branch will be re-integrated.
     For enum entries this can be achieved by specifying an explicit value
     with the `value=` parameter.

2. (B) Edit `build-config/VERSION` if you want to change the version. Re-run
   `./bootstrap` in a clean build directory, or ensure disabling of the Autoconf
   caching. This can be ensured by placing a file named `.autom4te.cfg` in the
   root directory with the following content:

   ```
   begin-language: "Autoconf"
   args: --no-cache
   end-language: "Autoconf"
   ```

3. Run `make generate`.

   In case you have merge conflicts in one of the `.sig` or outfile files, run
   `make generate-force`.

# Syntax for enum types and entries

There are 3 classes to specify a new enum type: `GeneralEnum`, `DefEnum`,
and `EventEnum`. They do not differ in the definition but each of them define
where they appear in the headers and therefore specify the usage of the enum
type in the records.

* `GeneralEnum` => `OTF2_GeneralDefinitions.h`

  Enum type is used in the general API e.g., `OTF2_Archive_Open()`.

* `DefEnum`     => `OTF2_Definitions.h`

  Enum type is used in definition records.

* `EventEnum`   => `OTF2_Events.h`

  Enum type is used in event records.

The syntax is as follows:

```python
Enum( documentation, name, [entries...], [options] )
```

* _string_ `documentation`:

  Describes the enum type.

* _string_ `name`:

  Name of the enum type in CamelCase without the `OTF2_` prefix.

* _EnumEntry_ `[entries...]`:

  The enum entries, see below.

* Accepted optional parameters

  * _Version_ `since`:

    The version since this enum type exists.

  * _Version_ `attic`:

    The version since this enum type was removed from the API.

  * _bool_ `bitset_like`:

    Hints the templates that this enum is used as a bitset.

  * _Otf2Types_ `type`:

    C type used for this enum type, defaults to `uint8_t`. See Appendix I.

  * _bool_|_EnumEntry_ `terminator`:

    If True a terminating enum entry is generated. The name of the entry is
    derived from the common prefix of all defined enum entries and `MAX`. The
    value is the maximum value of all defined enum entries plus 1.
    If the `terminator` is an EntryType, this will be used directly used as the
    terminating entry of the enum type.

  * _list<string>_ `includes`:

    A list of named enum types to be included into this enum type. The names
    are derived by removing the common prefix of the to be included enum
    entries and prefixed with the common prefix of this enum type.
    The values are continuously assigned starting with the current maximum
    value of all enum entries plus 1.

The syntax for an enum entry is as follows:

```python
EnumEntry( documentation, name, [value], [since], [options] )
```

* _string_ `documentation`:

  Describes the enum entry.

* _string_ `name`:

  The fully qualified name of the enum entry in UPPER_CASE_WITH_UNDERSCORES
  i.e., with `OTF2_` prefix and the name of the enum type.

* _int_|_string_ `value`:

  The value of this entry. Maybe an integer or a valid C expression as string.
  If not given, the entry gets the maximum of previously defined enum entries
  plus 1.

* _Version_ `since`:

  The version this enum entry was added, if not given, the version from the
  enum type is used.

* Some enum types expect optional parameters, which are too special to list
  here.

# Manual steps needed after adding new definitions or events

## `otf2-print`

* In `src/tools/otf2_print/otf2_print_inc.c` are templates to print new
  records. Copy the full function including the commented body to
  `src/tools/otf2_print/otf2_print.c` and adjust the `printf` call:

  * Let attributes start with upper case
  * Remove trailing comma from attribute listing

  For new named definitions, replace the `OTF2_UNDEFINED_STRING` argument in
  `otf2_print_add_def_name` call with the name attribute.

## Python bindings

* For most records, no changes to the low-level `_otf2` module are necessary:

  * Records that contain an array use a size and pointer parameters in the C,
    but only an array in the Python form, hence the Callback and Write function
    needs to be adapted and are exempt from generation. See e.g. `MetricClass`
    in `_otf2.GlobalDefReaderCallbacks.tmpl.py` and
    `_otf2.GlobalDefWriter.tmpl.py`

  * Records that contain a union type (AttributeValue) must apply a conversion
    upon reading. These are typically properties with the parameters

    * refered_def
    * name
    * type
    * value

    for which a conversion function convert_property_union is available that
    can be added as last parameter to _GlobalDefReader_callback_wrapper.
    See also `SystemTreeNodeProperty` in `_otf2.GlobalDefReaderCallbacks.tmpl.py`.
    This is an unfortunate limitation of ctypes not being able to handle unions
    as value parameters.

* The high-level `otf2` requires the following changes:

  * All definitions are manually created, most are really simple.
  * All definitions require an appropriate entry in the
    `DefinitionRegistry.write` chain that ensures proper ordering.
  * Polymorphic definitions require manual intervention (joint registry,
    filtered properties, base class).
  * Event records are usually generated automatically.

# Appendix I: Available Otf2Types

* `int8_t`
* `int16_t`
* `uint8_t`
* `uint16_t`
* `int32_t`
* `int64_t`
* `uint32_t`
* `uint64_t`
* `float`
* `double`
* `OTF2_TimeStamp`
* `OTF2_MetricValue`

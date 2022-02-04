"""
The bindings are very close to the original OTF2 API (See
https://silc.zih.tu-dresden.de/otf2-current/html/index.html), with the following exceptions:

0. `OTF2_` is omitted from all names. All symbols are directly in the package.
1. Return codes are checked internally and converted to exceptions of type :py:class:`Error`.
   Error codes are never returned by API functions.
2. Array input parameters that are length / pointer in C, are instead a single sequence
   parameter.
3. Output parameters that are handled by pointers, are instead provided as the return value
   of the function.
   In case of multiple returns outputs, a tuple is returned.
   Some functions also allow to hand in the reference parameters, but that does not work
   for basic python types.

Enums
-----

Enums are wrapped types, don't treat them as integers. Just assign and compare for (in)equality.
Use == for comparison, not is.

Callbacks
---------

Callbacks are normal python functions with appropriate arguments similar to those of normal
functions. The caller must ensure that these functions are not garbage collected for the time
that they might be used by OTF2!

.. warning:: DO NOT pass lambdas or bound methods that go out of scope!
    There is no exception here that will save you from attack of the nasal demons.

Strings
-------

`_otf2` uses the native str type of the respective python version. Internally OTF2 is defined to
use UTF-8 strings.
For python3 the conversion happens transparently, a user shall hand str objects to the API and
will always receive str objects by the API.
In python2 the user is allowed to pass str or unicode objects, but will always receive str
objects when reading the trace. These str objects are encoded in UTF-8 by definition.

ctypes
------

`_otf2` uses ctypes. Some of the types of the module are aliases to ctypes-types, but you never
need to explicitly use ctypes.
"""
__copyright__ = "Copyright (c) 2015-2016, Technische Universitaet Dresden, Germany"
__license__ = "BSD"

from .Archive import *
from .AttributeList import *
from .AttributeValue import *
from .Callbacks import *
from .Definitions import *
from .DefReader import *
from .DefReaderCallbacks import *
from .DefWriter import *
from .ErrorCodes import *
from .Events import *
from .EventSizeEstimator import *
from .EvtReader import *
from .EvtReaderCallbacks import *
from .EvtWriter import *
from .GeneralDefinitions import *
from .GlobalDefReader import *
from .GlobalDefReaderCallbacks import *
from .GlobalDefWriter import *
from .GlobalEvtReader import *
from .GlobalEvtReaderCallbacks import *
from .GlobalSnapReader import *
from .GlobalSnapReaderCallbacks import *
from .IdMap import *
from .Marker import *
from .MarkerReader import *
from .MarkerReaderCallbacks import *
from .MarkerWriter import *
from .Reader import *
from .SnapReader import *
from .SnapReaderCallbacks import *
from .SnapWriter import *
from .Thumbnail import *

# From GeneralDefinitions
__version__ = VERSION

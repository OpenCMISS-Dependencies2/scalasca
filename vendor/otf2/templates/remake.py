#! /usr/bin/env python

import sys
import os
import re
import textwrap

# we are called from the src directory directly, use path relative to
# our location, we sit in templates/
mypath = os.path.dirname(os.path.abspath(__file__))

from jinja2 import Environment, FileSystemLoader

def _execfile(filepath, globals=globals(), locals=None):
    if globals is None:
        globals = {}
    globals.update({
        "__file__": filepath,
        "__name__": "__main__",
    })
    with open(filepath, 'rb') as file:
        exec(compile(file.read(), filepath, 'exec'), globals, locals)

env = Environment(
    loader=FileSystemLoader(mypath),
    line_statement_prefix='#tmpl',
    line_comment_prefix='###',
    block_start_string='{#',
    block_end_string='#}',
    variable_start_string='##',
    variable_end_string='##',
    comment_start_string='/*#',
    comment_end_string='#*/',
    keep_trailing_newline=True
)

def jinja2filter(env):
    def make_filter(f):
        assert f.__name__[:7] == 'filter_', 'Function should start with filter_'
        env.filters[f.__name__[7:]] = f
        return f
    return make_filter

@jinja2filter(env)
def filter_prefix_path(values, prefix=' '):
    if len(values) == 0:
        return ''
    return prefix + prefix.join(values)

_execfile(os.path.join(mypath, 'templates.py'))

tmpl = env.get_template(os.path.basename(sys.argv[1]))
out = open(sys.argv[2], 'w')

tmpl.stream({
    'common_template_deps': common_template_deps,
    'templates':            templates
}).dump(out)
out.close()

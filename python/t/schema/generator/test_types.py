from nose.tools import assert_equal
from nose.tools import assert_not_equal
from nose.tools import assert_raises

from nix.schema.generator import *

types = {
	String : (str, ""),
	Object : (dict, {}),
	Array : (list, []),
	Int : (int, 0),
	Double : (float, 0),
	Bool : (bool, False),
	Null : (None, None),
}


def test_ctors():
	name = "name"
	for t in types:
		o = t(name)
		assert_equal(o.ident, name)
		assert_equal(o.base_type, types[t][0])
		assert_equal(o.content, types[t][1])


def test_allow_null():
	name = "name"
	for t in types:
		o = t(name, allow_null=True)
		assert_equal(o.content, None)

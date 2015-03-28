from nose.tools import assert_equal
from nose.tools import assert_not_equal
from nose.tools import assert_raises
import json

from nix.schema.generator import *

json_params = {
	"sort_keys" : True,
	"indent" : False,
}


def test_build():
	schema = Schema("test")
	schema.add_field(String("string_field"))
	schema.add_field(Int("int_field", allow_null=True))
	obj = Object("obj")
	inner_array = Array("inner")
	inner_array.append(String("")).append(Array(""))
	obj.add_field(inner_array).add_field(String("inner_field"))
	arr = Array("array")
	arr.append(String(""), Int(""), Bool(""), Double(""))
	schema.add_field(arr)
	schema.add_field(obj)

	expected = {
		"@id": "test",
		"@version": 0,
		"array": ["", 0, False, 0.0],
		"obj" : {
			"inner" : [
				"",
				[]
			],
			"inner_field" : ""
		},
		"string_field": "",
		"int_field" : None,
		"content": {},
	}

	assert_equal(
		schema.to_json(**json_params),
		json.dumps(expected, **json_params)
	)

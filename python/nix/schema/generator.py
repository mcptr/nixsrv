#!/usr/local/bin/python3 -tt

import json
from json import JSONEncoder


class Field(object):
	def __init__(self, ident, base_type, **kwargs):
		self.ident = ident
		self.base_type = base_type
		self.allow_null = bool(kwargs.pop("allow_null", False))
		self.content = None if self.allow_null else (
			kwargs.pop("default", base_type() if base_type else None)
		)

	def _ensure_is_field(self, f):
		if not isinstance(f, Field):
			raise ValueError("Expected Field instance, got: ", f)


class ComplexField(Field):
	def __init__(self, ident, base_type, **kwargs):
		super().__init__(ident, base_type, **kwargs)

	def add_field(self, f):
		self._ensure_is_field(f)
		self._add_field(f)
		return self


class Array(ComplexField):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, list, **kwargs)

	def append(self, *args):
		for f in list(args):
			self.add_field(f)
		return self

	def _add_field(self, f):
		self.content.append(f)


class Object(ComplexField):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, dict, **kwargs)

	def _add_field(self, f):
		self.content[f.ident] = f


class Int(Field):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, int, **kwargs)


class Double(Field):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, float, **kwargs)


class Bool(Field):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, bool, **kwargs)


class Null(Field):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, None, **kwargs)


class String(Field):
	def __init__(self, ident, **kwargs):
		super().__init__(ident, str, **kwargs)


class Schema(object):
	def __init__(self, ident, version = 0):
		self.ident = ident
		self.version = version
		self.root = {
			"@id" : self.ident,
			"@version" : self.version,
			"content" : {}
		}

	def _ensure_is_field(self, f):
		if not isinstance(f, Field):
			raise ValueError(f)

	def add_field(self, f):
		self._ensure_is_field(f)
		self.root[f.ident] = f

	def set_content(self, f):
		self._ensure_is_field(f)
		self.root["content"] = f

	def build(self):
		pass

	def to_json(self, **kwargs):
		return json.dumps(
			self.root,
			cls=FieldEncoder,
			sort_keys=kwargs.pop("sort_keys", False),
			indent=kwargs.pop("indent", False)
		)

	def validate(self):
		pass


class FieldEncoder(JSONEncoder):
	def default(self, o):
		return o.content


if __name__ == "__main__":
	schema = Schema("test", 1)
	inner_arr = Array("inner_arr")

	arr = Array("")
	arr.add_field(String("a_string"))
	arr.add_field(Int("a_string"))
	arr.add_field(String("a_string"))
	arr.add_field(inner_arr)

	schema.set_content(arr)

	print(schema.to_json())

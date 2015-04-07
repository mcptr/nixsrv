from util.nix_server import NixServer
from util.nix_client import NixClient

import json

from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal
# from nose.tools import assert_raises


def test_routing():
	with NixServer() as server:
		client = NixClient(server.get_address())
		result = client.call("Resolver", "list_routes", {}, 2000)
		assert_false(result.is_rejected(), "not rejected")
		assert_false(result.is_abandoned(), "not abandoned")
		assert_true(result.is_replied(), "is replied")

		expected = {
			"routing": {
				"list_routes": {
					"access": 0,
					"description": "",
					"processing_type": 1
				},
				"resolve": {
					"access": 2,
					"description": "",
					"processing_type": 1
				},
				"bind": {
					"access": 2,
					"description": "",
					"processing_type": 1
				},
				"unbind": {
					"access": 2,
					"description": "",
					"processing_type": 0
				}
			}
		}
		assert_equal(expected, result.data)

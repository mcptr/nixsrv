from util.nix_server import NixServer
from util.nix_client import NixClient

import json

from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal
# from nose.tools import assert_raises

expected_routing = {
	"routing": {
		"unbind": {
			"description": "",
			"processing_type": "VOID",
			"access_modifier": "API_PRIVATE"
		},
		"list_routes": {
			"description": "Display all routes handled by 'Resolver' module",
			"processing_type": "SYNC",
			"access_modifier": "ANY"
		},
		"resolve": {
			"description": "",
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE"
		},
		"bind": {
			"description": "",
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE"
		}
	}
}


def test_routing():
	with NixServer() as server:
		client = NixClient(server.get_address())
		result = client.call("Resolver", "list_routes", {}, 2000)
		assert_false(result.is_rejected(), "not rejected")
		assert_false(result.is_abandoned(), "not abandoned")
		assert_true(result.is_replied(), "is replied")
		# print(json.dumps(result.data, indent=4))
		assert_equal(expected_routing, result.data)

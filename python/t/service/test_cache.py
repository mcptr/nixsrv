from util.nix_server import NixServer
from util.nix_client import NixClient

import json
import time
from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "Cache"

expected_routing = {
	"@status_code": 0,
	"module": {
		"ident": "Cache",
		"version": 1
	},
	"routing": {
		"list_routes": {
			"access_modifier": "ANY",
			"description": "Display all routes handled by 'Cache' module",
			"processing_type": "SYNC"
		},
		"ping": {
			"access_modifier": "ANY",
			"description": "",
			"processing_type": "SYNC"
		},
		"remove": {
			"access_modifier": "API_PRIVATE",
			"description": "",
			"processing_type": "VOID"
		},
		"retrieve": {
			"access_modifier": "API_PRIVATE",
			"description": "",
			"processing_type": "SYNC"
		},
		"status": {
			"access_modifier": "API_PRIVATE",
			"description": "",
			"processing_type": "SYNC"
		},
		"store": {
			"access_modifier": "API_PRIVATE",
			"description": "",
			"processing_type": "SYNC"
		}
	}
}

development_key = "_development_key_"
development_key_public = "_development_key_public"


def test_routing_structure():
	with NixServer(modules=[module]) as server:
		client = NixClient(server.get_address())
		result = client.call(module, "list_routes", {}, 2000)
		# print(json.dumps(result.data, indent=4, sort_keys=True))
		assert_false(result.is_rejected(), "not rejected")
		assert_false(result.is_abandoned(), "not abandoned")
		assert_true(result.is_replied(), "is replied")
		assert_equal(expected_routing, result.data)


def test_service():
	test_key = "unittest"
	test_value = {"a_string" : "some string"}
	with NixServer(modules=[module]) as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}

		# testing 'store' route

		invalid_params = params.copy()
		response = client.call(module, "store", invalid_params, 2000)
		assert_equal(
			response.status_code(),
			501,
			"Code 501 - indicates invalid parameters"
		)

		# valid
		params.update({
			"key" : test_key,
			"value" : test_value
		})
		response = client.call(module, "store", params, 2000)
		assert_true(response.is_status_ok(), "Store succeeded")

		# # testing 'retrieve' route

		params = {"@api_key" : development_key}
		invalid_params = params.copy()
		response = client.call(module, "retrieve", invalid_params, 2000)
		assert_true(response.is_status_fail(), "Retrieve fails without key")

		# valid
		params = {"@api_key" : development_key, "key" : test_key}
		response = client.call(module, "retrieve", params, 2000)
		assert_true(response.is_status_ok(), "Is retrieve call ok?")
		assert_equal(
			response.data.get(test_key),
			test_value,
			"Is retrieved value correct?"
		)

		# testing 'remove' route
		response = client.send_one_way(module, "remove", params)
		time.sleep(0.2)
		# after remove key cannot be retrieved
		response = client.call(module, "retrieve", params, 2000)
		assert_true(response.is_status_fail(), "Is key removed?")

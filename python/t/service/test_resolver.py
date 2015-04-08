from util.nix_server import NixServer
from util.nix_client import NixClient

import time
from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "Resolver"

expected_routing = {
	"@status_code": 0,
	"module" : {
		"ident" : module,
		"version" : 1,
	},
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


development_key = "_development_key_"
development_key_public = "_development_key_public"


def test_routing_structure():
	with NixServer() as server:
		client = NixClient(server.get_address())
		result = client.call(module, "list_routes", {}, 2000)
		assert_false(result.is_rejected(), "not rejected")
		assert_false(result.is_abandoned(), "not abandoned")
		assert_true(result.is_replied(), "is replied")
		assert_equal(expected_routing, result.data)
		routing = result.data.get("routing")
		auth_error_code = 101
		# routes with access mod != ANY require api key -> code 101
		for route in routing:
			if routing[route]["access_modifier"] != "ANY":
				response = client.call(module, route, {}, 2000)
				if routing[route]["processing_type"] != "VOID":
					status_code = response.data.get("@status_code")
					assert_equal(status_code, auth_error_code)

		# test API_PRIVATE with invalid key (public) -> code 101
		for route in routing:
			am = routing[route]["access_modifier"]
			if am == "API_PRIVATE" and route != "list_routes":
				params = {"@api_key" : development_key_public}
				response = client.call(module, route, params, 2000)
				status_code = response.data.get("@status_code")
				assert_equal(status_code, 101)


def test_service():
	test_node = "unittest-01"
	test_address = "tcp://127.0.0.1:33333"
	with NixServer() as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}

		# testing 'bind' route

		invalid_params = params.copy()
		response = client.call(module, "bind", invalid_params, 2000)
		assert_equal(
			response.status_code(),
			501,
			"Code 501 - indicates invalid parameters"
		)

		# valid
		params.update({
			"node" : test_node,
			"address" : test_address}
		)
		response = client.call(module, "bind", params, 2000)
		assert_true(response.is_status_ok(), "Bind succeeded")

		# testing 'resolve' route
		params = {"@api_key" : development_key}
		invalid_params = params.copy()
		response = client.call(module, "resolve", invalid_params, 2000)
		assert_true(response.is_status_fail(), "Resolve fails without node")

		# valid
		params.update({"node" : test_node})
		response = client.call(module, "resolve", params, 2000)
		assert_true(response.is_status_ok(), "Is resolved ok?")
		assert_equal(
			response.data["address"],
			test_address,
			"Is resolved address correct?"
		)


		# testing 'unbind' route
		response = client.send_one_way(module, "unbind", params)
		time.sleep(0.2)
		# after unbind node cannot be resolved
		response = client.call(module, "resolve", params, 2000)
		assert_true(response.is_status_fail(), "Is node unbound?")

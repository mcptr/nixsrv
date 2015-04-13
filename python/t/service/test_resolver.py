from util.nix_server import NixServer
from util.nix_client import NixClient

import time
import json
from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "Resolver"

expected_routing = {
	"@status_code": 0,
	"module": {
		"ident": "Resolver",
		"version": 1
	},
	"routing": {
		"list_routes": {
			"access_modifier": "ANY",
			"description": "Display all routes handled by 'Resolver' module",
			"processing_type": "SYNC"
		},
		"node/bind": {
			"access_modifier": "API_PRIVATE",
			"description": "Bind node name to address",
			"processing_type": "SYNC"
		},
		"node/resolve": {
			"access_modifier": "API_PRIVATE",
			"description": "Resolve node's address",
			"processing_type": "SYNC"
		},
		"node/unbind": {
			"access_modifier": "API_PRIVATE",
			"description": "Unbind node's address",
			"processing_type": "SYNC"
		},
		"ping": {
			"access_modifier": "ANY",
			"description": "",
			"processing_type": "SYNC"
		},
		"service/bind": {
			"access_modifier": "API_PRIVATE",
			"description": "Register a node that serves given implementation ",
			"processing_type": "SYNC"
		},
		"service/resolve": {
			"access_modifier": "API_PRIVATE",
			"description": "Resolve a node that serves given implementation ",
			"processing_type": "SYNC"
		},
		"service/unbind": {
			"access_modifier": "API_PRIVATE",
			"description": "Unbind a node that serves given implementation ",
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


def test_nodes():
	test_node = "unittest-01"
	test_address = "tcp://127.0.0.1:33333"
	with NixServer(modules=[module]) as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}

		# testing 'bind' route

		invalid_params = params.copy()
		response = client.call(module, "node/bind", invalid_params, 2000)
		assert_equal(
			response.status_code(),
			501,
			"Code 501 - indicates invalid parameters"
		)

		# valid
		params.update({
			"nodename" : test_node,
			"address" : test_address
		})
		response = client.call(module, "node/bind", params, 2000)
		assert_true(response.is_status_ok(), "Bind succeeded")

		# testing 'resolve' route
		params = {"@api_key" : development_key}
		invalid_params = params.copy()
		response = client.call(module, "node/resolve", invalid_params, 2000)
		assert_true(response.is_status_fail(), "Resolve fails without node")

		# valid
		params.update({"nodename" : test_node})
		response = client.call(module, "node/resolve", params, 2000)
		assert_true(response.is_status_ok(), "Is resolved ok?")
		assert_equal(
			response.data[test_node],
			test_address,
			"Is resolved address correct?"
		)


		# testing 'unbind' route
		response = client.send_one_way(module, "node/unbind", params)
		time.sleep(0.2)
		# after unbind node cannot be resolved
		response = client.call(module, "node/resolve", params, 2000)
		assert_true(response.is_status_fail(), "Is node unbound?")


def test_services():
	test_service = "some_implementation"
	test_nodename1 = "service-node-1"
	test_address1 = "tcp://1.1.1.1:11111"
	test_nodename2 = "service-node-2"
	test_address2 = "tcp://2.2.2.2:22222"

	with NixServer(modules=[module]) as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}
		invalid_params = params.copy()
		response = client.call(module, "service/bind", invalid_params, 2000)
		assert_equal(
			response.status_code(),
			501,
			"Code 501 - indicates invalid parameters"
		)

		# fail bind when node is not yet known
		invalid_params = params.copy()
		invalid_params.update({
			"service" : "test",
			"nodename" : test_nodename1
		})
		response = client.call(module, "service/bind", params, 2000)
		assert_true(
			response.is_status_fail(),
			"bind service fails (node not bound yet)"
		)

		# bind nodes
		node_bind_params = params.copy()
		node_bind_params.update({
			"nodename" : test_nodename1,
			"address" : test_address1
		})

		response = client.call(module, "node/bind", node_bind_params)
		assert_true(response.is_status_ok(), "node 1 bound")

		node_bind_params.update({
			"nodename" : test_nodename2,
			"address" : test_address2
		})
		response = client.call(module, "node/bind", node_bind_params)
		assert_true(response.is_status_ok(), "node 2 bound")

		# first node
		params.update({
			"service" : test_service,
			"nodename" : test_nodename1

		})
		response = client.call(module, "service/bind", params, 2000)
		assert_true(response.is_status_ok(), "1st bind service ok")

		response = client.call(module, "service/resolve", params, 2000)
		assert_true(response.is_status_ok(), "resolved after 1st bind")
		assert_equal(
			response.data.get("addresses", []),
			[test_address1],
			"resolved to a single address (1 elem array)"
		)

		# second node
		params.update({
			"service" : test_service,
			"nodename" : test_nodename2
		})
		response = client.call(module, "service/bind", params, 2000)
		assert_true(response.is_status_ok(), "2nd bind service ok")

		response = client.call(module, "service/resolve", params, 2000)
		assert_true(response.is_status_ok(), "resolved after 2nd bind")
		assert_equal(
			response.data.get("addresses", []),
			[test_address1, test_address2],
			"resolved both addresses single address (1 elem array)"
		)
		response = client.call(module, "service/unbind", params, 2000)
		assert_true(response.is_status_ok(), "unbind 2nd address")

		response = client.call(module, "service/resolve", params, 2000)
		assert_true(response.is_status_ok(), "resolved after unbinding")
		assert_equal(
			response.data.get("addresses", []),
			[test_address1],
			"resolved a single address after unbind (1 elem array)"
		)

		params.update({
			"nodename" : test_nodename1
		})
		response = client.call(module, "node/unbind", params, 2000)
		assert_true(response.is_status_ok(), "unbind node1")

		params.update({
			"nodename" : test_nodename2
		})
		response = client.call(module, "node/unbind", params, 2000)
		assert_true(response.is_status_ok(), "unbind node1")

		response = client.call(module, "service/resolve", params, 2000)
		assert_true(
			response.is_status_fail(),
			"Unbinding nodes unbinds services"
		)
		assert_equal(response.status_msg(), "Unknown service.")

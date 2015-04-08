from util.nix_server import NixServer
from util.nix_client import NixClient

import json

from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "JobQueue"


expected_routing = {
	"@status_code": 0,
	"module": {
		"version": 1,
		"ident": "JobQueue"
	},
	"routing": {
		"job/result/get": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"list_routes": {
			"processing_type": "SYNC",
			"description": "Display all routes handled by 'JobQueue' module",
			"access_modifier": "ANY"
		},
		"job/progress/set": {
			"processing_type": "PUBLISH",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"job/result/set": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"job/submit": {
			"processing_type": "FUTURE",
			"description": "",
			"access_modifier": "API_PRIVATE"
		}
	},
}


development_key = "_development_key_"
development_key_public = "_development_key_public"
JOB_ID = None


def test_routing_structure():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())
		result = client.call(module, "list_routes", {}, 2000)
		# print(json.dumps(result.data, indent=4))
		assert_true(result.is_replied(), "is replied")
		assert_equal(expected_routing, result.data)


def test_submit():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}

		# testing 'submit' route

		response = client.call(module, "job/submit", params, 2000)
		assert_true(response.is_status_fail())

		params.update({"module" : "test", "action" : "test"})
		response = client.call(module, "job/submit", params, 2000)
		assert_true(response.is_status_ok())
		JOB_ID = response.data["job_id"]
		assert_true(len(JOB_ID))


def test_set_progress():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())
		params = {
			"@api_key" : development_key,
			"job_id" : JOB_ID
		}
		client
		# valid
		# params.update({
		#	"key" : test_key,
		#	"value" : test_value
		# })
		# response = client.call(module, "store", params, 2000)
		# assert_true(response.is_status_ok(), "Store succeeded")

		# # # testing 'retrieve' route

		# params = {"@api_key" : development_key}
		# invalid_params = params.copy()
		# response = client.call(module, "retrieve", invalid_params, 2000)
		# assert_true(response.is_status_fail(), "Retrieve fails without key")

		# # valid
		# params = {"@api_key" : development_key, "key" : test_key}
		# response = client.call(module, "retrieve", params, 2000)
		# assert_true(response.is_status_ok(), "Is retrieve call ok?")
		# assert_equal(
		#	response.data.get(test_key),
		#	test_value,
		#	"Is retrieved value correct?"
		# )

		# # testing 'remove' route
		# response = client.send_one_way(module, "remove", params)
		# time.sleep(0.2)
		# # after remove key cannot be retrieved
		# response = client.call(module, "retrieve", params, 2000)
		# assert_true(response.is_status_fail(), "Is key removed?")

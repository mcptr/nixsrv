from util.nix_server import NixServer
from util.nix_client import NixClient

import time
import json

from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "JobQueue"


expected_routing = {
	"routing": {
		"job/queue/manage": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "ADMIN"
		},
		"job/queue/clear": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "ADMIN"
		},
		"job/work/get": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"job/submit": {
			"processing_type": "FUTURE",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"list_routes": {
			"processing_type": "SYNC",
			"description": "Display all routes handled by 'JobQueue' module",
			"access_modifier": "ANY"
		},
		"job/result/set": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"job/result/get": {
			"processing_type": "SYNC",
			"description": "",
			"access_modifier": "API_PRIVATE"
		},
		"job/progress/set": {
			"processing_type": "VOID",
			"description": "",
			"access_modifier": "API_PRIVATE"
		}
	},
	"module": {
		"ident": "JobQueue",
		"version": 1
	},
	"@status_code": 0
}

development_key = "_development_key_"
development_key_public = "_development_key_public"
JOB_ID = None


def test_routing_structure():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())
		result = client.call(module, "list_routes", {}, 100)
		# print(json.dumps(result.data, indent=4))
		assert_true(result.is_replied(), "is replied")
		assert_equal(expected_routing, result.data)


def test_submit():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())
		params = {"@api_key" : development_key}

		# testing 'submit' route

		response = client.call(module, "job/submit", params, 100)
		assert_true(response.is_status_fail())

		job_module = "test"
		job_action = "test"
		job_param_name = "param"
		job_param_value = "πœę©ß←"

		params.update({
			"module" : job_module,
			"action" : job_action,
			job_param_name : job_param_value
		})

		response = client.call(module, "job/submit", params, 100)
		assert_true(response.is_status_ok())
		JOB_ID = response.data["job_id"]
		assert_true(len(JOB_ID))

		params = {
			"@api_key" : development_key,
			"job_id" : JOB_ID
		}
		response = client.call(module, "job/result/get", params, 100)
		assert_true(response.is_status_fail(), "Fail - job not started")

		params = {
			"@api_key" : development_key,
			"module" : "test"  # queue name
		}
		response = client.call(module, "job/work/get", params, 100)
		assert_true(response.is_status_ok(), "Got job to process")
		# this does not happen in real world, but this is a test
		# and this is the only submitted job
		assert_equal(response.data["job_id"], JOB_ID, "Job id")
		# ---
		assert_equal(
			response.data["queue_node"],
			server.get_nodename(), "Origin node name set")
		job_payload = response.data["parameters"]
		assert_equal(job_payload["@api_key"], development_key, "api key")
		# no need to test module - it was a name of the queue
		assert_equal(job_payload["action"], job_action, "action")
		assert_equal(
			job_payload[job_param_name],
			job_param_value, "API key")

		# simulate some processing
		total = 121
		for processed in range(0, total, 20):
			params = {
				"@api_key" : development_key,
				"job_id" : JOB_ID,
				"progress" : float(processed) / total * 100
			}
			client.send_one_way(module, "job/progress/set", params)
			time.sleep(0.01)
			response = client.call(module, "job/result/get", params, 100)
			if processed < total:
				assert_equal(
					response.status_code(),
					2, "Got progress report (code 2: not_ready)")

		params = {
			"@api_key" : development_key,
			"job_id" : JOB_ID,
			job_param_name : "Result: %s" % job_param_value
		}

		response = client.call(module, "job/result/set", params, 100)
		assert_true(response.is_status_ok(), "set response")

		params = {
			"@api_key" : development_key,
			"job_id" : JOB_ID,
		}
		response = client.call(module, "job/result/get", params, 100)
		assert_true(response.is_status_ok(), "got result")
		data = response.data
		assert_equal(data["@api_key"], development_key, "result: api key")
		assert_equal(data["job_id"], JOB_ID, "result: job_id")
		assert_equal(
			data[job_param_name],
			"Result: %s" % job_param_value, "result: job value")

import os
import time
import json

from util.nix_server import NixServer
from util.nix_client import NixClient

from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal

module = "JobQueue"


expected_routing = {
	"routing": {
		"ping" : {
			"access_modifier": "ANY",
			"description": "",
			"processing_type": "SYNC"
		},
		"job/result/set": {
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE",
			"description": ""
		},
		"manage": {
			"processing_type": "SYNC",
			"access_modifier": "ADMIN",
			"description": ""
		},
		"status": {
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE",
			"description": ""
		},
		"job/result/get": {
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE",
			"description": ""
		},
		"job/progress/set": {
			"processing_type": "VOID",
			"access_modifier": "API_PRIVATE",
			"description": ""
		},
		"job/work/get": {
			"processing_type": "SYNC",
			"access_modifier": "API_PRIVATE",
			"description": ""
		},
		"list_routes": {
			"processing_type": "SYNC",
			"access_modifier": "ANY",
			"description": "Display all routes handled by 'JobQueue' module"
		},
		"job/submit": {
			"processing_type": "FUTURE",
			"access_modifier": "API_PRIVATE",
			"description": ""
		}
	},
	"module": {
		"ident": "JobQueue",
		"version": 1
	},
	"@status_code": 0
}

development_key = "_development_key_"
development_key_public = "_development_key_public_"
development_key_admin = "_development_key_admin_"
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
		assert_true(response.is_status_fail(), "Fails without params")

		job_module = "test"
		job_action = "test"
		job_param_name = "param"
		job_param_value = "πœę©ß←"

		params.update({
			"module" : job_module,
			"action" : job_action,
			"data" : {
				job_param_name : job_param_value
			}
		})

		response = client.call(module, "job/submit", params, 1000)
		assert_true(response.is_status_ok(), "Submit ok")
		JOB_ID = response.data["@job_id"]
		assert_true(len(JOB_ID))

		params = {
			"@api_key" : development_key,
			"@job_id" : JOB_ID
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
		assert_equal(response.data["@job_id"], JOB_ID, "Job id")
		# ---
		assert_equal(
			response.data["@queue_node"],
			server.get_nodename(), "Origin node name set")
		job_payload = response.data["parameters"]
		assert_equal(job_payload["@api_key"], development_key, "api key")
		# no need to test module - it was a name of the queue
		assert_equal(job_payload["action"], job_action, "action")
		assert_equal(
			job_payload.get("data", {}).get(job_param_name),
			job_param_value, "API key")

		# simulate some processing
		total = 121
		for processed in range(0, total, 20):
			params = {
				"@api_key" : development_key,
				"@job_id" : JOB_ID,
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
			"@job_id" : JOB_ID,
			job_param_name : "Result: %s" % job_param_value
		}

		response = client.call(module, "job/result/set", params, 100)
		assert_true(response.is_status_ok(), "set response")

		params = {
			"@api_key" : development_key,
			"@job_id" : JOB_ID,
		}
		response = client.call(module, "job/result/get", params, 100)
		assert_true(response.is_status_ok(), "got result")
		data = response.data
		assert_equal(data["@api_key"], development_key, "result: api key")
		assert_equal(data["@job_id"], JOB_ID, "result: job_id")
		assert_equal(
			data[job_param_name],
			"Result: %s" % job_param_value, "result: job value")


def test_queue_management():
	with NixServer(modules=["job-queue"]) as server:
		client = NixClient(server.get_address())

		params = {
			"@api_key" : development_key,
		}

		invalid_auth_params = {
			"@api_key" : development_key_public,
		}
		response = client.call(
			module,
			"manage",
			invalid_auth_params
		)
		assert_equal(response.status_code(), 101, "Auth error")

		status_params = {
			"@api_key" : development_key_admin,
		}
		response = client.call(module, "status", status_params, 2000)
		assert_true(response.is_status_ok(), "got status")
		status = response.data
		assert_equal(status["total_completed"], 0, "no jobs done yey")
		assert_equal(status["results_awaiting"], 0, "no results awaiting")
		assert_equal(status["in_progress"], 0, "no jobs in progress")
		assert_equal(status["total_pending"], 0, "no pending jobs")
		assert_equal(
			status["queues"]["test"]["closed"],
			False,
			"test queue is not closed"
		)

		queue_name_1 = "unittest-queue-%d" % os.getpid()
		queue_name_2 = "second-queue-%d" % os.getpid()
		create_params = {
			"@api_key" : development_key_admin,
			"queue" : queue_name_1,
			"create" : {
				"size" : 256,
			}
		}
		response = client.call(module, "manage", create_params)
		assert_true(response.is_status_ok(), "created queue")

		create_params["queue"] = queue_name_2
		response = client.call(module, "manage", create_params)
		assert_true(response.is_status_ok(), "created second queue")

		response = client.call(module, "manage", create_params)
		assert_true(response.is_status_fail(), "queue already exists")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status")
		status = response.data
		assert_equal(
			status["queues"][queue_name_1]["closed"],
			False,
			"created queue is not closed"
		)

		assert_equal(
			status["queues"][queue_name_2]["closed"],
			False,
			"second created queue is not closed"
		)

		assert_equal(
			status["queues"][queue_name_1]["pending"],
			0, "no jobs in created queue")

		assert_equal(
			status["queues"][queue_name_2]["pending"],
			0, "no jobs in second created queue")

		# FIRST queue - submit jobs

		submitted_jobs = []
		batch_size = 10
		submit_params = params.copy()
		for i in range(0, batch_size):
			submit_params.update({
				"module" : queue_name_1,
				"action" : "dummy",
				"data" : [i, "a string", True, {"test" : []}, None]
			})

			response = client.call(
				module, "job/submit", submit_params)

			assert_true(response.is_status_ok(), "submitted job")
			job_id = response.data["@job_id"]
			assert_true(len(job_id), "got job id")
			submitted_jobs.append(job_id)

		assert_equal(
			len(set(submitted_jobs)),
			batch_size,
			"submitted jobs have unique ids"
		)

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status")
		status = response.data
		assert_equal(
			status["total_pending"],
			batch_size, "number of pending jobs")

		# SECOND queue - submit jobs

		for i in range(0, batch_size):
			submit_params.update({
				"module" : queue_name_2,
				"action" : "dummy",
				"data" : [i, "a string", True, {"test" : []}, None]
			})

			response = client.call(
				module, "job/submit", submit_params)

			assert_true(response.is_status_ok(), "submitted job")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status")
		status = response.data
		assert_equal(
			status["queues"][queue_name_2]["pending"],
			batch_size,
			"second queue got jobs"
		)
		assert_equal(
			status["total_pending"],
			batch_size * 2, "number of pending jobs")

		# Close second queue
		switch_params = params.copy()
		switch_params["queue"] = queue_name_2
		switch_params["enable"] = False
		response = client.call(module, "manage", switch_params)
		assert_true(response.is_status_ok(), "close queue call")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status after close")
		status = response.data
		assert_equal(
			status["queues"][queue_name_2]["closed"],
			True,
			"second queue is now closed"
		)
		# Reopen second queue
		switch_params = params.copy()
		switch_params["queue"] = queue_name_2
		switch_params["enable"] = True
		response = client.call(module, "manage", switch_params)
		assert_true(response.is_status_ok(), "close queue call")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status after reopen")
		status = response.data
		assert_equal(
			status["queues"][queue_name_2]["closed"],
			False,
			"second queue is now enabled"
		)
		assert_equal(
			status["queues"][queue_name_2]["pending"],
			batch_size,
			"second queue still has pending jobs"
		)

		# clear jobs in SECOND queue
		clear_params = params.copy()
		clear_params["queue"] = queue_name_2
		clear_params["clear"] = True
		response = client.call(module, "manage", clear_params)
		assert_true(response.is_status_ok(), "close queue call")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status after reopen")
		status = response.data
		assert_equal(
			status["queues"][queue_name_2]["pending"],
			0, "second queue cleared")

		# remove SECOND queue
		remove_params = params.copy()
		remove_params["queue"] = queue_name_2
		remove_params["remove"] = True
		response = client.call(module, "manage", remove_params)
		assert_true(response.is_status_ok(), "remove queue call")

		response = client.call(module, "status", status_params)
		assert_true(response.is_status_ok(), "got status after remove")
		status = response.data
		assert_false(queue_name_2 in status["queues"], "second queue removed")

		params = {
			"@api_key" : development_key,
			"server" : True,
			"stats" : True,
			"module_stats" : True,
			"routing" : True,
		}

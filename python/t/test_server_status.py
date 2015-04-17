from util.nix_server import NixServer
from util.nix_client import NixClient

import json
import time
from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal


development_key = "_development_key_"

server_modules = ["cache", "resolver", "job-queue"]
server_modules_names = ["Cache", "Resolver", "JobQueue", "Status"]


def test_basic():
	with NixServer(modules=server_modules) as server:
		params = {
			"@api_key" : development_key,
			"server" : True,
			"stats" : True,
			"module_stats" : True,
			"routing" : True,
		}
		client = NixClient(server.get_address())
		response = client.call("Status", "status", params)
		# print(json.dumps(response.data, indent=4, sort_keys=True))
		assert_true(response.is_status_ok(), "Got server status")
		data = response.data
		assert_true(data.get("nodename", ""), "has nodename")
		assert_true(
			data.get("server", {}).get("routing", ""),
			"has routing")
		assert_true(data.get("stats", {}), "stats")
		for name in server_modules_names:
			assert_true(
				data["stats"].get(name),
				"has stats for: %s" % name
			)
		# # will check uptime
		# time.sleep(1)
		# response = client.call("Status", "status", params)
		# assert_true(data.get("server.uptime", 9) > 1, "uptime")

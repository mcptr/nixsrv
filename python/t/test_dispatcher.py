from util.nix_server import NixServer
from util.nix_client import NixClient

import time
from nose.tools import assert_false
from nose.tools import assert_true
from nose.tools import assert_equal


development_key = "_development_key_"
development_key_public = "_development_key_public"


def test_basic():
	with NixServer(modules=["cache"]) as server:
		client = NixClient(server.get_address())
		result = client.call("Invalid", "invalid", {}, 2000)
		assert_true(result.is_rejected(), "Invalid route (reject)")

		result = client.call("Cache", "store", {}, 2000)
		assert_true(result.is_status_fail(), "Is failed?")
		assert_equal(result.status_code(), 101, "Auth error")

		params = {
			"@api_key" : development_key,
			"key" : "key",
			"value" : []
		}
		result = client.call("Cache", "store", params, 2000)
		assert_true(result.is_status_ok(), "succeeded with auth")
		assert_equal(result.status_code(), 0, "success indicated by 0")

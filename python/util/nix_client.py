# import sys
import json
import nix.lib.yami as yami


class Result(object):
	def __init__(self):
		self.state = None
		self.data = {}

	def status_code(self):
		return self.data.get("@status_code", None)

	def status_msg(self):
		return self.data.get("@status", None)

	def is_status_ok(self):
		return (self.is_replied() and
				self.data.get("@status_code", None) == 0)

	def is_status_fail(self):
		return (self.is_replied() and
				self.data.get("@status_code", None) != 0)

	def is_replied(self):
		return (self.state[0] == yami.OutgoingMessage.REPLIED)

	def is_rejected(self):
		return (self.state[0] == yami.OutgoingMessage.REJECTED)

	def is_abandoned(self):
		return (self.state[0] == yami.OutgoingMessage.ABANDONED)

	def is_posted(self):
		return (self.state[0] == yami.OutgoingMessage.POSTED)

	def is_transmitted(self):
		return (self.state[0] == yami.OutgoingMessage.TRANSMITTED)


class NixClient(object):
	def __init__(self, address = None):
		self.__address = address
		self.__agent = yami.Agent({"tcp_nonblocking" : 1})

	def call(self, mod, route, params = None, timeout_ms = 3000):
		params = (params or {})
		result = Result()
		with self.__agent.send(
				self.__address,
				mod, route,
				{"message" : json.dumps(params)}) as msg:
			msg.wait_for_completion(timeout_ms)
			result.state = msg.get_state()

			if result.state[0] == yami.OutgoingMessage.REPLIED:
				reply = msg.get_reply()
				if "message" in reply:
					result.data = json.loads(reply["message"])
			return result

	def send_one_way(self, mod, route, params = None):
		params = (params or {})
		self.__agent.send_one_way(
			self.__address,
			mod, route,
			{"message" : json.dumps(params)})

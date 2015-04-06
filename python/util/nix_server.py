import os
import sys
import signal
import random
import time
import errno
from subprocess import Popen


class NixServer(object):
	def __init__(self, address = None):
		self.project_root = os.getenv("PROJECT_ROOT")
		self.__address = address
		if not self.__address:
			port = random.randint(65515, 65535)
			self.__address = "tcp://127.0.0.1:%d" % port
		self.__pidfile = "/tmp/nix.%d.pid" % os.getpid()

	def get_address(self):
		return self.__address

	def start(self):
		executable = os.path.join(self.project_root, "bin", "NIX")
		cmd = [
			executable, "-F",
			"--enable-resolver", "-A", self.__address,
			"--pidfile", self.__pidfile
		]
		Popen(cmd, stdout=sys.stdout, shell=False)
		print("")
		wait = 3
		while not os.path.isfile(self.__pidfile):
			time.sleep(1)
			wait -= 1

		if not os.path.isfile(self.__pidfile):
			raise Exception("Server startup failed")

		wait = 10
		try:
			with open(self.__pidfile, "r") as fh:
				pid = int(fh.read())
				while wait and (os.kill(pid, 0) is not None):
					time.sleep(1)
					wait -= 1
				if os.kill(pid, 0) is not None:
					raise Exception("Server startup failed")
		except Exception as e:
			print(e)
			raise
		print("Server ready")

	def stop(self):
		print("Stopping server")
		with open(self.__pidfile, "r") as fh:
			pid = int(fh.read())
			wait = 10
			try:
				os.kill(pid, signal.SIGTERM)
				while (os.kill(pid, 0) is None) and wait:
					time.sleep(1)
					print("Still alive")
					wait -= 1

				if os.kill(pid, 0) is None:
					print("Killing with SIGKILL")
					os.kill(pid, signal.SIGKILL)
			except OSError as e:
				if e.errno != errno.ESRCH:
					raise

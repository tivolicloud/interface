import os
import json

if ("HIFI_DOMAIN_SERVER_PORT" in os.environ):
	CONFIG_PATH = "/root/.local/share/Tivoli Cloud VR/domain-server/config.json"
	DOMAIN_PORT = 40102
	DOMAIN_PORT = int(os.environ["HIFI_DOMAIN_SERVER_PORT"])

	def mkdirp(path):
		try:
			os.makedirs(path)
		except:
			pass

	if (os.path.isfile(CONFIG_PATH)):
		with open(CONFIG_PATH, "r") as jsonStr:
			data = json.load(jsonStr)

			if ("metaverse" not in data):
				data["metaverse"] = {}

			data["metaverse"]["local_port"] = DOMAIN_PORT 

			f = open(CONFIG_PATH, "w")
			f.write(json.dumps(data, indent=4))
			f.close()

	else:
		mkdirp(os.path.dirname(CONFIG_PATH))
		f = open(CONFIG_PATH, "w")
		f.write(json.dumps({"metaverse": {"local_port": DOMAIN_PORT}}, indent=4))
		f.close()

import subprocess
import requests
import json

class VisionStream:
    def __init__(self, config, executable="vision_http_server"):
        # Initialize the string used in the system call
        self.start_command = f"{executable} --config {config}"

        # Set default address values
        ip = "0.0.0.0"
        port = "8080"

        # Read json and find http server information
        with open(config) as f:
            data = json.load(f)
            for module in data["config"]:
                if module["module"] == "HTTP":
                    ip = module["ip"]
                    port = str(module["port"])

        self.address = f"http://{ip}:{port}"
        print(self.start_command)

    def start(self):
        # System call to initialize the server
        if not self.open():
            subprocess.Popen(self.start_command.split())

    def wait(self):
        # Wait for server to open
        while True:
            if self.open():
                break

    def open(self, timeout=0.5):
        # Call a standard status check to see if server is open
        try:
            response = requests.get(self.address + "/online", timeout=timeout)
        except requests.exceptions.ConnectionError:
            return False
        except requests.exceptions.ReadTimeout:
            return False
        else:
            return True

    def close(self, timeout=1):
        # Request for the server to close, return true if everything ran smoothly
        try:
            response = requests.get(self.address + "/stop", timeout=timeout)
            if response.status_code == 200:
                return True
            else:
                return False
        except requests.exceptions.ConnectionError:
            return False
        except requests.exceptions.ReadTimeout:
            return False

    def get_objects(self, timeout=1):
        response = requests.get(self.address + "/detected_objects", timeout=timeout)
        try:
            if response.status_code == 200:
                return response.json()
            else:
                return "{BadResponse}"
        except requests.exceptions.ConnectionError:
            return "{ConnectionError}"
        except requests.exceptions.ReadTimeout:
            return "{Timeout}"


if __name__ == '__main__':
    import time

    print("Creating the server")

    test_server = VisionStream("/home/tipo/Documents/rumarino_config.json")

    test_server.start()

    print("Waiting for server to open")

    test_server.wait()

    print("Server is open")

    # Get frame every half second
    for i in range(5):
        print(test_server.get_objects())
        time.sleep(0.5)
    test_server.close()

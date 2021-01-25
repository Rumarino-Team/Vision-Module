import subprocess
import requests


class VisionStream:
    def __init__(self, model, executable="vision_http_server", **arguments):
        # Initialize the string used in the system call
        self.start_command = f"{executable} --yolo_model {model}"

        # Set default address values
        ip = "0.0.0.0"
        port = "8080"

        # Build the string arguments
        for key, value in arguments.items():
            self.start_command += f" --{key} {value}"
            if key == "ip":
                ip = value
            elif key == "port":
                port = value

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

    args = {
        "zed_play": "/home/tipo/Downloads/test_input_video.svo",
        "model_record": "/home/tipo/Downloads/result.avi",
        "model_fps": "15",
        "confidence": "60",
        "ip": "0.0.0.0",
        "port": "8080"
    }

    test_server = VisionStream("/home/tipo/Downloads/RUBBER-DUCKY", **args)

    test_server.start()

    print("Waiting for server to open")
    while True:
        if test_server.open():
            break

    print("Server is open")

    # Get frame every half second
    for i in range(5):
        print(test_server.get_objects())
        time.sleep(0.5)
    test_server.close()

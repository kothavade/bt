# for file in examples folder
# run buildDir/bt <file>

import os
import subprocess


def main():
    for file in os.listdir("examples"):
        if file.endswith(".torrent"):
            print("Running file: " + file)
            subprocess.run(["./cmake-build-debug/bt", "examples/" + file])


if __name__ == "__main__":
    main()

mkdir ~/git
cd ~/git
git clone https://github.com/pybind/pybind11.git
cd pybind11

# Classic CMake build compatible with 3.10.2
mkdir build
cd build
cmake ..
make install

# This may not be needed so it's commented out
#pip install .
#pip3 install .

#sudo apt get install python-pybind11
#sudo apt get install python3-pybind11

# Export QnTools library to your eviroment:
```
  $ export QnTools_DIR=$LD_LIBRARY_PATH:/path/to/QnTools/install/lib/cmake/QnTools
```
# Build the project
```
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make -j
```
# Run the test using 
```
  $ test /path/to/qn_tools_test/au123_fw3_rs_rescale.root
```
The script works with root file inside the repository. It creates the output file named "result.root". 

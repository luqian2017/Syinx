1 #!/bin/bash

cmake .
make -j4
cp gameserver ../bin/ -r
cp conf/* ../bin/Config -r

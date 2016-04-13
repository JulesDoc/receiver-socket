set +e


c++ -O3 -fpermissive -Wall -Wextra -Wno-unused-parameter \
    -ggdb -g -pthread -fpermissive \
     -o fastReceiverUDP fastReceiverUDP.cpp -fpermissive \


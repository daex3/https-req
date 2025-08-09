# HTTPS Req
Download a file raw via HTTPS with openssl
## Setup
```shell
sudo ln -s $PWD/https-req.c /usr/include/https-req.c
make bin name=https-req
https-req abc.com
```
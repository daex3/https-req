# HTTPS Req
Download a file raw via HTTPS
## Setup
```shell
sudo ln -s $PWD/https-req.c /usr/include/https-req.c
make bin name=https-req
https-req abc.com
```
## Libs
- openssl
## Example
```c
#include <https-req.c>

int main() {
	char *s = simply_download("github.com", 30000);
}
```
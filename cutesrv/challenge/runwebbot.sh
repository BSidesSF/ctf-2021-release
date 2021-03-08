#!/bin/bash

docker run --network=host --cap-add=SYS_ADMIN -e PORT=3000 -e COOKIES="loginsvc=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJsb2dpbnN2YyIsImV4cCI6MTYxNDgxNTUyOCwiaWF0IjoxNjEyMTM3MTI4LCJpc3MiOiJsb2dpbnN2YyIsIm5iZiI6MTYxMjEzNzEyOCwic3ViIjoiZm9vIn0.sLhooKVhWDZK5ghqU6b9wzBafqmvv_m1Fuhx8VWfcfgLochUY7rA3cUoe7MR7cfWBsfoRLfO-6uMZdGFi49FYw;URL=http://localhost:5123/;HttpOnly" gcr.io/bsides-sf-ctf-2021/webbot:latest

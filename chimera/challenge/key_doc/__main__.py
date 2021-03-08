#!/bin/env python

import base64

print(base64.b64decode("emlwIGtleTogZXRlcm5hbGZpcmVzb2ZjaGltZXJhCg==").decode("utf-8"), end='')

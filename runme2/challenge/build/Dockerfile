FROM debian:buster-slim
MAINTAINER "Ron Bowes"

# Install all the tools
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install build-essential -y

# We're going to mount the source here
RUN mkdir /src

# Upload the build script
COPY build.sh /build.sh
RUN chmod +x /build.sh

RUN ls -l /

CMD ["/build.sh"]

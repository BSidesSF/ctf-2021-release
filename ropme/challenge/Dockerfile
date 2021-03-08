FROM i386/debian:10-slim

RUN adduser --disabled-password --gecos '' ctf
RUN apt-get update && apt-get install -y xinetd

# Make necessary directories
RUN mkdir -p /home/ctf
WORKDIR /home/ctf

# Upload files
ADD src/ropme /home/ctf/ropme
RUN chmod +x /home/ctf/ropme
ADD flag.txt /home/ctf/flag.txt
RUN chmod +r /home/ctf/flag.txt

# Service file
ADD service.xinetd /etc/xinetd.d/service

# xinetd itself must run as root
# USER ctf
EXPOSE 1338

CMD service xinetd restart && sleep infinity

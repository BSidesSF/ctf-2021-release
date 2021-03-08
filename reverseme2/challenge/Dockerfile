FROM debian:10-slim

RUN adduser --disabled-password --gecos '' ctf
RUN apt-get update && apt-get install -y xinetd

# Make necessary directories
RUN mkdir -p /home/ctf
WORKDIR /home/ctf

# Upload files
ADD src/reverseme2 /home/ctf/reverseme2
RUN chmod +x /home/ctf/reverseme2
ADD flag.txt /home/ctf/flag.txt
RUN chmod +r /home/ctf/flag.txt

# Service file
ADD service.xinetd /etc/xinetd.d/service

# xinetd itself must run as root
# USER ctf
EXPOSE 1339

CMD service xinetd restart && sleep infinity

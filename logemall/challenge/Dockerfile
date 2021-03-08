FROM debian:10-slim

RUN adduser --disabled-password --gecos '' ctf
RUN apt-get update && apt-get install -y xinetd

# Make necessary directories
RUN mkdir -p /home/ctf
WORKDIR /home/ctf

# Upload files
ADD src/logemall /home/ctf/logemall
ADD data /home/ctf/data
ADD encounters /home/ctf/encounters
ADD maps /home/ctf/maps
ADD flag.txt /home/ctf/flag.txt

RUN chmod -R +r /home/ctf
RUN chmod -R +X /home/ctf

# Service file
ADD service.xinetd /etc/xinetd.d/service

# xinetd itself must run as root
# USER ctf
EXPOSE 666

CMD service xinetd restart && sleep infinity

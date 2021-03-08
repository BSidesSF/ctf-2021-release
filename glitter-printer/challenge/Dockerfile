FROM i386/debian:10-slim

RUN adduser --disabled-password --gecos '' printer
RUN apt-get update && apt-get install -y xinetd netcat-traditional

RUN mkdir /src
WORKDIR /src

# Source files
ADD src/core src/harness /src/
RUN chmod +rx /src/harness
RUN chmod +r /src/core

# Flag
ADD flag.txt /
RUN chmod +r /flag.txt

# Make necessary directories
RUN mkdir -p /spool/queues /spool/jobs/control /spool/jobs/data
RUN chown -R printer.printer /spool

# Service file
ADD service.xinetd /etc/xinetd.d/service

# xinetd itself must run as root
#USER printer
EXPOSE 515

CMD service xinetd restart && sleep infinity

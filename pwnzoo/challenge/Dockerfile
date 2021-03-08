FROM gcr.io/bsides-sf-ctf-2021/base:latest

COPY flag.txt /home/ctf/flag.txt
COPY pwnzoo /home/ctf/pwnzoo
RUN chown -R root:ctf /home/ctf && \
  chmod 755 /home/ctf && \
  chmod 755 /home/ctf/pwnzoo && \
  chmod 644 /home/ctf/flag.txt

EXPOSE 1234

USER ctf

CMD ["/usr/bin/socat", "TCP4-LISTEN:1234,reuseaddr,fork", "EXEC:/home/ctf/pwnzoo,stderr"]

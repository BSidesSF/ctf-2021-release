FROM gcr.io/bsides-sf-ctf-2021/base:latest

COPY phicorps /home/ctf
COPY flag.txt /home/ctf

RUN chown -R root:ctf /home/ctf
RUN chmod -R ugo=rX /home/ctf

EXPOSE 25519

WORKDIR /home/ctf
USER ctf

CMD ["/usr/bin/socat", "TCP4-LISTEN:25519,reuseaddr,fork", "EXEC:/home/ctf/phicorps,stderr"]

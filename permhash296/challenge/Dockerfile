FROM gcr.io/bsides-sf-ctf-2021/base:latest

COPY challenge /home/ctf
COPY banner.txt /home/ctf
COPY flag_easy.txt /home/ctf
COPY flag_med.txt /home/ctf
COPY flag_hard.txt /home/ctf
ADD src/ /home/ctf/src

RUN chmod -R ugo=rX /home/ctf
RUN chown -R root:ctf /home/ctf

EXPOSE 4560

WORKDIR /home/ctf
USER ctf

CMD ["/usr/bin/socat", "TCP4-LISTEN:4560,reuseaddr,fork", "EXEC:/home/ctf/challenge,stderr"]
